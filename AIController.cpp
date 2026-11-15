#include "AIController.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

AIController::AIController() {
    Reset();
}

void AIController::Reset() {
    m_actionCooldown = 22 + (rand() % 14);
    m_movementTimer = 0;
    m_moveDir = 0;
    m_isBlocking = false;
    m_blockDuration = 0;
    m_footsiesPhase = 0;
    m_footsiesTimer = 0;
    m_simInput = RawInputState{};
}

void AIController::TriggerSpecial(InputManager& input, CharacterId charId, int role) {
    switch (charId) {
    case CHAR_COMMANDER:
        if (role == 0) input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        else if (role == 1) input.InjectSpecialMotion(MOTION_DP_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCB_K, BTN_LK);
        break;
    case CHAR_HIGH_BENCH:
        input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_REDCAP:
        if (role == 3) input.InjectSpecialMotion(MOTION_COMMAND_GRAB, BTN_HP);
        else if (role == 2) input.InjectSpecialMotion(MOTION_CHARGE_B_F_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_COASTAL_GOV:
        if (role == 1) input.InjectSpecialMotion(MOTION_CHARGE_D_U_K, BTN_HK);
        else input.InjectSpecialMotion(MOTION_QCF_K, BTN_HK);
        break;
    case CHAR_TECH_VISIONARY:
        if (role == 2) input.InjectSpecialMotion(MOTION_QCB_P, BTN_LP);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_FILIBUSTER:
        if (role == 1) input.InjectSpecialMotion(MOTION_CHARGE_D_U_K, BTN_HK);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_MASCOT:
        if (role == 1) input.InjectSpecialMotion(MOTION_DP_K, BTN_HK);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_POPULIST:
        if (role == 1) input.InjectSpecialMotion(MOTION_DP_K, BTN_HK);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_DOCTOR_ABDUL:
        if (role == 0) input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCB_K, BTN_HK);
        break;
    case CHAR_COUCHIE:
        input.InjectSpecialMotion(MOTION_QCB_P, BTN_HP);
        break;
    case CHAR_BLEXAS:
        if (role == 1) input.InjectSpecialMotion(MOTION_DP_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_SI:
        if (role == 3) input.InjectSpecialMotion(MOTION_DP_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_KIM:
        input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_BEBE:
        if (role == 1) input.InjectSpecialMotion(MOTION_DP_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_PALE_MOTH:
        if (role == 0) input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCB_P, BTN_HP);
        break;
    case CHAR_THE_DICTATOR:
        if (role == 2 || role == 3) input.InjectSpecialMotion(MOTION_QCF_K, BTN_HK);
        else input.InjectSpecialMotion(MOTION_DOWN_DOWN, BTN_HP);
        break;
    case CHAR_FLOTUS:
        if (role == 1) input.InjectSpecialMotion(MOTION_DP_K, BTN_HK);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_W:
        if (role == 1) input.InjectSpecialMotion(MOTION_DP_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_DICK:
        if (role == 1) input.InjectSpecialMotion(MOTION_DP_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    case CHAR_AYATOLLAH:
        if (role == 1) input.InjectSpecialMotion(MOTION_DP_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    default:
        if (role == 1) input.InjectSpecialMotion(MOTION_DP_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    }
}

void AIController::MakeDecision(InputManager& input, const Fighter& cpu, const Fighter& opponent, float dist, bool oppAirborne, bool oppAttacking) {
    bool desperation = (cpu.GetHealth() < 35);
    CharacterId cid = cpu.GetCharacterId();
    bool isWorldBoss = (cid == CHAR_KIM || cid == CHAR_BEBE || cid == CHAR_PALE_MOTH || cid == CHAR_THE_DICTATOR || cid == CHAR_AYATOLLAH);
    int roll = rand() % 100;

    if (cpu.GetSuperMeter() >= 100 && cid != CHAR_COUCHIE && dist < 135.0f && roll < (isWorldBoss ? 42 : 24)) {
        input.InjectSpecialMotion(MOTION_SUPER_OMNIBUS, BTN_HP);
        m_moveDir = 0;
        return;
    }

    if (oppAttacking && (cid == CHAR_THE_DICTATOR || cid == CHAR_COUCHIE) && roll < 55) {
        TriggerSpecial(input, cid, 0);
        m_moveDir = 0;
        return;
    }

    int antiAirSpecThresh = (m_difficulty == AI_DIFF_EASY) ? 18 : ((m_difficulty == AI_DIFF_HARD || isWorldBoss) ? 74 : 40);
    int antiAirNormThresh = (m_difficulty == AI_DIFF_EASY) ? 32 : ((m_difficulty == AI_DIFF_HARD || isWorldBoss) ? 92 : 60);

    if (oppAirborne && dist < 125.0f) {
        if (roll < antiAirSpecThresh) {
            TriggerSpecial(input, cpu.GetCharacterId(), 1);
            m_moveDir = 0;
            return;
        } else if (roll < antiAirNormThresh) {
            m_moveDir = 2;
            m_simInput.hp = true;
            return;
        }
    }

    if (dist > 150.0f) {
        if (roll < 48) {
            m_moveDir = 1;
            m_movementTimer = 18 + (rand() % 14);
        } else if (roll < 82) {
            TriggerSpecial(input, cpu.GetCharacterId(), 0);
            m_moveDir = 0;
        } else if (roll < 94) {
            m_moveDir = 3;
            m_simInput.up = true;
            m_movementTimer = 8;
        } else {
            m_simInput.taunt = (rand() % 3 == 0);
            m_moveDir = 0;
            m_movementTimer = 14;
        }
        return;
    }

    if (dist >= 62.0f && dist <= 150.0f) {
        if (roll < 30) {
            m_footsiesPhase = (m_footsiesPhase + 1) % 2;
            m_moveDir = (m_footsiesPhase == 0) ? 1 : -1;
            m_movementTimer = 10 + (rand() % 10);
        } else if (roll < 52) {
            m_moveDir = 2;
            if (rand() % 2 == 0) m_simInput.lk = true;
            else m_simInput.hk = true;
            m_movementTimer = 10;
        } else if (roll < 72) {
            m_moveDir = 0;
            if (rand() % 2 == 0) m_simInput.lp = true;
            else m_simInput.hk = true;
        } else if (roll < 92 || desperation) {
            TriggerSpecial(input, cpu.GetCharacterId(), 2);
            m_moveDir = 0;
        } else {
            m_moveDir = -1;
            m_movementTimer = 12;
        }
        return;
    }

    if (roll < 18 && !opponent.IsAirborne()) {

        m_simInput.lp = true;
        m_simInput.lk = true;
        m_moveDir = 0;
    } else if (roll < 42) {
        m_moveDir = 0;
        if (rand() % 2 == 0) m_simInput.lp = true;
        else m_simInput.lk = true;
    } else if (roll < 66) {
        m_moveDir = (rand() % 2 == 0) ? 2 : 0;
        if (rand() % 2 == 0) m_simInput.hp = true;
        else m_simInput.hk = true;
    } else if (roll < 86 || desperation) {
        TriggerSpecial(input, cpu.GetCharacterId(), 3);
        m_moveDir = 0;
    } else {
        m_moveDir = 4;
        m_simInput.up = true;
        m_movementTimer = 8;
    }
}

void AIController::Update(InputManager& input, const Fighter& cpu, const Fighter& opponent, float dt) {
    (void)dt;
    m_simInput = RawInputState{};

    bool facingRight = cpu.IsFacingRight();
    float dist = std::abs(cpu.GetPosition().x - opponent.GetPosition().x);
    bool oppAirborne = opponent.IsAirborne();
    bool oppAttacking = (opponent.GetState() == FSTATE_ATTACK || opponent.GetState() == FSTATE_SPECIAL || !opponent.GetProjectiles().empty());
    bool desperation = (cpu.GetHealth() < 35);

    CharacterId cid = cpu.GetCharacterId();
    bool isBlexas = (cid == CHAR_BLEXAS);
    bool isWorldBoss = (cid == CHAR_KIM || cid == CHAR_BEBE || cid == CHAR_PALE_MOTH || cid == CHAR_THE_DICTATOR || cid == CHAR_AYATOLLAH);

    int blockChance = (m_difficulty == AI_DIFF_EASY) ? 22 : ((m_difficulty == AI_DIFF_HARD) ? 86 : 46);
    if (isBlexas) blockChance = std::min(68, (int)(blockChance * 1.12f));
    else if (isWorldBoss) blockChance = std::min(88, blockChance + 24);

    if (oppAttacking && dist < 190.0f && !m_isBlocking && m_blockDuration <= 0) {
        if ((rand() % 100) < blockChance) {
            m_isBlocking = true;
            m_blockDuration = 16 + (rand() % 14);
        }
    }

    if (m_isBlocking) {
        m_blockDuration--;
        if (m_blockDuration <= 0 || !oppAttacking) {
            m_isBlocking = false;
        } else {
            if (facingRight) m_simInput.left = true;
            else m_simInput.right = true;
            if (opponent.IsCrouching()) m_simInput.down = true;
            input.UpdateSimulated(m_simInput, facingRight);
            return;
        }
    }

    if (m_actionCooldown > 0) {
        m_actionCooldown--;
    } else {
        MakeDecision(input, cpu, opponent, dist, oppAirborne, oppAttacking);
        int cd = 20;
        if (m_difficulty == AI_DIFF_EASY) cd = desperation ? (22 + rand() % 12) : (34 + rand() % 14);
        else if (m_difficulty == AI_DIFF_HARD) cd = desperation ? (5 + rand() % 5) : (9 + rand() % 7);
        else cd = desperation ? (12 + rand() % 10) : (18 + rand() % 12);

        if (isBlexas) cd = std::max(8, (int)(cd * 0.88f));
        else if (isWorldBoss) cd = std::max(5, (int)(cd * 0.75f));
        m_actionCooldown = cd;
    }

    if (m_movementTimer > 0) m_movementTimer--;

    switch (m_moveDir) {
    case 1:
        if (facingRight) m_simInput.right = true;
        else m_simInput.left = true;
        break;
    case -1:
        if (facingRight) m_simInput.left = true;
        else m_simInput.right = true;
        break;
    case 2:
        m_simInput.down = true;
        break;
    case 3:
        m_simInput.up = true;
        if (facingRight) m_simInput.right = true;
        else m_simInput.left = true;
        break;
    case 4:
        m_simInput.up = true;
        if (facingRight) m_simInput.left = true;
        else m_simInput.right = true;
        break;
    default:
        break;
    }

    input.UpdateSimulated(m_simInput, facingRight);
}
