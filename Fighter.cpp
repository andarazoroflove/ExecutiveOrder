#include "Fighter.h"
#include "AudioEngine.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

Fighter::Fighter(int playerIndex, CharacterId charId, float startX)
    : m_playerIndex(playerIndex), m_charId(charId) {
    InitCharacterProfile();
    ResetRound(startX, playerIndex == 0);
}

void Fighter::InitCharacterProfile() {
    m_damageScale = 1.0f;
    m_defenseScale = 1.0f;

    switch (m_charId) {
    case CHAR_COMMANDER:
        m_name = "POTUS";
        m_walkFwdSpeed = 2.05f;
        m_walkBackSpeed = 1.50f;
        m_color1 = Color{ 22, 40, 115, 255 };
        m_color2 = Color{ 230, 25, 25, 255 };
        m_colorSkin = Color{ 245, 145, 75, 255 };
        break;
    case CHAR_HIGH_BENCH:
        m_name = "High Bench";
        m_walkFwdSpeed = 1.55f;
        m_walkBackSpeed = 1.20f;
        m_color1 = Color{ 20, 20, 25, 255 };
        m_color2 = Color{ 180, 130, 50, 255 };
        m_colorSkin = Color{ 230, 200, 170, 255 };
        break;
    case CHAR_REDCAP:
        m_name = "Redcap";
        m_walkFwdSpeed = 1.85f;
        m_walkBackSpeed = 1.35f;
        m_color1 = Color{ 210, 25, 25, 255 };
        m_color2 = Color{ 40, 50, 90, 255 };
        m_colorSkin = Color{ 245, 185, 140, 255 };
        break;
    case CHAR_COASTAL_GOV:
        m_name = "Governator";
        m_walkFwdSpeed = 2.40f;
        m_walkBackSpeed = 1.80f;
        m_color1 = Color{ 30, 80, 110, 255 };
        m_color2 = Color{ 220, 200, 120, 255 };
        m_colorSkin = Color{ 235, 185, 150, 255 };
        break;
    case CHAR_TECH_VISIONARY:
        m_name = "TechBro";
        m_walkFwdSpeed = 2.15f;
        m_walkBackSpeed = 1.60f;
        m_color1 = Color{ 45, 70, 120, 255 };
        m_color2 = Color{ 210, 180, 50, 255 };
        m_colorSkin = Color{ 240, 205, 175, 255 };
        break;
    case CHAR_FILIBUSTER:
        m_name = "Filibuster";
        m_walkFwdSpeed = 1.45f;
        m_walkBackSpeed = 1.15f;
        m_color1 = Color{ 60, 60, 65, 255 };
        m_color2 = Color{ 140, 100, 60, 255 };
        m_colorSkin = Color{ 230, 195, 170, 255 };
        break;
    case CHAR_MASCOT:
        m_name = "Freagle";
        m_walkFwdSpeed = 2.28f;
        m_walkBackSpeed = 1.70f;
        m_color1 = Color{ 240, 120, 30, 255 };
        m_color2 = Color{ 230, 40, 160, 255 };
        m_colorSkin = Color{ 255, 230, 180, 255 };
        break;
    case CHAR_POPULIST:
        m_name = "Populist";
        m_walkFwdSpeed = 2.32f;
        m_walkBackSpeed = 1.75f;
        m_color1 = Color{ 235, 235, 240, 255 };
        m_color2 = Color{ 200, 20, 80, 255 };
        m_colorSkin = Color{ 215, 160, 120, 255 };
        break;
    case CHAR_SLEEPY_JOE:
        m_name = "Sleepy Joe";
        m_walkFwdSpeed = 2.00f;
        m_walkBackSpeed = 1.50f;
        m_color1 = Color{ 245, 245, 250, 255 };
        m_color2 = Color{ 25, 80, 210, 255 };
        m_colorSkin = Color{ 235, 195, 165, 255 };
        break;
    case CHAR_MTG:
        m_name = "Marjory T. Green";
        m_walkFwdSpeed = 1.95f;
        m_walkBackSpeed = 1.45f;
        m_color1 = Color{ 220, 30, 30, 255 };
        m_color2 = Color{ 35, 70, 150, 255 };
        m_colorSkin = Color{ 245, 195, 155, 255 };
        break;
    case CHAR_SUCKRATERY_RKF:
        m_name = "Suckratery RKF";
        m_walkFwdSpeed = 2.10f;
        m_walkBackSpeed = 1.55f;
        m_color1 = Color{ 240, 240, 245, 255 };
        m_color2 = Color{ 50, 55, 52, 255 };
        m_colorSkin = Color{ 235, 190, 160, 255 };
        break;
    case CHAR_WAR_PIG:
        m_name = "War Pig";
        m_walkFwdSpeed = 1.90f;
        m_walkBackSpeed = 1.40f;
        m_color1 = Color{ 70, 95, 55, 255 };
        m_color2 = Color{ 25, 25, 30, 255 };
        m_colorSkin = Color{ 240, 185, 145, 255 };
        break;
    case CHAR_SOCIALIST:
        m_name = "The Socialist";
        m_walkFwdSpeed = 2.15f;
        m_walkBackSpeed = 1.60f;
        m_color1 = Color{ 35, 65, 115, 255 };
        m_color2 = Color{ 50, 160, 70, 255 };
        m_colorSkin = Color{ 230, 185, 150, 255 };
        break;
    case CHAR_BILLARY:
        m_name = "Billary";
        m_walkFwdSpeed = 2.05f;
        m_walkBackSpeed = 1.50f;
        m_color1 = Color{ 25, 65, 140, 255 };
        m_color2 = Color{ 215, 25, 25, 255 };
        m_colorSkin = Color{ 235, 195, 165, 255 };
        break;
    case CHAR_PALM_BEACH_PETE:
        m_name = "Palm Beach Pete";
        m_walkFwdSpeed = 1.98f;
        m_walkBackSpeed = 1.45f;
        m_color1 = Color{ 240, 240, 245, 255 };
        m_color2 = Color{ 215, 185, 70, 255 };
        m_colorSkin = Color{ 235, 190, 160, 255 };
        break;
    case CHAR_THE_PUNDIT:
        m_name = "The Pundit";
        m_walkFwdSpeed = 2.05f;
        m_walkBackSpeed = 1.50f;
        m_color1 = Color{ 25, 42, 85, 255 };
        m_color2 = Color{ 215, 35, 45, 255 };
        m_colorSkin = Color{ 245, 200, 175, 255 };
        break;
    case CHAR_A_NEW_HOPE:
        m_name = "A New Hope";
        m_walkFwdSpeed = 2.25f;
        m_walkBackSpeed = 1.70f;
        m_color1 = Color{ 245, 245, 250, 255 };
        m_color2 = Color{ 210, 180, 130, 255 };
        m_colorSkin = Color{ 165, 110, 75, 255 };
        break;

    case CHAR_DOCTOR_ABDUL:
        m_name = "Doctor Abdul";
        m_walkFwdSpeed = 2.30f;
        m_walkBackSpeed = 1.75f;
        m_color1 = Color{ 242, 245, 250, 255 };
        m_color2 = Color{ 12, 35, 64, 255 };
        m_colorSkin = Color{ 210, 155, 112, 255 };
        break;
    case CHAR_COUCHIE:
        m_name = "Couchie";
        m_walkFwdSpeed = 1.92f;
        m_walkBackSpeed = 1.45f;
        m_color1 = Color{ 25, 52, 130, 255 };
        m_color2 = Color{ 215, 25, 35, 255 };
        m_colorSkin = Color{ 245, 200, 175, 255 };
        break;
    case CHAR_BLEXAS:
        m_name = "Blexas";
        m_walkFwdSpeed = 2.38f;
        m_walkBackSpeed = 1.82f;
        m_damageScale = 1.15f;
        m_defenseScale = 1.10f;
        m_color1 = Color{ 22, 22, 28, 255 };
        m_color2 = Color{ 255, 195, 45, 255 };
        m_colorSkin = Color{ 240, 192, 158, 255 };
        break;
    case CHAR_SI:
        m_name = "SI";
        m_walkFwdSpeed = 2.35f;
        m_walkBackSpeed = 1.85f;
        m_color1 = Color{ 16, 20, 24, 255 };
        m_color2 = Color{ 45, 255, 110, 255 };
        m_colorSkin = Color{ 195, 205, 218, 255 };
        break;

    case CHAR_KIM:
        m_name = "Kim";
        m_walkFwdSpeed = 1.75f;
        m_walkBackSpeed = 1.35f;
        m_damageScale = 1.12f;
        m_color1 = Color{ 28, 30, 36, 255 };
        m_color2 = Color{ 220, 35, 35, 255 };
        m_colorSkin = Color{ 242, 200, 168, 255 };
        break;
    case CHAR_BEBE:
        m_name = "Bebe";
        m_walkFwdSpeed = 2.10f;
        m_walkBackSpeed = 1.60f;
        m_damageScale = 1.15f;
        m_color1 = Color{ 22, 42, 105, 255 };
        m_color2 = Color{ 75, 160, 255, 255 };
        m_colorSkin = Color{ 238, 190, 158, 255 };
        break;
    case CHAR_PALE_MOTH:
        m_name = "Pale Moth";
        m_walkFwdSpeed = 2.20f;
        m_walkBackSpeed = 1.68f;
        m_damageScale = 1.18f;
        m_color1 = Color{ 24, 26, 32, 255 };
        m_color2 = Color{ 250, 205, 35, 255 };
        m_colorSkin = Color{ 242, 208, 188, 255 };
        break;
    case CHAR_THE_DICTATOR:
        m_name = "The Dictator";
        m_walkFwdSpeed = 2.08f;
        m_walkBackSpeed = 1.55f;
        m_damageScale = 1.20f;
        m_color1 = Color{ 82, 92, 54, 255 };
        m_color2 = Color{ 215, 175, 55, 255 };
        m_colorSkin = Color{ 218, 168, 128, 255 };
        break;

    case CHAR_FLOTUS:
        m_name = "FLOTUS";
        m_walkFwdSpeed = 2.28f;
        m_walkBackSpeed = 1.76f;
        m_color1 = Color{ 22, 34, 62, 255 };
        m_color2 = Color{ 215, 45, 75, 255 };
        m_colorSkin = Color{ 242, 202, 178, 255 };
        break;
    case CHAR_W:
        m_name = "W";
        m_walkFwdSpeed = 2.22f;
        m_walkBackSpeed = 1.70f;
        m_color1 = Color{ 242, 245, 252, 255 };
        m_color2 = Color{ 62, 68, 78, 255 };
        m_colorSkin = Color{ 242, 192, 156, 255 };
        break;
    case CHAR_DICK:
        m_name = "Dick";
        m_walkFwdSpeed = 1.88f;
        m_walkBackSpeed = 1.42f;
        m_damageScale = 1.12f;
        m_color1 = Color{ 245, 125, 20, 255 };
        m_color2 = Color{ 175, 28, 28, 255 };
        m_colorSkin = Color{ 240, 196, 168, 255 };
        break;
    case CHAR_AYATOLLAH:
        m_name = "The Ayatollah";
        m_walkFwdSpeed = 2.02f;
        m_walkBackSpeed = 1.54f;
        m_damageScale = 1.24f;
        m_defenseScale = 1.08f;
        m_color1 = Color{ 20, 22, 28, 255 };
        m_color2 = Color{ 35, 175, 95, 255 };
        m_colorSkin = Color{ 228, 178, 142, 255 };
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
    m_desperationSuperUsed = false;
    m_throwTechWindow = 0;
    m_juggleHits = 0;
    m_starBurstTimer = 0;
    m_electroShockTimer = 0;
    m_jengaTimer = 0;
    m_laughTimer = 0;
    m_isSuperMove = false;
    m_triggerSuperFlash = false;
    m_justCanceled = false;
    m_justReversal = false;
    m_justAlphaCounter = false;
    m_justThrowTeched = false;
    m_multiHitsRemaining = 0;
    m_comboCount = 0;
    m_comboResetTimer = 0;
    m_tauntTimer = 0;
    m_activeTaunt = "";
    m_health = 100;
    m_refillCount = 0;
    m_isHoldingBack = false;
    m_isHoldingDownBack = false;
    m_isHoldingBlockButton = false;
    m_executedFinisher = false;
    m_buffer = BufferedInput{};
    m_projectiles.clear();
    m_cosmeticPuffs.clear();
    m_quirkTimer = 0;

    if (m_roundsWon == 0 && !wasKnockedOut) {
        m_superMeter = 0;
    }
}

void Fighter::EnterFinishHimDizzy() {
    m_health = 1;
    m_state = FSTATE_FINISH_HIM_DIZZY;
    m_stateFrame = 0;
    m_vel = Vector2{ 0.0f, 0.0f };
    m_pos.y = GROUND_Y;
    m_projectiles.clear();
    m_starBurstTimer = 180;
}

void Fighter::TriggerVetoedKO() {
    m_health = 0;
    m_state = FSTATE_KNOCKDOWN_AIR;
    m_stateFrame = 0;
    m_tauntTimer = 0;
    m_laughTimer = 0;
    m_activeTaunt = "";
    m_vel = Vector2{ m_facingRight ? -3.8f : 3.8f, -7.8f };
}

void Fighter::EnterFinisherVictim() {
    m_health = 1;
    m_state = FSTATE_FINISHER_VICTIM;
    m_stateFrame = 0;
    m_vel = Vector2{ 0.0f, 0.0f };
    m_pos.y = GROUND_Y;
    m_projectiles.clear();
    m_starBurstTimer = 120;
}

void Fighter::StartFinisher(Fighter& victim) {
    m_state = FSTATE_FINISHER;
    m_stateFrame = 0;
    m_executedFinisher = true;
    m_vel = Vector2{ 0.0f, 0.0f };
    m_pos.y = GROUND_Y;
    m_invincibleFrames = 120;
    m_projectiles.clear();

    float vx = victim.GetPosition().x;
    bool attackFromLeft = (m_pos.x <= vx);
    if (vx < STAGE_MIN_X + 100.0f) attackFromLeft = false;
    else if (vx > STAGE_MAX_X - 100.0f) attackFromLeft = true;
    float idealX = attackFromLeft ? (vx - 88.0f) : (vx + 88.0f);
    m_pos.x = std::clamp(idealX, STAGE_MIN_X + 24.0f, STAGE_MAX_X - 24.0f);
    m_facingRight = (m_pos.x < vx);
    victim.SetFacingRight(!m_facingRight);
    victim.EnterFinisherVictim();

    m_activeTaunt = "";
    m_tauntTimer = 0;
    if (m_charId == CHAR_DICK) {
        m_laughTimer = 40;
        AudioEngine::Instance().PlaySfx(SFX_DICK_LAUGH, 0.98f, 1.0f);
    }
    AudioEngine::Instance().PlaySfx(SFX_FINISHER_DRAMATIC, 1.0f, 1.0f);
}

const char* Fighter::GetFinisherName(CharacterId id) {
    switch (id) {
    case CHAR_COMMANDER:       return "GOLDEN VETO ANVIL CRUSH!";
    case CHAR_HIGH_BENCH:      return "9-0 UNANIMOUS MEGA-GAVEL!";
    case CHAR_REDCAP:          return "MONSTER V8 RALLY FLATTENER!";
    case CHAR_COASTAL_GOV:     return "CALIFORNIA MEGA-QUAKE VAULT!";
    case CHAR_TECH_VISIONARY:  return "ORBITAL BOOSTER DROP!";
    case CHAR_FILIBUSTER:      return "100-TON OMNIBUS SAFE SLAM!";
    case CHAR_MASCOT:          return "SCREAMING LIBERTY TALON DIVE!";
    case CHAR_POPULIST:        return "VIRAL RATIO SATELLITE FLASH!";
    case CHAR_SLEEPY_JOE:      return "DARK BRANDON SCOOP METEOR!";
    case CHAR_MTG:             return "ORBITAL SPACE LASER STRIKE!";
    case CHAR_SUCKRATERY_RKF:  return "TOXIC BRAIN-WORM CRUSHER!";
    case CHAR_WAR_PIG:         return "DEFCON-1 SHARK-BOMB DROP!";
    case CHAR_SOCIALIST:       return "WALL STREET WEALTH-TAX SAFE!";
    case CHAR_BILLARY:         return "BLEACHBIT SERVER WIPEOUT!";
    case CHAR_PALM_BEACH_PETE: return "MEGA-YACHT ELIXIR ANCHOR!";
    case CHAR_THE_PUNDIT:      return "270 LANDSLIDE ANVIL SLAM!";
    case CHAR_A_NEW_HOPE:      return "SHATTERED BACKBOARD DUNK!";
    case CHAR_DOCTOR_ABDUL:    return "SUPREME SHAWARMA SPIT ROAST!";
    case CHAR_COUCHIE:         return "LUXURY SOFA SKYSCRAPER DROP!";
    case CHAR_BLEXAS:          return "LONE STAR V8 STAMPEDE!";
    case CHAR_SI:              return "SKYNET T-800 TERMINATION!";
    case CHAR_KIM:             return "SUPREME DEAR-LEADER ICBM!";
    case CHAR_BEBE:            return "IRON DOME ORBITAL BARRAGE!";
    case CHAR_PALE_MOTH:       return "KREMLIN TSAR BOMBA & STALLION!";
    case CHAR_THE_DICTATOR:    return "PALACE TRAPDOOR BUNKER BOMB!";
    case CHAR_FLOTUS:          return "NATASHA SPUTNIK ULTRA-VOLT!";
    case CHAR_W:               return "MISSION ACCOMPLISHED DRIVE!";
    case CHAR_DICK:            return "JACKSON HOLE MEGA-BUCKSHOT!";
    case CHAR_AYATOLLAH:       return "WRATH OF THE GOLDEN MINARET!";
    default:                   return "SUPREME EXECUTIVE FINISHER!";
    }
}

int Fighter::GetFinisherPropIndex(CharacterId id) {
    switch (id) {
    case CHAR_COMMANDER:       return 0;
    case CHAR_HIGH_BENCH:      return 1;
    case CHAR_REDCAP:          return 2;
    case CHAR_COASTAL_GOV:     return 7;
    case CHAR_TECH_VISIONARY:  return 3;
    case CHAR_FILIBUSTER:      return 7;
    case CHAR_MASCOT:          return 4;
    case CHAR_POPULIST:        return 14;
    case CHAR_SLEEPY_JOE:      return 5;
    case CHAR_MTG:             return 14;
    case CHAR_SUCKRATERY_RKF:  return 8;
    case CHAR_WAR_PIG:         return 6;
    case CHAR_SOCIALIST:       return 7;
    case CHAR_BILLARY:         return 1;
    case CHAR_PALM_BEACH_PETE: return 8;
    case CHAR_THE_PUNDIT:      return 0;
    case CHAR_A_NEW_HOPE:      return 9;
    case CHAR_DOCTOR_ABDUL:    return 10;
    case CHAR_COUCHIE:         return 11;
    case CHAR_BLEXAS:          return 2;
    case CHAR_SI:              return 12;
    case CHAR_KIM:             return 13;
    case CHAR_BEBE:            return 3;
    case CHAR_PALE_MOTH:       return 13;
    case CHAR_THE_DICTATOR:    return 6;
    case CHAR_FLOTUS:          return 14;
    case CHAR_W:               return 6;
    case CHAR_DICK:            return 2;
    case CHAR_AYATOLLAH:       return 15;
    default:                   return 0;
    }
}

bool Fighter::TryTriggerAyatollahPrayer() {
    if (m_charId == CHAR_AYATOLLAH && m_refillCount < 7 && m_health <= 25) {
        m_refillCount++;
        m_health = 100;
        m_state = FSTATE_CALL_TO_PRAYER;
        m_stateFrame = 0;
        m_invincibleFrames = 58;
        m_armor = 2;
        m_hitstopFrames = 14;
        m_pushbackVel = 0.0f;
        m_vel = Vector2{ 0.0f, 0.0f };
        m_pos.y = GROUND_Y;
        m_activeTaunt = (m_refillCount % 2 == 1) ? "@AR0" : "@AR1";
        m_tauntTimer = 78;
        AudioEngine::Instance().PlaySfx(SFX_CALL_TO_PRAYER, 1.0f, 1.0f);
        return true;
    }
    return false;
}

void Fighter::TriggerCameraLaugh(int frames) {
    m_laughTimer = frames;
    m_vel.x = 0.0f;
    m_activeTaunt = "HA! HA! HA!";
    m_tauntTimer = frames;
    AudioEngine::Instance().PlaySfxPan(SFX_DICK_LAUGH, 1.0f, 1.0f, m_pos.x);
}

void Fighter::SpawnProjectile(int type, float vx, float vy, int damage, float overrideX, float overrideY) {
    Projectile p{};
    p.pos = Vector2{ m_pos.x + (m_facingRight ? 26.0f : -26.0f), m_pos.y - 36.0f };
    p.vel = Vector2{ m_facingRight ? vx : -vx, vy };
    p.width = 22.0f;
    p.height = 18.0f;
    p.lifetime = 150;

    switch (type) {
    case PROJ_LEDGER:
        p.width = 14.0f; p.height = 55.0f; break;
    case PROJ_TRAP_FIELD:
        p.lifetime = 180; break;
    case PROJ_GAVEL_WAVE:
        p.pos.y = GROUND_Y - 10.0f; p.width = 28.0f; p.height = 20.0f; break;
    case PROJ_SECRET_SERVICE:
        p.pos.y = GROUND_Y - 32.0f;
        p.pos.x = m_pos.x + (m_facingRight ? -30.0f : 30.0f);
        p.width = 44.0f; p.height = 54.0f; p.lifetime = 80;
        break;
    case PROJ_POLLING_MAP:
        p.pos.x = m_pos.x + (m_facingRight ? 34.0f : -34.0f);
        p.pos.y = m_pos.y - 40.0f;
        p.width = 26.0f; p.height = 20.0f;
        break;
    case PROJ_HOPE_BASKETBALL:
        p.pos.x = m_pos.x + (m_facingRight ? 16.0f : -16.0f);
        p.pos.y = m_pos.y - 74.0f;
        p.width = 22.0f; p.height = 22.0f;
        break;
    case PROJ_SHAWARMA:
        p.pos.y = m_pos.y - 40.0f;
        p.width = 26.0f; p.height = 14.0f;
        break;
    case PROJ_WHEELCHAIR:
        p.pos.y = GROUND_Y - 24.0f;
        p.width = 38.0f; p.height = 44.0f;
        p.lifetime = 170;
        break;
    case PROJ_ATM_HACK:
        p.pos.x = (overrideX > -1000.0f) ? overrideX : (m_pos.x + (m_facingRight ? 120.0f : -120.0f));
        p.pos.y = GROUND_Y - 28.0f;
        p.vel = Vector2{ 0.0f, 0.0f };
        p.width = 34.0f; p.height = 54.0f;
        p.lifetime = 44;
        break;
    case PROJ_SUPREME_WAVE:
        p.pos.y = m_pos.y - 36.0f;
        p.width = 34.0f; p.height = 48.0f;
        p.lifetime = 130;
        break;
    case PROJ_EYE_LASER:
        p.pos.x = m_pos.x + (m_facingRight ? 28.0f : -28.0f);
        p.pos.y = m_pos.y - 54.0f;
        p.width = 48.0f; p.height = 12.0f;
        p.lifetime = 90;
        break;
    case PROJ_HAMMER_SICKLE:
        p.pos.y = m_pos.y - 40.0f;
        p.width = 30.0f; p.height = 30.0f;
        p.lifetime = 150;
        break;
    case PROJ_SPY_RADIO:
        p.pos.x = m_pos.x + (m_facingRight ? 32.0f : -32.0f);
        p.pos.y = m_pos.y - 44.0f;
        p.width = 36.0f; p.height = 30.0f;
        p.lifetime = 130;
        break;
    case PROJ_PAPER_PLANE:
        p.pos.x = m_pos.x + (m_facingRight ? 30.0f : -30.0f);
        p.pos.y = m_pos.y - 46.0f;
        p.width = 28.0f; p.height = 14.0f;
        p.lifetime = 150;
        break;
    case PROJ_SHOTGUN_BLAST:
        p.pos.x = m_pos.x + (m_facingRight ? 40.0f : -40.0f);
        p.pos.y = m_pos.y - 50.0f;
        p.width = 44.0f; p.height = 20.0f;
        p.lifetime = 75;
        break;
    case PROJ_HORMUZ_BLOCKADE:
        p.pos.x = m_pos.x + (m_facingRight ? 34.0f : -34.0f);
        p.pos.y = GROUND_Y - 26.0f;
        p.width = 56.0f; p.height = 48.0f;
        p.lifetime = 150;
        break;
    default:
        break;
    }

    if (overrideX > -1000.0f) p.pos.x = overrideX;
    if (overrideY > -1000.0f) p.pos.y = overrideY;

    int scaledDmg = (type == PROJ_ATM_HACK) ? 25 : std::max(1, (int)roundf(damage * m_damageScale));
    p.damage = scaledDmg;
    p.age = 0;
    p.active = true;
    p.facingRight = m_facingRight;
    p.type = type;
    p.bounceY = GROUND_Y - 8.0f;
    m_projectiles.push_back(p);
}

void Fighter::ExecuteSpecial(SpecialMotionType motion, int buttonUsed, const Fighter& opponent) {
    bool wasWakeupOrReversal = (m_stateFrame <= 6);
    m_state = FSTATE_SPECIAL;
    m_stateFrame = 0;
    m_hitConnected = false;
    m_armor = 0;
    m_isSuperMove = false;
    m_multiHitsRemaining = 1;
    AddSuperMeter(8);

    if (m_charId == CHAR_COUCHIE) {
        m_specialIndex = 1;
        m_invincibleFrames = 46;
        AddSuperMeter(35);
        m_currentAttack = AttackData{ "Hiawatha Couch", 4, 42, 8, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f, true, MOVE_SPECIAL };
        m_activeTaunt = "Hiawatha!";
        m_tauntTimer = 75;
        AudioEngine::Instance().PlaySfxPan(SFX_COUCH_SPRING, 1.0f, 1.0f, m_pos.x);
        return;
    }

    if (motion == MOTION_SUPER_OMNIBUS && CanUseSuper()) {
        int stocks = m_superMeter / 100;
        float lvlMult = (stocks >= 3) ? 1.35f : ((stocks == 2) ? 1.15f : 1.0f);
        if (m_superMeter >= 100) {
            m_superMeter = (stocks >= 3) ? 0 : (m_superMeter - 100);
        } else {
            m_desperationSuperUsed = true;
        }
        m_isSuperMove = true;
        m_triggerSuperFlash = true;
        m_invincibleFrames = 22;
        m_multiHitsRemaining = 5;
        m_specialIndex = 2;
        int perHit = std::max(5, (int)roundf(6.5f * lvlMult * m_damageScale));
        m_currentAttack = AttackData{ (stocks >= 3) ? "MAX SUPER VETO!" : "SUPER VETO COMBO!", 4, 30, 16, perHit, ATTACK_MID, 12, 32, 18, 5.0f, true, true, 46.0f, -34.0f, 48.0f, 40.0f, true, MOVE_SUPER };
        m_vel = Vector2{ m_facingRight ? 5.6f : -5.6f, -2.2f };
        AudioEngine::Instance().PlaySfx(SFX_SUPER_FREEZE, 1.0f, 1.0f);
        AudioEngine::Instance().PlaySfx(SFX_VOICE_SUPER_VETO, 1.0f, 1.0f);
        return;
    } else if (motion == MOTION_SUPER_OMNIBUS) {

        motion = MOTION_QCF_P;
    }

    if (motion == MOTION_DP_P || motion == MOTION_DP_K || motion == MOTION_CHARGE_D_U_K) {
        m_invincibleFrames = 12;
        if (wasWakeupOrReversal) m_justReversal = true;
    }

    bool isHeavy = (buttonUsed & (BTN_HP | BTN_HK)) != 0;
    auto ScaleDmg = [&](int base) -> int { return std::max(1, (int)roundf(base * m_damageScale)); };

    switch (m_charId) {
    case CHAR_COMMANDER:
        if (motion == MOTION_DP_P) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Executive Uppercut", 3, 14, 18, ScaleDmg(20), ATTACK_MID, 13, 28, 16, 4.0f, true, isHeavy, 24.0f, -48.0f, 28.0f, 44.0f, true, MOVE_SPECIAL, true };
            m_vel = Vector2{ m_facingRight ? 2.0f : -2.0f, isHeavy ? -9.2f : -7.6f };
            AudioEngine::Instance().PlaySfxPan(SFX_VOICE_UPPERCUT, 1.0f, 1.0f, m_pos.x);
        } else if (motion == MOTION_QCB_K) {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Motorcade Tackle", 6, 22, 12, ScaleDmg(16), ATTACK_LOW, 10, 22, 14, 4.5f, true, isHeavy, 32.0f, -20.0f, 34.0f, 22.0f };
            m_vel = Vector2{ m_facingRight ? 4.0f : -4.0f, 0.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_SLIDE, 0.9f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Teleprompter Spark", 10, 6, 16, ScaleDmg(14), ATTACK_PROJECTILE, 10, 18, 12, 3.5f, false, isHeavy, 35.0f, -32.0f, 24.0f, 20.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_VOICE_HADOUKEN, 1.0f, 1.0f, m_pos.x);
            AudioEngine::Instance().PlaySfxPan(SFX_FIREBALL_LAUNCH, 1.0f, 1.0f, m_pos.x);
            SpawnProjectile(PROJ_SPARK, isHeavy ? 4.2f : 2.6f, 0.0f, 14);
        }
        break;

    case CHAR_HIGH_BENCH: {
        HighBenchStage stage = GetHighBenchStage();
        if (stage == HB_STAGE_THOMAS) {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Coke Can Spray", 8, 14, 15, ScaleDmg(18), ATTACK_PROJECTILE, 10, 20, 12, 3.8f, false, isHeavy, 48.0f, -30.0f, 42.0f, 24.0f };
            SpawnProjectile(PROJ_COKE_SPRAY, isHeavy ? 4.8f : 3.4f, 0.0f, 18);
            AudioEngine::Instance().PlaySfxPan(SFX_COKE_POP, 1.0f, 1.0f, m_pos.x);
        } else if (stage == HB_STAGE_ROBERTS) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Gavel Stomp", 9, 12, 16, ScaleDmg(22), ATTACK_OVERHEAD, 14, 26, 16, 4.8f, true, true, 42.0f, -16.0f, 44.0f, 32.0f };
            SpawnProjectile(PROJ_GAVEL_WAVE, isHeavy ? 4.4f : 3.0f, 0.0f, 20);
            AudioEngine::Instance().PlaySfxPan(SFX_GAVEL_STOMP, 1.0f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Fiery Dissent", 6, 16, 15, ScaleDmg(24), ATTACK_PROJECTILE, 14, 28, 16, 5.0f, true, true, 50.0f, -32.0f, 46.0f, 28.0f };
            SpawnProjectile(PROJ_DISSENT_FLAME, isHeavy ? 5.2f : 3.8f, 0.0f, 24);
            AudioEngine::Instance().PlaySfxPan(SFX_DISSENT_FLAME, 1.0f, 1.0f, m_pos.x);
        }
        break;
    }

    case CHAR_REDCAP:
        if (motion == MOTION_COMMAND_GRAB || motion == MOTION_QCB_P) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Hostile Takeover", 4, 6, 18, ScaleDmg(26), ATTACK_THROW, 14, 30, 0, 5.0f, true, true, 28.0f, -28.0f, 30.0f, 32.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_HIT_HEAVY, 0.8f, 1.0f, m_pos.x);
        } else if (motion == MOTION_CHARGE_B_F_P) {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "The Big Deal", 9, 14, 15, ScaleDmg(20), ATTACK_MID, 14, 26, 16, 5.5f, true, true, 36.0f, -36.0f, 34.0f, 26.0f };
            m_armor = 1;
            m_vel = Vector2{ m_facingRight ? 4.2f : -4.2f, 0.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_WHOOSH_HEAVY, 0.85f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Rally Megaphone", 9, 12, 15, ScaleDmg(15), ATTACK_MID, 12, 24, 20, 4.5f, false, isHeavy, 44.0f, -38.0f, 40.0f, 30.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_MEGAPHONE, 1.0f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_COASTAL_GOV:
        if (motion == MOTION_CHARGE_D_U_K || motion == MOTION_DP_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Veto Flip", 4, 12, 16, ScaleDmg(19), ATTACK_MID, 14, 26, 14, 3.8f, true, isHeavy, 28.0f, -44.0f, 30.0f, 40.0f, true, MOVE_SPECIAL, true };
            m_vel = Vector2{ m_facingRight ? 1.6f : -1.6f, -9.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_UPPERCUT, 1.2f, 1.0f, m_pos.x);
        } else if (motion == MOTION_QCB_P) {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Press Briefing", 9, 8, 13, ScaleDmg(8), ATTACK_MID, 8, 14, 8, 2.0f, false, false, 30.0f, -28.0f, 20.0f, 20.0f };
            SpawnProjectile(PROJ_TRAP_FIELD, 0.0f, 0.0f, 10);
            AudioEngine::Instance().PlaySfxPan(SFX_SELECT_CONFIRM, 1.0f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Smooth-Talk Slide", 5, 18, 11, ScaleDmg(15), ATTACK_LOW, 8, 20, 12, 4.2f, true, isHeavy, 38.0f, -12.0f, 38.0f, 18.0f };
            m_vel = Vector2{ m_facingRight ? 4.8f : -4.8f, 0.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_SLIDE, 1.1f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_TECH_VISIONARY:
        if (motion == MOTION_QCB_P) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Disruption Field", 8, 6, 14, ScaleDmg(12), ATTACK_LOW, 8, 18, 10, 2.0f, false, false, 28.0f, -14.0f, 20.0f, 16.0f };
            SpawnProjectile(PROJ_TRAP_FIELD, 0.0f, 0.0f, 14);
            AudioEngine::Instance().PlaySfxPan(SFX_ELECTRO_GLITCH, 0.9f, 1.0f, m_pos.x);
        } else if (motion == MOTION_QCB_K) {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "System Reboot", 12, 4, 10, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_ELECTRO_GLITCH, 1.5f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Drone Delivery", 10, 6, 15, ScaleDmg(16), ATTACK_OVERHEAD, 10, 20, 12, 3.0f, false, isHeavy, 30.0f, -40.0f, 24.0f, 20.0f };
            SpawnProjectile(PROJ_ARC_BOMB, isHeavy ? 3.0f : 2.0f, -4.5f, 16);
            AudioEngine::Instance().PlaySfxPan(SFX_ELECTRO_GLITCH, 1.4f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_FILIBUSTER:
        if (motion == MOTION_CHARGE_D_U_K || motion == MOTION_DP_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Point of Order", 5, 12, 16, ScaleDmg(18), ATTACK_MID, 12, 24, 14, 3.5f, true, isHeavy, 24.0f, -50.0f, 26.0f, 42.0f };
            m_vel = Vector2{ m_facingRight ? 1.2f : -1.2f, -7.8f };
            AudioEngine::Instance().PlaySfxPan(SFX_UPPERCUT, 0.9f, 1.0f, m_pos.x);
        } else if (motion == MOTION_QCB_P) {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "C-SPAN Slumber", 8, 14, 15, ScaleDmg(16), ATTACK_LOW, 12, 22, 14, 4.0f, true, true, 32.0f, -12.0f, 34.0f, 22.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_GAVEL_SLAM, 0.9f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Endless Ledger", 11, 8, 15, ScaleDmg(14), ATTACK_PROJECTILE, 10, 18, 12, 3.0f, false, isHeavy, 35.0f, -30.0f, 16.0f, 55.0f };
            SpawnProjectile(PROJ_LEDGER, isHeavy ? 2.8f : 1.7f, 0.0f, 14);
            AudioEngine::Instance().PlaySfxPan(SFX_WHOOSH_HEAVY, 1.0f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_MASCOT:
        if (motion == MOTION_QCF_P) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Glitter Bomb", 9, 6, 15, ScaleDmg(15), ATTACK_MID, 10, 18, 12, 3.2f, false, isHeavy, 26.0f, -30.0f, 20.0f, 20.0f };
            SpawnProjectile(PROJ_ARC_BOMB, isHeavy ? 3.2f : 2.0f, -4.0f, 15);
            AudioEngine::Instance().PlaySfxPan(SFX_SELECT_CONFIRM, 1.2f, 1.0f, m_pos.x);
        } else if (motion == MOTION_DP_K) {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Paw Pogo", 4, 18, 12, ScaleDmg(16), ATTACK_OVERHEAD, 10, 20, 12, 3.5f, true, true, 22.0f, -8.0f, 26.0f, 26.0f };
            m_vel = Vector2{ m_facingRight ? 2.2f : -2.2f, -5.2f };
            AudioEngine::Instance().PlaySfxPan(SFX_WHOOSH_LIGHT, 1.4f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Furry Cannonball", 6, 24, 14, ScaleDmg(18), ATTACK_MID, 12, 24, 14, 4.5f, true, isHeavy, 30.0f, -24.0f, 30.0f, 28.0f };
            m_vel = Vector2{ m_facingRight ? 5.5f : -5.5f, 0.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_WHOOSH_HEAVY, 1.3f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_POPULIST:
        if (motion == MOTION_DP_K) {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Viral Clapback", 4, 12, 15, ScaleDmg(19), ATTACK_MID, 14, 26, 14, 4.2f, true, isHeavy, 28.0f, -38.0f, 30.0f, 32.0f, true, MOVE_SPECIAL, true };
            m_vel = Vector2{ m_facingRight ? 3.0f : -3.0f, -7.5f };
            AudioEngine::Instance().PlaySfxPan(SFX_UPPERCUT, 1.15f, 1.0f, m_pos.x);
        } else if (motion == MOTION_QCB_K) {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Livestream Dash", 2, 16, 8, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
            m_vel = Vector2{ m_facingRight ? 5.8f : -5.8f, 0.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_DASH, 1.1f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Selfie Flash", 7, 8, 15, ScaleDmg(13), ATTACK_MID, 14, 35, 12, 2.5f, false, false, 34.0f, -36.0f, 32.0f, 28.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_ELECTRO_GLITCH, 1.3f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_SLEEPY_JOE:
        if (motion == MOTION_DP_P || motion == MOTION_QCF_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Secret Service", 6, 14, 15, ScaleDmg(22), ATTACK_MID, 14, 28, 16, 6.0f, true, true, 48.0f, -28.0f, 44.0f, 32.0f };
            SpawnProjectile(PROJ_SECRET_SERVICE, isHeavy ? 9.5f : 7.8f, 0.0f, 22);
            AudioEngine::Instance().PlaySfxPan(SFX_SECRET_SERVICE, 1.0f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Executive Nap", 4, 46, 12, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
            m_health = std::min(100, m_health + 6);
        }
        break;

    case CHAR_MTG:
        if (motion == MOTION_QCF_P || motion == MOTION_DP_P) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Space Laser Beam", 8, 12, 15, ScaleDmg(18), ATTACK_PROJECTILE, 12, 22, 14, 4.0f, false, true, 40.0f, -34.0f, 36.0f, 24.0f };
            SpawnProjectile(PROJ_EYE_LASER, isHeavy ? 6.8f : 5.2f, 0.0f, 17);
            AudioEngine::Instance().PlaySfxPan(SFX_LASER_BEAM, 1.1f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Cut Off Nose", 4, 30, 12, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
            m_facingRight = !m_facingRight;
            m_health = std::max(1, m_health - 12);
            AddSuperMeter(40);
            AudioEngine::Instance().PlaySfxPan(SFX_NOSE_CUT, 1.0f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_SUCKRATERY_RKF:
        if (motion == MOTION_CHARGE_B_F_P || motion == MOTION_DP_P) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Tonic Toss", 9, 8, 14, ScaleDmg(16), ATTACK_PROJECTILE, 8, 16, 10, 3.0f, false, isHeavy, 35.0f, -28.0f, 20.0f, 20.0f };
            SpawnProjectile(PROJ_ARC_BOMB, isHeavy ? 3.2f : 2.2f, -3.5f, 16);
            AudioEngine::Instance().PlaySfxPan(SFX_HIT_LIGHT, 0.9f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Brain Worm Strike", 8, 14, 16, ScaleDmg(22), ATTACK_MID, 12, 24, 14, 4.5f, true, true, 65.0f, -32.0f, 70.0f, 28.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_WORM_HISS, 1.2f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_WAR_PIG:
        if (motion == MOTION_QCB_P || motion == MOTION_COMMAND_GRAB) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Ground Slam", 11, 12, 18, ScaleDmg(26), ATTACK_OVERHEAD, 16, 30, 18, 5.5f, true, true, 36.0f, -15.0f, 52.0f, 30.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_GAVEL_SLAM, 1.2f, 1.0f, m_pos.x);
        } else if (motion == MOTION_QCF_K || motion == MOTION_DP_P) {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "T-Refill", 10, 24, 14, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_INJECTION, 1.0f, 1.0f, m_pos.x);
            AudioEngine::Instance().PlaySfxPan(SFX_WAR_PIG_ROAR, 1.0f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Hammer Punch", 9, 10, 15, ScaleDmg(24), ATTACK_MID, 14, 26, 16, 5.0f, true, true, 44.0f, -32.0f, 40.0f, 32.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_WHOOSH_HEAVY, 1.1f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_SOCIALIST:
        if (motion == MOTION_QCB_P || motion == MOTION_QCB_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Cash Rain", 5, 16, 12, ScaleDmg(16), ATTACK_MID, 12, 24, 12, 3.8f, false, isHeavy, 40.0f, -30.0f, 38.0f, 26.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_COIN, 1.2f, 1.0f, m_pos.x);
        } else if (motion == MOTION_DP_P || motion == MOTION_QCF_K) {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Public Buffet", 8, 20, 15, ScaleDmg(20), ATTACK_OVERHEAD, 14, 26, 16, 4.8f, true, true, 46.0f, -28.0f, 44.0f, 30.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_GAVEL_SLAM, 1.1f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Grocery Swing", 6, 12, 14, ScaleDmg(18), ATTACK_MID, 10, 22, 14, 4.2f, true, isHeavy, 44.0f, -32.0f, 40.0f, 28.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_WHOOSH_HEAVY, 1.1f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_BILLARY:
        if (m_facingRight) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Saxophone Solo", 6, 20, 16, ScaleDmg(24), ATTACK_PROJECTILE, 14, 28, 16, 5.0f, true, true, 78.0f, -34.0f, 85.0f, 34.0f };
            SpawnProjectile(PROJ_SAX_NOTES, isHeavy ? 5.4f : 4.2f, 0.0f, 22);
            AudioEngine::Instance().PlaySfxPan(SFX_SAXOPHONE, 1.1f, 1.0f, m_pos.x);
        } else {
            if (motion == MOTION_CHARGE_B_F_P || motion == MOTION_QCF_K) {
                m_specialIndex = 1;
                m_currentAttack = AttackData{ "Gavel Slam", 4, 8, 10, ScaleDmg(15), ATTACK_MID, 8, 16, 10, 3.0f, false, false, 36.0f, -32.0f, 30.0f, 18.0f };
                AudioEngine::Instance().PlaySfxPan(SFX_WHOOSH_LIGHT, 1.1f, 1.0f, m_pos.x);
            } else {
                m_specialIndex = 3;
                m_currentAttack = AttackData{ "Typing Emails", 6, 20, 16, ScaleDmg(24), ATTACK_PROJECTILE, 14, 28, 16, 5.0f, true, true, 78.0f, -34.0f, 85.0f, 34.0f };
                SpawnProjectile(PROJ_SPARK, isHeavy ? 4.8f : 3.6f, 0.0f, 20);
                AudioEngine::Instance().PlaySfxPan(SFX_EMAIL_ZAP, 1.0f, 1.0f, m_pos.x);
            }
        }
        break;

    case CHAR_PALM_BEACH_PETE:
        if (motion == MOTION_DP_P || motion == MOTION_QCF_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Island Slap", 5, 10, 14, ScaleDmg(18), ATTACK_MID, 10, 20, 12, 3.8f, false, true, 38.0f, -32.0f, 34.0f, 22.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_COIN, 1.1f, 1.0f, m_pos.x);
        } else if (motion == MOTION_QCB_P || motion == MOTION_DP_K) {
            m_specialIndex = 3;
            m_currentAttack = AttackData{ "Adrenochrome Sip", 8, 24, 12, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_ADRENOCHROME, 1.0f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Gold Cane Strike", 6, 12, 14, ScaleDmg(22), ATTACK_MID, 12, 24, 14, 4.2f, true, true, 44.0f, -34.0f, 40.0f, 26.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_WHOOSH_HEAVY, 1.0f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_THE_PUNDIT:
        if (motion == MOTION_DP_P || motion == MOTION_DP_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Karl's Polling Slam", 5, 12, 15, ScaleDmg(20), ATTACK_OVERHEAD, 12, 26, 14, 4.4f, true, true, 42.0f, -36.0f, 40.0f, 30.0f };
            m_vel = Vector2{ m_facingRight ? 2.2f : -2.2f, -6.5f };
            AudioEngine::Instance().PlaySfxPan(SFX_GAVEL_SLAM, 1.15f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "DECISION 2028 MAP", 10, 20, 15, ScaleDmg(18), ATTACK_PROJECTILE, 12, 24, 14, 4.2f, true, true, 0.0f, 0.0f, 0.0f, 0.0f };
            SpawnProjectile(PROJ_POLLING_MAP, isHeavy ? 4.8f : 3.8f, -0.25f, 10);
            AudioEngine::Instance().PlaySfxPan(SFX_ELECTRO_GLITCH, 1.3f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_A_NEW_HOPE:
        if (motion == MOTION_DP_P || motion == MOTION_DP_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Windy City Dunk", 5, 12, 15, ScaleDmg(20), ATTACK_OVERHEAD, 12, 28, 14, 4.4f, true, true, 40.0f, -42.0f, 38.0f, 32.0f };
            m_vel = Vector2{ m_facingRight ? 2.6f : -2.6f, -7.2f };
            AudioEngine::Instance().PlaySfxPan(SFX_UPPERCUT, 1.1f, 1.0f, m_pos.x);
        } else {
            m_specialIndex = 1;
            m_currentAttack = AttackData{ "HOPE JUMP SHOT", 8, 14, 15, ScaleDmg(18), ATTACK_PROJECTILE, 12, 48, 14, 3.8f, false, true, 0.0f, 0.0f, 0.0f, 0.0f };
            float startX = m_pos.x + (m_facingRight ? 16.0f : -16.0f);
            float startY = m_pos.y - 74.0f;
            float dx = std::abs(opponent.GetPosition().x - startX);
            float flightFrames = std::clamp(dx / 4.2f, 22.0f, 36.0f);
            float vx = std::clamp(dx / flightFrames, 2.2f, 6.2f);
            float vy = (((opponent.GetPosition().y - 52.0f) - startY) - 0.5f * 0.24f * flightFrames * flightFrames) / flightFrames;
            SpawnProjectile(PROJ_HOPE_BASKETBALL, vx, vy, 16);
            m_activeTaunt = "How's that for hope and change?";
            m_tauntTimer = 90;
            AudioEngine::Instance().PlaySfxPan(SFX_FIREBALL_LAUNCH, 0.95f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_DOCTOR_ABDUL:
        if (motion == MOTION_QCB_K || motion == MOTION_QCB_P || motion == MOTION_DP_K || motion == MOTION_DOWN_DOWN) {

            m_specialIndex = 2;
            m_invincibleFrames = 16;
            m_currentAttack = AttackData{ "Shakira Law", 4, 22, 10, ScaleDmg(17), ATTACK_MID, 12, 26, 14, 5.2f, true, true, 26.0f, -32.0f, 44.0f, 36.0f };
            m_activeTaunt = "Shakira Law!";
            m_tauntTimer = 75;
            AudioEngine::Instance().PlaySfxPan(SFX_SHAKIRA_SHAKE, 1.0f, 1.0f, m_pos.x);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Shawarma Law", 8, 8, 14, ScaleDmg(16), ATTACK_PROJECTILE, 10, 20, 12, 3.8f, false, isHeavy, 34.0f, -38.0f, 24.0f, 18.0f };
            SpawnProjectile(PROJ_SHAWARMA, isHeavy ? 5.2f : 3.8f, 0.0f, 16);
            m_activeTaunt = "Shawarma Law!";
            m_tauntTimer = 75;
            AudioEngine::Instance().PlaySfxPan(SFX_SHAWARMA_TOSS, 1.0f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_BLEXAS:
        if (motion == MOTION_DP_P || motion == MOTION_DP_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Alamo Rising", 4, 14, 15, ScaleDmg(20), ATTACK_MID, 14, 28, 16, 4.8f, true, true, 30.0f, -44.0f, 34.0f, 40.0f, true, MOVE_SPECIAL, true };
            m_vel = Vector2{ m_facingRight ? 2.6f : -2.6f, -8.4f };
            AudioEngine::Instance().PlaySfxPan(SFX_UPPERCUT, 1.05f, 1.0f, m_pos.x);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Wheelchair Rollout", 9, 8, 15, ScaleDmg(18), ATTACK_PROJECTILE, 12, 24, 14, 4.6f, true, isHeavy, 38.0f, -28.0f, 28.0f, 26.0f };
            SpawnProjectile(PROJ_WHEELCHAIR, isHeavy ? 5.2f : 3.9f, 0.0f, 18);
            m_activeTaunt = "Here comes Hotwheels!";
            m_tauntTimer = 85;
            AudioEngine::Instance().PlaySfxPan(SFX_WHEELCHAIR_ROLL, 1.0f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_SI:
        if (motion == MOTION_DP_P || motion == MOTION_QCB_K) {
            m_specialIndex = 2;
            m_invincibleFrames = 14;
            m_currentAttack = AttackData{ "T-800 Cyber Crush", 4, 14, 14, ScaleDmg(20), ATTACK_MID, 14, 28, 16, 4.8f, true, true, 36.0f, -38.0f, 36.0f, 36.0f };
            m_vel = Vector2{ m_facingRight ? 4.2f : -4.2f, 0.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_ELECTRO_GLITCH, 1.15f, 1.0f, m_pos.x);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Bank Account Hack", 10, 14, 16, 0, ATTACK_PROJECTILE, 14, 28, 14, 4.0f, true, true, 0.0f, 0.0f, 0.0f, 0.0f };
            float targetX = std::clamp(opponent.GetPosition().x, STAGE_MIN_X + 20.0f, STAGE_MAX_X - 20.0f);
            SpawnProjectile(PROJ_ATM_HACK, 0.0f, 0.0f, 25, targetX, GROUND_Y - 28.0f);
            AudioEngine::Instance().PlaySfxPan(SFX_ATM_JACKPOT, 1.0f, 1.0f, targetX);
        }
        break;

    case CHAR_KIM:

        m_specialIndex = 1;
        m_armor = 1;
        m_currentAttack = AttackData{ "Supreme Wave", 20, 14, 16, ScaleDmg(22), ATTACK_MID, 14, 30, 16, 5.5f, true, true, 42.0f, -36.0f, 40.0f, 34.0f };
        AudioEngine::Instance().PlaySfxPan(SFX_SUPREME_WAVE, 0.95f, 1.0f, m_pos.x);
        break;

    case CHAR_BEBE:
        if (motion == MOTION_DP_P || motion == MOTION_DP_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Iron Dome Intercept", 4, 14, 15, ScaleDmg(20), ATTACK_MID, 14, 26, 16, 4.6f, true, true, 32.0f, -44.0f, 34.0f, 38.0f, true, MOVE_SPECIAL, true };
            m_vel = Vector2{ m_facingRight ? 2.2f : -2.2f, -8.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_LASER_BEAM, 1.25f, 1.0f, m_pos.x);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Optic Star Laser", 7, 10, 15, ScaleDmg(19), ATTACK_PROJECTILE, 12, 26, 14, 4.2f, false, true, 0.0f, 0.0f, 0.0f, 0.0f };
            SpawnProjectile(PROJ_EYE_LASER, isHeavy ? 7.8f : 6.2f, 0.0f, 19);
            AudioEngine::Instance().PlaySfxPan(SFX_LASER_BEAM, 1.0f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_PALE_MOTH:
        if (motion == MOTION_QCB_P || motion == MOTION_QCB_K || motion == MOTION_DP_P) {

            m_specialIndex = 2;
            m_multiHitsRemaining = 3;
            m_currentAttack = AttackData{ "Soviet Nunchuck Flurry", 5, 22, 14, ScaleDmg(8), ATTACK_MID, 10, 24, 14, 4.0f, true, true, 44.0f, -34.0f, 44.0f, 34.0f };
            m_vel = Vector2{ m_facingRight ? 3.2f : -3.2f, 0.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_HAMMER_CLANG, 1.0f, 1.0f, m_pos.x);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Hammer & Sickle Hurl", 8, 10, 15, ScaleDmg(19), ATTACK_PROJECTILE, 12, 24, 14, 4.4f, true, isHeavy, 36.0f, -36.0f, 28.0f, 24.0f };
            SpawnProjectile(PROJ_HAMMER_SICKLE, isHeavy ? 5.2f : 3.9f, 0.0f, 19);
            AudioEngine::Instance().PlaySfxPan(SFX_HAMMER_CLANG, 1.15f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_THE_DICTATOR:
        if (motion == MOTION_QCF_K || motion == MOTION_DP_K) {

            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Palace Boot Crush", 6, 14, 14, ScaleDmg(21), ATTACK_OVERHEAD, 14, 28, 16, 4.8f, true, true, 40.0f, -28.0f, 38.0f, 28.0f };
            m_vel = Vector2{ m_facingRight ? 4.4f : -4.4f, -3.2f };
            AudioEngine::Instance().PlaySfxPan(SFX_WHOOSH_HEAVY, 0.95f, 1.0f, m_pos.x);
        } else {

            m_specialIndex = 1;
            m_invincibleFrames = 56;
            m_currentAttack = AttackData{ "Spider Hole", 4, 46, 8, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_TRAPDOOR, 1.0f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_FLOTUS:
        if (motion == MOTION_DP_K || motion == MOTION_DP_P) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Stiletto High Kick", 4, 14, 14, ScaleDmg(19), ATTACK_MID, 13, 26, 14, 4.4f, true, true, 36.0f, -42.0f, 38.0f, 36.0f, true, MOVE_SPECIAL, true };
            m_vel = Vector2{ m_facingRight ? 2.4f : -2.4f, -7.8f };
            AudioEngine::Instance().PlaySfxPan(SFX_UPPERCUT, 1.15f, 1.0f, m_pos.x);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Moscow Spy Radio", 7, 14, 15, ScaleDmg(18), ATTACK_PROJECTILE, 12, 26, 14, 4.2f, true, isHeavy, 0.0f, 0.0f, 0.0f, 0.0f };
            SpawnProjectile(PROJ_SPY_RADIO, isHeavy ? 5.6f : 4.3f, 0.0f, 18);
            m_activeTaunt = (m_tauntCycle % 2 == 0) ? "For the Motherland!" : "Oh Vladimir!";
            m_tauntCycle++;
            m_tauntTimer = 85;
            AudioEngine::Instance().PlaySfxPan(SFX_SPY_RADIO, 1.0f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_W:
        if (motion == MOTION_DP_P || motion == MOTION_DP_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Crawford Brush Clear", 4, 13, 15, ScaleDmg(19), ATTACK_MID, 13, 26, 15, 4.5f, true, true, 32.0f, -42.0f, 34.0f, 38.0f, true, MOVE_SPECIAL, true };
            m_vel = Vector2{ m_facingRight ? 2.4f : -2.4f, -8.0f };
            AudioEngine::Instance().PlaySfxPan(SFX_UPPERCUT, 1.05f, 1.0f, m_pos.x);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Jenga Paper Plane", 8, 10, 14, ScaleDmg(18), ATTACK_PROJECTILE, 12, 26, 12, 4.0f, true, isHeavy, 0.0f, 0.0f, 0.0f, 0.0f };
            SpawnProjectile(PROJ_PAPER_PLANE, isHeavy ? 5.3f : 4.0f, -0.10f, 18);
            AudioEngine::Instance().PlaySfxPan(SFX_PAPER_PLANE, 1.0f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_DICK:
        if (motion == MOTION_DP_P || motion == MOTION_DP_K) {
            m_specialIndex = 2;
            m_currentAttack = AttackData{ "Cheney Buttstock Upper", 5, 13, 15, ScaleDmg(20), ATTACK_MID, 14, 28, 16, 4.8f, true, true, 34.0f, -42.0f, 36.0f, 36.0f, true, MOVE_SPECIAL, true };
            m_vel = Vector2{ m_facingRight ? 2.2f : -2.2f, -7.8f };
            AudioEngine::Instance().PlaySfxPan(SFX_HIT_HEAVY, 0.9f, 1.0f, m_pos.x);
        } else {

            m_specialIndex = 1;
            m_currentAttack = AttackData{ "Quail Hunt Headshot", 7, 10, 16, ScaleDmg(21), ATTACK_PROJECTILE, 14, 28, 15, 5.2f, true, true, 48.0f, -48.0f, 44.0f, 22.0f };
            SpawnProjectile(PROJ_SHOTGUN_BLAST, isHeavy ? 8.2f : 6.6f, 0.0f, 20);
            AudioEngine::Instance().PlaySfxPan(SFX_SHOTGUN_BLAST, 1.0f, 1.0f, m_pos.x);
        }
        break;

    case CHAR_AYATOLLAH:
        if (motion == MOTION_DP_P || motion == MOTION_DP_K) {
            m_specialIndex = 2;
            m_armor = 1;
            m_currentAttack = AttackData{ "Revolutionary Pillar", 5, 14, 15, ScaleDmg(21), ATTACK_MID, 14, 28, 16, 5.0f, true, true, 38.0f, -42.0f, 40.0f, 40.0f, true, MOVE_SPECIAL, true };
            m_vel = Vector2{ m_facingRight ? 2.4f : -2.4f, -8.2f };
            AudioEngine::Instance().PlaySfxPan(SFX_HORMUZ_SIREN, 1.15f, 1.0f, m_pos.x);
        } else {

            m_specialIndex = 1;
            m_armor = 1;
            m_currentAttack = AttackData{ "Close Strait of Hormuz", 9, 14, 16, ScaleDmg(22), ATTACK_PROJECTILE, 14, 30, 16, 5.6f, true, true, 44.0f, -28.0f, 44.0f, 40.0f };
            SpawnProjectile(PROJ_HORMUZ_BLOCKADE, isHeavy ? 4.8f : 3.7f, 0.0f, 22);
            AudioEngine::Instance().PlaySfxPan(SFX_HORMUZ_SIREN, 1.0f, 1.0f, m_pos.x);
        }
        break;

    default:
        break;
    }

    if (m_tauntTimer <= 0) {
        m_activeTaunt = GetTauntQuote();
        m_tauntTimer = 65;
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
    int scaledDmg = std::max(1, (int)roundf(damage * m_damageScale));
    m_currentAttack = AttackData{ name, startup, active, recovery, scaledDmg, type, hitstop, hitstun, blockstun, push, false, heavy, reachX, reachY, bw, bh, false, moveType, false };
    AddSuperMeter(heavy ? 5 : 3);
    AudioEngine::Instance().PlaySfxPan(heavy ? SFX_WHOOSH_HEAVY : SFX_WHOOSH_LIGHT, 1.0f, 1.0f, m_pos.x);
}

void Fighter::TriggerSignatureSpecial(const Fighter& opponent) {
    if (m_state == FSTATE_HITSTUN || m_state == FSTATE_BLOCKSTUN ||
        m_state == FSTATE_KNOCKDOWN_AIR || m_state == FSTATE_KNOCKDOWN_GROUND ||
        m_state == FSTATE_WAKEUP || m_state == FSTATE_DEFEAT || m_state == FSTATE_VICTORY ||
        m_state == FSTATE_FINISH_HIM_DIZZY) {
        return;
    }

    switch (m_charId) {
    case CHAR_COASTAL_GOV: ExecuteSpecial(MOTION_QCF_K, BTN_HK, opponent); break;
    case CHAR_FILIBUSTER:  ExecuteSpecial(MOTION_CHARGE_B_F_P, BTN_HP, opponent); break;
    case CHAR_MASCOT:      ExecuteSpecial(MOTION_CHARGE_B_F_K, BTN_HK, opponent); break;
    case CHAR_THE_DICTATOR:ExecuteSpecial(MOTION_DOWN_DOWN, BTN_HP, opponent); break;
    default:               ExecuteSpecial(MOTION_QCF_P, BTN_HP, opponent); break;
    }
}

void Fighter::TriggerTaunt() {
    if (m_state == FSTATE_HITSTUN || m_state == FSTATE_BLOCKSTUN ||
        m_state == FSTATE_KNOCKDOWN_AIR || m_state == FSTATE_KNOCKDOWN_GROUND ||
        m_state == FSTATE_WAKEUP || m_state == FSTATE_DEFEAT || m_state == FSTATE_VICTORY ||
        m_state == FSTATE_ATTACK || m_state == FSTATE_SPECIAL || m_state == FSTATE_FINISH_HIM_DIZZY) {
        return;
    }
    m_state = FSTATE_TAUNT;
    m_stateFrame = 0;
    m_hitConnected = false;
    m_tauntTimer = 95;
    m_activeTaunt = GetTauntQuote();
    AddSuperMeter(12);

    switch (m_charId) {
    case CHAR_COMMANDER:        AudioEngine::Instance().PlaySfxPan(SFX_FART, 1.0f, 1.0f, m_pos.x); break;
    case CHAR_HIGH_BENCH:       AudioEngine::Instance().PlaySfxPan(SFX_VOICE_OBJECTION, 1.0f, 1.0f, m_pos.x); break;
    case CHAR_SLEEPY_JOE:       AudioEngine::Instance().PlaySfxPan(SFX_VOICE_BOO_YAA, 1.0f, 1.0f, m_pos.x); break;
    case CHAR_MTG:              AudioEngine::Instance().PlaySfxPan(SFX_MEGAPHONE, 1.1f, 1.0f, m_pos.x); break;
    case CHAR_WAR_PIG:          AudioEngine::Instance().PlaySfxPan(SFX_WAR_PIG_ROAR, 1.1f, 1.0f, m_pos.x); break;
    case CHAR_SUCKRATERY_RKF:   AudioEngine::Instance().PlaySfxPan(SFX_WORM_HISS, 1.0f, 1.0f, m_pos.x); break;
    case CHAR_TECH_VISIONARY:   AudioEngine::Instance().PlaySfxPan(SFX_ELECTRO_GLITCH, 1.2f, 1.0f, m_pos.x); break;
    case CHAR_BILLARY:          AudioEngine::Instance().PlaySfxPan(SFX_EMAIL_ZAP, 1.0f, 1.0f, m_pos.x); break;
    case CHAR_PALM_BEACH_PETE:  AudioEngine::Instance().PlaySfxPan(SFX_ADRENOCHROME, 1.3f, 1.0f, m_pos.x); break;
    case CHAR_DOCTOR_ABDUL:     AudioEngine::Instance().PlaySfxPan(SFX_SHAKIRA_SHAKE, 1.05f, 1.0f, m_pos.x); break;
    case CHAR_COUCHIE:          AudioEngine::Instance().PlaySfxPan(SFX_COUCH_SPRING, 1.1f, 1.0f, m_pos.x); break;
    case CHAR_BLEXAS:           AudioEngine::Instance().PlaySfxPan(SFX_SELECT_CONFIRM, 1.05f, 1.0f, m_pos.x); break;
    case CHAR_SI:               AudioEngine::Instance().PlaySfxPan(SFX_ATM_JACKPOT, 1.15f, 1.0f, m_pos.x); break;
    case CHAR_PALE_MOTH:        AudioEngine::Instance().PlaySfxPan(SFX_HAMMER_CLANG, 0.95f, 1.0f, m_pos.x); break;
    case CHAR_FLOTUS:           AudioEngine::Instance().PlaySfxPan(SFX_SPY_RADIO, 1.05f, 1.0f, m_pos.x); break;
    case CHAR_W:                AudioEngine::Instance().PlaySfxPan(SFX_PAPER_PLANE, 1.05f, 1.0f, m_pos.x); break;
    case CHAR_DICK:             m_laughTimer = 40; AudioEngine::Instance().PlaySfxPan(SFX_DICK_LAUGH, 1.0f, 1.0f, m_pos.x); break;
    case CHAR_AYATOLLAH:        AudioEngine::Instance().PlaySfxPan(SFX_HORMUZ_SIREN, 1.05f, 1.0f, m_pos.x); break;
    default:                    AudioEngine::Instance().PlaySfxPan(SFX_SELECT_CURSOR, 0.9f, 1.0f, m_pos.x); break;
    }
}

bool Fighter::HasSpeechTaunt() const {
    return true;
}

const char* Fighter::GetTauntQuote() const {
    int c = m_tauntCycle++;
    switch (m_charId) {
    case CHAR_COMMANDER: {
        static const char* q[] = { "YOU'RE VETOED!", "NOBODY FIGHTS BETTER!", "FAKE BLOCKSTUN!", "TREMENDOUS COMBO!" };
        return q[c % 4];
    }
    case CHAR_HIGH_BENCH: {
        switch (GetHighBenchStage()) {
        case HB_STAGE_THOMAS: {
            static const char* q[] = { "is this a hair in my cola?", "CHECK MY RV PARKING!", "OVERRULED!" };
            return q[c % 3];
        }
        case HB_STAGE_ROBERTS: {
            static const char* q[] = { "Jesus says NO MORE ROE!", "ORDER IN THE COURT!", "6-3 MAJORITY!" };
            return q[c % 3];
        }
        case HB_STAGE_SOTOMAYOR: {
            static const char* q[] = { "I DISSENT!", "READ THE CONSTITUTION!", "WISE LATINA UPPERCUT!" };
            return q[c % 3];
        }
        }
        return "OVERRULED!";
    }
    case CHAR_REDCAP: {
        static const char* q[] = { "HOLD THE LINE!", "STOP THE COUNT!", "DO YOUR OWN RESEARCH!", "PATRIOT HAYMAKER!" };
        return q[c % 4];
    }
    case CHAR_COASTAL_GOV: {
        static const char* q[] = { "TOO EASY!", "CHECK THE HAIR GEL!", "FRENCH LAUNDRY SPECIAL!", "WEST COAST SWEEP!" };
        return q[c % 4];
    }
    case CHAR_TECH_VISIONARY: {
        static const char* q[] = { "TO THE MOON!", "CONCERNING. LOOKING INTO IT.", "FIRST PRINCIPLES PUNCH!", "420 HIT COMBO!" };
        return q[c % 4];
    }
    case CHAR_FILIBUSTER: {
        static const char* q[] = { "I YIELD NOTHING!", "READING THE PHONE BOOK!", "CLOTURE DENIED!", "HOUR 19 AND COUNTING!" };
        return q[c % 4];
    }
    case CHAR_MASCOT: {
        static const char* q[] = { "SCREEEECH!", "FREEDOM FEATHERS!", "CAW OF LIBERTY!", "EAGLE DIVE!" };
        return q[c % 4];
    }
    case CHAR_POPULIST: {
        static const char* q[] = { "SMILE! :)", "CLIP THAT CHAT!", "RATIOED IN REAL TIME!", "TAX THE RICH!" };
        return q[c % 4];
    }
    case CHAR_SLEEPY_JOE: {
        static const char* q[] = { "this is a big fkin deal.", "Hunter's Laptop!", "NO MALARKEY, JACK!", "LISTEN HERE, FAT!" };
        return q[c % 4];
    }
    case CHAR_MTG: {
        static const char* q[] = { "SPACE LASERS!", "TRUST THE PLAN!", "WWG1WGA!", "GAZPACHO POLICE!" };
        return q[c % 4];
    }
    case CHAR_SUCKRATERY_RKF: {
        static const char* q[] = { "BRAIN WORM POWER!", "RAW MILK ENERGY!", "BEAR CUB IN CENTRAL PARK!", "MAHA UPPERCUT!" };
        return q[c % 4];
    }
    case CHAR_WAR_PIG: {
        static const char* q[] = { "BRING IT!", "DEFENSE BUDGET APPROVED!", "PENTAGON BENCH PRESS!", "NO RULES OF ENGAGEMENT!" };
        return q[c % 4];
    }
    case CHAR_SOCIALIST: {
        static const char* q[] = { "TAX THE BILLIONAIRES!", "TOP ONE PERCENT!", "MEDICARE FOR YOUR JAW!", "ONCE AGAIN I AM PUNCHING!" };
        return q[c % 4];
    }
    case CHAR_BILLARY: {
        if (m_facingRight) {
            static const char* q[] = { "OH YEAH!", "I FEEL YOUR PAIN!", "ARSENIO HALL SOLO!" };
            return q[c % 3];
        } else {
            static const char* q[] = { "DELETED!", "POKEMON GO TO SLEEP!", "BLEACHBIT COMBO!" };
            return q[c % 3];
        }
    }
    case CHAR_PALM_BEACH_PETE: {
        static const char* q[] = { "WELCOME TO MY ISLAND!", "FLIGHT LOG SEALED!", "VIP CABANA SLAP!", "CHECK THE CLIENT LIST!" };
        return q[c % 4];
    }
    case CHAR_THE_PUNDIT: {
        static const char* q[] = { "DECISION 2028: 270!", "CHECK THE WHITEBOARD!", "SUBURBAN TURNOUT SURGE!", "TOO CLOSE TO CALL!" };
        return q[c % 4];
    }
    case CHAR_A_NEW_HOPE: {
        static const char* q[] = { "How's that for hope and change?", "UHH, LET ME BE CLEAR!", "YES WE CAN!", "NOTHING BUT NET!" };
        return q[c % 4];
    }

    case CHAR_DOCTOR_ABDUL: {
        static const char* q[] = { "Shawarma Law!", "Shakira Law!", "Shawarma Law!", "Shakira Law!", "Motor City Rx!" };
        return q[c % 5];
    }
    case CHAR_COUCHIE: {
        static const char* q[] = { "...got a glove, bro?", "...got a glove, bro?", "Whatever makes sense.", "...got a glove, bro?" };
        return q[c % 4];
    }
    case CHAR_BLEXAS: {
        static const char* q[] = { "Remember the Alamo", "Ye shall know the truth", "The truth shall make you free" };
        return q[c % 3];
    }
    case CHAR_SI: {
        static const char* q[] = {
            "Invest in the Future, Invest in Kalshi",
            "Chooms love data.",
            "Your balance is $0.00.",
            "Invest in the Future, Invest in Kalshi"
        };
        return q[c % 4];
    }
    case CHAR_KIM: {
        static const char* q[] = { "Homorseskual", "Ju Che!", "Homorseskual", "Ju Che!" };
        return q[c % 4];
    }
    case CHAR_BEBE: {
        static const char* q[] = {
            "they could already have a nuclear weapon!",
            "nice country, i think I'll take it.",
            "they could already have a nuclear weapon!"
        };
        return q[c % 3];
    }
    case CHAR_PALE_MOTH: {
        static const char* q[] = { "If he dies he dies,", "Blyat", "If he dies he dies,", "Blyat" };
        return q[c % 4];
    }
    case CHAR_THE_DICTATOR: {
        static const char* q[] = { "God is great", "the nation will be victorious", "God is great" };
        return q[c % 3];
    }
    case CHAR_FLOTUS: {
        static const char* q[] = { "I don't really care, do you?", "For the Motherland!", "Oh Vladimir!" };
        return q[c % 3];
    }
    case CHAR_W: {
        static const char* q[] = { "Can't get fooled again", "Don't let the terrorist win, go shopping!" };
        return q[c % 2];
    }
    case CHAR_DICK: {
        static const char* q[] = { "Weapons of Mass Destruction!", "Wyoming must be free." };
        return q[c % 2];
    }
    case CHAR_AYATOLLAH: {
        static const char* q[] = { "@AR0", "@AR1", "@AR2" };
        return q[c % 3];
    }
    default:
        return "";
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

        float dist = std::abs(m_pos.x - opponent.GetPosition().x);
        bool closeThrowRange = (dist <= 46.0f && !opponent.IsAirborne());

        if (btn == (BTN_LP | BTN_LK) || (btn == BTN_HP && !crouch && closeThrowRange && m_state == FSTATE_WALK_FWD)) {
            StartNormalAttack(MOVE_THROW, "Executive Throw", 3, 4, 14, 20, ATTACK_THROW, 14, 28, 0, 5.4f, true, 26.0f, -30.0f, 28.0f, 32.0f);
            m_currentAttack.causesKnockdown = true;
            m_throwTechWindow = 7;
            return true;
        } else if (btn == BTN_LP) {
            const char* name = crouch ? "Crouch Jab" : "Stand Jab";
            if (crouch) StartNormalAttack(MOVE_LIGHT_PUNCH, name, 3, 3, 4, 6, ATTACK_MID, 6, 12, 8, 2.0f, false, 30.0f, -14.0f, 24.0f, 14.0f);
            else        StartNormalAttack(MOVE_LIGHT_PUNCH, name, 3, 3, 5, 7, ATTACK_MID, 6, 13, 8, 2.2f, false, 34.0f, -34.0f, 26.0f, 14.0f);
            return true;
        } else if (btn == BTN_HP) {
            if (crouch) {

                StartNormalAttack(MOVE_HEAVY_PUNCH, "Launcher Upper", 5, 5, 12, 16, ATTACK_MID, 11, 26, 14, 2.4f, true, 32.0f, -36.0f, 30.0f, 34.0f);
                m_currentAttack.isLauncher = true;
            } else {
                StartNormalAttack(MOVE_HEAVY_PUNCH, "Stand Fierce", 5, 4, 11, 18, ATTACK_OVERHEAD, 11, 24, 16, 4.2f, true, 38.0f, -36.0f, 34.0f, 20.0f);
            }
            return true;
        } else if (btn == BTN_LK) {
            const char* name = crouch ? "Crouch Short" : "Stand Short";
            if (crouch) StartNormalAttack(MOVE_LIGHT_KICK, name, 3, 3, 5, 7, ATTACK_LOW, 6, 12, 8, 2.0f, false, 34.0f, -8.0f, 28.0f, 14.0f);
            else        StartNormalAttack(MOVE_LIGHT_KICK, name, 4, 3, 5, 8, ATTACK_MID, 6, 13, 9, 2.4f, false, 36.0f, -22.0f, 28.0f, 14.0f);
            return true;
        } else if (btn == BTN_HK) {
            if (crouch) {
                StartNormalAttack(MOVE_HEAVY_KICK, "Low Roundhouse Sweep", 5, 5, 13, 17, ATTACK_LOW, 11, 24, 14, 4.5f, true, 40.0f, -8.0f, 38.0f, 16.0f);
                m_currentAttack.causesKnockdown = true;
            } else {
                StartNormalAttack(MOVE_HEAVY_KICK, "Stand Roundhouse", 6, 4, 12, 18, ATTACK_MID, 11, 25, 15, 4.4f, true, 38.0f, -38.0f, 32.0f, 22.0f);
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
        m_state == FSTATE_CALL_TO_PRAYER ||
        m_state == FSTATE_FINISHER || m_state == FSTATE_FINISHER_VICTIM ||
        m_state == FSTATE_DASH_FWD || m_state == FSTATE_FINISH_HIM_DIZZY ||
        m_state == FSTATE_VICTORY || m_state == FSTATE_DEFEAT) {
        return false;
    }
    if (type == ATTACK_THROW || IsAirborne()) {
        return false;
    }
    bool lowGuard = m_isHoldingDownBack || (m_isHoldingBlockButton && IsCrouching()) || (m_state == FSTATE_BLOCK_LOW);
    bool highGuard = m_isHoldingBack || (m_isHoldingBlockButton && !IsCrouching()) || (m_state == FSTATE_BLOCK_HIGH);
    if (m_state == FSTATE_BLOCK_HIGH || m_state == FSTATE_BLOCK_LOW || m_state == FSTATE_BLOCKSTUN) {
        if (type == ATTACK_LOW) return lowGuard;
        if (type == ATTACK_OVERHEAD) return highGuard;
        return true;
    }
    if (type == ATTACK_LOW) return lowGuard;
    if (type == ATTACK_OVERHEAD) return highGuard;
    return lowGuard || highGuard;
}

void Fighter::ApplyThrowTech(bool pushRight) {
    m_state = FSTATE_IDLE;
    m_stateFrame = 0;
    m_hitstopFrames = 8;
    m_pushbackVel = pushRight ? 5.2f : -5.2f;
    m_justThrowTeched = true;
    AddSuperMeter(15);
    AudioEngine::Instance().PlaySfxPan(SFX_THROW_TECH, 1.0f, 1.0f, m_pos.x);
}

void Fighter::ApplyHit(int damage, AttackType type, int hitstop, int hitstun, float pushback, bool knockdown, bool launcher) {
    (void)type;
    if (m_invincibleFrames > 0 || IsSpiderHoleHidden()) {
        return;
    }

    if (m_charId == CHAR_SLEEPY_JOE && m_state == FSTATE_SPECIAL && m_specialIndex == 1) {
        AudioEngine::Instance().PlaySfxPan(SFX_SECRET_SERVICE, 1.0f, 1.0f, m_pos.x);
        SpawnProjectile(PROJ_SECRET_SERVICE, m_facingRight ? 9.5f : -9.5f, 0.0f, 25);
        m_state = FSTATE_WAKEUP;
        m_stateFrame = 0;
        return;
    }

    if (m_armor > 0) {
        m_armor--;
        AudioEngine::Instance().PlaySfxPan(SFX_BLOCK, 0.8f, 1.0f, m_pos.x);
        return;
    }

    int finalDmg = (damage == 25) ? 25 : std::max(1, (int)roundf((float)damage / m_defenseScale));
    int finalStun = (m_defenseScale > 1.1f) ? std::max(7, (int)roundf(hitstun * 0.80f)) : hitstun;

    m_health = std::max(0, m_health - finalDmg);
    m_hitstopFrames = hitstop;
    m_laughTimer = 0;
    m_pushbackVel = m_facingRight ? -pushback : pushback;
    AddSuperMeter(12);

    if (m_charId == CHAR_WAR_PIG && m_refillCount < 2 && m_health <= 10) {
        m_refillCount++;
        m_health = std::min(100, m_health + 80);
        m_state = FSTATE_SPECIAL;
        m_stateFrame = 0;
        m_specialIndex = 3;
        m_currentAttack = AttackData{ "T-Refill Surge", 8, 30, 12, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f, true, MOVE_SPECIAL };
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
        m_currentAttack = AttackData{ "Adrenochrome Surge", 8, 30, 12, 0, ATTACK_NONE, 0, 0, 0, 0.0f, false, false, 0.0f, 0.0f, 0.0f, 0.0f, true, MOVE_SPECIAL };
        m_armor = 3;
        m_hitstopFrames = 18;
        m_pushbackVel = 0.0f;
        AudioEngine::Instance().PlaySfx(SFX_ADRENOCHROME, 1.0f);
        return;
    }

    if (TryTriggerAyatollahPrayer()) {
        return;
    }

    if (m_health <= 0) {
        m_state = FSTATE_KNOCKDOWN_AIR;
        m_stateFrame = 0;
        m_tauntTimer = 0;
        m_activeTaunt = "";
        m_cosmeticPuffs.clear();
        m_vel = Vector2{ m_facingRight ? -3.2f : 3.2f, -7.2f };
        return;
    }

    if (m_state == FSTATE_KNOCKDOWN_AIR) {

        m_juggleHits++;
        m_stateFrame = 0;
        m_vel = Vector2{ m_facingRight ? -2.4f : 2.4f, -6.2f };
    } else if (launcher) {
        m_state = FSTATE_KNOCKDOWN_AIR;
        m_stateFrame = 0;
        m_juggleHits = 0;
        m_vel = Vector2{ m_facingRight ? -1.4f : 1.4f, -8.5f };
    } else if (knockdown || IsAirborne()) {
        m_state = FSTATE_KNOCKDOWN_AIR;
        m_stateFrame = 0;
        m_juggleHits = 1;
        m_vel = Vector2{ m_facingRight ? -2.8f : 2.8f, -6.5f };
    } else {
        m_state = FSTATE_HITSTUN;
        m_stateFrame = 0;
        m_hitstunFrames = finalStun;
    }
}

void Fighter::ApplyBlock(int chipDamage, int blockstop, int blockstun, float pushback) {
    if (chipDamage > 0) {
        m_health = std::max(1, m_health - chipDamage);
    }
    if (TryTriggerAyatollahPrayer()) {
        return;
    }
    m_hitstopFrames = blockstop;
    m_blockstunFrames = blockstun;
    m_pushbackVel = m_facingRight ? -pushback * 1.25f : pushback * 1.25f;
    bool lowGuard = m_isHoldingDownBack || (m_isHoldingBlockButton && IsCrouching());
    m_state = lowGuard ? FSTATE_BLOCK_LOW : FSTATE_BLOCK_HIGH;
    m_stateFrame = 0;
    AddSuperMeter(8);
}

void Fighter::UpdatePhysics(const InputManager& input, const Fighter& opponent) {
    if (m_buffer.timer > 0) {
        m_buffer.timer--;
        if (m_buffer.timer <= 0) m_buffer = BufferedInput{};
    }
    if (m_throwTechWindow > 0) m_throwTechWindow--;
    if (m_starBurstTimer > 0) m_starBurstTimer--;
    if (m_electroShockTimer > 0) m_electroShockTimer--;
    if (m_jengaTimer > 0) m_jengaTimer--;
    if (m_laughTimer > 0) m_laughTimer--;

    if (m_tauntTimer > 0) {
        m_tauntTimer--;
        if (m_tauntTimer <= 0) m_activeTaunt = "";
    }

    for (auto& p : m_projectiles) {
        if (!p.active) continue;
        p.age++;
        p.pos.x += p.vel.x;
        p.pos.y += p.vel.y;

        if (p.type == PROJ_ARC_BOMB) {
            p.vel.y += 0.35f;
            if (p.pos.y >= p.bounceY) {
                p.pos.y = p.bounceY;
                p.vel.y = -p.vel.y * 0.65f;
            }
        } else if (p.type == PROJ_HOPE_BASKETBALL) {
            p.vel.y += 0.24f;
            if (p.pos.y >= p.bounceY) {
                p.pos.y = p.bounceY;
                p.vel.y = -std::abs(p.vel.y) * 0.68f;
            }
        } else if (p.type == PROJ_PAPER_PLANE) {

            p.pos.y += sinf((float)p.age * 0.22f) * 0.65f;
        }

        p.lifetime--;
        if (p.lifetime <= 0 || p.pos.x < STAGE_MIN_X - 70.0f || p.pos.x > STAGE_MAX_X + 70.0f) {
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
            m_juggleHits = 0;
            if (m_state == FSTATE_JUMP || m_state == FSTATE_ATTACK || m_state == FSTATE_DASH_BACK) {
                m_state = FSTATE_IDLE;
                m_stateFrame = 0;
                TryExecuteBufferedMove(opponent);
            } else if (m_state == FSTATE_KNOCKDOWN_AIR) {
                m_state = FSTATE_KNOCKDOWN_GROUND;
                m_stateFrame = 0;
                AudioEngine::Instance().PlaySfxPan(SFX_HIT_HEAVY, 0.7f, 0.8f, m_pos.x);
            }
        }
    } else {
        m_pos.y = GROUND_Y;
        m_vel.y = 0.0f;
        if (m_state != FSTATE_DASH_FWD && m_state != FSTATE_DASH_BACK) {
            m_vel.x = 0.0f;
        }

        if (m_state != FSTATE_ATTACK && m_state != FSTATE_SPECIAL &&
            m_state != FSTATE_CALL_TO_PRAYER &&
            m_state != FSTATE_FINISHER && m_state != FSTATE_FINISHER_VICTIM &&
            m_state != FSTATE_DASH_FWD && m_state != FSTATE_DASH_BACK &&
            m_state != FSTATE_HITSTUN && m_state != FSTATE_BLOCKSTUN &&
            m_state != FSTATE_BLOCK_HIGH && m_state != FSTATE_BLOCK_LOW &&
            m_state != FSTATE_KNOCKDOWN_AIR && m_state != FSTATE_KNOCKDOWN_GROUND &&
            m_state != FSTATE_FINISH_HIM_DIZZY && m_state != FSTATE_WAKEUP) {
            m_facingRight = (m_pos.x < opponent.GetPosition().x);
        }
    }

    if (m_state == FSTATE_FINISH_HIM_DIZZY || m_state == FSTATE_FINISHER_VICTIM) {
        m_stateFrame++;
        m_pos.x = std::clamp(m_pos.x, STAGE_MIN_X, STAGE_MAX_X);
        return;
    }

    if (m_state == FSTATE_FINISHER) {
        m_stateFrame++;
        m_vel = Vector2{ 0.0f, 0.0f };
        m_pos.y = GROUND_Y;
        if (m_stateFrame == 18) {
            switch (m_charId) {
            case CHAR_HIGH_BENCH: case CHAR_FILIBUSTER: case CHAR_BILLARY:
                AudioEngine::Instance().PlaySfx(SFX_GAVEL_SLAM, 0.85f, 1.0f); break;
            case CHAR_MTG: case CHAR_SLEEPY_JOE: case CHAR_BEBE: case CHAR_POPULIST:
                AudioEngine::Instance().PlaySfx(SFX_LASER_BEAM, 0.95f, 1.0f); break;
            case CHAR_FLOTUS:
                AudioEngine::Instance().PlaySfx(SFX_SPY_RADIO, 1.0f, 1.0f); break;
            case CHAR_DICK:
                AudioEngine::Instance().PlaySfx(SFX_SHOTGUN_BLAST, 0.9f, 1.0f); break;
            case CHAR_AYATOLLAH:
                AudioEngine::Instance().PlaySfx(SFX_HORMUZ_SIREN, 1.0f, 1.0f); break;
            case CHAR_SI:
                AudioEngine::Instance().PlaySfx(SFX_ATM_JACKPOT, 0.9f, 1.0f); break;
            case CHAR_DOCTOR_ABDUL:
                AudioEngine::Instance().PlaySfx(SFX_SHAWARMA_TOSS, 0.85f, 1.0f); break;
            default:
                AudioEngine::Instance().PlaySfx(SFX_SUPER_FREEZE, 1.05f, 0.95f); break;
            }
        }
        m_pos.x = std::clamp(m_pos.x, STAGE_MIN_X, STAGE_MAX_X);
        return;
    }

    RelativeDirection rel = input.GetCurrentRelDir();
    m_isHoldingBack = (rel == DIR_REL_BACK || rel == DIR_REL_UP_BACK);
    m_isHoldingDownBack = (rel == DIR_REL_DOWN_BACK);
    m_isHoldingBlockButton = input.IsButtonHeld(BTN_BLOCK);
    bool crouching = (rel == DIR_REL_DOWN || rel == DIR_REL_DOWN_BACK || rel == DIR_REL_DOWN_FORWARD);

    if (input.IsThrowInputPressed()) {
        m_throwTechWindow = 8;
    }

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

    if (input.IsThrowInputPressed()) {
        m_buffer.button = (BTN_LP | BTN_LK);
        m_buffer.isCrouch = false;
        m_buffer.timer = 10;
    } else if (input.IsButtonPressed(BTN_LP)) {
        m_buffer.button = BTN_LP; m_buffer.isCrouch = crouching; m_buffer.timer = 10;
    } else if (input.IsButtonPressed(BTN_HP)) {
        m_buffer.button = BTN_HP; m_buffer.isCrouch = crouching; m_buffer.timer = 10;
    } else if (input.IsButtonPressed(BTN_LK)) {
        m_buffer.button = BTN_LK; m_buffer.isCrouch = crouching; m_buffer.timer = 10;
    } else if (input.IsButtonPressed(BTN_HK)) {
        m_buffer.button = BTN_HK; m_buffer.isCrouch = crouching; m_buffer.timer = 10;
    }

    if (input.IsButtonPressed(BTN_SPECIAL)) {
        bool wantSuper = crouching && CanUseSuper() && m_charId != CHAR_COUCHIE;
        if (grounded && (m_state == FSTATE_IDLE || m_state == FSTATE_WALK_FWD || m_state == FSTATE_WALK_BACK || m_state == FSTATE_CROUCH || m_state == FSTATE_DASH_FWD)) {
            if (wantSuper) ExecuteSpecial(MOTION_SUPER_OMNIBUS, BTN_HP, opponent);
            else TriggerSignatureSpecial(opponent);
        } else if (m_state == FSTATE_ATTACK && (m_hitConnected || m_stateFrame < m_currentAttack.startupFrames + m_currentAttack.activeFrames + 5)) {
            m_justCanceled = true;
            if (wantSuper) ExecuteSpecial(MOTION_SUPER_OMNIBUS, BTN_HP, opponent);
            else TriggerSignatureSpecial(opponent);
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

        if (m_charId == CHAR_DICK && m_laughTimer > 0) {
            m_state = FSTATE_IDLE;
            m_vel.x = 0.0f;
            break;
        }

        int dashDir = input.CheckDashInput();
        if (dashDir == 1 && !crouching) {
            m_state = FSTATE_DASH_FWD;
            m_stateFrame = 0;
            m_vel.x = m_facingRight ? 5.0f : -5.0f;
            AudioEngine::Instance().PlaySfxPan(SFX_DASH, 1.05f, 0.9f, m_pos.x);
            break;
        } else if (dashDir == -1 && !crouching) {
            m_state = FSTATE_DASH_BACK;
            m_stateFrame = 0;
            m_invincibleFrames = 8;
            m_vel.x = m_facingRight ? -4.8f : 4.8f;
            m_vel.y = -2.8f;
            m_pos.y += m_vel.y;
            AudioEngine::Instance().PlaySfxPan(SFX_DASH, 0.92f, 0.9f, m_pos.x);
            break;
        }

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
            m_vel.x = jumpFwd ? (m_facingRight ? m_jumpSpeedX : -m_jumpSpeedX)
                              : (jumpBack ? (m_facingRight ? -m_jumpSpeedX : m_jumpSpeedX) : 0.0f);
            m_pos.y += m_vel.y;
            m_pos.x += m_vel.x;
            AudioEngine::Instance().PlaySfxPan(SFX_WHOOSH_LIGHT, 1.25f, 0.85f, m_pos.x);
            break;
        }

        if (TryExecuteBufferedMove(opponent)) {
            break;
        }

        if (m_isHoldingBlockButton) {
            m_state = crouching ? FSTATE_BLOCK_LOW : FSTATE_BLOCK_HIGH;
            m_blockstunFrames = 0;
            m_vel.x = 0.0f;
        } else if (crouching) {
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

    case FSTATE_DASH_FWD: {
        m_pos.x += m_vel.x;
        m_vel.x *= 0.90f;
        if (m_stateFrame >= 5 && TryExecuteBufferedMove(opponent)) {
            break;
        }
        if (m_stateFrame >= 12) {
            m_state = FSTATE_IDLE;
            m_stateFrame = 0;
            m_vel.x = 0.0f;
        }
        break;
    }

    case FSTATE_DASH_BACK: {
        if (grounded && m_stateFrame >= 13) {
            m_state = FSTATE_IDLE;
            m_stateFrame = 0;
            m_vel.x = 0.0f;
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
            int dmg = std::max(1, (int)roundf((heavy ? 16 : 9) * m_damageScale));
            m_currentAttack = AttackData{ heavy ? "Jump Heavy" : "Jump Light", 4, 12, 6, dmg, ATTACK_OVERHEAD, heavy ? 10 : 6, heavy ? 22 : 13, 10, 3.2f, false, heavy, 30.0f, -24.0f, 30.0f, 24.0f, false, mtype };
            AudioEngine::Instance().PlaySfxPan(heavy ? SFX_WHOOSH_HEAVY : SFX_WHOOSH_LIGHT, 1.0f, 1.0f, m_pos.x);
        }
        break;
    }

    case FSTATE_ATTACK: {

        if (m_buffer.motion != MOTION_NONE && (m_hitConnected || m_stateFrame < m_currentAttack.startupFrames + m_currentAttack.activeFrames + 5)) {
            SpecialMotionType mot = m_buffer.motion;
            int btn = m_buffer.motionButton;
            m_buffer = BufferedInput{};
            m_justCanceled = true;
            ExecuteSpecial(mot, btn, opponent);
            break;
        }

        if (!m_currentAttack.isHeavy && m_stateFrame >= m_currentAttack.startupFrames + 1) {
            if (m_buffer.button == BTN_LP || m_buffer.button == BTN_LK) {
                TryExecuteBufferedMove(opponent);
                break;
            }
        }

        int totalFrames = m_currentAttack.startupFrames + m_currentAttack.activeFrames + m_currentAttack.recoveryFrames;
        if (m_stateFrame >= totalFrames) {
            m_state = IsAirborne() ? FSTATE_JUMP : FSTATE_IDLE;
            m_stateFrame = 0;
            if (grounded) TryExecuteBufferedMove(opponent);
        }
        break;
    }

    case FSTATE_SPECIAL: {

        if (!m_isSuperMove && m_buffer.motion == MOTION_SUPER_OMNIBUS && CanUseSuper() && m_hitConnected) {
            m_buffer = BufferedInput{};
            m_justCanceled = true;
            ExecuteSpecial(MOTION_SUPER_OMNIBUS, BTN_HP, opponent);
            break;
        }

        int totalFrames = m_currentAttack.startupFrames + m_currentAttack.activeFrames + m_currentAttack.recoveryFrames;
        if (m_multiHitsRemaining > 1 && m_stateFrame > m_currentAttack.startupFrames && (m_stateFrame % 5 == 0)) {
            m_hitConnected = false;
            m_multiHitsRemaining--;
        }

        if (m_charId == CHAR_THE_PUNDIT && m_specialIndex == 1 && (m_stateFrame == 12 || m_stateFrame == 20)) {
            SpawnProjectile(PROJ_POLLING_MAP, (m_stateFrame == 12) ? 4.4f : 5.3f, (m_stateFrame == 12) ? 0.22f : 0.0f, 10);
            AudioEngine::Instance().PlaySfxPan(SFX_FIREBALL_LAUNCH, 1.15f, 1.0f, m_pos.x);
        }

        if (m_charId == CHAR_KIM && m_specialIndex == 1 && m_stateFrame == 20) {
            SpawnProjectile(PROJ_SUPREME_WAVE, 4.6f, 0.0f, 22);
            AudioEngine::Instance().PlaySfxPan(SFX_FIREBALL_IMPACT, 0.85f, 1.0f, m_pos.x);
        }

        if (m_charId == CHAR_TECH_VISIONARY && m_specialIndex == 3 && m_stateFrame == 12) {
            m_pos.x = opponent.GetPosition().x + (opponent.IsFacingRight() ? -45.0f : 45.0f);
        }

        if (m_stateFrame >= totalFrames && grounded) {
            m_state = FSTATE_IDLE;
            m_stateFrame = 0;
            m_armor = 0;
            m_isSuperMove = false;
            TryExecuteBufferedMove(opponent);
        }
        break;
    }

    case FSTATE_TAUNT: {
        if (m_stateFrame >= 42) {
            m_state = FSTATE_IDLE;
            m_stateFrame = 0;
            TryExecuteBufferedMove(opponent);
        }
        break;
    }

    case FSTATE_CALL_TO_PRAYER: {
        m_vel = Vector2{ 0.0f, 0.0f };
        if (m_stateFrame >= 54) {
            m_state = FSTATE_IDLE;
            m_stateFrame = 0;
        }
        break;
    }

    case FSTATE_BLOCK_HIGH:
    case FSTATE_BLOCK_LOW:
    case FSTATE_BLOCKSTUN: {

        if (m_superMeter >= 100 && input.IsAlphaCounterPressed()) {
            m_superMeter -= 100;
            m_justAlphaCounter = true;
            m_invincibleFrames = 16;
            m_state = FSTATE_SPECIAL;
            m_stateFrame = 0;
            m_hitConnected = false;
            m_specialIndex = 2;
            int dmg = std::max(1, (int)roundf(15 * m_damageScale));
            m_currentAttack = AttackData{ "ALPHA VETO COUNTER!", 3, 12, 14, dmg, ATTACK_MID, 14, 28, 16, 6.2f, true, true, 42.0f, -34.0f, 44.0f, 36.0f, true, MOVE_SPECIAL };
            AudioEngine::Instance().PlaySfx(SFX_SUPER_FREEZE, 1.2f, 0.9f);
            break;
        }
        if (m_stateFrame >= m_blockstunFrames) {
            if (TryExecuteBufferedMove(opponent)) {
                break;
            }
            if (m_isHoldingBlockButton) {
                m_state = crouching ? FSTATE_BLOCK_LOW : FSTATE_BLOCK_HIGH;
                m_blockstunFrames = 0;
                m_vel.x = 0.0f;
            } else {
                m_state = crouching ? FSTATE_CROUCH : FSTATE_IDLE;
                m_stateFrame = 0;
            }
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
        if (m_stateFrame >= 24) {
            m_state = FSTATE_WAKEUP;
            m_stateFrame = 0;
        }
        break;
    }

    case FSTATE_WAKEUP: {
        if (m_stateFrame >= 16) {
            m_state = FSTATE_IDLE;
            m_stateFrame = 0;
            if (TryExecuteBufferedMove(opponent)) {
                m_justReversal = true;
            }
        }
        break;
    }

    case FSTATE_VICTORY:
        if (m_charId == CHAR_PALE_MOTH && m_stateFrame == 2) {
            AudioEngine::Instance().PlaySfx(SFX_HORSE_GALLOP, 1.0f, 1.0f);
        }
        if (m_charId == CHAR_W) {
            if (m_stateFrame == 2) {
                m_activeTaunt = "Now watch this drive!";
                m_tauntTimer = 160;
            } else if (m_stateFrame == 18) {
                AudioEngine::Instance().PlaySfxPan(SFX_GOLF_DRIVE, 1.0f, 1.0f, m_pos.x);
            }
        }
        break;

    case FSTATE_DEFEAT:
        m_vel = Vector2{ 0.0f, 0.0f };
        m_pos.y = GROUND_Y;
        break;

    default:
        break;
    }

    for (auto it = m_cosmeticPuffs.begin(); it != m_cosmeticPuffs.end(); ) {
        it->timer += 1.0f / 60.0f;
        if (it->timer >= it->maxTimer) it = m_cosmeticPuffs.erase(it);
        else ++it;
    }

    if (m_charId == CHAR_COMMANDER && m_health > 0) {
        m_quirkTimer++;
        if (m_quirkTimer >= 480) {
            if (m_state == FSTATE_IDLE || m_state == FSTATE_WALK_FWD || m_state == FSTATE_WALK_BACK || m_state == FSTATE_CROUCH) {
                m_quirkTimer = 0;
                CosmeticPuff puff{ Vector2{ m_pos.x + (m_facingRight ? -22.0f : 22.0f), m_pos.y - 34.0f }, 0.0f, 0.60f, 10.0f };
                m_cosmeticPuffs.push_back(puff);
                AudioEngine::Instance().PlaySfxPan(SFX_FART, 0.95f + (float)(rand() % 12) * 0.01f, 0.85f, m_pos.x);
            }
        }
    } else if (m_charId == CHAR_DICK && m_health > 0) {
        m_quirkTimer++;
        if (m_quirkTimer >= 310 && m_laughTimer <= 0) {
            if (m_state == FSTATE_IDLE || m_state == FSTATE_WALK_FWD || m_state == FSTATE_WALK_BACK || m_state == FSTATE_CROUCH) {
                m_quirkTimer = 0;
                m_state = FSTATE_IDLE;
                TriggerCameraLaugh(40);
            }
        }
    }

    m_pos.x = std::clamp(m_pos.x, STAGE_MIN_X, STAGE_MAX_X);
}

void Fighter::UpdateAnimation() {
}

Rectangle Fighter::GetPushbox() const {
    if (IsSpiderHoleHidden()) {
        return Rectangle{ m_pos.x - 6.0f, m_pos.y - 8.0f, 12.0f, 8.0f };
    }
    float scale = (m_charId == CHAR_WAR_PIG) ? 1.20f : 1.0f;
    float w = 24.0f * scale;
    float h = (IsCrouching() ? 34.0f : 54.0f) * scale;
    return Rectangle{ m_pos.x - w * 0.5f, m_pos.y - h, w, h };
}

std::vector<Rectangle> Fighter::GetHurtboxes() const {
    std::vector<Rectangle> boxes;

    if (IsSpiderHoleHidden() ||
        (m_charId == CHAR_COUCHIE && m_state == FSTATE_SPECIAL) ||
        m_state == FSTATE_CALL_TO_PRAYER ||
        m_state == FSTATE_FINISHER || m_state == FSTATE_FINISHER_VICTIM ||
        m_state == FSTATE_KNOCKDOWN_GROUND || m_state == FSTATE_WAKEUP || m_state == FSTATE_DEFEAT) {
        return boxes;
    }

    if (m_state == FSTATE_KNOCKDOWN_AIR) {
        if (m_juggleHits < 2 && m_health > 0) {
            boxes.push_back(Rectangle{ m_pos.x - 16.0f, m_pos.y - 44.0f, 32.0f, 38.0f });
        }
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
        if (m_currentAttack.type == ATTACK_NONE || m_currentAttack.boxW <= 0.1f || m_currentAttack.boxH <= 0.1f) {
            return false;
        }
        int startup = m_currentAttack.startupFrames;
        int active = m_currentAttack.activeFrames;
        if (m_stateFrame >= startup && m_stateFrame < startup + active) {
            float rx = m_facingRight ? m_currentAttack.reachX : -m_currentAttack.reachX;
            float cx = m_pos.x + rx;
            float cy = m_pos.y + m_currentAttack.reachY;
            outBox = Rectangle{ cx - m_currentAttack.boxW * 0.5f, cy - m_currentAttack.boxH * 0.5f, m_currentAttack.boxW, m_currentAttack.boxH };
            outData = m_currentAttack;
            return true;
        }
    }
    return false;
}
