#include "InputManager.h"
#include <cmath>

namespace {
bool s_queuedKeyPress[512] = { false };
}

void InputManager::BeginRenderFrame() {
    for (int i = 0; i < 512; ++i) {
        s_queuedKeyPress[i] = false;
    }
    int k = GetKeyPressed();
    while (k > 0) {
        if (k < 512) {
            s_queuedKeyPress[k] = true;
        }
        k = GetKeyPressed();
    }
}

bool InputManager::WasKeyPressed(int key) {
    if (key <= 0 || key >= 512) return false;
    return IsKeyPressed(key) || s_queuedKeyPress[key];
}

InputManager::InputManager() {
    m_history.fill(BufferedFrame{});
}

RelativeDirection InputManager::ComputeRelativeDirection(const RawInputState& state, bool facingRight) {
    bool up = state.up;
    bool down = state.down;
    bool left = state.left;
    bool right = state.right;

    if (left && right) {
        left = false;
        right = false;
    }
    if (up && down) {
        down = false;
    }

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
    bool altHeld = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
    auto KeyActive = [](int key) -> bool {
        return IsKeyDown(key) || WasKeyPressed(key);
    };

    if (playerIndex == 0) {
        if (KeyActive(KEY_W) || KeyActive(KEY_UP)) state.up = true;
        if (KeyActive(KEY_S) || KeyActive(KEY_DOWN)) state.down = true;
        if (KeyActive(KEY_A) || KeyActive(KEY_LEFT)) state.left = true;
        if (KeyActive(KEY_D) || KeyActive(KEY_RIGHT)) state.right = true;

        if (KeyActive(KEY_U) || KeyActive(KEY_Z)) state.lp = true;
        if (KeyActive(KEY_I) || KeyActive(KEY_X)) state.hp = true;
        if (KeyActive(KEY_J) || KeyActive(KEY_C)) state.lk = true;
        if (KeyActive(KEY_K) || KeyActive(KEY_V)) state.hk = true;

        if (KeyActive(KEY_O) || KeyActive(KEY_E)) state.special = true;
        if (KeyActive(KEY_Y) || KeyActive(KEY_Q) || KeyActive(KEY_T)) state.taunt = true;
        if (KeyActive(KEY_L) || KeyActive(KEY_LEFT_SHIFT) || KeyActive(KEY_RIGHT_SHIFT) ||
            KeyActive(KEY_LEFT_CONTROL) || KeyActive(KEY_RIGHT_CONTROL)) state.block = true;
        if (KeyActive(KEY_R)) state.finisher = true;
        if (KeyActive(KEY_BACKSPACE)) state.select = true;
        if (!altHeld && (KeyActive(KEY_ENTER) || KeyActive(KEY_KP_ENTER) || KeyActive(KEY_ONE))) state.start = true;
        if (KeyActive(KEY_SPACE) || KeyActive(KEY_FIVE)) state.coin = true;
    } else {
        if (KeyActive(KEY_KP_8)) state.up = true;
        if (KeyActive(KEY_KP_2)) state.down = true;
        if (KeyActive(KEY_KP_4)) state.left = true;
        if (KeyActive(KEY_KP_6)) state.right = true;

        if (KeyActive(KEY_KP_7)) state.lp = true;
        if (KeyActive(KEY_KP_9)) state.hp = true;
        if (KeyActive(KEY_KP_1)) state.lk = true;
        if (KeyActive(KEY_KP_3)) state.hk = true;

        if (KeyActive(KEY_KP_ADD)) state.special = true;
        if (KeyActive(KEY_KP_DECIMAL)) state.taunt = true;
        if (KeyActive(KEY_KP_0)) state.block = true;
        if (KeyActive(KEY_KP_SUBTRACT)) state.finisher = true;
    }

    int startPad = (playerIndex == 0) ? 0 : GetGamepadSlotForPlayer(playerIndex);
    int endPad = (playerIndex == 0) ? 3 : startPad;
    for (int pad = startPad; pad >= 0 && pad <= endPad; ++pad) {
        if (!IsGamepadAvailable(pad)) continue;

        constexpr float STICK_UP_THRESHOLD = -0.35f;
        constexpr float STICK_DOWN_THRESHOLD = 0.35f;
        constexpr float STICK_HORIZ_THRESHOLD = 0.30f;

        float axisX = GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_X);
        float axisY = GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_Y);

        if (axisY < STICK_UP_THRESHOLD || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_UP)) state.up = true;
        if (axisY > STICK_DOWN_THRESHOLD || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) state.down = true;
        if (axisX < -STICK_HORIZ_THRESHOLD || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) state.left = true;
        if (axisX > STICK_HORIZ_THRESHOLD || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) state.right = true;

        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) state.lp = true;
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_UP)) state.hp = true;
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) state.lk = true;
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) state.hk = true;

        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2) ||
            GetGamepadAxisMovement(pad, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.30f) {
            state.special = true;
        }
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
            state.finisher = true;
        }

        bool l2Down = IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_2) ||
                      (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.30f);
        if (l2Down) {
            state.block = true;
        }
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) {
            state.taunt = true;
        }
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT)) state.start = true;
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_LEFT) ||
            (!l2Down && IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE))) {
            state.coin = true;
            state.select = true;
        }
    }

    return state;
}

bool InputManager::HasHumanInput(int playerIndex) {
    return (playerIndex == 0);
}

void InputManager::RecordFrame(const RawInputState& state, bool facingRight) {
    m_head = (m_head + 1) % BUFFER_SIZE;
    BufferedFrame& frame = m_history[m_head];

    frame.rawDir = 0;
    if (state.up) frame.rawDir |= 1;
    if (state.down) frame.rawDir |= 2;
    if (state.left) frame.rawDir |= 4;
    if (state.right) frame.rawDir |= 8;

    frame.relDir = ComputeRelativeDirection(state, facingRight);

    frame.buttonsHeld = 0;
    if (state.lp) frame.buttonsHeld |= BTN_LP;
    if (state.hp) frame.buttonsHeld |= BTN_HP;
    if (state.lk) frame.buttonsHeld |= BTN_LK;
    if (state.hk) frame.buttonsHeld |= BTN_HK;
    if (state.special) frame.buttonsHeld |= BTN_SPECIAL;
    if (state.taunt) frame.buttonsHeld |= BTN_TAUNT;
    if (state.block) frame.buttonsHeld |= BTN_BLOCK;
    if (state.finisher) frame.buttonsHeld |= BTN_FINISHER;
    if (state.select) frame.buttonsHeld |= BTN_SELECT;
    if (state.start) frame.buttonsHeld |= BTN_START;
    if (state.coin) frame.buttonsHeld |= BTN_COIN;

    frame.buttonsPressed = 0;
    if (state.lp && !m_prevState.lp) frame.buttonsPressed |= BTN_LP;
    if (state.hp && !m_prevState.hp) frame.buttonsPressed |= BTN_HP;
    if (state.lk && !m_prevState.lk) frame.buttonsPressed |= BTN_LK;
    if (state.hk && !m_prevState.hk) frame.buttonsPressed |= BTN_HK;
    if (state.special && !m_prevState.special) frame.buttonsPressed |= BTN_SPECIAL;
    if (state.taunt && !m_prevState.taunt) frame.buttonsPressed |= BTN_TAUNT;
    if (state.block && !m_prevState.block) frame.buttonsPressed |= BTN_BLOCK;
    if (state.finisher && !m_prevState.finisher) frame.buttonsPressed |= BTN_FINISHER;
    if (state.select && !m_prevState.select) frame.buttonsPressed |= BTN_SELECT;
    if (state.start && !m_prevState.start) frame.buttonsPressed |= BTN_START;
    if (state.coin && !m_prevState.coin) frame.buttonsPressed |= BTN_COIN;

    frame.buttonsReleased = 0;
    if (!state.lp && m_prevState.lp) frame.buttonsReleased |= BTN_LP;
    if (!state.hp && m_prevState.hp) frame.buttonsReleased |= BTN_HP;
    if (!state.lk && m_prevState.lk) frame.buttonsReleased |= BTN_LK;
    if (!state.hk && m_prevState.hk) frame.buttonsReleased |= BTN_HK;

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

void InputManager::LatchRenderFrame(int playerIndex) {
    RawInputState s = PollRawInputs(playerIndex);
    if (s.up) m_latchedPress.up = true;
    if (s.down) m_latchedPress.down = true;
    if (s.left) m_latchedPress.left = true;
    if (s.right) m_latchedPress.right = true;
    if (s.lp) m_latchedPress.lp = true;
    if (s.hp) m_latchedPress.hp = true;
    if (s.lk) m_latchedPress.lk = true;
    if (s.hk) m_latchedPress.hk = true;
    if (s.special) m_latchedPress.special = true;
    if (s.taunt) m_latchedPress.taunt = true;
    if (s.block) m_latchedPress.block = true;
    if (s.finisher) m_latchedPress.finisher = true;
    if (s.select) m_latchedPress.select = true;
    if (s.start) m_latchedPress.start = true;
    if (s.coin) m_latchedPress.coin = true;
}

void InputManager::Update(int playerIndex, bool facingRight, bool isExtraTurboSubstep) {
    m_prevState = m_currentState;
    RawInputState polled = PollRawInputs(playerIndex);
    if (!isExtraTurboSubstep) {
        polled.up = polled.up || m_latchedPress.up;
        polled.down = polled.down || m_latchedPress.down;
        polled.left = polled.left || m_latchedPress.left;
        polled.right = polled.right || m_latchedPress.right;
        polled.lp = polled.lp || m_latchedPress.lp;
        polled.hp = polled.hp || m_latchedPress.hp;
        polled.lk = polled.lk || m_latchedPress.lk;
        polled.hk = polled.hk || m_latchedPress.hk;
        polled.special = polled.special || m_latchedPress.special;
        polled.taunt = polled.taunt || m_latchedPress.taunt;
        polled.block = polled.block || m_latchedPress.block;
        polled.finisher = polled.finisher || m_latchedPress.finisher;
        polled.select = polled.select || m_latchedPress.select;
        polled.start = polled.start || m_latchedPress.start;
        polled.coin = polled.coin || m_latchedPress.coin;
        m_latchedPress = RawInputState{};
    }
    m_currentState = polled;
    RecordFrame(m_currentState, facingRight);
}

void InputManager::UpdateSimulated(const RawInputState& simState, bool facingRight) {
    m_prevState = m_currentState;
    m_currentState = simState;
    RecordFrame(m_currentState, facingRight);
}

void InputManager::InjectSpecialMotion(SpecialMotionType motion, uint16_t buttonMask) {
    m_injectedMotion = motion;
    m_injectedButton = buttonMask;
}

int InputManager::CheckDashInput() const {
    const BufferedFrame& curr = m_history[m_head];
    const BufferedFrame& prev = GetHistoryFrame(1);

    if (curr.relDir == DIR_REL_FORWARD && prev.relDir != DIR_REL_FORWARD) {
        bool sawNeutral = false;
        for (int i = 1; i <= 12; i++) {
            RelativeDirection d = GetHistoryFrame(i).relDir;
            if (d == DIR_REL_NEUTRAL) sawNeutral = true;
            else if (sawNeutral && d == DIR_REL_FORWARD) return 1;
            else if (d == DIR_REL_BACK || d == DIR_REL_UP || d == DIR_REL_DOWN) break;
        }
    } else if (curr.relDir == DIR_REL_BACK && prev.relDir != DIR_REL_BACK) {
        bool sawNeutral = false;
        for (int i = 1; i <= 12; i++) {
            RelativeDirection d = GetHistoryFrame(i).relDir;
            if (d == DIR_REL_NEUTRAL) sawNeutral = true;
            else if (sawNeutral && d == DIR_REL_BACK) return -1;
            else if (d == DIR_REL_FORWARD || d == DIR_REL_UP || d == DIR_REL_DOWN) break;
        }
    }
    return 0;
}

bool InputManager::IsThrowInputPressed() const {
    const BufferedFrame& curr = m_history[m_head];
    const BufferedFrame& prev = GetHistoryFrame(1);

    uint16_t recentPress = curr.buttonsPressed | prev.buttonsPressed;
    if ((curr.buttonsPressed & (BTN_LP | BTN_LK)) && ((recentPress & (BTN_LP | BTN_LK)) == (BTN_LP | BTN_LK))) {
        return true;
    }

    return false;
}

bool InputManager::IsAlphaCounterPressed() const {
    const BufferedFrame& curr = m_history[m_head];
    if (curr.relDir == DIR_REL_FORWARD || curr.relDir == DIR_REL_DOWN_FORWARD) {
        uint16_t both = (curr.buttonsPressed | curr.buttonsHeld);
        if ((curr.buttonsPressed & (BTN_HP | BTN_HK | BTN_SPECIAL)) &&
            (((both & (BTN_HP | BTN_HK)) == (BTN_HP | BTN_HK)) || (curr.buttonsPressed & BTN_SPECIAL))) {
            return true;
        }
    }
    return false;
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

    uint16_t trig = curr.buttonsPressed | (curr.buttonsReleased & (BTN_LP | BTN_HP | BTN_LK | BTN_HK));
    if (trig == 0) {
        return MOTION_NONE;
    }

    outButtonUsed = (curr.buttonsPressed != 0) ? curr.buttonsPressed : trig;

    auto GetPastFrame = [&](int framesAgo) -> const BufferedFrame& {
        int idx = (m_head - framesAgo + BUFFER_SIZE) % BUFFER_SIZE;
        return m_history[idx];
    };

    if (trig & (BTN_LP | BTN_HP | BTN_LK | BTN_HK | BTN_SPECIAL)) {
        int sStage = 0;
        for (int i = 28; i >= 0; i--) {
            RelativeDirection d = GetPastFrame(i).relDir;
            if (sStage == 0 && d == DIR_REL_DOWN) sStage = 1;
            else if (sStage == 1 && (d == DIR_REL_DOWN_FORWARD || d == DIR_REL_FORWARD)) sStage = 2;
            else if (sStage == 2 && d == DIR_REL_DOWN) sStage = 3;
            else if (sStage == 3 && (d == DIR_REL_DOWN_FORWARD || d == DIR_REL_FORWARD)) {
                return MOTION_SUPER_OMNIBUS;
            }
        }

        bool twoButtons = ((curr.buttonsHeld & (BTN_LP | BTN_HP)) == (BTN_LP | BTN_HP)) ||
                          ((curr.buttonsHeld & (BTN_LK | BTN_HK)) == (BTN_LK | BTN_HK));
        if (twoButtons && (curr.buttonsPressed & (BTN_LP | BTN_HP | BTN_LK | BTN_HK))) {
            bool sawD = false, sawF = false;
            for (int i = 14; i >= 0; i--) {
                RelativeDirection d = GetPastFrame(i).relDir;
                if (d == DIR_REL_DOWN) sawD = true;
                else if (sawD && (d == DIR_REL_DOWN_FORWARD || d == DIR_REL_FORWARD)) sawF = true;
            }
            if (sawD && sawF) return MOTION_SUPER_OMNIBUS;
        }
    }

    if (curr.buttonsPressed & BTN_HP) {
        bool sawForward = false, sawDown = false, sawBack = false;
        for (int i = 0; i < 22; i++) {
            RelativeDirection d = GetPastFrame(i).relDir;
            if (d == DIR_REL_FORWARD || d == DIR_REL_DOWN_FORWARD) sawForward = true;
            if (sawForward && (d == DIR_REL_DOWN || d == DIR_REL_DOWN_BACK || d == DIR_REL_DOWN_FORWARD)) sawDown = true;
            if (sawDown && (d == DIR_REL_BACK || d == DIR_REL_DOWN_BACK)) { sawBack = true; break; }
        }
        if (sawForward && sawDown && sawBack) return MOTION_COMMAND_GRAB;
    }

    if (trig & (BTN_LP | BTN_HP | BTN_LK | BTN_HK)) {
        bool sawFwdOrDF = (curr.relDir == DIR_REL_FORWARD || curr.relDir == DIR_REL_DOWN_FORWARD);
        int step = sawFwdOrDF ? 1 : 0;
        for (int i = step; i < MOTION_WINDOW; i++) {
            RelativeDirection d = GetPastFrame(i).relDir;
            if (step == 0) {
                if (d == DIR_REL_FORWARD || d == DIR_REL_DOWN_FORWARD) step = 1;
            } else if (step == 1) {
                if (d == DIR_REL_DOWN || d == DIR_REL_DOWN_BACK) step = 2;
            } else if (step == 2) {
                if (d == DIR_REL_FORWARD || d == DIR_REL_DOWN_FORWARD || d == DIR_REL_UP_FORWARD) {
                    if (trig & (BTN_LP | BTN_HP)) return MOTION_DP_P;
                    if (trig & (BTN_LK | BTN_HK)) return MOTION_DP_K;
                }
            }
        }
    }

    if (trig & (BTN_LP | BTN_HP | BTN_LK | BTN_HK)) {
        bool sawEndF = false, sawStartD = false;
        for (int i = 0; i < MOTION_WINDOW; i++) {
            RelativeDirection d = GetPastFrame(i).relDir;
            if (!sawEndF) {
                if (d == DIR_REL_FORWARD || d == DIR_REL_DOWN_FORWARD) sawEndF = true;
            } else {
                if (d == DIR_REL_DOWN) { sawStartD = true; break; }
            }
        }
        if (sawEndF && sawStartD) {
            if (trig & (BTN_LP | BTN_HP)) return MOTION_QCF_P;
            if (trig & (BTN_LK | BTN_HK)) return MOTION_QCF_K;
        }
    }

    if (trig & (BTN_LP | BTN_HP | BTN_LK | BTN_HK)) {
        bool sawEndB = false, sawStartD = false;
        for (int i = 0; i < MOTION_WINDOW; i++) {
            RelativeDirection d = GetPastFrame(i).relDir;
            if (!sawEndB) {
                if (d == DIR_REL_BACK || d == DIR_REL_DOWN_BACK) sawEndB = true;
            } else {
                if (d == DIR_REL_DOWN) { sawStartD = true; break; }
            }
        }
        if (sawEndB && sawStartD) {
            if (trig & (BTN_LP | BTN_HP)) return MOTION_QCB_P;
            if (trig & (BTN_LK | BTN_HK)) return MOTION_QCB_K;
        }
    }

    if (curr.buttonsPressed & (BTN_LP | BTN_HP | BTN_LK | BTN_HK)) {
        if (curr.relDir == DIR_REL_DOWN) {
            bool sawN = false;
            for (int i = 1; i < 16; i++) {
                RelativeDirection d = GetPastFrame(i).relDir;
                if (d == DIR_REL_NEUTRAL) sawN = true;
                else if (sawN && d == DIR_REL_DOWN) return MOTION_DOWN_DOWN;
            }
        }
    }

    if (curr.relDir == DIR_REL_FORWARD || curr.relDir == DIR_REL_DOWN_FORWARD) {
        bool hadCharge = false;
        for (int i = 1; i <= 10; i++) {
            int backCount = 0;
            for (int k = i; k < i + 50 && k < BUFFER_SIZE; k++) {
                RelativeDirection d = GetPastFrame(k).relDir;
                if (d == DIR_REL_BACK || d == DIR_REL_DOWN_BACK || d == DIR_REL_UP_BACK) backCount++;
                else break;
            }
            if (backCount >= CHARGE_REQUIRED_FRAMES) { hadCharge = true; break; }
        }
        if (hadCharge) {
            if (trig & (BTN_LP | BTN_HP)) return MOTION_CHARGE_B_F_P;
            if (trig & (BTN_LK | BTN_HK)) return MOTION_CHARGE_B_F_K;
        }
    }

    if (curr.relDir == DIR_REL_UP || curr.relDir == DIR_REL_UP_FORWARD || curr.relDir == DIR_REL_UP_BACK) {
        bool hadCharge = false;
        for (int i = 1; i <= 10; i++) {
            int downCount = 0;
            for (int k = i; k < i + 50 && k < BUFFER_SIZE; k++) {
                RelativeDirection d = GetPastFrame(k).relDir;
                if (d == DIR_REL_DOWN || d == DIR_REL_DOWN_BACK || d == DIR_REL_DOWN_FORWARD) downCount++;
                else break;
            }
            if (downCount >= CHARGE_REQUIRED_FRAMES) { hadCharge = true; break; }
        }
        if (hadCharge) {
            if (trig & (BTN_LK | BTN_HK)) return MOTION_CHARGE_D_U_K;
            if (trig & (BTN_LP | BTN_HP)) return MOTION_CHARGE_B_F_P;
        }
    }

    return MOTION_NONE;
}

void InputManager::ConsumeMotion() {
    m_history[m_head].buttonsPressed = 0;
    m_history[m_head].buttonsReleased = 0;
    m_chargeBackFrames = 0;
    m_chargeDownFrames = 0;
}

void InputManager::Reset() {
    m_currentState = RawInputState{};
    m_prevState = RawInputState{};
    m_latchedPress = RawInputState{};
    m_history.fill(BufferedFrame{});
    m_head = 0;
    m_chargeBackFrames = 0;
    m_chargeDownFrames = 0;
    m_injectedMotion = MOTION_NONE;
    m_injectedButton = 0;
}

bool InputManager::IsTriggerEasterEggPressed() const {
    if (WasKeyPressed(KEY_F4)) {
        return true;
    }
    for (int pad = 0; pad < 4; pad++) {
        if (!IsGamepadAvailable(pad)) continue;
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_LEFT) &&
            IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
            return true;
        }
    }
    return false;
}

bool InputManager::IsSecretMenuComboPressed() const {
    if (WasKeyPressed(KEY_F3) ||
        (IsKeyDown(KEY_LEFT_BRACKET) && IsKeyDown(KEY_RIGHT_BRACKET))) {
        return true;
    }
    for (int pad = 0; pad < 4; pad++) {
        if (!IsGamepadAvailable(pad)) continue;
        if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_LEFT) &&
            IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1)) {
            return true;
        }
        bool leftTrig = IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_2) ||
                        (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.4f);
        bool rightTrig = IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2) ||
                         (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.4f);
        bool trigEdge = IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_2) ||
                        IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_2) ||
                        (GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_TRIGGER) > 0.4f &&
                         GetGamepadAxisMovement(pad, GAMEPAD_AXIS_RIGHT_TRIGGER) > 0.4f);
        if (leftTrig && rightTrig && trigEdge) return true;
    }
    return false;
}
