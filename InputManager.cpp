#include "InputManager.h"
#include <cmath>

InputManager::InputManager() {
    m_history.fill(BufferedFrame{});
}

RelativeDirection InputManager::ComputeRelativeDirection(const RawInputState& state, bool facingRight) {
    bool up = state.up;
    bool down = state.down;
    bool left = state.left;
    bool right = state.right;

    bool fwd = facingRight ? right : left;
    bool back = facingRight ? left : right;

    if (up && fwd) return DIR_REL_UP_FORWARD;
    if (up && back) return DIR_REL_UP_BACK;
    if (up) return DIR_REL_UP;
    if (down && fwd) return DIR_REL_DOWN_FORWARD;
    if (down && back) return DIR_REL_DOWN_BACK;
    if (down) return DIR_REL_DOWN;
    if (fwd) return DIR_REL_FORWARD;
    if (back) return DIR_REL_BACK;
    return DIR_REL_NEUTRAL;
}

bool InputManager::IsGamepadActive() {
    for (int pad = 0; pad < 4; ++pad) {
        if (IsGamepadAvailable(pad)) return true;
    }
    return false;
}

const char* InputManager::GetActiveControllerName(int playerIndex) {
    int pad = GetGamepadSlotForPlayer(playerIndex);
    if (pad >= 0) {
        return GetGamepadName(pad);
    }
    return "Keyboard";
}

int InputManager::GetGamepadSlotForPlayer(int playerIndex) {
    if (playerIndex != 0) return -1;
    for (int pad = 0; pad < 4; ++pad) {
        if (IsGamepadAvailable(pad)) {
            return pad;
        }
    }
    return -1;
}

RawInputState InputManager::PollRawInputs(int playerIndex) {
    RawInputState state{};

    if (playerIndex == 0) {

        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) state.up = true;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) state.down = true;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) state.left = true;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) state.right = true;

        if (IsKeyDown(KEY_U) || IsKeyDown(KEY_Z)) state.lp = true;
        if (IsKeyDown(KEY_I) || IsKeyDown(KEY_X)) state.hp = true;

        if (IsKeyDown(KEY_J) || IsKeyDown(KEY_C)) state.lk = true;
        if (IsKeyDown(KEY_K) || IsKeyDown(KEY_V)) state.hk = true;

        if (IsKeyDown(KEY_O) || IsKeyDown(KEY_E)) state.special = true;

        if (IsKeyDown(KEY_Y) || IsKeyDown(KEY_Q) || IsKeyDown(KEY_T)) state.taunt = true;

        if (IsKeyDown(KEY_TAB) || IsKeyDown(KEY_BACKSPACE) || IsKeyDown(KEY_ESCAPE)) state.select = true;

        if (IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_ONE)) state.start = true;
        if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_FIVE) || IsKeyDown(KEY_LEFT_SHIFT)) state.coin = true;
    } else {

        if (IsKeyDown(KEY_KP_8)) state.up = true;
        if (IsKeyDown(KEY_KP_2)) state.down = true;
        if (IsKeyDown(KEY_KP_4)) state.left = true;
        if (IsKeyDown(KEY_KP_6)) state.right = true;

        if (IsKeyDown(KEY_KP_7)) state.lp = true;
        if (IsKeyDown(KEY_KP_9)) state.hp = true;

        if (IsKeyDown(KEY_KP_1)) state.lk = true;
        if (IsKeyDown(KEY_KP_3)) state.hk = true;

        if (IsKeyDown(KEY_KP_ADD)) state.special = true;
        if (IsKeyDown(KEY_KP_DECIMAL)) state.taunt = true;

        if (IsKeyDown(KEY_KP_ENTER)) state.start = true;
    }

    int pad = GetGamepadSlotForPlayer(playerIndex);
    if (pad >= 0 && IsGamepadAvailable(pad)) {
        constexpr float STICK_UP_THRESHOLD = -0.35f;
        constexpr float STICK_DOWN_THRESHOLD = 0.35f;
        constexpr float STICK_HORIZ_THRESHOLD = 0.30f;

        float axisX = GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_X);
        float axisY = GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_Y);

        if (axisY < STICK_UP_THRESHOLD || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_UP)) state.up = true;
        if (axisY > STICK_DOWN_THRESHOLD || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) state.down = true;
        if (axisX < -STICK_HORIZ_THRESHOLD || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) state.left = true;
        if (axisX > STICK_HORIZ_THRESHOLD || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) state.right = true;

        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) {
            state.lp = true;
        }

        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_UP)) {
            state.hp = true;
        }

        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
            state.lk = true;
        }

        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
            state.hk = true;
        }

        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ||
            IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2) ||
            GetGamepadAxisMovement(pad, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.35f) {
            state.special = true;
        }

        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ||
            IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_2) ||
            GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.35f) {
            state.taunt = true;
        }

        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
            state.start = true;
        }

        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_LEFT) ||
            IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE)) {
            state.coin = true;
            state.select = true;
        }
    }

    return state;
}

bool InputManager::HasHumanInput(int playerIndex) {
    if (playerIndex == 0) return true;
    return false;
}

void InputManager::Update(int playerIndex, bool facingRight) {
    m_prevState = m_currentState;
    m_currentState = PollRawInputs(playerIndex);

    m_head = (m_head + 1) % BUFFER_SIZE;
    BufferedFrame& frame = m_history[m_head];

    frame.rawDir = 0;
    if (m_currentState.up) frame.rawDir |= 1;
    if (m_currentState.down) frame.rawDir |= 2;
    if (m_currentState.left) frame.rawDir |= 4;
    if (m_currentState.right) frame.rawDir |= 8;

    frame.relDir = ComputeRelativeDirection(m_currentState, facingRight);

    frame.buttonsHeld = 0;
    if (m_currentState.lp) frame.buttonsHeld |= BTN_LP;
    if (m_currentState.hp) frame.buttonsHeld |= BTN_HP;
    if (m_currentState.lk) frame.buttonsHeld |= BTN_LK;
    if (m_currentState.hk) frame.buttonsHeld |= BTN_HK;
    if (m_currentState.special) frame.buttonsHeld |= BTN_SPECIAL;
    if (m_currentState.taunt) frame.buttonsHeld |= BTN_TAUNT;
    if (m_currentState.select) frame.buttonsHeld |= BTN_SELECT;
    if (m_currentState.start) frame.buttonsHeld |= BTN_START;
    if (m_currentState.coin) frame.buttonsHeld |= BTN_COIN;

    frame.buttonsPressed = 0;
    if (m_currentState.lp && !m_prevState.lp) frame.buttonsPressed |= BTN_LP;
    if (m_currentState.hp && !m_prevState.hp) frame.buttonsPressed |= BTN_HP;
    if (m_currentState.lk && !m_prevState.lk) frame.buttonsPressed |= BTN_LK;
    if (m_currentState.hk && !m_prevState.hk) frame.buttonsPressed |= BTN_HK;
    if (m_currentState.special && !m_prevState.special) frame.buttonsPressed |= BTN_SPECIAL;
    if (m_currentState.taunt && !m_prevState.taunt) frame.buttonsPressed |= BTN_TAUNT;
    if (m_currentState.select && !m_prevState.select) frame.buttonsPressed |= BTN_SELECT;
    if (m_currentState.start && !m_prevState.start) frame.buttonsPressed |= BTN_START;
    if (m_currentState.coin && !m_prevState.coin) frame.buttonsPressed |= BTN_COIN;

    if (frame.relDir == DIR_REL_BACK || frame.relDir == DIR_REL_DOWN_BACK || frame.relDir == DIR_REL_UP_BACK) {
        m_chargeBackFrames++;
    } else {
        m_chargeBackFrames = 0;
    }

    if (frame.relDir == DIR_REL_DOWN || frame.relDir == DIR_REL_DOWN_BACK || frame.relDir == DIR_REL_DOWN_FORWARD) {
        m_chargeDownFrames++;
    } else {
        m_chargeDownFrames = 0;
    }
}

void InputManager::UpdateSimulated(const RawInputState& simState, bool facingRight) {
    m_prevState = m_currentState;
    m_currentState = simState;

    m_head = (m_head + 1) % BUFFER_SIZE;
    BufferedFrame& frame = m_history[m_head];

    frame.rawDir = 0;
    if (m_currentState.up) frame.rawDir |= 1;
    if (m_currentState.down) frame.rawDir |= 2;
    if (m_currentState.left) frame.rawDir |= 4;
    if (m_currentState.right) frame.rawDir |= 8;

    frame.relDir = ComputeRelativeDirection(m_currentState, facingRight);

    frame.buttonsHeld = 0;
    if (m_currentState.lp) frame.buttonsHeld |= BTN_LP;
    if (m_currentState.hp) frame.buttonsHeld |= BTN_HP;
    if (m_currentState.lk) frame.buttonsHeld |= BTN_LK;
    if (m_currentState.hk) frame.buttonsHeld |= BTN_HK;
    if (m_currentState.special) frame.buttonsHeld |= BTN_SPECIAL;
    if (m_currentState.taunt) frame.buttonsHeld |= BTN_TAUNT;
    if (m_currentState.select) frame.buttonsHeld |= BTN_SELECT;
    if (m_currentState.start) frame.buttonsHeld |= BTN_START;
    if (m_currentState.coin) frame.buttonsHeld |= BTN_COIN;

    frame.buttonsPressed = 0;
    if (m_currentState.lp && !m_prevState.lp) frame.buttonsPressed |= BTN_LP;
    if (m_currentState.hp && !m_prevState.hp) frame.buttonsPressed |= BTN_HP;
    if (m_currentState.lk && !m_prevState.lk) frame.buttonsPressed |= BTN_LK;
    if (m_currentState.hk && !m_prevState.hk) frame.buttonsPressed |= BTN_HK;
    if (m_currentState.special && !m_prevState.special) frame.buttonsPressed |= BTN_SPECIAL;
    if (m_currentState.taunt && !m_prevState.taunt) frame.buttonsPressed |= BTN_TAUNT;
    if (m_currentState.select && !m_prevState.select) frame.buttonsPressed |= BTN_SELECT;
    if (m_currentState.start && !m_prevState.start) frame.buttonsPressed |= BTN_START;
    if (m_currentState.coin && !m_prevState.coin) frame.buttonsPressed |= BTN_COIN;

    if (frame.relDir == DIR_REL_BACK || frame.relDir == DIR_REL_DOWN_BACK || frame.relDir == DIR_REL_UP_BACK) {
        m_chargeBackFrames++;
    } else {
        m_chargeBackFrames = 0;
    }

    if (frame.relDir == DIR_REL_DOWN || frame.relDir == DIR_REL_DOWN_BACK || frame.relDir == DIR_REL_DOWN_FORWARD) {
        m_chargeDownFrames++;
    } else {
        m_chargeDownFrames = 0;
    }
}

void InputManager::InjectSpecialMotion(SpecialMotionType motion, uint16_t buttonMask) {
    m_injectedMotion = motion;
    m_injectedButton = buttonMask;
}

SpecialMotionType InputManager::CheckSpecialMotion(int& outButtonUsed) {
    if (m_injectedMotion != MOTION_NONE) {
        SpecialMotionType m = m_injectedMotion;
        outButtonUsed = m_injectedButton;
        m_injectedMotion = MOTION_NONE;
        m_injectedButton = 0;
        return m;
    }

    const BufferedFrame& curr = m_history[m_head];
    if (curr.buttonsPressed == 0) {
        return MOTION_NONE;
    }

    outButtonUsed = curr.buttonsPressed;

    if (curr.buttonsPressed & (BTN_LP | BTN_HP | BTN_LK | BTN_HK)) {
        int sStage = 0;
        for (int i = 26; i >= 0; i--) {
            int idx = (m_head - i + BUFFER_SIZE) % BUFFER_SIZE;
            RelativeDirection d = m_history[idx].relDir;
            if (sStage == 0 && d == DIR_REL_DOWN) sStage = 1;
            else if (sStage == 1 && (d == DIR_REL_DOWN_FORWARD || d == DIR_REL_FORWARD)) sStage = 2;
            else if (sStage == 2 && d == DIR_REL_DOWN) sStage = 3;
            else if (sStage == 3 && (d == DIR_REL_DOWN_FORWARD || d == DIR_REL_FORWARD)) {
                return MOTION_SUPER_OMNIBUS;
            }
        }
    }

    auto GetPastFrame = [&](int framesAgo) -> const BufferedFrame& {
        int idx = (m_head - framesAgo + BUFFER_SIZE) % BUFFER_SIZE;
        return m_history[idx];
    };

    if (curr.buttonsPressed & BTN_HP) {
        bool sawForward = false;
        bool sawDown = false;
        bool sawBack = false;
        for (int i = 0; i < 20; i++) {
            RelativeDirection d = GetPastFrame(i).relDir;
            if (d == DIR_REL_FORWARD || d == DIR_REL_DOWN_FORWARD) sawForward = true;
            if (sawForward && (d == DIR_REL_DOWN || d == DIR_REL_DOWN_BACK || d == DIR_REL_DOWN_FORWARD)) sawDown = true;
            if (sawDown && (d == DIR_REL_BACK || d == DIR_REL_DOWN_BACK)) {
                sawBack = true;
                break;
            }
        }
        if (sawForward && sawDown && sawBack) {
            return MOTION_COMMAND_GRAB;
        }
    }

    if (curr.buttonsPressed & (BTN_LP | BTN_HP | BTN_LK | BTN_HK)) {
        bool sawFwdOrDF = (curr.relDir == DIR_REL_FORWARD || curr.relDir == DIR_REL_DOWN_FORWARD);
        int step = sawFwdOrDF ? 1 : 0;
        int dIndex = -1;

        for (int i = step; i < MOTION_WINDOW; i++) {
            RelativeDirection d = GetPastFrame(i).relDir;
            if (step == 0) {
                if (d == DIR_REL_FORWARD || d == DIR_REL_DOWN_FORWARD) {
                    step = 1;
                }
            } else if (step == 1) {
                if (d == DIR_REL_DOWN || d == DIR_REL_DOWN_BACK || d == DIR_REL_DOWN_FORWARD) {
                    step = 2;
                    dIndex = i;
                }
            } else if (step == 2) {
                if (d == DIR_REL_FORWARD || d == DIR_REL_UP_FORWARD) {

                    if (curr.buttonsPressed & (BTN_LP | BTN_HP)) return MOTION_DP_P;
                    if (curr.buttonsPressed & (BTN_LK | BTN_HK)) return MOTION_DP_K;
                }
            }
        }
    }

    if (curr.buttonsPressed & (BTN_LP | BTN_HP | BTN_LK | BTN_HK)) {
        bool sawFwd = (curr.relDir == DIR_REL_FORWARD || curr.relDir == DIR_REL_DOWN_FORWARD);
        int stage = sawFwd ? 1 : 0;

        for (int i = (sawFwd ? 1 : 0); i < MOTION_WINDOW; i++) {
            RelativeDirection d = GetPastFrame(i).relDir;
            if (stage == 0) {
                if (d == DIR_REL_FORWARD) stage = 1;
            } else if (stage == 1) {
                if (d == DIR_REL_DOWN_FORWARD || d == DIR_REL_FORWARD) stage = 2;
            } else if (stage == 2) {
                if (d == DIR_REL_DOWN || d == DIR_REL_DOWN_FORWARD) {
                    if (curr.buttonsPressed & (BTN_LP | BTN_HP)) return MOTION_QCF_P;
                    if (curr.buttonsPressed & (BTN_LK | BTN_HK)) return MOTION_QCF_K;
                }
            }
        }
    }

    if (curr.buttonsPressed & (BTN_LP | BTN_HP | BTN_LK | BTN_HK)) {
        bool sawBack = (curr.relDir == DIR_REL_BACK || curr.relDir == DIR_REL_DOWN_BACK);
        int stage = sawBack ? 1 : 0;

        for (int i = (sawBack ? 1 : 0); i < MOTION_WINDOW; i++) {
            RelativeDirection d = GetPastFrame(i).relDir;
            if (stage == 0) {
                if (d == DIR_REL_BACK) stage = 1;
            } else if (stage == 1) {
                if (d == DIR_REL_DOWN_BACK || d == DIR_REL_BACK) stage = 2;
            } else if (stage == 2) {
                if (d == DIR_REL_DOWN || d == DIR_REL_DOWN_BACK) {
                    if (curr.buttonsPressed & (BTN_LP | BTN_HP)) return MOTION_QCB_P;
                    if (curr.buttonsPressed & (BTN_LK | BTN_HK)) return MOTION_QCB_K;
                }
            }
        }
    }

    if (curr.relDir == DIR_REL_FORWARD || curr.relDir == DIR_REL_DOWN_FORWARD) {

        bool hadCharge = false;
        for (int i = 1; i <= 8; i++) {
            int pastCharge = (m_head - i + BUFFER_SIZE) % BUFFER_SIZE;

            int backCount = 0;
            for (int k = i; k < i + 50 && k < BUFFER_SIZE; k++) {
                RelativeDirection d = GetPastFrame(k).relDir;
                if (d == DIR_REL_BACK || d == DIR_REL_DOWN_BACK || d == DIR_REL_UP_BACK) {
                    backCount++;
                } else {
                    break;
                }
            }
            if (backCount >= CHARGE_REQUIRED_FRAMES - 5) {
                hadCharge = true;
                break;
            }
        }
        if (hadCharge) {
            if (curr.buttonsPressed & (BTN_LP | BTN_HP)) return MOTION_CHARGE_B_F_P;
            if (curr.buttonsPressed & (BTN_LK | BTN_HK)) return MOTION_CHARGE_B_F_K;
        }
    }

    if (curr.relDir == DIR_REL_UP || curr.relDir == DIR_REL_UP_FORWARD || curr.relDir == DIR_REL_UP_BACK) {
        bool hadCharge = false;
        for (int i = 1; i <= 8; i++) {
            int downCount = 0;
            for (int k = i; k < i + 50 && k < BUFFER_SIZE; k++) {
                RelativeDirection d = GetPastFrame(k).relDir;
                if (d == DIR_REL_DOWN || d == DIR_REL_DOWN_BACK || d == DIR_REL_DOWN_FORWARD) {
                    downCount++;
                } else {
                    break;
                }
            }
            if (downCount >= CHARGE_REQUIRED_FRAMES - 5) {
                hadCharge = true;
                break;
            }
        }
        if (hadCharge) {
            if (curr.buttonsPressed & (BTN_LK | BTN_HK)) return MOTION_CHARGE_D_U_K;
            if (curr.buttonsPressed & (BTN_LP | BTN_HP)) return MOTION_CHARGE_B_F_P;
        }
    }

    return MOTION_NONE;
}

void InputManager::ConsumeMotion() {
    m_history[m_head].buttonsPressed = 0;
    m_chargeBackFrames = 0;
    m_chargeDownFrames = 0;
}

void InputManager::Reset() {
    m_currentState = RawInputState{};
    m_prevState = RawInputState{};
    m_history.fill(BufferedFrame{});
    m_head = 0;
    m_chargeBackFrames = 0;
    m_chargeDownFrames = 0;
    m_injectedMotion = MOTION_NONE;
    m_injectedButton = 0;
}

bool InputManager::IsTriggerEasterEggPressed() const {

    if ((IsKeyDown(KEY_Q) && IsKeyDown(KEY_E)) ||
        (IsKeyDown(KEY_LEFT_BRACKET) && IsKeyDown(KEY_RIGHT_BRACKET)) ||
        IsKeyPressed(KEY_T) ||
        (IsKeyDown(KEY_U) && IsKeyDown(KEY_O))) {
        return true;
    }

    for (int pad = 0; pad < 4; pad++) {
        if (!IsGamepadAvailable(pad)) continue;

        bool leftTrigger = IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ||
                           IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_2) ||
                           (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.3f);

        bool rightTrigger = IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ||
                            IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2) ||
                            (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.3f);

        if (leftTrigger && rightTrigger) {
            return true;
        }
    }

    return false;
}

bool InputManager::IsSecretMenuComboPressed() const {

    if ((IsKeyDown(KEY_Q) && IsKeyDown(KEY_E)) ||
        (IsKeyDown(KEY_LEFT_BRACKET) && IsKeyDown(KEY_RIGHT_BRACKET))) {
        return true;
    }

    for (int pad = 0; pad < 4; pad++) {
        if (!IsGamepadAvailable(pad)) continue;

        bool leftTrigger = IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ||
                           IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_2) ||
                           (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.4f);

        bool rightTrigger = IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ||
                            IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2) ||
                            (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.4f);

        if (leftTrigger && rightTrigger) {
            return true;
        }
    }

    return false;
}
