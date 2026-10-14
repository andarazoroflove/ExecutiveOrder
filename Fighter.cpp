#include "Fighter.h"
#include "AudioEngine.h"
#include <cmath>
#include <cstdlib>

Fighter::Fighter(int playerIndex, CharacterId charId, float startX)
    : m_playerIndex(playerIndex), m_charId(charId) {
    InitCharacterProfile();
    ResetRound(startX, playerIndex == 0);
}

void Fighter::InitCharacterProfile() {
    switch (m_charId) {
    case CHAR_COMMANDER:
        m_name = "POTUS";
        m_walkFwdSpeed = 1.95f;
        m_walkBackSpeed = 1.40f;
        m_color1 = Color{ 22, 40, 115, 255 };
        m_color2 = Color{ 230, 25, 25, 255 };
        m_colorSkin = Color{ 245, 145, 75, 255 };
        break;
    case CHAR_HIGH_BENCH:
        m_name = "High Bench";
        m_walkFwdSpeed = 1.40f;
        m_walkBackSpeed = 1.10f;
        m_color1 = Color{ 20, 20, 25, 255 };
        m_color2 = Color{ 180, 130, 50, 255 };
        m_colorSkin = Color{ 230, 200, 170, 255 };
        break;
    case CHAR_REDCAP:
        m_name = "Redcap";
        m_walkFwdSpeed = 1.70f;
        m_walkBackSpeed = 1.25f;
        m_color1 = Color{ 210, 25, 25, 255 };
        m_color2 = Color{ 40, 50, 90, 255 };
        m_colorSkin = Color{ 245, 185, 140, 255 };
        break;
    case CHAR_COASTAL_GOV:
        m_name = "Governator";
        m_walkFwdSpeed = 2.30f;
        m_walkBackSpeed = 1.70f;
        m_color1 = Color{ 30, 80, 110, 255 };
        m_color2 = Color{ 220, 200, 120, 255 };
        m_colorSkin = Color{ 235, 185, 150, 255 };
        break;
    case CHAR_TECH_VISIONARY:
        m_name = "TechBro";
        m_walkFwdSpeed = 2.05f;
        m_walkBackSpeed = 1.50f;
        m_color1 = Color{ 45, 70, 120, 255 };
        m_color2 = Color{ 210, 180, 50, 255 };
        m_colorSkin = Color{ 240, 205, 175, 255 };
        break;
    case CHAR_FILIBUSTER:
        m_name = "Filibuster";
        m_walkFwdSpeed = 1.30f;
        m_walkBackSpeed = 1.05f;
        m_color1 = Color{ 60, 60, 65, 255 };
        m_color2 = Color{ 140, 100, 60, 255 };
        m_colorSkin = Color{ 230, 195, 170, 255 };
        break;
    case CHAR_MASCOT:
        m_name = "Freagle";
        m_walkFwdSpeed = 2.15f;
        m_walkBackSpeed = 1.60f;
        m_color1 = Color{ 240, 120, 30, 255 };
        m_color2 = Color{ 230, 40, 160, 255 };
        m_colorSkin = Color{ 255, 230, 180, 255 };
        break;
    case CHAR_POPULIST:
        m_name = "Populist";
        m_walkFwdSpeed = 2.20f;
        m_walkBackSpeed = 1.65f;
        m_color1 = Color{ 235, 235, 240, 255 };
        m_color2 = Color{ 200, 20, 80, 255 };
        m_colorSkin = Color{ 215, 160, 120, 255 };
        break;
    case CHAR_SLEEPY_JOE:
        m_name = "Sleepy Joe";
        m_walkFwdSpeed = 1.95f;
        m_walkBackSpeed = 1.45f;
        m_color1 = Color{ 245, 245, 250, 255 };
        m_color2 = Color{ 25, 80, 210, 255 };
        m_colorSkin = Color{ 235, 195, 165, 255 };
        break;
    case CHAR_MTG:
        m_name = "Marjory Taylor Green";
        m_walkFwdSpeed = 1.85f;
        m_walkBackSpeed = 1.35f;
        m_color1 = Color{ 220, 30, 30, 255 };
        m_color2 = Color{ 35, 70, 150, 255 };
        m_colorSkin = Color{ 245, 195, 155, 255 };
        break;
    case CHAR_SUCKRATERY_RKF:
        m_name = "Suckratery RKF";
        m_walkFwdSpeed = 2.05f;
        m_walkBackSpeed = 1.50f;
        m_color1 = Color{ 240, 240, 245, 255 };
        m_color2 = Color{ 50, 55, 52, 255 };
        m_colorSkin = Color{ 235, 190, 160, 255 };
        break;
    case CHAR_WAR_PIG:
        m_name = "War Pig";
        m_walkFwdSpeed = 1.85f;
        m_walkBackSpeed = 1.35f;
        m_color1 = Color{ 70, 95, 55, 255 };
        m_color2 = Color{ 25, 25, 30, 255 };
        m_colorSkin = Color{ 240, 185, 145, 255 };
        break;
    case CHAR_SOCIALIST:
        m_name = "The Socialist";
        m_walkFwdSpeed = 2.10f;
        m_walkBackSpeed = 1.55f;
        m_color1 = Color{ 35, 65, 115, 255 };
        m_color2 = Color{ 50, 160, 70, 255 };
        m_colorSkin = Color{ 230, 185, 150, 255 };
        break;
    case CHAR_BILLARY:
        m_name = "Billary";
        m_walkFwdSpeed = 1.95f;
        m_walkBackSpeed = 1.45f;
        m_color1 = Color{ 25, 65, 140, 255 };
        m_color2 = Color{ 215, 25, 25, 255 };
        m_colorSkin = Color{ 235, 195, 165, 255 };
        break;
    case CHAR_PALM_BEACH_PETE:
        m_name = "Palm Beach Pete";
        m_walkFwdSpeed = 1.90f;
        m_walkBackSpeed = 1.40f;
        m_color1 = Color{ 240, 240, 245, 255 };
        m_color2 = Color{ 215, 185, 70, 255 };
        m_colorSkin = Color{ 235, 190, 160, 255 };
        break;
    case CHAR_THE_PUNDIT:
        m_name = "The Pundit";
        m_walkFwdSpeed = 1.95f;
        m_walkBackSpeed = 1.45f;
        m_color1 = Color{ 25, 42, 85, 255 };
        m_color2 = Color{ 215, 35, 45, 255 };
        m_colorSkin = Color{ 245, 200, 175, 255 };
        break;
    case CHAR_A_NEW_HOPE:
        m_name = "A New Hope";
        m_walkFwdSpeed = 2.15f;
        m_walkBackSpeed = 1.65f;
        m_color1 = Color{ 245, 245, 250, 255 };
        m_color2 = Color{ 210, 180, 130, 255 };
        m_colorSkin = Color{ 165, 110, 75, 255 };
        break;
    default:
        break;
    }
}

void Fighter::ResetRound(float startX, bool faceRight, bool wasKnockedOut) {
    m_pos = Vector2{ startX, GROUND_Y };
    m_vel = Vector2{ 0.0f, 0.0f };
    m_facingRight = faceRight;
    m_state = wasKnockedOut ? FSTATE_KNOCKDOWN_GROUND : FSTATE_IDLE;
    m_stateFrame = 0;
    m_hitstopFrames = 0;
    m_hitstunFrames = 0;
    m_blockstunFrames = 0;
    m_pushbackVel = 0.0f;
    m_hitConnected = false;
    m_armor = 0;
    m_invincibleFrames = 0;
    m_isSuperMove = false;
    m_triggerSuperFlash = false;
    m_justCanceled = false;
    m_justReversal = false;
    m_multiHitsRemaining = 0;
    m_comboCount = 0;
    m_comboResetTimer = 0;
    m_tauntTimer = 0;
    m_activeTaunt = "";
    m_health = 100;
    m_refillCount = 0;
    m_isHoldingBack = false;
    m_isHoldingDownBack = false;
    m_buffer = BufferedInput{};
    m_projectiles.clear();
    m_cosmeticPuffs.clear();
    m_quirkTimer = 0;
}

void Fighter::SpawnProjectile(int type, float vx, float vy, int damage) {
    Projectile p{};
    p.pos = Vector2{ m_pos.x + (m_facingRight ? 24.0f : -24.0f), m_pos.y - 36.0f };
    p.vel = Vector2{ m_facingRight ? vx : -vx, vy };
    p.width = (type == 1) ? 14.0f : ((type == 7) ? 44.0f : 20.0f);
    p.height = (type == 1) ? 55.0f : ((type == 7) ? 54.0f : 16.0f);
    if (type == 5) {

        p.pos.y = GROUND_Y - 10.0f;
        p.width = 28.0f;
        p.height = 20.0f;
    } else if (type == 7) {

        p.pos.y = GROUND_Y - 32.0f;
        p.pos.x = m_pos.x + (m_facingRight ? -30.0f : 30.0f);
    } else if (type == 9) {

        p.pos.x = m_pos.x + (m_facingRight ? 34.0f : -34.0f);
        p.pos.y = m_pos.y - 40.0f;
        p.width = 26.0f;
        p.height = 20.0f;
    } else if (type == 10) {

        p.pos.x = m_pos.x + (m_facingRight ? 16.0f : -16.0f);
        p.pos.y = m_pos.y - 74.0f;
        p.width = 22.0f;
        p.height = 22.0f;
    }
    p.damage = damage;
    p.lifetime = (type == 3) ? 180 : ((type == 7) ? 80 : 140);
    p.active = true;
    p.facingRight = m_facingRight;
    p.type = type;
    p.bounceY = GROUND_Y - 8.0f;
    m_projectiles.push_back(p);
}

void Fighter::ExecuteSpecial(SpecialMotionType motion, int buttonUsed, const Fighter& opponent) {
    bool wasInHitstunWakeup = (m_stateFrame <= 5);
    m_state = FSTATE_SPECIAL;
    m_stateFrame = 0;
    m_hitConnected = false;
    m_armor = 0;
    m_isSuperMove = false;
    m_multiHitsRemaining = 1;

    if (motion == MOTION_SUPER_OMNIBUS) {
        m_isSuperMove = true;
        m_triggerSuperFlash = true;
        m_invincibleFrames = 18;
        m_multiHitsRemaining = 5;
        m_specialIndex = 2;
        m_currentAttack = AttackData{ "TOTAL VETO FLURRY", 4, 28, 18, 6, ATTACK_MID, 14, 30, 18, 5.2f, true, true, 44.0f, -34.0f, 46.0f, 38.0f, true, MOVE_SPECIAL };
        m_vel = Vector2{ m_facingRight ? 5.2f : -5.2f, -2.5f };
        AudioEngine::Instance().PlaySfx(SFX_VOICE_TOTAL_VETO, 1.0f);
        AudioEngine::Instance().PlaySfx(SFX_UPPERCUT, 1.1f);
        return;
    }
    if (motion == MOTION_DP_P || motion == MOTION_DP_K || motion == MOTION_CHARGE_D_U_K) {
        m_invincibleFrames = 10;
        if (wasInHitstunWakeup) m_justReversal = true;
    }

    bool isHeavy = (buttonUsed & (BTN_HP | BTN_HK)) != 0;

    switch (m_charId) {
    case CHAR_COMMANDER:
        if (motion == MOTION_DP_P) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Executive Uppercut", 3, 14, 20, 20, ATTACK_MID, 14, 28, 16, 4.0f, true, isHeavy, 22.0f, -48.0f, 26.0f, 44.0f };
            m_vel = Vector2{ m_facingRight ? 1.8f : -1.8f, isHeavy ? -9.2f : -7.5f };
            AudioEngine::Instance().PlaySfx(SFX_VOICE_UPPERCUT, 1.0f);
            AudioEngine::Instance().PlaySfx(SFX_UPPERCUT, 1.0f);
        } else if (motion == MOTION_QCB_K) {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Motorcade Tackle", 6, 22, 12, 16, ATTACK_LOW, 10, 22, 14, 4.5f, true, isHeavy, 30.0f, -20.0f, 32.0f, 22.0f };
            m_vel = Vector2{ m_facingRight ? 3.6f : -3.6f, 0.0f };
            AudioEngine::Instance().PlaySfx(SFX_SLIDE, 0.9f);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Teleprompter Spark", 12, 6, 18, 14, ATTACK_PROJECTILE, 10, 18, 12, 3.5f, false, isHeavy, 35.0f, -32.0f, 24.0f, 20.0f };
            AudioEngine::Instance().PlaySfx(SFX_VOICE_HADOUKEN, 1.0f);
            AudioEngine::Instance().PlaySfx(SFX_FIREBALL_LAUNCH, 1.0f);
            SpawnProjectile(0, isHeavy ? 3.8f : 2.2f, 0.0f, 14);
        }
        break;

    case CHAR_HIGH_BENCH: {
        HighBenchStage stage = GetHighBenchStage();
        if (stage == HB_STAGE_THOMAS) {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Coke Can Spray", 8, 14, 16, 18, ATTACK_PROJECTILE, 10, 20, 12, 3.8f, false, isHeavy, 48.0f, -30.0f, 42.0f, 24.0f };
            SpawnProjectile(4, isHeavy ? 4.6f : 3.2f, 0.0f, 18);
            AudioEngine::Instance().PlaySfx(SFX_COKE_POP, 1.0f);
        } else if (stage == HB_STAGE_ROBERTS) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Gavel Stomp", 10, 12, 18, 22, ATTACK_OVERHEAD, 14, 26, 16, 4.8f, true, true, 42.0f, -16.0f, 44.0f, 32.0f };
            SpawnProjectile(5, isHeavy ? 4.2f : 2.8f, 0.0f, 20);
            AudioEngine::Instance().PlaySfx(SFX_GAVEL_STOMP, 1.0f);
        } else {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Fiery Dissent", 6, 16, 16, 24, ATTACK_PROJECTILE, 14, 28, 16, 5.0f, true, true, 50.0f, -32.0f, 46.0f, 28.0f };
            SpawnProjectile(6, isHeavy ? 5.0f : 3.6f, 0.0f, 24);
            AudioEngine::Instance().PlaySfx(SFX_DISSENT_FLAME, 1.0f);
        }
        break;
    }

    case CHAR_REDCAP:
        if (motion == MOTION_COMMAND_GRAB || motion == MOTION_QCB_P) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Hostile Takeover", 4, 6, 20, 26, ATTACK_THROW, 14, 30, 0, 5.0f, true, true, 26.0f, -28.0f, 28.0f, 32.0f };
            AudioEngine::Instance().PlaySfx(SFX_HIT_HEAVY, 0.8f);
        } else if (motion == MOTION_CHARGE_B_F_P) {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "The Big Deal", 10, 14, 16, 20, ATTACK_MID, 14, 26, 16, 5.5f, true, true, 34.0f, -36.0f, 32.0f, 26.0f };
            m_armor = 1;
            m_vel = Vector2{ m_facingRight ? 3.8f : -3.8f, 0.0f };
            AudioEngine::Instance().PlaySfx(SFX_WHOOSH_HEAVY, 0.85f);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Rally Megaphone", 10, 12, 16, 15, ATTACK_MID, 12, 24, 20, 4.5f, false, isHeavy, 42.0f, -38.0f, 38.0f, 30.0f };
            AudioEngine::Instance().PlaySfx(SFX_MEGAPHONE, 1.0f);
        }
        break;

    case CHAR_COASTAL_GOV:
        if (motion == MOTION_CHARGE_D_U_K) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Veto Flip", 4, 12, 18, 18, ATTACK_MID, 14, 26, 14, 3.8f, true, isHeavy, 26.0f, -44.0f, 28.0f, 40.0f };
            m_vel = Vector2{ m_facingRight ? 1.4f : -1.4f, -9.0f };
            AudioEngine::Instance().PlaySfx(SFX_UPPERCUT, 1.2f);
        } else if (motion == MOTION_QCB_P) {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Press Briefing", 10, 8, 14, 8, ATTACK_MID, 8, 14, 8, 2.0f, false, false, 30.0f, -28.0f, 20.0f, 20.0f };
            SpawnProjectile(3, 0.0f, 0.0f, 10);
            AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM, 1.0f);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Smooth-Talk Slide", 6, 18, 12, 14, ATTACK_LOW, 8, 20, 12, 4.0f, true, isHeavy, 36.0f, -12.0f, 36.0f, 18.0f };
            m_vel = Vector2{ m_facingRight ? 4.6f : -4.6f, 0.0f };
            AudioEngine::Instance().PlaySfx(SFX_SLIDE, 1.1f);
        }
        break;

    case CHAR_TECH_VISIONARY:
        if (motion == MOTION_QCB_P) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Disruption Field", 8, 6, 14, 12, ATTACK_LOW, 8, 18, 10, 2.0f, false, false, 28.0f, -14.0f, 20.0f, 16.0f };
            SpawnProjectile(3, 0.0f, 0.0f, 14);
            AudioEngine::Instance().PlaySfx(SFX_ELECTRO_GLITCH, 0.9f);
        } else if (motion == MOTION_QCB_K) {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "System Reboot", 14, 4, 10, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
            AudioEngine::Instance().PlaySfx(SFX_ELECTRO_GLITCH, 1.5f);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Drone Delivery", 12, 6, 16, 16, ATTACK_OVERHEAD, 10, 20, 12, 3.0f, false, isHeavy, 30.0f, -40.0f, 24.0f, 20.0f };
            SpawnProjectile(2, isHeavy ? 2.8f : 1.8f, -4.5f, 16);
            AudioEngine::Instance().PlaySfx(SFX_ELECTRO_GLITCH, 1.4f);
        }
        break;

    case CHAR_FILIBUSTER:
        if (motion == MOTION_CHARGE_D_U_K) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Point of Order", 5, 12, 16, 18, ATTACK_MID, 12, 24, 14, 3.5f, true, isHeavy, 24.0f, -50.0f, 26.0f, 42.0f };
            m_vel = Vector2{ m_facingRight ? 1.0f : -1.0f, -7.8f };
            AudioEngine::Instance().PlaySfx(SFX_UPPERCUT, 0.9f);
        } else if (motion == MOTION_QCB_P) {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "C-SPAN Slumber", 8, 14, 16, 16, ATTACK_LOW, 12, 22, 14, 4.0f, true, true, 30.0f, -12.0f, 32.0f, 22.0f };
            AudioEngine::Instance().PlaySfx(SFX_GAVEL_SLAM, 0.9f);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Endless Ledger", 12, 8, 16, 14, ATTACK_PROJECTILE, 10, 18, 12, 3.0f, false, isHeavy, 35.0f, -30.0f, 16.0f, 55.0f };
            SpawnProjectile(1, isHeavy ? 2.6f : 1.5f, 0.0f, 14);
            AudioEngine::Instance().PlaySfx(SFX_WHOOSH_HEAVY, 1.0f);
        }
        break;

    case CHAR_MASCOT:
        if (motion == MOTION_QCF_P) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Glitter Bomb", 10, 6, 16, 15, ATTACK_MID, 10, 18, 12, 3.2f, false, isHeavy, 26.0f, -30.0f, 20.0f, 20.0f };
            SpawnProjectile(2, isHeavy ? 3.0f : 1.8f, -4.0f, 15);
            AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM, 1.2f);
        } else if (motion == MOTION_DP_K) {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Paw Pogo", 4, 18, 12, 16, ATTACK_OVERHEAD, 10, 20, 12, 3.5f, true, true, 20.0f, -8.0f, 24.0f, 26.0f };
            m_vel = Vector2{ m_facingRight ? 2.0f : -2.0f, -5.0f };
            AudioEngine::Instance().PlaySfx(SFX_WHOOSH_LIGHT, 1.4f);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Furry Cannonball", 6, 24, 14, 18, ATTACK_MID, 12, 24, 14, 4.5f, true, isHeavy, 28.0f, -24.0f, 28.0f, 28.0f };
            m_vel = Vector2{ m_facingRight ? 5.2f : -5.2f, 0.0f };
            AudioEngine::Instance().PlaySfx(SFX_WHOOSH_HEAVY, 1.3f);
        }
        break;

    case CHAR_POPULIST:
        if (motion == MOTION_DP_K) {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Viral Clapback", 4, 12, 16, 19, ATTACK_MID, 14, 26, 14, 4.2f, true, isHeavy, 26.0f, -38.0f, 28.0f, 32.0f };
            m_vel = Vector2{ m_facingRight ? 2.8f : -2.8f, -7.5f };
            AudioEngine::Instance().PlaySfx(SFX_UPPERCUT, 1.15f);
        } else if (motion == MOTION_QCB_K) {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Livestream Dash", 2, 16, 8, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
            m_vel = Vector2{ m_facingRight ? 5.5f : -5.5f, 0.0f };
            AudioEngine::Instance().PlaySfx(SFX_WHOOSH_LIGHT, 1.2f);
        } else {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Selfie Flash", 8, 8, 16, 12, ATTACK_MID, 14, 35, 12, 2.5f, false, false, 32.0f, -36.0f, 30.0f, 28.0f };
            AudioEngine::Instance().PlaySfx(SFX_ELECTRO_GLITCH, 1.3f);
        }
        break;

    case CHAR_SLEEPY_JOE:
        if (motion == MOTION_DP_P || motion == MOTION_QCF_K) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Secret Service", 6, 14, 16, 22, ATTACK_MID, 14, 28, 16, 6.0f, true, true, 48.0f, -28.0f, 44.0f, 32.0f };
            SpawnProjectile(7, isHeavy ? 9.5f : 7.5f, 0.0f, 22);
            AudioEngine::Instance().PlaySfx(SFX_SECRET_SERVICE, 1.0f);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Executive Nap", 4, 48, 14, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
            m_health = std::min(100, m_health + 6);
        }
        break;

    case CHAR_MTG:

        m_specialIndex = 1;
        m_currentAttack = AttackData{ "Cut Off Nose", 4, 32, 12, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
        m_facingRight = !m_facingRight;
        m_health = std::max(1, m_health - 15);
        AudioEngine::Instance().PlaySfx(SFX_NOSE_CUT, 1.0f);
        break;

    case CHAR_SUCKRATERY_RKF:
        if (motion == MOTION_CHARGE_B_F_P || motion == MOTION_DP_P) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Tonic Toss", 10, 8, 14, 16, ATTACK_PROJECTILE, 8, 16, 10, 3.0f, false, isHeavy, 35.0f, -28.0f, 20.0f, 20.0f };
            SpawnProjectile(2, isHeavy ? 3.0f : 2.0f, -3.5f, 16);
            AudioEngine::Instance().PlaySfx(SFX_HIT_LIGHT, 0.9f);
        } else {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Brain Worm Strike", 8, 14, 18, 22, ATTACK_MID, 12, 24, 14, 4.5f, true, true, 65.0f, -32.0f, 70.0f, 28.0f };
            AudioEngine::Instance().PlaySfx(SFX_WORM_HISS, 1.2f);
        }
        break;

    case CHAR_WAR_PIG:
        if (motion == MOTION_QCB_P || motion == MOTION_COMMAND_GRAB) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Ground Slam", 12, 12, 20, 26, ATTACK_OVERHEAD, 16, 30, 18, 5.5f, true, true, 35.0f, -15.0f, 50.0f, 30.0f };
            AudioEngine::Instance().PlaySfx(SFX_GAVEL_SLAM, 1.2f);
        } else if (motion == MOTION_QCF_K || motion == MOTION_DP_P) {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "T-Refill", 10, 25, 15, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
            AudioEngine::Instance().PlaySfx(SFX_INJECTION, 1.0f);
            AudioEngine::Instance().PlaySfx(SFX_WAR_PIG_ROAR, 1.0f);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Hammer Punch", 10, 10, 16, 24, ATTACK_MID, 14, 26, 16, 5.0f, true, true, 42.0f, -32.0f, 38.0f, 32.0f };
            AudioEngine::Instance().PlaySfx(SFX_WHOOSH_HEAVY, 1.1f);
            AudioEngine::Instance().PlaySfx(SFX_HIT_HEAVY, 1.0f);
        }
        break;

    case CHAR_SOCIALIST:
        if (motion == MOTION_QCB_P || motion == MOTION_QCB_K) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Cash Rain", 5, 16, 12, 16, ATTACK_MID, 12, 24, 12, 3.8f, false, isHeavy, 40.0f, -30.0f, 38.0f, 26.0f };
            AudioEngine::Instance().PlaySfx(SFX_COIN, 1.2f);
        } else if (motion == MOTION_DP_P || motion == MOTION_QCF_K) {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Public Buffet", 8, 20, 16, 20, ATTACK_OVERHEAD, 14, 26, 16, 4.8f, true, true, 46.0f, -28.0f, 44.0f, 30.0f };
            AudioEngine::Instance().PlaySfx(SFX_GAVEL_SLAM, 1.1f);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Grocery Swing", 6, 12, 14, 18, ATTACK_MID, 10, 22, 14, 4.2f, true, isHeavy, 44.0f, -32.0f, 40.0f, 28.0f };
            AudioEngine::Instance().PlaySfx(SFX_WHOOSH_HEAVY, 1.1f);
            AudioEngine::Instance().PlaySfx(SFX_HIT_HEAVY, 1.0f);
        }
        break;

    case CHAR_BILLARY:
        if (m_facingRight) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Saxophone Solo", 6, 20, 18, 24, ATTACK_PROJECTILE, 14, 28, 16, 5.0f, true, true, 78.0f, -34.0f, 85.0f, 34.0f };
            SpawnProjectile(8, isHeavy ? 5.2f : 4.0f, 0.0f, 22);
            AudioEngine::Instance().PlaySfx(SFX_SAXOPHONE, 1.1f);
        } else {

            if (motion == MOTION_CHARGE_B_F_P || motion == MOTION_QCF_K) {

                m_specialIndex = 1;
                m_currentAttack = AttackData{ "Gavel Slam", 4, 8, 10, 14, ATTACK_MID, 8, 16, 10, 3.0f, false, false, 36.0f, -32.0f, 30.0f, 18.0f };
                AudioEngine::Instance().PlaySfx(SFX_WHOOSH_LIGHT, 1.1f);
            } else {

                m_specialIndex = 3;
                m_currentAttack = AttackData{ "Typing Emails", 6, 20, 18, 24, ATTACK_PROJECTILE, 14, 28, 16, 5.0f, true, true, 78.0f, -34.0f, 85.0f, 34.0f };
                SpawnProjectile(0, isHeavy ? 4.6f : 3.4f, 0.0f, 20);
                AudioEngine::Instance().PlaySfx(SFX_EMAIL_ZAP, 1.0f);
            }
        }
        break;

    case CHAR_PALM_BEACH_PETE:
        if (motion == MOTION_DP_P || motion == MOTION_QCF_K) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Island Slap", 5, 10, 14, 18, ATTACK_MID, 10, 20, 12, 3.8f, false, true, 38.0f, -32.0f, 34.0f, 22.0f };
            AudioEngine::Instance().PlaySfx(SFX_COIN, 1.1f);
        } else if (motion == MOTION_QCB_P || motion == MOTION_DP_K) {

            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Adrenochrome Sip", 8, 25, 12, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
            AudioEngine::Instance().PlaySfx(SFX_ADRENOCHROME, 1.0f);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Gold Cane Strike", 6, 12, 14, 22, ATTACK_MID, 12, 24, 14, 4.2f, true, true, 44.0f, -34.0f, 40.0f, 26.0f };
            AudioEngine::Instance().PlaySfx(SFX_WHOOSH_HEAVY, 1.0f);
            AudioEngine::Instance().PlaySfx(SFX_HIT_HEAVY, 1.0f);
        }
        break;

    case CHAR_THE_PUNDIT:
        if (motion == MOTION_DP_P || motion == MOTION_DP_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Karl's Polling Slam", 5, 12, 16, 20, ATTACK_OVERHEAD, 12, 26, 14, 4.4f, true, true, 42.0f, -36.0f, 40.0f, 30.0f };
            m_vel = Vector2{ m_facingRight ? 2.2f : -2.2f, -6.5f };
            AudioEngine::Instance().PlaySfx(SFX_GAVEL_SLAM, 1.15f);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "DECISION 2028 MAP", 10, 20, 16, 18, ATTACK_PROJECTILE, 12, 24, 14, 4.2f, true, true, 0.0f, 0.0f, 0.0f, 0.0f };
            SpawnProjectile(9, isHeavy ? 4.8f : 3.8f, -0.25f, 10);
            AudioEngine::Instance().PlaySfx(SFX_ELECTRO_GLITCH, 1.3f);
            AudioEngine::Instance().PlaySfx(SFX_FIREBALL_LAUNCH, 1.05f);
        }
        break;

    case CHAR_A_NEW_HOPE:
        if (motion == MOTION_DP_P || motion == MOTION_DP_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Windy City Dunk", 5, 12, 16, 20, ATTACK_OVERHEAD, 12, 28, 14, 4.4f, true, true, 40.0f, -42.0f, 38.0f, 32.0f };
            m_vel = Vector2{ m_facingRight ? 2.6f : -2.6f, -7.2f };
            AudioEngine::Instance().PlaySfx(SFX_UPPERCUT, 1.1f);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "HOPE JUMP SHOT", 8, 14, 16, 18, ATTACK_PROJECTILE, 12, 48, 14, 3.8f, false, true, 0.0f, 0.0f, 0.0f, 0.0f };
            float startX = m_pos.x + (m_facingRight ? 16.0f : -16.0f);
            float startY = m_pos.y - 74.0f;
            float targetX = opponent.GetPosition().x;
            float targetY = opponent.GetPosition().y - 52.0f;
            float dx = std::abs(targetX - startX);
            float flightFrames = std::clamp(dx / 4.2f, 22.0f, 36.0f);
            float vx = std::clamp(dx / flightFrames, 2.2f, 6.2f);
            float vy = ((targetY - startY) - 0.5f * 0.24f * flightFrames * flightFrames) / flightFrames;
            SpawnProjectile(10, vx, vy, 16);
            m_activeTaunt = "How's that for hope and change?";
            m_tauntTimer = 95;
            AudioEngine::Instance().PlaySfx(SFX_WHOOSH_HEAVY, 1.15f);
            AudioEngine::Instance().PlaySfx(SFX_FIREBALL_LAUNCH, 0.95f);
        }
        break;
    }

    m_currentAttack.isSpecial = true;
    m_currentAttack.moveType = MOVE_SPECIAL;
}

void Fighter::StartNormalAttack(AttackMoveType moveType, const char* name, int startup, int active, int recovery,
                               int damage, AttackType type, int hitstop, int hitstun,
                               int blockstun, float push, bool heavy, float reachX, float reachY, float bw, float bh) {
    m_state = FSTATE_ATTACK;
    m_stateFrame = 0;
    m_hitConnected = false;
    m_currentAttack = AttackData{ name, startup, active, recovery, damage, type, hitstop, hitstun, blockstun, push, false, heavy, reachX, reachY, bw, bh };
    m_currentAttack.isSpecial = false;
    m_currentAttack.moveType = moveType;
    AudioEngine::Instance().PlaySfx(heavy ? SFX_WHOOSH_HEAVY : SFX_WHOOSH_LIGHT, 1.0f);
}

void Fighter::TriggerSignatureSpecial(const Fighter& opponent) {
    if (m_state == FSTATE_HITSTUN || m_state == FSTATE_BLOCKSTUN ||
        m_state == FSTATE_KNOCKDOWN_AIR || m_state == FSTATE_KNOCKDOWN_GROUND ||
        m_state == FSTATE_WAKEUP || m_state == FSTATE_DEFEAT || m_state == FSTATE_VICTORY) {
        return;
    }

    switch (m_charId) {
    case CHAR_COMMANDER:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    case CHAR_HIGH_BENCH:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    case CHAR_REDCAP:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    case CHAR_COASTAL_GOV:
        ExecuteSpecial(MOTION_QCF_K, BTN_HK, opponent);
        break;
    case CHAR_TECH_VISIONARY:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    case CHAR_FILIBUSTER:
        ExecuteSpecial(MOTION_CHARGE_B_F_P, BTN_HP, opponent);
        break;
    case CHAR_MASCOT:
        ExecuteSpecial(MOTION_CHARGE_B_F_K, BTN_HK, opponent);
        break;
    case CHAR_POPULIST:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    case CHAR_SLEEPY_JOE:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    case CHAR_MTG:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    case CHAR_SUCKRATERY_RKF:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    case CHAR_WAR_PIG:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    case CHAR_SOCIALIST:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    case CHAR_BILLARY:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    case CHAR_PALM_BEACH_PETE:
    case CHAR_THE_PUNDIT:
    case CHAR_A_NEW_HOPE:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    default:
        ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent);
        break;
    }
}

void Fighter::TriggerTaunt() {
    if (m_state == FSTATE_HITSTUN || m_state == FSTATE_BLOCKSTUN ||
        m_state == FSTATE_KNOCKDOWN_AIR || m_state == FSTATE_KNOCKDOWN_GROUND ||
        m_state == FSTATE_WAKEUP || m_state == FSTATE_DEFEAT || m_state == FSTATE_VICTORY ||
        m_state == FSTATE_ATTACK || m_state == FSTATE_SPECIAL) {
        return;
    }
    m_state = FSTATE_TAUNT;
    m_stateFrame = 0;
    m_hitConnected = false;
    m_tauntTimer = 90;
    m_activeTaunt = GetTauntQuote();

    switch (m_charId) {
    case CHAR_COMMANDER:
        AudioEngine::Instance().PlaySfx(SFX_FART, 1.0f);
        break;
    case CHAR_HIGH_BENCH:
        AudioEngine::Instance().PlaySfx(SFX_VOICE_OBJECTION, 1.0f);
        break;
    case CHAR_SLEEPY_JOE:
        AudioEngine::Instance().PlaySfx(SFX_VOICE_BOO_YAA, 1.0f);
        break;
    case CHAR_MTG:
        AudioEngine::Instance().PlaySfx(SFX_MEGAPHONE, 1.1f);
        break;
    case CHAR_WAR_PIG:
        AudioEngine::Instance().PlaySfx(SFX_WAR_PIG_ROAR, 1.1f);
        break;
    case CHAR_SUCKRATERY_RKF:
        AudioEngine::Instance().PlaySfx(SFX_WORM_HISS, 1.0f);
        break;
    case CHAR_TECH_VISIONARY:
        AudioEngine::Instance().PlaySfx(SFX_ELECTRO_GLITCH, 1.2f);
        break;
    case CHAR_BILLARY:
        AudioEngine::Instance().PlaySfx(SFX_EMAIL_ZAP, 1.0f);
        break;
    case CHAR_PALM_BEACH_PETE:
        AudioEngine::Instance().PlaySfx(SFX_ADRENOCHROME, 1.3f);
        break;
    default:
        AudioEngine::Instance().PlaySfx(SFX_WHOOSH_LIGHT, 1.0f);
        break;
    }
}

bool Fighter::HasSpeechTaunt() const {
    switch (m_charId) {
    case CHAR_HIGH_BENCH:
    case CHAR_COASTAL_GOV:
    case CHAR_POPULIST:
    case CHAR_WAR_PIG:
    case CHAR_BILLARY:
    case CHAR_SLEEPY_JOE:
    case CHAR_MTG:
    case CHAR_THE_PUNDIT:
    case CHAR_A_NEW_HOPE:
        return true;
    default:
        return false;
    }
}

const char* Fighter::GetTauntQuote() const {
    switch (m_charId) {
    case CHAR_HIGH_BENCH: {
        switch (GetHighBenchStage()) {
        case HB_STAGE_THOMAS:   return "is this a hair in my cola?";
        case HB_STAGE_ROBERTS:  return "Jesus says NO MORE ROE!";
        case HB_STAGE_SOTOMAYOR:return "I DISSENT!";
        }
    }
    case CHAR_COASTAL_GOV:      return "TOO EASY!";
    case CHAR_POPULIST:         return "SMILE! :)";
    case CHAR_WAR_PIG:          return "BRING IT!";
    case CHAR_BILLARY:          return m_facingRight ? "OH YEAH!" : "DELETED!";
    case CHAR_THE_PUNDIT:       return (rand() % 2 == 0) ? "DECISION 2028: 270!" : "CHECK THE WHITEBOARD!";
    case CHAR_A_NEW_HOPE:       return "How's that for hope and change?";
    case CHAR_SLEEPY_JOE:       return (rand() % 2 == 0) ? "this is a big fkin deal." : "Hunter's Laptop!";
    case CHAR_MTG: {
        static int mtgTauntCycle = 0;
        int cur = mtgTauntCycle;
        mtgTauntCycle = (mtgTauntCycle + 1) % 3;
        if (cur == 0) return "SPACE LASERS!";
        if (cur == 1) return "TRUST THE PLAN!";
        return "WWG1WGA!";
    }
    default:                    return "";
    }
}

bool Fighter::TryExecuteBufferedMove(const Fighter& opponent) {
    if (m_buffer.timer <= 0) return false;

    if (m_buffer.motion != MOTION_NONE) {
        SpecialMotionType mot = m_buffer.motion;
        int btn = m_buffer.motionButton;
        m_buffer = BufferedInput{};
        ExecuteSpecial(mot, btn, opponent);
        return true;
    }

    if (m_buffer.button != 0) {
        int btn = m_buffer.button;
        bool crouch = m_buffer.isCrouch;
        m_buffer = BufferedInput{};

        if (btn == (BTN_LP | BTN_LK)) {
            StartNormalAttack(MOVE_HEAVY_PUNCH, "Bipartisan Toss", 3, 4, 14, 20, ATTACK_THROW, 14, 28, 0, 5.2f, true, 24.0f, -30.0f, 26.0f, 30.0f);
            m_currentAttack.causesKnockdown = true;
            return true;
        } else if (btn == BTN_LP) {
            const char* name = (m_charId == CHAR_BILLARY) ?
                (m_facingRight ? "Presidential Point" : "Gavel Jab") :
                (crouch ? "Crouch Jab" : "Stand Jab");
            if (crouch) StartNormalAttack(MOVE_LIGHT_PUNCH, name, 3, 3, 4, 6, ATTACK_MID, 6, 11, 8, 2.0f, false, 28.0f, -14.0f, 22.0f, 14.0f);
            else StartNormalAttack(MOVE_LIGHT_PUNCH, name, 3, 3, 5, 7, ATTACK_MID, 6, 12, 8, 2.2f, false, 32.0f, -34.0f, 24.0f, 14.0f);
            return true;
        } else if (btn == BTN_HP) {
            const char* name = (m_charId == CHAR_BILLARY) ?
                (m_facingRight ? "Executive Haymaker" : "Gavel Smash") :
                (crouch ? "Crouch Haymaker" : "Stand Haymaker");
            if (crouch) StartNormalAttack(MOVE_HEAVY_PUNCH, name, 5, 4, 12, 16, ATTACK_MID, 11, 22, 14, 3.8f, true, 30.0f, -24.0f, 28.0f, 24.0f);
            else StartNormalAttack(MOVE_HEAVY_PUNCH, name, 5, 4, 12, 18, ATTACK_OVERHEAD, 11, 24, 16, 4.2f, true, 36.0f, -36.0f, 32.0f, 20.0f);
            return true;
        } else if (btn == BTN_LK) {
            const char* name = (m_charId == CHAR_BILLARY) ?
                (m_facingRight ? "Cabinet Kick" : "Low Subpoena") :
                (crouch ? "Crouch Light Kick" : "Stand Light Kick");
            if (crouch) StartNormalAttack(MOVE_LIGHT_KICK, name, 3, 3, 5, 7, ATTACK_LOW, 6, 12, 8, 2.0f, false, 32.0f, -8.0f, 26.0f, 14.0f);
            else StartNormalAttack(MOVE_LIGHT_KICK, name, 4, 3, 5, 8, ATTACK_MID, 6, 13, 9, 2.4f, false, 34.0f, -22.0f, 26.0f, 14.0f);
            return true;
        } else if (btn == BTN_HK) {
            const char* name = (m_charId == CHAR_BILLARY) ?
                (m_facingRight ? "Arkansas Roundhouse" : "Executive Veto") :
                (crouch ? "Crouch Strong Kick Sweep" : "Stand Strong Kick");
            if (crouch) {
                StartNormalAttack(MOVE_HEAVY_KICK, name, 6, 4, 14, 17, ATTACK_LOW, 11, 24, 14, 4.5f, true, 38.0f, -8.0f, 36.0f, 16.0f);
                m_currentAttack.causesKnockdown = true;
            } else {
                StartNormalAttack(MOVE_HEAVY_KICK, name, 6, 4, 13, 18, ATTACK_MID, 11, 25, 15, 4.4f, true, 36.0f, -38.0f, 30.0f, 22.0f);
            }
            return true;
        }
    }
    return false;
}

bool Fighter::CheckCanBlock(AttackType type) const {
    if (m_state == FSTATE_HITSTUN || m_state == FSTATE_KNOCKDOWN_AIR ||
        m_state == FSTATE_KNOCKDOWN_GROUND || m_state == FSTATE_WAKEUP ||
        m_state == FSTATE_ATTACK || m_state == FSTATE_SPECIAL ||
        m_state == FSTATE_VICTORY || m_state == FSTATE_DEFEAT) {
        return false;
    }

    if (type == ATTACK_THROW) {
        return false;
    }

    if (IsAirborne()) {
        return false;
    }

    if (m_state == FSTATE_BLOCK_HIGH || m_state == FSTATE_BLOCK_LOW || m_state == FSTATE_BLOCKSTUN) {
        if (type == ATTACK_LOW) return m_isHoldingDownBack || (m_state == FSTATE_BLOCK_LOW);
        if (type == ATTACK_OVERHEAD) return m_isHoldingBack || (m_state == FSTATE_BLOCK_HIGH);
        return true;
    }

    if (type == ATTACK_LOW) {
        return m_isHoldingDownBack;
    }
    if (type == ATTACK_OVERHEAD) {
        return m_isHoldingBack;
    }

    return m_isHoldingBack || m_isHoldingDownBack;
}

void Fighter::ApplyHit(int damage, AttackType type, int hitstop, int hitstun, float pushback, bool knockdown) {
    if (m_invincibleFrames > 0) {
        return;
    }

    if (m_charId == CHAR_SLEEPY_JOE && m_state == FSTATE_SPECIAL && m_specialIndex == 1) {
        AudioEngine::Instance().PlaySfx(SFX_SECRET_SERVICE, 1.0f);
        SpawnProjectile(7, m_facingRight ? 9.5f : -9.5f, 0.0f, 25);
        m_state = FSTATE_WAKEUP;
        m_stateFrame = 0;
        return;
    }

    if (m_armor > 0) {
        m_armor--;
        AudioEngine::Instance().PlaySfx(SFX_BLOCK, 0.8f);
        return;
    }

    m_health = std::max(0, m_health - damage);
    m_hitstopFrames = hitstop;
    m_pushbackVel = m_facingRight ? -pushback : pushback;

    if (m_charId == CHAR_WAR_PIG && m_refillCount < 2 && m_health <= 10) {
        m_refillCount++;
        m_health = std::min(100, m_health + 80);
        m_state = FSTATE_SPECIAL;
        m_stateFrame = 0;
        m_specialIndex = 3;
        m_currentAttack = AttackData{ "T-Refill Surge", 8, 30, 12, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
        m_currentAttack.isSpecial = true;
        m_armor = 3;
        m_hitstopFrames = 18;
        m_pushbackVel = 0.0f;
        AudioEngine::Instance().PlaySfx(SFX_INJECTION, 1.0f);
        AudioEngine::Instance().PlaySfx(SFX_WAR_PIG_ROAR, 1.0f);
        return;
    }

    if (m_charId == CHAR_PALM_BEACH_PETE && m_refillCount < 4 && m_health <= 10) {
        m_refillCount++;
        m_health = std::min(100, m_health + 80);
        m_state = FSTATE_SPECIAL;
        m_stateFrame = 0;
        m_specialIndex = 3;
        m_currentAttack = AttackData{ "Adrenochrome Surge", 8, 30, 12, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
        m_currentAttack.isSpecial = true;
        m_armor = 3;
        m_hitstopFrames = 18;
        m_pushbackVel = 0.0f;
        AudioEngine::Instance().PlaySfx(SFX_ADRENOCHROME, 1.0f);
        return;
    }

    if (m_health <= 0) {
        m_state = FSTATE_KNOCKDOWN_AIR;
        m_stateFrame = 0;
        m_tauntTimer = 0;
        m_activeTaunt = "";
        m_cosmeticPuffs.clear();
        m_vel = Vector2{ m_facingRight ? -3.0f : 3.0f, -7.0f };
        return;
    }

    if (knockdown || IsAirborne()) {
        m_state = FSTATE_KNOCKDOWN_AIR;
        m_stateFrame = 0;
        m_vel = Vector2{ m_facingRight ? -2.8f : 2.8f, -6.5f };
    } else {
        m_state = FSTATE_HITSTUN;
        m_stateFrame = 0;
        m_hitstunFrames = hitstun;
    }
}

void Fighter::ApplyBlock(int chipDamage, int blockstop, int blockstun, float pushback) {

    if (chipDamage > 0) {
        m_health = std::max(1, m_health - chipDamage);
    }
    m_hitstopFrames = blockstop;
    m_blockstunFrames = blockstun;
    m_pushbackVel = m_facingRight ? -pushback * 1.3f : pushback * 1.3f;
    m_state = m_isHoldingDownBack ? FSTATE_BLOCK_LOW : FSTATE_BLOCK_HIGH;
    m_stateFrame = 0;
}

void Fighter::UpdatePhysics(const InputManager& input, const Fighter& opponent) {

    if (m_buffer.timer > 0) {
        m_buffer.timer--;
        if (m_buffer.timer <= 0) {
            m_buffer = BufferedInput{};
        }
    }

    if (m_tauntTimer > 0) {
        m_tauntTimer--;
        if (m_tauntTimer <= 0) {
            m_activeTaunt = "";
        }
    }

    for (auto& p : m_projectiles) {
        if (!p.active) continue;
        p.pos.x += p.vel.x;
        p.pos.y += p.vel.y;

        if (p.type == 2) {
            p.vel.y += 0.35f;
            if (p.pos.y >= p.bounceY) {
                p.pos.y = p.bounceY;
                p.vel.y = -p.vel.y * 0.65f;
            }
        } else if (p.type == 10) {

            p.vel.y += 0.24f;
            if (p.pos.y >= p.bounceY) {
                p.pos.y = p.bounceY;
                p.vel.y = -std::abs(p.vel.y) * 0.68f;
            }
        }

        p.lifetime--;
        if (p.lifetime <= 0 || p.pos.x < STAGE_MIN_X - 60.0f || p.pos.x > STAGE_MAX_X + 60.0f) {
            p.active = false;
        }
    }

    if (m_hitstopFrames > 0) {
        m_hitstopFrames--;
        return;
    }

    if (std::abs(m_pushbackVel) > 0.05f) {
        m_pos.x += m_pushbackVel;
        m_pushbackVel *= 0.82f;
    } else {
        m_pushbackVel = 0.0f;
    }

    bool isAirborne = (m_pos.y < GROUND_Y || m_state == FSTATE_JUMP || m_state == FSTATE_KNOCKDOWN_AIR || m_vel.y < 0.0f);
    bool grounded = !isAirborne;

    if (isAirborne) {

        m_vel.y += m_gravity;
        m_pos.x += m_vel.x;
        m_pos.y += m_vel.y;

        if (m_pos.y >= GROUND_Y) {

            m_pos.y = GROUND_Y;
            m_vel = Vector2{ 0.0f, 0.0f };
            if (m_state == FSTATE_JUMP || m_state == FSTATE_ATTACK) {
                m_state = FSTATE_IDLE;
                m_stateFrame = 0;
                TryExecuteBufferedMove(opponent);
            } else if (m_state == FSTATE_KNOCKDOWN_AIR) {
                m_state = FSTATE_KNOCKDOWN_GROUND;
                m_stateFrame = 0;
                AudioEngine::Instance().PlaySfx(SFX_HIT_HEAVY, 0.7f, 0.8f);
            }
        }
    } else {

        m_pos.y = GROUND_Y;
        m_vel.y = 0.0f;
        m_vel.x = 0.0f;

        if (m_state != FSTATE_ATTACK && m_state != FSTATE_SPECIAL &&
            m_state != FSTATE_HITSTUN && m_state != FSTATE_BLOCKSTUN &&
            m_state != FSTATE_BLOCK_HIGH && m_state != FSTATE_BLOCK_LOW &&
            m_state != FSTATE_KNOCKDOWN_AIR && m_state != FSTATE_KNOCKDOWN_GROUND &&
            m_state != FSTATE_WAKEUP) {
            m_facingRight = (m_pos.x < opponent.GetPosition().x);
        }
    }

    RelativeDirection rel = input.GetCurrentRelDir();
    m_isHoldingBack = (rel == DIR_REL_BACK || rel == DIR_REL_UP_BACK);
    m_isHoldingDownBack = (rel == DIR_REL_DOWN_BACK);
    bool crouching = (rel == DIR_REL_DOWN || rel == DIR_REL_DOWN_BACK || rel == DIR_REL_DOWN_FORWARD);

    int specialButton = 0;
    SpecialMotionType motion = const_cast<InputManager&>(input).CheckSpecialMotion(specialButton);
    if (motion != MOTION_NONE) {
        const_cast<InputManager&>(input).ConsumeMotion();
        m_buffer.motion = motion;
        m_buffer.motionButton = specialButton;
        m_buffer.timer = 12;
    }

    if (m_invincibleFrames > 0) m_invincibleFrames--;
    if (m_comboResetTimer > 0) {
        m_comboResetTimer--;
        if (m_comboResetTimer <= 0) m_comboCount = 0;
    }

    if ((input.IsButtonPressed(BTN_LP) && input.IsButtonHeld(BTN_LK)) ||
        (input.IsButtonPressed(BTN_LK) && input.IsButtonHeld(BTN_LP))) {
        m_buffer.button = (BTN_LP | BTN_LK);
        m_buffer.isCrouch = false;
        m_buffer.timer = 10;
    } else if (input.IsButtonPressed(BTN_LP)) {
        m_buffer.button = BTN_LP;
        m_buffer.isCrouch = crouching;
        m_buffer.timer = 10;
    } else if (input.IsButtonPressed(BTN_HP)) {
        m_buffer.button = BTN_HP;
        m_buffer.isCrouch = crouching;
        m_buffer.timer = 10;
    } else if (input.IsButtonPressed(BTN_LK)) {
        m_buffer.button = BTN_LK;
        m_buffer.isCrouch = crouching;
        m_buffer.timer = 10;
    } else if (input.IsButtonPressed(BTN_HK)) {
        m_buffer.button = BTN_HK;
        m_buffer.isCrouch = crouching;
        m_buffer.timer = 10;
    }

    if (input.IsButtonPressed(BTN_SPECIAL)) {
        if (grounded && (m_state == FSTATE_IDLE || m_state == FSTATE_WALK_FWD || m_state == FSTATE_WALK_BACK || m_state == FSTATE_CROUCH)) {
            TriggerSignatureSpecial(opponent);
        } else if (m_state == FSTATE_ATTACK && (m_hitConnected || m_stateFrame < m_currentAttack.startupFrames + m_currentAttack.activeFrames + 4)) {
            m_justCanceled = true;
            TriggerSignatureSpecial(opponent);
        }
    }

    if (input.IsButtonPressed(BTN_TAUNT)) {
        if (grounded && (m_state == FSTATE_IDLE || m_state == FSTATE_WALK_FWD || m_state == FSTATE_WALK_BACK || m_state == FSTATE_CROUCH)) {
            TriggerTaunt();
        }
    }

    m_stateFrame++;

    switch (m_state) {
    case FSTATE_IDLE:
    case FSTATE_WALK_FWD:
    case FSTATE_WALK_BACK:
    case FSTATE_CROUCH: {

        bool rawUp = input.GetCurrentState().up;
        bool wantJump = (rel == DIR_REL_UP || rel == DIR_REL_UP_FORWARD || rel == DIR_REL_UP_BACK || rawUp);

        if (wantJump) {
            bool rawFwd = m_facingRight ? input.GetCurrentState().right : input.GetCurrentState().left;
            bool rawBack = m_facingRight ? input.GetCurrentState().left : input.GetCurrentState().right;

            bool jumpFwd = (rel == DIR_REL_UP_FORWARD) || (rawUp && rawFwd) || (rawUp && m_state == FSTATE_WALK_FWD);
            bool jumpBack = (rel == DIR_REL_UP_BACK) || (rawUp && rawBack) || (rawUp && m_state == FSTATE_WALK_BACK);

            m_state = FSTATE_JUMP;
            m_stateFrame = 0;
            m_vel.y = m_jumpSpeedY;

            if (jumpFwd) {
                m_vel.x = m_facingRight ? m_jumpSpeedX : -m_jumpSpeedX;
            } else if (jumpBack) {
                m_vel.x = m_facingRight ? -m_jumpSpeedX : m_jumpSpeedX;
            } else {
                m_vel.x = 0.0f;
            }

            m_pos.y += m_vel.y;
            m_pos.x += m_vel.x;
            AudioEngine::Instance().PlaySfx(SFX_WHOOSH_LIGHT, 1.25f, 0.85f);
            break;
        }

        if (TryExecuteBufferedMove(opponent)) {
            break;
        }

        if (crouching) {
            m_state = FSTATE_CROUCH;
            m_vel.x = 0.0f;
        } else if (rel == DIR_REL_FORWARD) {
            m_state = FSTATE_WALK_FWD;
            m_pos.x += m_facingRight ? m_walkFwdSpeed : -m_walkFwdSpeed;
        } else if (rel == DIR_REL_BACK) {
            m_state = FSTATE_WALK_BACK;
            m_pos.x += m_facingRight ? -m_walkBackSpeed : m_walkBackSpeed;
        } else {
            m_state = FSTATE_IDLE;
        }
        break;
    }

    case FSTATE_JUMP: {

        if (m_buffer.button != 0) {
            bool heavy = (m_buffer.button == BTN_HP || m_buffer.button == BTN_HK);
            AttackMoveType mtype = MOVE_LIGHT_PUNCH;
            if (m_buffer.button == BTN_HP) mtype = MOVE_HEAVY_PUNCH;
            else if (m_buffer.button == BTN_LK) mtype = MOVE_LIGHT_KICK;
            else if (m_buffer.button == BTN_HK) mtype = MOVE_HEAVY_KICK;

            m_buffer = BufferedInput{};
            m_state = FSTATE_ATTACK;
            m_stateFrame = 0;
            m_hitConnected = false;
            m_currentAttack = AttackData{ heavy ? "Jump Heavy" : "Jump Light", 4, 12, 6, heavy ? 16 : 8, ATTACK_OVERHEAD, heavy ? 10 : 6, heavy ? 22 : 12, 10, 3.2f, false, heavy, 28.0f, -24.0f, 28.0f, 22.0f };
            m_currentAttack.isSpecial = false;
            m_currentAttack.moveType = mtype;
            AudioEngine::Instance().PlaySfx(heavy ? SFX_WHOOSH_HEAVY : SFX_WHOOSH_LIGHT, 1.0f);
        }
        break;
    }

    case FSTATE_ATTACK: {

        if (m_buffer.motion != MOTION_NONE && (m_hitConnected || m_stateFrame < m_currentAttack.startupFrames + m_currentAttack.activeFrames + 4)) {
            SpecialMotionType mot = m_buffer.motion;
            int btn = m_buffer.motionButton;
            m_buffer = BufferedInput{};
            m_justCanceled = true;
            ExecuteSpecial(mot, btn, opponent);
            break;
        }

        if (!m_currentAttack.isHeavy && m_stateFrame >= m_currentAttack.startupFrames) {
            if (m_buffer.button == BTN_LP || m_buffer.button == BTN_LK) {
                TryExecuteBufferedMove(opponent);
                break;
            }
        }

        int totalFrames = m_currentAttack.startupFrames + m_currentAttack.activeFrames + m_currentAttack.recoveryFrames;
        if (m_stateFrame >= totalFrames) {
            m_state = IsAirborne() ? FSTATE_JUMP : FSTATE_IDLE;
            m_stateFrame = 0;
            if (grounded) {
                TryExecuteBufferedMove(opponent);
            }
        }
        break;
    }

    case FSTATE_SPECIAL: {
        int totalFrames = m_currentAttack.startupFrames + m_currentAttack.activeFrames + m_currentAttack.recoveryFrames;
        if (m_multiHitsRemaining > 1 && m_stateFrame > m_currentAttack.startupFrames && (m_stateFrame % 5 == 0)) {
            m_hitConnected = false;
            m_multiHitsRemaining--;
        }

        if (m_charId == CHAR_THE_PUNDIT && m_specialIndex == 1 && (m_stateFrame == 12 || m_stateFrame == 20)) {
            SpawnProjectile(9, (m_stateFrame == 12) ? 4.4f : 5.3f, (m_stateFrame == 12) ? 0.22f : 0.0f, 10);
            AudioEngine::Instance().PlaySfx(SFX_FIREBALL_LAUNCH, 1.15f);
        }

        if (m_charId == CHAR_TECH_VISIONARY && m_specialIndex == 3 && m_stateFrame == 12) {
            m_pos.x = opponent.GetPosition().x + (opponent.IsFacingRight() ? -45.0f : 45.0f);
        }
        if (m_stateFrame >= totalFrames && grounded) {
            m_state = FSTATE_IDLE;
            m_stateFrame = 0;
            m_armor = 0;
            TryExecuteBufferedMove(opponent);
        }
        break;
    }

    case FSTATE_TAUNT: {
        if (m_stateFrame >= 45) {
            m_state = FSTATE_IDLE;
            m_stateFrame = 0;
            TryExecuteBufferedMove(opponent);
        }
        break;
    }

    case FSTATE_BLOCK_HIGH:
    case FSTATE_BLOCK_LOW:
    case FSTATE_BLOCKSTUN: {
        if (m_stateFrame >= m_blockstunFrames) {
            m_state = FSTATE_IDLE;
            m_stateFrame = 0;
            TryExecuteBufferedMove(opponent);
        }
        break;
    }

    case FSTATE_HITSTUN: {
        if (m_stateFrame >= m_hitstunFrames) {
            m_state = FSTATE_IDLE;
            m_stateFrame = 0;
            TryExecuteBufferedMove(opponent);
        }
        break;
    }

    case FSTATE_KNOCKDOWN_GROUND: {

        if (m_health <= 0) {
            m_state = FSTATE_DEFEAT;
            m_vel = Vector2{ 0.0f, 0.0f };
            m_pos.y = GROUND_Y;
            break;
        }
        if (m_stateFrame >= 25) {
            m_state = FSTATE_WAKEUP;
            m_stateFrame = 0;
        }
        break;
    }

    case FSTATE_WAKEUP: {

        if (m_stateFrame >= 18) {
            m_state = FSTATE_IDLE;
            m_stateFrame = 0;
            TryExecuteBufferedMove(opponent);
        }
        break;
    }

    case FSTATE_VICTORY:
        break;
    case FSTATE_DEFEAT:
        m_vel = Vector2{ 0.0f, 0.0f };
        m_pos.y = GROUND_Y;
        break;
    }

    for (auto it = m_cosmeticPuffs.begin(); it != m_cosmeticPuffs.end(); ) {
        it->timer += 1.0f / 60.0f;
        if (it->timer >= it->maxTimer) {
            it = m_cosmeticPuffs.erase(it);
        } else {
            ++it;
        }
    }

    if (m_charId == CHAR_COMMANDER && m_health > 0) {
        m_quirkTimer++;

        if (m_quirkTimer >= 480) {
            if (m_state == FSTATE_IDLE || m_state == FSTATE_WALK_FWD || m_state == FSTATE_WALK_BACK || m_state == FSTATE_CROUCH) {
                m_quirkTimer = 0;
                CosmeticPuff puff;
                puff.pos = Vector2{ m_pos.x + (m_facingRight ? -22.0f : 22.0f), m_pos.y - 34.0f };
                puff.timer = 0.0f;
                puff.maxTimer = 0.60f;
                puff.baseSize = 10.0f;
                m_cosmeticPuffs.push_back(puff);
                AudioEngine::Instance().PlaySfx(SFX_FART, 0.95f + (float)(rand() % 12) * 0.01f, 0.85f);
            }
        }
    }

    m_pos.x = std::clamp(m_pos.x, STAGE_MIN_X, STAGE_MAX_X);
}

void Fighter::UpdateAnimation() {

}

Rectangle Fighter::GetPushbox() const {
    float scale = (m_charId == CHAR_WAR_PIG) ? 1.20f : 1.0f;
    float w = 24.0f * scale;
    float h = (IsCrouching() ? 34.0f : 54.0f) * scale;
    return Rectangle{ m_pos.x - w * 0.5f, m_pos.y - h, w, h };
}

std::vector<Rectangle> Fighter::GetHurtboxes() const {
    std::vector<Rectangle> boxes;
    if (m_state == FSTATE_KNOCKDOWN_AIR || m_state == FSTATE_KNOCKDOWN_GROUND || m_state == FSTATE_WAKEUP) {
        return boxes;
    }

    float scale = (m_charId == CHAR_WAR_PIG) ? 1.20f : 1.0f;
    if (IsCrouching()) {
        boxes.push_back(Rectangle{ m_pos.x - 14.0f * scale, m_pos.y - 36.0f * scale, 28.0f * scale, 36.0f * scale });
    } else if (IsAirborne()) {
        boxes.push_back(Rectangle{ m_pos.x - 14.0f * scale, m_pos.y - 50.0f * scale, 28.0f * scale, 48.0f * scale });
    } else {

        boxes.push_back(Rectangle{ m_pos.x - 9.0f * scale, m_pos.y - 58.0f * scale, 18.0f * scale, 16.0f * scale });

        boxes.push_back(Rectangle{ m_pos.x - 13.0f * scale, m_pos.y - 42.0f * scale, 26.0f * scale, 22.0f * scale });

        boxes.push_back(Rectangle{ m_pos.x - 12.0f * scale, m_pos.y - 20.0f * scale, 24.0f * scale, 20.0f * scale });
    }
    return boxes;
}

bool Fighter::GetActiveHitbox(Rectangle& outBox, AttackData& outData) const {
    if (m_hitConnected) return false;

    if (m_state == FSTATE_ATTACK || m_state == FSTATE_SPECIAL) {
        int startup = m_currentAttack.startupFrames;
        int active = m_currentAttack.activeFrames;
        if (m_stateFrame >= startup && m_stateFrame < startup + active) {
            outData = m_currentAttack;
            float reach = m_facingRight ? m_currentAttack.reachX : -m_currentAttack.reachX - m_currentAttack.boxW;
            outBox = Rectangle{
                m_pos.x + reach,
                m_pos.y + m_currentAttack.reachY,
                m_currentAttack.boxW,
                m_currentAttack.boxH
            };
            return true;
        }
    }
    return false;
}
