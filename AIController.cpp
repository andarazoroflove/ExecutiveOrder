#include "AIController.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

AIController::AIController() {
    Reset();
}

void AIController::Reset() {
    m_actionCooldown = 25 + (rand() % 15);
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
        else if (role == 2) input.InjectSpecialMotion(MOTION_QCB_K, BTN_LK);
        else input.InjectSpecialMotion(MOTION_DP_P, BTN_LP);
        break;

    case CHAR_HIGH_BENCH:
        if (role == 0 || role == 2) input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        else if (role == 1) input.InjectSpecialMotion(MOTION_QCB_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_CHARGE_B_F_K, BTN_HK);
        break;

    case CHAR_REDCAP:
        if (role == 0 || role == 2) input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        else if (role == 3) input.InjectSpecialMotion(MOTION_COMMAND_GRAB, BTN_HP);
        else input.InjectSpecialMotion(MOTION_CHARGE_B_F_P, BTN_HP);
        break;

    case CHAR_COASTAL_GOV:
        if (role == 1) input.InjectSpecialMotion(MOTION_CHARGE_D_U_K, BTN_HK);
        else if (role == 0 || role == 2) input.InjectSpecialMotion(MOTION_QCF_K, BTN_LK);
        else input.InjectSpecialMotion(MOTION_QCB_P, BTN_LP);
        break;

    case CHAR_TECH_VISIONARY:
        if (role == 0) input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        else if (role == 2) input.InjectSpecialMotion(MOTION_QCB_P, BTN_LP);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_LP);
        break;

    case CHAR_FILIBUSTER:
        if (role == 0) input.InjectSpecialMotion(MOTION_CHARGE_B_F_P, BTN_HP);
        else if (role == 1) input.InjectSpecialMotion(MOTION_CHARGE_D_U_K, BTN_HK);
        else input.InjectSpecialMotion(MOTION_QCB_P, BTN_HP);
        break;

    case CHAR_MASCOT:
        if (role == 0 || role == 2) input.InjectSpecialMotion(MOTION_CHARGE_B_F_K, BTN_HK);
        else if (role == 1) input.InjectSpecialMotion(MOTION_CHARGE_D_U_K, BTN_HK);
        else input.InjectSpecialMotion(MOTION_CHARGE_B_F_K, BTN_LK);
        break;

    case CHAR_POPULIST:
        if (role == 1) input.InjectSpecialMotion(MOTION_DP_K, BTN_HK);
        else if (role == 0 || role == 2) input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCB_K, BTN_LK);
        break;

    case CHAR_SUCKRATERY_RKF:
        if (role == 0 || role == 2) input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        else if (role == 1) input.InjectSpecialMotion(MOTION_QCF_K, BTN_HK);
        else input.InjectSpecialMotion(MOTION_CHARGE_B_F_P, BTN_HP);
        break;

    case CHAR_WAR_PIG:
        if (role == 0 || role == 2) input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        else if (role == 1 || role == 3) input.InjectSpecialMotion(MOTION_QCB_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_CHARGE_B_F_P, BTN_HP);
        break;

    case CHAR_SOCIALIST:
        if (role == 0) input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        else if (role == 1) input.InjectSpecialMotion(MOTION_DP_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCB_P, BTN_LP);
        break;

    case CHAR_BILLARY:
        if (role == 0 || role == 2) input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        else if (role == 1) input.InjectSpecialMotion(MOTION_DP_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_CHARGE_B_F_P, BTN_HP);
        break;

    case CHAR_PALM_BEACH_PETE:
        if (role == 0 || role == 2) input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        else if (role == 1) input.InjectSpecialMotion(MOTION_DP_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCB_P, BTN_HP);
        break;

    case CHAR_THE_PUNDIT:
    case CHAR_A_NEW_HOPE:
        if (role == 1) input.InjectSpecialMotion(MOTION_DP_P, BTN_HP);
        else input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;

    default:
        input.InjectSpecialMotion(MOTION_QCF_P, BTN_HP);
        break;
    }
}

void AIController::MakeDecision(InputManager& input, const Fighter& cpu, const Fighter& opponent, float dist, bool oppAirborne, bool oppAttacking) {
    bool facingRight = cpu.IsFacingRight();
    bool desperation = (cpu.GetHealth() < 35);
    int roll = rand() % 100;

    int antiAirSpecThresh = (m_difficulty == AI_DIFF_EASY) ? 15 : ((m_difficulty == AI_DIFF_HARD) ? 70 : 35);
    int antiAirNormThresh = (m_difficulty == AI_DIFF_EASY) ? 28 : ((m_difficulty == AI_DIFF_HARD) ? 92 : 55);

    if (oppAirborne && dist < 120.0f) {
        if (roll < antiAirSpecThresh) {
            TriggerSpecial(input, cpu.GetCharacterId(), 1);
            m_moveDir = 0;
            return;
        } else if (roll < antiAirNormThresh) {
            m_simInput.hp = true;
            m_moveDir = 0;
            return;
        }
    }

    if (dist > 150.0f) {
        if (roll < 55) {

            m_moveDir = 1;
            m_movementTimer = 20 + (rand() % 15);
        } else if (roll < 80) {

            TriggerSpecial(input, cpu.GetCharacterId(), 0);
            m_moveDir = 0;
        } else if (roll < 92) {

            m_moveDir = 3;
            m_simInput.up = true;
            m_movementTimer = 8;
        } else {

            m_moveDir = 0;
            m_movementTimer = 15;
        }
        return;
    }

    if (dist >= 65.0f && dist <= 150.0f) {
        if (roll < 35) {

            m_footsiesPhase = (m_footsiesPhase + 1) % 2;
            m_moveDir = (m_footsiesPhase == 0) ? 1 : -1;
            m_movementTimer = 12 + (rand() % 10);
        } else if (roll < 55) {

            m_moveDir = 2;
            if (rand() % 2 == 0) m_simInput.lk = true;
            else m_simInput.hk = true;
            m_movementTimer = 10;
        } else if (roll < 75) {

            m_moveDir = 0;
            if (rand() % 2 == 0) m_simInput.lp = true;
            else m_simInput.hk = true;
        } else if (roll < 90 || desperation) {

            TriggerSpecial(input, cpu.GetCharacterId(), 2);
            m_moveDir = 0;
        } else {

            m_moveDir = -1;
            m_movementTimer = 14;
        }
        return;
    }

    {
        if (roll < 30) {

            m_moveDir = 0;
            if (rand() % 2 == 0) m_simInput.lp = true;
            else m_simInput.lk = true;
        } else if (roll < 55) {

            m_moveDir = 0;
            if (rand() % 2 == 0) m_simInput.hp = true;
            else m_simInput.hk = true;
        } else if (roll < 75 || desperation) {

            TriggerSpecial(input, cpu.GetCharacterId(), 3);
            m_moveDir = 0;
        } else if (roll < 88) {

            m_moveDir = 2;
            m_simInput.lk = true;
            m_movementTimer = 10;
        } else {

            m_moveDir = 4;
            m_simInput.up = true;
            m_movementTimer = 8;
        }
    }
}

void AIController::Update(InputManager& input, const Fighter& cpu, const Fighter& opponent, float dt) {
    (void)dt;

    m_simInput.lp = false;
    m_simInput.hp = false;
    m_simInput.lk = false;
    m_simInput.hk = false;

    m_simInput.up = false;
    m_simInput.down = false;
    m_simInput.left = false;
    m_simInput.right = false;

    bool facingRight = cpu.IsFacingRight();
    float dist = std::abs(cpu.GetPosition().x - opponent.GetPosition().x);
    bool oppAirborne = opponent.IsAirborne();
    bool oppAttacking = (opponent.GetState() == FSTATE_ATTACK || opponent.GetState() == FSTATE_SPECIAL);
    bool desperation = (cpu.GetHealth() < 35);

    int blockChance = (m_difficulty == AI_DIFF_EASY) ? 18 : ((m_difficulty == AI_DIFF_HARD) ? 85 : 40);
    if (oppAttacking && dist < 170.0f && !m_isBlocking && m_blockDuration <= 0) {
        if ((rand() % 100) < blockChance) {
            m_isBlocking = true;
            m_blockDuration = 18 + (rand() % 16);
        }
    }

    if (m_isBlocking) {
        m_blockDuration--;
        if (m_blockDuration <= 0 || !oppAttacking) {
            m_isBlocking = false;
        } else {

            if (facingRight) m_simInput.left = true;
            else m_simInput.right = true;

            if (opponent.IsCrouching()) {
                m_simInput.down = true;
            }

            input.UpdateSimulated(m_simInput, facingRight);
            return;
        }
    }

    if (m_actionCooldown > 0) {
        m_actionCooldown--;
    } else {
        MakeDecision(input, cpu, opponent, dist, oppAirborne, oppAttacking);

        if (m_difficulty == AI_DIFF_EASY) {
            m_actionCooldown = desperation ? (24 + rand() % 12) : (38 + rand() % 16);
        } else if (m_difficulty == AI_DIFF_HARD) {
            m_actionCooldown = desperation ? (6 + rand() % 6) : (10 + rand() % 8);
        } else {
            m_actionCooldown = desperation ? (14 + rand() % 12) : (22 + rand() % 16);
        }
    }

    if (m_movementTimer > 0) {
        m_movementTimer--;
    }

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
