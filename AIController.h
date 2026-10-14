#pragma once
#include "Common.h"
#include "Fighter.h"
#include "InputManager.h"

class AIController {
public:
    AIController();
    void Reset();
    void Update(InputManager& input, const Fighter& cpu, const Fighter& opponent, float dt);

    void SetDifficulty(AIDifficulty diff) { m_difficulty = diff; }
    AIDifficulty GetDifficulty() const { return m_difficulty; }

private:
    void MakeDecision(InputManager& input, const Fighter& cpu, const Fighter& opponent, float dist, bool oppAirborne, bool oppAttacking);
    void TriggerSpecial(InputManager& input, CharacterId charId, int role);

    AIDifficulty m_difficulty = AI_DIFF_EASY;
    int m_actionCooldown = 30;
    int m_movementTimer = 0;
    int m_moveDir = 0;

    bool m_isBlocking = false;
    int m_blockDuration = 0;

    int m_footsiesPhase = 0;
    int m_footsiesTimer = 0;

    RawInputState m_simInput{};
};
