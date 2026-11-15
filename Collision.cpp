#include "Collision.h"
#include "Fighter.h"
#include "AudioEngine.h"
#include <algorithm>
#include <cmath>

bool CollisionSystem::CheckAABB(const Rectangle& a, const Rectangle& b) {
    return CheckCollisionRecs(a, b);
}

Rectangle CollisionSystem::GetIntersection(const Rectangle& a, const Rectangle& b) {
    return GetCollisionRec(a, b);
}

void CollisionSystem::ResolvePushboxes(Fighter& f1, Fighter& f2) {
    if ((f1.GetState() == FSTATE_SPECIAL && f1.GetCharacterId() == CHAR_POPULIST && f1.GetSpecialIndex() == 3) ||
        (f2.GetState() == FSTATE_SPECIAL && f2.GetCharacterId() == CHAR_POPULIST && f2.GetSpecialIndex() == 3) ||
        f1.IsSpiderHoleHidden() || f2.IsSpiderHoleHidden()) {
        return;
    }

    Rectangle b1 = f1.GetPushbox();
    Rectangle b2 = f2.GetPushbox();

    if (CheckCollisionRecs(b1, b2)) {
        Rectangle overlap = GetCollisionRec(b1, b2);
        float shift = overlap.width * 0.5f;
        float p1x = f1.GetPosition().x;
        float p2x = f2.GetPosition().x;

        if (p1x < p2x) {
            float newP1 = p1x - shift;
            float newP2 = p2x + shift;
            if (newP1 < STAGE_MIN_X) { newP2 += (STAGE_MIN_X - newP1); newP1 = STAGE_MIN_X; }
            else if (newP2 > STAGE_MAX_X) { newP1 -= (newP2 - STAGE_MAX_X); newP2 = STAGE_MAX_X; }
            f1.SetPositionX(newP1);
            f2.SetPositionX(newP2);
        } else {
            float newP1 = p1x + shift;
            float newP2 = p2x - shift;
            if (newP2 < STAGE_MIN_X) { newP1 += (STAGE_MIN_X - newP2); newP2 = STAGE_MIN_X; }
            else if (newP1 > STAGE_MAX_X) { newP2 -= (newP1 - STAGE_MAX_X); newP1 = STAGE_MAX_X; }
            f1.SetPositionX(newP1);
            f2.SetPositionX(newP2);
        }
    }
}

static void TestStrike(Fighter& attacker, Fighter& defender,
                       std::vector<SparkEffect>& sparks, float& cameraShake) {
    Rectangle hitBox{};
    AttackData attack{};
    if (!attacker.GetActiveHitbox(hitBox, attack)) {
        return;
    }

    auto hurtBoxes = defender.GetHurtboxes();
    for (const auto& hurtBox : hurtBoxes) {
        if (CollisionSystem::CheckAABB(hitBox, hurtBox)) {
            Rectangle inter = CollisionSystem::GetIntersection(hitBox, hurtBox);
            Vector2 sparkPos{ inter.x + inter.width * 0.5f, inter.y + inter.height * 0.5f };

            attacker.SetHitConnected(true);

            if (attack.type == ATTACK_THROW && defender.CanTechThrow() && defender.GetState() != FSTATE_FINISH_HIM_DIZZY) {
                bool defRight = (defender.GetPosition().x >= attacker.GetPosition().x);
                attacker.ApplyThrowTech(!defRight);
                defender.ApplyThrowTech(defRight);
                sparks.push_back(SparkEffect{ sparkPos, true, 0, 10, true });
                cameraShake = 2.0f;
                return;
            }

            bool isBlocking = defender.CheckCanBlock(attack.type);
            int remFrames = std::max(1, (attack.startupFrames + attack.activeFrames + attack.recoveryFrames) - attacker.GetStateFrame());

            if (isBlocking) {
                int chip = attack.isSpecial ? std::max(1, attack.damage / 5) : 0;
                defender.ApplyBlock(chip, 6, attack.blockstunFrames, attack.pushback);
                attacker.SetHitstop(6);
                attacker.AddSuperMeter(10);
                attacker.SetLastFrameAdvantage(attack.blockstunFrames - remFrames);
                defender.SetLastFrameAdvantage(remFrames - attack.blockstunFrames);
                cameraShake = 1.3f;
                sparks.push_back(SparkEffect{ sparkPos, true, 0, 8, true });
                AudioEngine::Instance().PlaySfxPan(SFX_BLOCK, 1.0f, 1.0f, sparkPos.x);
            } else {
                int hs = attack.isSpecial ? 13 : (attack.isHeavy ? 10 : 6);
                defender.ApplyHit(attack.damage, attack.type, hs, attack.hitstunFrames, attack.pushback, attack.causesKnockdown, attack.isLauncher);
                attacker.SetHitstop(hs);
                attacker.AddComboHit();
                attacker.AddSuperMeter(attack.isSpecial ? 14 : (attack.isHeavy ? 18 : 10));
                defender.ResetCombo();
                attacker.SetLastFrameAdvantage(attack.hitstunFrames - remFrames);
                defender.SetLastFrameAdvantage(remFrames - attack.hitstunFrames);
                cameraShake = attack.isSpecial ? 4.4f : (attack.isHeavy ? 3.0f : 1.5f);

                sparks.push_back(SparkEffect{ sparkPos, false, 0, 10, true });
                AudioEngine::Instance().PlaySfxPan(attack.isHeavy ? SFX_HIT_HEAVY : SFX_HIT_LIGHT, 1.0f, 1.0f, sparkPos.x);
                if (attack.causesKnockdown || attack.moveType == MOVE_SUPER) {
                    AudioEngine::Instance().PlaySfx(SFX_CROWD_CHEER, 1.0f, 0.55f);
                }
            }
            break;
        }
    }
}

static void TestProjectiles(Fighter& owner, Fighter& opponent,
                            std::vector<SparkEffect>& sparks, float& cameraShake) {
    auto& projs = owner.GetProjectiles();
    auto hurtBoxes = opponent.GetHurtboxes();

    for (auto& p : projs) {
        if (!p.active) continue;

        if (p.type == PROJ_ATM_HACK && p.age < 10) continue;

        Rectangle pBox{ p.pos.x - p.width * 0.5f, p.pos.y - p.height * 0.5f, p.width, p.height };

        for (const auto& hb : hurtBoxes) {
            if (CollisionSystem::CheckAABB(pBox, hb)) {
                Rectangle inter = CollisionSystem::GetIntersection(pBox, hb);
                Vector2 sparkPos{ inter.x + inter.width * 0.5f, inter.y + inter.height * 0.5f };

                bool isBlocking = opponent.CheckCanBlock(ATTACK_PROJECTILE);
                if (isBlocking) {
                    int chip = std::max(1, p.damage / 5);
                    opponent.ApplyBlock(chip, 6, 13, 3.6f);
                    owner.AddSuperMeter(8);
                    sparks.push_back(SparkEffect{ sparkPos, true, 0, 6, true });
                    AudioEngine::Instance().PlaySfxPan(SFX_BLOCK, 1.0f, 1.0f, sparkPos.x);
                } else {
                    bool causesKd = (p.type == PROJ_WHEELCHAIR || p.type == PROJ_ATM_HACK || p.type == PROJ_SUPREME_WAVE ||
                                     p.type == PROJ_HAMMER_SICKLE || p.type == PROJ_SPY_RADIO || p.type == PROJ_PAPER_PLANE ||
                                     p.type == PROJ_SHOTGUN_BLAST || p.type == PROJ_HORMUZ_BLOCKADE);
                    opponent.ApplyHit(p.damage, (p.type == PROJ_HOPE_BASKETBALL) ? ATTACK_OVERHEAD : ATTACK_PROJECTILE,
                                      (p.type == PROJ_ATM_HACK || p.type == PROJ_SHOTGUN_BLAST) ? 14 : 10,
                                      (p.type == PROJ_HOPE_BASKETBALL) ? 52 : 20,
                                      (p.type == PROJ_HORMUZ_BLOCKADE) ? 5.6f : 4.0f, causesKd, false);
                    owner.AddComboHit();
                    owner.AddSuperMeter(16);
                    opponent.ResetCombo();

                    if (p.type == PROJ_EYE_LASER) {
                        opponent.TriggerStarBurst(65);
                    }

                    if (p.type == PROJ_SPY_RADIO) {
                        opponent.TriggerElectroShock(64);
                        AudioEngine::Instance().PlaySfxPan(SFX_ELEC_SPARK, 1.05f, 1.0f, sparkPos.x);
                    }

                    if (p.type == PROJ_PAPER_PLANE) {
                        opponent.TriggerJengaCollapse(68);
                        AudioEngine::Instance().PlaySfxPan(SFX_JENGA_CRASH, 1.0f, 1.0f, sparkPos.x);
                    }
                    sparks.push_back(SparkEffect{ sparkPos, false, 0, 8, true });
                    SfxId impactSfx = SFX_FIREBALL_IMPACT;
                    if (p.type == PROJ_ATM_HACK) impactSfx = SFX_ATM_JACKPOT;
                    else if (p.type == PROJ_PAPER_PLANE) impactSfx = SFX_JENGA_CRASH;
                    else if (p.type == PROJ_SPY_RADIO) impactSfx = SFX_ELEC_SPARK;
                    else if (p.type == PROJ_SHOTGUN_BLAST) impactSfx = SFX_HIT_HEAVY;
                    AudioEngine::Instance().PlaySfxPan(impactSfx, 1.0f, 1.0f, sparkPos.x);
                    cameraShake = (p.type == PROJ_ATM_HACK || p.type == PROJ_SHOTGUN_BLAST || p.type == PROJ_HORMUZ_BLOCKADE) ? 4.5f : 2.6f;
                }

                p.active = false;
                break;
            }
        }
    }
}

static void TestProjectileClash(Fighter& f1, Fighter& f2, std::vector<SparkEffect>& sparks) {
    auto& list1 = f1.GetProjectiles();
    auto& list2 = f2.GetProjectiles();

    for (auto& p1 : list1) {
        if (!p1.active || p1.type == PROJ_ATM_HACK) continue;
        Rectangle b1{ p1.pos.x - p1.width * 0.5f, p1.pos.y - p1.height * 0.5f, p1.width, p1.height };

        for (auto& p2 : list2) {
            if (!p2.active || p2.type == PROJ_ATM_HACK) continue;
            Rectangle b2{ p2.pos.x - p2.width * 0.5f, p2.pos.y - p2.height * 0.5f, p2.width, p2.height };

            if (CollisionSystem::CheckAABB(b1, b2)) {
                Rectangle inter = CollisionSystem::GetIntersection(b1, b2);
                Vector2 sparkPos{ inter.x + inter.width * 0.5f, inter.y + inter.height * 0.5f };
                sparks.push_back(SparkEffect{ sparkPos, false, 0, 8, true });
                AudioEngine::Instance().PlaySfxPan(SFX_FIREBALL_IMPACT, 1.2f, 1.0f, sparkPos.x);

                if (p1.type == PROJ_LEDGER || p1.type == PROJ_WHEELCHAIR || p1.type == PROJ_SUPREME_WAVE || p1.type == PROJ_HORMUZ_BLOCKADE || p1.type == PROJ_SHOTGUN_BLAST) {
                    p2.active = false;
                } else if (p2.type == PROJ_LEDGER || p2.type == PROJ_WHEELCHAIR || p2.type == PROJ_SUPREME_WAVE || p2.type == PROJ_HORMUZ_BLOCKADE || p2.type == PROJ_SHOTGUN_BLAST) {
                    p1.active = false;
                } else {
                    p1.active = false;
                    p2.active = false;
                }
                break;
            }
        }
    }
}

void CollisionSystem::ResolveCombat(Fighter& p1, Fighter& p2,
                                    std::vector<SparkEffect>& sparks, float& cameraShake) {
    TestStrike(p1, p2, sparks, cameraShake);
    TestStrike(p2, p1, sparks, cameraShake);
    TestProjectiles(p1, p2, sparks, cameraShake);
    TestProjectiles(p2, p1, sparks, cameraShake);
    TestProjectileClash(p1, p2, sparks);
}
