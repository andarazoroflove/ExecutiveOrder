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
        (f2.GetState() == FSTATE_SPECIAL && f2.GetCharacterId() == CHAR_POPULIST && f2.GetSpecialIndex() == 3)) {
        return;
    }

    Rectangle b1 = f1.GetPushbox();
    Rectangle b2 = f2.GetPushbox();

    if (CheckCollisionRecs(b1, b2)) {
        Rectangle overlap = GetCollisionRec(b1, b2);
        float overlapWidth = overlap.width;

        float p1x = f1.GetPosition().x;
        float p2x = f2.GetPosition().x;

        if (p1x < p2x) {
            float shift = overlapWidth * 0.5f;
            float newP1 = p1x - shift;
            float newP2 = p2x + shift;

            if (newP1 < STAGE_MIN_X) {
                float excess = STAGE_MIN_X - newP1;
                newP1 = STAGE_MIN_X;
                newP2 += excess;
            } else if (newP2 > STAGE_MAX_X) {
                float excess = newP2 - STAGE_MAX_X;
                newP2 = STAGE_MAX_X;
                newP1 -= excess;
            }

            f1.SetPositionX(newP1);
            f2.SetPositionX(newP2);
        } else {
            float shift = overlapWidth * 0.5f;
            float newP1 = p1x + shift;
            float newP2 = p2x - shift;

            if (newP2 < STAGE_MIN_X) {
                float excess = STAGE_MIN_X - newP2;
                newP2 = STAGE_MIN_X;
                newP1 += excess;
            } else if (newP1 > STAGE_MAX_X) {
                float excess = newP1 - STAGE_MAX_X;
                newP1 = STAGE_MAX_X;
                newP2 -= excess;
            }

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

            bool isBlocking = defender.CheckCanBlock(attack.type);

            attacker.SetHitConnected(true);

            if (isBlocking) {
                int chip = attack.isSpecial ? std::max(1, attack.damage / 5) : 0;
                defender.ApplyBlock(chip, 6, attack.blockstunFrames, attack.pushback);
                attacker.SetHitstop(6);
                cameraShake = 1.0f;

                sparks.push_back(SparkEffect{ sparkPos, true, 0, 6, true });
                AudioEngine::Instance().PlaySfx(SFX_BLOCK, 1.0f);
            } else {
                defender.ApplyHit(attack.damage, attack.type, attack.hitstopFrames, attack.hitstunFrames, attack.pushback, attack.causesKnockdown);
                attacker.SetHitstop(attack.hitstopFrames);
                cameraShake = attack.isHeavy ? 2.5f : 1.5f;

                sparks.push_back(SparkEffect{ sparkPos, false, 0, 6, true });
                AudioEngine::Instance().PlaySfx(attack.isHeavy ? SFX_HIT_HEAVY : SFX_HIT_LIGHT, 1.0f);
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

        Rectangle pBox{ p.pos.x - p.width * 0.5f, p.pos.y - p.height * 0.5f, p.width, p.height };

        for (const auto& hb : hurtBoxes) {
            if (CollisionSystem::CheckAABB(pBox, hb)) {
                Rectangle inter = CollisionSystem::GetIntersection(pBox, hb);
                Vector2 sparkPos{ inter.x + inter.width * 0.5f, inter.y + inter.height * 0.5f };

                bool isBlocking = opponent.CheckCanBlock(ATTACK_PROJECTILE);
                if (isBlocking) {
                    int chip = std::max(1, p.damage / 5);
                    opponent.ApplyBlock(chip, 6, 12, 3.5f);
                    sparks.push_back(SparkEffect{ sparkPos, true, 0, 6, true });
                    AudioEngine::Instance().PlaySfx(SFX_BLOCK, 1.0f);
                } else {
                    opponent.ApplyHit(p.damage, ATTACK_PROJECTILE, 10, 16, 3.8f, false);
                    sparks.push_back(SparkEffect{ sparkPos, false, 0, 6, true });
                    AudioEngine::Instance().PlaySfx(SFX_FIREBALL_IMPACT, 1.0f);
                    cameraShake = 2.0f;
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
        if (!p1.active) continue;
        Rectangle b1{ p1.pos.x - p1.width * 0.5f, p1.pos.y - p1.height * 0.5f, p1.width, p1.height };

        for (auto& p2 : list2) {
            if (!p2.active) continue;
            Rectangle b2{ p2.pos.x - p2.width * 0.5f, p2.pos.y - p2.height * 0.5f, p2.width, p2.height };

            if (CollisionSystem::CheckAABB(b1, b2)) {

                Rectangle inter = CollisionSystem::GetIntersection(b1, b2);
                Vector2 sparkPos{ inter.x + inter.width * 0.5f, inter.y + inter.height * 0.5f };
                sparks.push_back(SparkEffect{ sparkPos, false, 0, 6, true });
                AudioEngine::Instance().PlaySfx(SFX_FIREBALL_IMPACT, 1.2f);

                if (p1.type == 1) {
                    p2.active = false;
                } else if (p2.type == 1) {
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
