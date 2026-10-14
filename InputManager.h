#pragma once
#include "Common.h"
#include <array>
#include <vector>

enum RelativeDirection : uint8_t {
    DIR_REL_NEUTRAL = 5,
    DIR_REL_UP = 8,
    DIR_REL_DOWN = 2,
    DIR_REL_FORWARD = 6,
    DIR_REL_BACK = 4,
    DIR_REL_UP_FORWARD = 9,
    DIR_REL_UP_BACK = 7,
    DIR_REL_DOWN_FORWARD = 3,
    DIR_REL_DOWN_BACK = 1
};

enum SpecialMotionType {
    MOTION_NONE = 0,
    MOTION_QCF_P,
    MOTION_QCF_K,
    MOTION_QCB_P,
    MOTION_QCB_K,
    MOTION_DP_P,
    MOTION_DP_K,
    MOTION_CHARGE_B_F_P,
    MOTION_CHARGE_B_F_K,
    MOTION_CHARGE_D_U_K,
    MOTION_COMMAND_GRAB
};

struct RawInputState {
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool lp = false;
    bool hp = false;
    bool lk = false;
    bool hk = false;
    bool special = false;
    bool taunt = false;
    bool select = false;
    bool start = false;
    bool coin = false;
};

struct BufferedFrame {
    uint8_t rawDir = 0;
    RelativeDirection relDir = DIR_REL_NEUTRAL;
    uint16_t buttonsHeld = 0;
    uint16_t buttonsPressed = 0;
};

class InputManager {
public:
    static constexpr int BUFFER_SIZE = 60;
    static constexpr int MOTION_WINDOW = 15;
    static constexpr int CHARGE_REQUIRED_FRAMES = 45;

    InputManager();
    void Update(int playerIndex, bool facingRight);
    void UpdateSimulated(const RawInputState& simState, bool facingRight);
    void InjectSpecialMotion(SpecialMotionType motion, uint16_t buttonMask);
    static bool HasHumanInput(int playerIndex);

    const RawInputState& GetCurrentState() const { return m_currentState; }
    RelativeDirection GetCurrentRelDir() const { return m_history[m_head].relDir; }
    bool IsButtonPressed(uint16_t btnMask) const { return (m_history[m_head].buttonsPressed & btnMask) != 0; }
    bool IsButtonHeld(uint16_t btnMask) const { return (m_history[m_head].buttonsHeld & btnMask) != 0; }

    bool IsUpPressed() const { return m_currentState.up && !m_prevState.up; }
    bool IsDownPressed() const { return m_currentState.down && !m_prevState.down; }
    bool IsLeftPressed() const { return m_currentState.left && !m_prevState.left; }
    bool IsRightPressed() const { return m_currentState.right && !m_prevState.right; }

    bool IsTriggerEasterEggPressed() const;

    bool IsSecretMenuComboPressed() const;

    SpecialMotionType CheckSpecialMotion(int& outButtonUsed);

    int GetChargeBackFrames() const { return m_chargeBackFrames; }
    int GetChargeDownFrames() const { return m_chargeDownFrames; }

    void ConsumeMotion();
    void Reset();

    static bool IsGamepadActive();
    static const char* GetActiveControllerName(int playerIndex = 0);
    static int GetGamepadSlotForPlayer(int playerIndex);

private:
    RawInputState PollRawInputs(int playerIndex);
    RelativeDirection ComputeRelativeDirection(const RawInputState& state, bool facingRight);

    RawInputState m_currentState{};
    RawInputState m_prevState{};
    std::array<BufferedFrame, BUFFER_SIZE> m_history{};
    int m_head = 0;

    int m_chargeBackFrames = 0;
    int m_chargeDownFrames = 0;

    SpecialMotionType m_injectedMotion = MOTION_NONE;
    uint16_t m_injectedButton = 0;
};
