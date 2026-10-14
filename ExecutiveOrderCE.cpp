// ============================================================================
// EXECUTIVE ORDER II: TOTAL VETO - CHAMPIONSHIP EDITION (ExecutiveOrderCE.cpp)
// Complete Single-File C++17 / Raylib 5.0 16-Bit Arcade Fighting Game Engine
// ============================================================================
// Embedded Raylib / zlib License Notice:
// Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
// This software is provided "as-is", without any express or implied warranty.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the standard zlib/libpng license restrictions.
// ============================================================================

// --- Common.h ---
#include <cstdint>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include "raylib.h"

constexpr int SCREEN_WIDTH = 384;
constexpr int SCREEN_HEIGHT = 224;
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr int TARGET_FPS = 60;
constexpr float FIXED_DT = 1.0f / 60.0f;
constexpr float PI_F = 3.14159265358979323846f;

constexpr float STAGE_MIN_X = 40.0f;
constexpr float STAGE_MAX_X = 760.0f;
constexpr float STAGE_TOTAL_WIDTH = 800.0f;
constexpr float GROUND_Y = 196.0f;
constexpr float MAX_FIGHTER_DISTANCE = 330.0f;

enum CharacterId {
    CHAR_COMMANDER = 0,
    CHAR_HIGH_BENCH = 1,
    CHAR_REDCAP = 2,
    CHAR_COASTAL_GOV = 3,
    CHAR_TECH_VISIONARY = 4,
    CHAR_FILIBUSTER = 5,
    CHAR_MASCOT = 6,
    CHAR_POPULIST = 7,
    CHAR_SLEEPY_JOE = 8,
    CHAR_MTG = 9,
    CHAR_SUCKRATERY_RKF = 10,
    CHAR_WAR_PIG = 11,
    CHAR_SOCIALIST = 12,
    CHAR_BILLARY = 13,
    CHAR_PALM_BEACH_PETE = 14,
    CHARACTER_COUNT = 15
};
constexpr int PLAYABLE_CHARACTER_COUNT = 10;

enum StageId {
    STAGE_DEMOLISHED_EAST_WING = 0,
    STAGE_REFLECTING_POOL = 1,
    STAGE_CONGRESSIONAL_FLOOR = 2,
    STAGE_WHITE_HOUSE_MMA = 3,
    STAGE_PRESS_BRIEFING = 4,
    STAGE_PENTAGON_ALTAR = 5,
    STAGE_WTC_PLAZA = 6,
    STAGE_COMET_PING_PONG = 7,
    STAGE_FLORIDA_KEYS = 8,
    STAGE_COUNT = 9
};

enum GameState {
    STATE_SPLASH,
    STATE_SELECT_MAP,
    STATE_SECRET_SELECT,
    STATE_VERSUS,
    STATE_FIGHT,
    STATE_VICTORY_SPLASH,
    STATE_BONUS_CAR,
    STATE_MATCH_OVER,
    STATE_ENDING_CELEBRATION,
    STATE_CTHULHU_ENDING
};

enum AttackType {
    ATTACK_NONE = 0,
    ATTACK_MID,
    ATTACK_LOW,
    ATTACK_OVERHEAD,
    ATTACK_THROW,
    ATTACK_PROJECTILE
};

enum AttackMoveType {
    MOVE_LIGHT_PUNCH = 0,
    MOVE_HEAVY_PUNCH = 1,
    MOVE_LIGHT_KICK = 2,
    MOVE_HEAVY_KICK = 3,
    MOVE_SPECIAL = 4
};

enum InputButtons : uint16_t {
    BTN_NONE = 0,
    BTN_LP = (1 << 0),
    BTN_HP = (1 << 1),
    BTN_LK = (1 << 2),
    BTN_HK = (1 << 3),
    DIR_UP = (1 << 4),
    DIR_DOWN = (1 << 5),
    DIR_LEFT = (1 << 6),
    DIR_RIGHT = (1 << 7),
    BTN_START = (1 << 8),
    BTN_COIN = (1 << 9),
    BTN_SPECIAL = (1 << 10),
    BTN_TAUNT = (1 << 11),
    BTN_SELECT = (1 << 12)
};

enum AIDifficulty {
    AI_DIFF_EASY = 0,
    AI_DIFF_NORMAL = 1,
    AI_DIFF_HARD = 2
};

struct Palette16 {
    Color colors[16];
};

inline Rectangle MakeRect(float x, float y, float w, float h) {
    return Rectangle{ x, y, w, h };
}

inline Vector2 MakeVec2(float x, float y) {
    return Vector2{ x, y };
}

// --- Shaders.h ---

#if defined(GRAPHICS_API_OPENGL_ES2) || defined(GRAPHICS_API_OPENGL_ES3)

inline const char* CRT_FRAGMENT_SHADER = R"(#version 100
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float time;
uniform float scanlineIntensity; // default ~0.35
uniform float curvature;         // default ~0.02
uniform float bloom;             // default ~0.15

void main()
{
    vec2 uv = fragTexCoord;
    vec2 dc = uv - 0.5;
    float dist = dot(dc, dc);
    uv = uv + dc * (dist * curvature);

    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec4 col = texture2D(texture0, uv);

    float scanline = sin(uv.y * 224.0 * 3.14159265 * 2.0);
    float scanMod = 1.0 - (scanlineIntensity * (0.5 + 0.5 * scanline));
    col.rgb *= scanMod;

    float subpixel = mod(floor(uv.x * 384.0 * 3.0), 3.0);
    vec3 mask = vec3(0.9, 0.9, 0.9);
    if (subpixel < 1.0) mask = vec3(1.15, 0.88, 0.88);
    else if (subpixel < 2.0) mask = vec3(0.88, 1.15, 0.88);
    else mask = vec3(0.88, 0.88, 1.15);
    col.rgb = mix(col.rgb, col.rgb * mask, bloom);

    float vx = uv.x * (1.0 - uv.x);
    float vy = uv.y * (1.0 - uv.y);
    float vig = clamp(16.0 * vx * vy, 0.0, 1.0);
    col.rgb *= (0.78 + 0.22 * vig);

    gl_FragColor = col * fragColor;
}
)";

inline const char* FLAG_FRAGMENT_SHADER = R"(#version 100
precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform float time;

void main()
{
    vec2 uv = fragTexCoord;
    float ripple = sin(uv.x * 12.0 - time * 6.0) * 0.04 * (uv.x + 0.2);
    float rippleY = cos(uv.x * 8.0 - time * 5.0) * 0.02 * (uv.x + 0.2);
    uv.y += ripple;
    uv.x += rippleY;

    vec4 col = texture2D(texture0, uv);
    float shadow = 1.0 + (cos(uv.x * 12.0 - time * 6.0) * 0.15 * uv.x);
    col.rgb *= shadow;

    gl_FragColor = col * fragColor;
}
)";

#else

inline const char* CRT_FRAGMENT_SHADER = R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float time;
uniform float scanlineIntensity; // default ~0.35
uniform float curvature;         // default ~0.02
uniform float bloom;             // default ~0.15

void main()
{
    // Barrel distortion (subtle 1-pixel curve like a genuine Sony Trinitron / arcade tube)
    vec2 uv = fragTexCoord;
    vec2 dc = uv - 0.5;
    float dist = dot(dc, dc);
    uv = uv + dc * (dist * curvature);

    // Hard clip out-of-bounds screen bezel
    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec4 col = texture(texture0, uv);

    // Horizontal scanline rasterization (alternating dark lines at native 224 scanline pitch)
    float scanline = sin(uv.y * 224.0 * 3.14159265 * 2.0);
    float scanMod = 1.0 - (scanlineIntensity * (0.5 + 0.5 * scanline));
    col.rgb *= scanMod;

    // Subpixel aperture grille / phosphor triad bloom simulation (RGB columns)
    float subpixel = mod(floor(uv.x * 384.0 * 3.0), 3.0);
    vec3 mask = vec3(0.9, 0.9, 0.9);
    if (subpixel < 1.0) mask = vec3(1.15, 0.88, 0.88);
    else if (subpixel < 2.0) mask = vec3(0.88, 1.15, 0.88);
    else mask = vec3(0.88, 0.88, 1.15);
    col.rgb *= mix(vec3(1.0), mask, bloom);

    // Subtle edge vignette (smooth polynomial approximation - 10x faster than pow)
    float vx = uv.x * (1.0 - uv.x);
    float vy = uv.y * (1.0 - uv.y);
    float vig = clamp(16.0 * vx * vy, 0.0, 1.0);
    col.rgb *= (0.78 + 0.22 * vig);

    finalColor = col * fragColor;
}
)";

inline const char* FLAG_FRAGMENT_SHADER = R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;

void main()
{
    vec2 uv = fragTexCoord;
    // Horizontal wind ripples stronger on free right edge
    float ripple = sin(uv.x * 12.0 - time * 6.0) * 0.04 * (uv.x + 0.2);
    float rippleY = cos(uv.x * 8.0 - time * 5.0) * 0.02 * (uv.x + 0.2);
    uv.y += ripple;
    uv.x += rippleY;

    vec4 col = texture(texture0, uv);
    // Light shading on wave crests
    float shadow = 1.0 + (cos(uv.x * 12.0 - time * 6.0) * 0.15 * uv.x);
    col.rgb *= shadow;

    finalColor = col * fragColor;
}
)";

#endif

// --- AssetManager.h ---
#include "raylib.h"

namespace AssetManager {
    void Init();
    bool HasEmbeddedAssets();
    bool FileExists(const char* path);
    Texture2D LoadTexture(const char* path);
    Image LoadImage(const char* path);
}

// --- InputManager.h ---
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
    MOTION_COMMAND_GRAB,
    MOTION_SUPER_OMNIBUS
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
    const BufferedFrame& GetHistoryFrame(int age) const { int idx = (m_head - age + BUFFER_SIZE) % BUFFER_SIZE; return m_history[idx]; }

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

// --- AudioEngine.h ---
#include <vector>
#include <atomic>

enum SfxId {
    SFX_WHOOSH_LIGHT = 0,
    SFX_WHOOSH_HEAVY,
    SFX_HIT_LIGHT,
    SFX_HIT_HEAVY,
    SFX_BLOCK,
    SFX_FIREBALL_LAUNCH,
    SFX_FIREBALL_IMPACT,
    SFX_UPPERCUT,
    SFX_GAVEL_SLAM,
    SFX_MEGAPHONE,
    SFX_ELECTRO_GLITCH,
    SFX_SLIDE,
    SFX_TITLE_SLAM,
    SFX_COIN,
    SFX_SELECT_CURSOR,
    SFX_SELECT_CONFIRM,
    SFX_MATCH_START_FANFARE,
    SFX_VOICE_ROUND1,
    SFX_VOICE_ROUND2,
    SFX_VOICE_FINAL_ROUND,
    SFX_VOICE_FIGHT,
    SFX_VOICE_YOU_WIN,
    SFX_VOICE_TOTAL_VETO,
    SFX_VOICE_HADOUKEN,
    SFX_VOICE_UPPERCUT,
    SFX_VOICE_OBJECTION,
    SFX_VOICE_BOO_YAA,
    SFX_ROUND_WIN,
    SFX_VICTORY_FANFARE,
    SFX_CAR_ALARM,
    SFX_METAL_CRUNCH,
    SFX_GLASS_BREAK,
    SFX_ELEC_SPARK,
    SFX_FART,
    SFX_WORM_HISS,
    SFX_INJECTION,
    SFX_WAR_PIG_ROAR,
    SFX_EMAIL_ZAP,
    SFX_ADRENOCHROME,
    SFX_JESUS_FANFARE,
    SFX_COKE_POP,
    SFX_GAVEL_STOMP,
    SFX_DISSENT_FLAME,
    SFX_SECRET_SERVICE,
    SFX_NOSE_CUT,
    SFX_CTHULHU_ROAR,
    SFX_SHARK_CHOMP,
    SFX_CTHULHU_SNORE,
    SFX_SAXOPHONE,
    SFX_COUNT
};

class AudioEngine {
public:
    static AudioEngine& Instance();

    void Init();
    void Shutdown();

    void PlaySfx(SfxId id, float pitch = 1.0f, float volume = 1.0f);
    void SetBgmTrack(int trackId);
    void StopBgm();
    void Update();
    static void SetCriticalTempo(bool critical);

private:
    AudioEngine() = default;
    ~AudioEngine() = default;

    void SynthesizeAll();
    void SynthesizeBgmTracks();
    Sound BuildWaveformSound(int sampleRate, int sampleCount, int16_t* pcmData);

    static void BgmAudioCallback(void* bufferData, unsigned int frames);

    bool m_initialized = false;
    Sound m_sounds[SFX_COUNT]{};

    std::vector<int16_t> m_bgmTracks[6];
    AudioStream m_bgmStream{};
    bool m_streamActive = false;

    static std::atomic<int> s_currentTrack;
    static std::atomic<size_t> s_trackPlayhead;
    static std::vector<int16_t>* s_trackDataPtr;
};

// --- Fighter.h ---
#include <vector>
#include <string>

enum FighterState {
    FSTATE_IDLE,
    FSTATE_WALK_FWD,
    FSTATE_WALK_BACK,
    FSTATE_JUMP,
    FSTATE_CROUCH,
    FSTATE_BLOCK_HIGH,
    FSTATE_BLOCK_LOW,
    FSTATE_HITSTUN,
    FSTATE_BLOCKSTUN,
    FSTATE_KNOCKDOWN_AIR,
    FSTATE_KNOCKDOWN_GROUND,
    FSTATE_WAKEUP,
    FSTATE_ATTACK,
    FSTATE_SPECIAL,
    FSTATE_TAUNT,
    FSTATE_VICTORY,
    FSTATE_DEFEAT
};

enum HighBenchStage {
    HB_STAGE_THOMAS = 0,
    HB_STAGE_ROBERTS = 1,
    HB_STAGE_SOTOMAYOR = 2
};

struct AttackData {
    std::string name = "Attack";
    int startupFrames = 4;
    int activeFrames = 4;
    int recoveryFrames = 8;
    int damage = 10;
    AttackType type = ATTACK_MID;
    int hitstopFrames = 8;
    int hitstunFrames = 14;
    int blockstunFrames = 10;
    float pushback = 3.5f;
    bool causesKnockdown = false;
    bool isHeavy = false;
    float reachX = 30.0f;
    float reachY = 0.0f;
    float boxW = 28.0f;
    float boxH = 16.0f;
    bool isSpecial = false;
    AttackMoveType moveType = MOVE_LIGHT_PUNCH;
};

struct Projectile {
    Vector2 pos;
    Vector2 vel;
    float width = 18.0f;
    float height = 14.0f;
    int damage = 12;
    int lifetime = 180;
    bool active = false;
    bool facingRight = true;
    int type = 0;
    float bounceY = 0.0f;
};

struct CosmeticPuff {
    Vector2 pos{ 0.0f, 0.0f };
    float timer = 0.0f;
    float maxTimer = 0.5f;
    float baseSize = 8.0f;
};

struct BufferedInput {
    int button = 0;
    SpecialMotionType motion = MOTION_NONE;
    int motionButton = 0;
    bool isCrouch = false;
    int timer = 0;
};

class Fighter {
public:
    Fighter(int playerIndex, CharacterId charId, float startX);

    void ResetRound(float startX, bool faceRight, bool wasKnockedOut = false);
    void UpdatePhysics(const InputManager& input, const Fighter& opponent);
    void UpdateAnimation();

    bool CheckCanBlock(AttackType type) const;
    void ApplyHit(int damage, AttackType type, int hitstop, int hitstun, float pushback, bool knockdown);
    void ApplyBlock(int damage, int blockstop, int blockstun, float pushback);

    Rectangle GetPushbox() const;
    std::vector<Rectangle> GetHurtboxes() const;
    bool GetActiveHitbox(Rectangle& outBox, AttackData& outData) const;

    std::vector<Projectile>& GetProjectiles() { return m_projectiles; }
    const std::vector<Projectile>& GetProjectiles() const { return m_projectiles; }

    std::vector<CosmeticPuff>& GetCosmeticPuffs() { return m_cosmeticPuffs; }
    const std::vector<CosmeticPuff>& GetCosmeticPuffs() const { return m_cosmeticPuffs; }

    int GetPlayerIndex() const { return m_playerIndex; }
    CharacterId GetCharacterId() const { return m_charId; }
    HighBenchStage GetHighBenchStage() const {
        if (m_health > 66) return HB_STAGE_THOMAS;
        if (m_health > 33) return HB_STAGE_ROBERTS;
        return HB_STAGE_SOTOMAYOR;
    }
    std::string GetName() const {
        if (m_charId == CHAR_HIGH_BENCH) {
            HighBenchStage stage = GetHighBenchStage();
            if (stage == HB_STAGE_THOMAS) return "C. THOMAS";
            if (stage == HB_STAGE_ROBERTS) return "J. ROBERTS";
            return "S. SOTOMAYOR";
        }
        return m_name;
    }
    Vector2 GetPosition() const { return m_pos; }
    void SetPositionX(float x) { m_pos.x = x; }
    Vector2 GetVelocity() const { return m_vel; }
    bool IsFacingRight() const { return m_facingRight; }
    void SetFacingRight(bool fr) { m_facingRight = fr; }
    FighterState GetState() const { return m_state; }
    void SetState(FighterState st) { m_state = st; m_stateFrame = 0; }
    void TickWakeup() { if (m_state == FSTATE_WAKEUP) { m_stateFrame++; if (m_stateFrame >= 24) { m_state = FSTATE_IDLE; m_stateFrame = 0; } } }
    int GetHealth() const { return m_health; }
    int GetRefillCount() const { return m_refillCount; }
    int GetRoundsWon() const { return m_roundsWon; }
    void AddRoundWin() { m_roundsWon++; }
    bool IsInHitstop() const { return m_hitstopFrames > 0; }
    void SetHitstop(int frames) { m_hitstopFrames = frames; }
    int GetHitstopFrames() const { return m_hitstopFrames; }
    int GetStateFrame() const { return m_stateFrame; }
    bool HasHitConnected() const { return m_hitConnected; }
    void SetHitConnected(bool connected) { m_hitConnected = connected; }
    bool IsCrouching() const { return m_state == FSTATE_CROUCH || m_state == FSTATE_BLOCK_LOW; }
    bool IsAirborne() const { return m_pos.y < GROUND_Y; }
    bool IsHoldingBack() const { return m_isHoldingBack; }
    bool IsHoldingDownBack() const { return m_isHoldingDownBack; }
    const AttackData& GetCurrentAttack() const { return m_currentAttack; }
    int GetSpecialIndex() const { return m_specialIndex; }
    float GetSuperMeter() const { return m_superMeter; }
    void AddSuperMeter(float amt) { m_superMeter = std::clamp(m_superMeter + amt, 0.0f, 100.0f); }
    int GetComboCount() const { return m_comboCount; }
    void AddComboHit() { m_comboCount++; m_comboResetTimer = 50; }
    void ResetCombo() { m_comboCount = 0; m_comboResetTimer = 0; }
    int GetLastFrameAdvantage() const { return m_lastFrameAdvantage; }
    void SetLastFrameAdvantage(int adv) { m_lastFrameAdvantage = adv; }
    bool IsSuperMoveActive() const { return m_isSuperMove; }
    bool PopSuperFlashTrigger() { bool v = m_triggerSuperFlash; m_triggerSuperFlash = false; return v; }
    bool PopCancelCallout() { bool v = m_justCanceled; m_justCanceled = false; return v; }
    bool PopReversalCallout() { bool v = m_justReversal; m_justReversal = false; return v; }

    Color GetPrimaryColor() const { return m_color1; }
    Color GetSecondaryColor() const { return m_color2; }
    Color GetSkinColor() const { return m_colorSkin; }

public:
    void StartNormalAttack(AttackMoveType moveType, const char* name, int startup, int active, int recovery,
                           int damage, AttackType type, int hitstop, int hitstun,
                           int blockstun, float push, bool heavy, float reachX, float reachY, float bw, float bh);
    void StartNormalAttack(const char* name, int startup, int active, int recovery,
                           int damage, AttackType type, int hitstop, int hitstun,
                           int blockstun, float push, bool heavy, float reachX, float reachY, float bw, float bh) {
        StartNormalAttack(heavy ? MOVE_HEAVY_PUNCH : MOVE_LIGHT_PUNCH, name, startup, active, recovery,
                          damage, type, hitstop, hitstun, blockstun, push, heavy, reachX, reachY, bw, bh);
    }
    void ExecuteSpecial(SpecialMotionType motion, int buttonUsed, const Fighter& opponent);
    void TriggerSignatureSpecial(const Fighter& opponent);
    void TriggerTaunt();
    bool HasSpeechTaunt() const;
    const char* GetTauntQuote() const;
    int GetTauntTimer() const { return m_tauntTimer; }
    const std::string& GetActiveTaunt() const { return m_activeTaunt; }

private:
    void InitCharacterProfile();
    void SpawnProjectile(int type, float vx, float vy, int damage);
    bool TryExecuteBufferedMove(const Fighter& opponent);

    int m_playerIndex = 0;
    CharacterId m_charId = CHAR_COMMANDER;
    std::string m_name = "POTUS";

    Vector2 m_pos{ 0.0f, GROUND_Y };
    Vector2 m_vel{ 0.0f, 0.0f };
    bool m_facingRight = true;

    FighterState m_state = FSTATE_IDLE;
    int m_stateFrame = 0;
    int m_hitstopFrames = 0;
    int m_hitstunFrames = 0;
    int m_blockstunFrames = 0;
    float m_pushbackVel = 0.0f;
    bool m_hitConnected = false;
    int m_armor = 0;
    float m_superMeter = 0.0f;
    int m_comboCount = 0;
    int m_comboResetTimer = 0;
    int m_lastFrameAdvantage = 0;
    int m_invincibleFrames = 0;
    bool m_isSuperMove = false;
    bool m_triggerSuperFlash = false;
    bool m_justCanceled = false;
    bool m_justReversal = false;
    int m_multiHitsRemaining = 0;

    bool m_isHoldingBack = false;
    bool m_isHoldingDownBack = false;
    BufferedInput m_buffer{};

    int m_health = 100;
    int m_roundsWon = 0;
    int m_refillCount = 0;

    AttackData m_currentAttack{};
    int m_specialIndex = 0;
    std::vector<Projectile> m_projectiles;
    int m_quirkTimer = 0;
    std::vector<CosmeticPuff> m_cosmeticPuffs;
    int m_tauntTimer = 0;
    std::string m_activeTaunt = "";

    float m_walkFwdSpeed = 2.30f;
    float m_walkBackSpeed = 1.70f;
    float m_jumpSpeedY = -9.2f;
    float m_jumpSpeedX = 2.3f;
    float m_gravity = 0.48f;

    Color m_color1{ 20, 40, 140, 255 };
    Color m_color2{ 180, 20, 20, 255 };
    Color m_colorSkin{ 240, 190, 150, 255 };
};

// --- Collision.h ---

struct BoxAABB {
    Rectangle rect;
    bool active = false;
};

struct SparkEffect {
    Vector2 pos;
    bool isBlock = false;
    int frame = 0;
    int maxFrames = 6;
    bool alive = false;
};

class Fighter;

class CollisionSystem {
public:
    static bool CheckAABB(const Rectangle& a, const Rectangle& b);
    static Rectangle GetIntersection(const Rectangle& a, const Rectangle& b);

    static void ResolvePushboxes(Fighter& f1, Fighter& f2);

    static void ResolveCombat(Fighter& p1, Fighter& p2, std::vector<SparkEffect>& sparks, float& cameraShake);
};

// --- Stages.h ---
#include <string>

struct VenueDetails {
    std::string name;
    std::string location;
    Color skyTop;
    Color skyBottom;
    Color midArchColor;
    Color floorColor;
    Color hazardAccent;
};

class StageManager {
public:
    StageManager();
    ~StageManager();

    void Init();
    void Shutdown();

    void SetStage(StageId id);
    StageId GetCurrentStage() const { return m_stageId; }
    const VenueDetails& GetCurrentVenue() const { return m_venues[m_stageId]; }

    void DrawFarLayer(float camX, float time);
    void DrawMidLayer(float camX, float time);
    void DrawPlayfield(float camX, float time);
    void DrawForeground(float camX, float time);

private:
    void DrawEastWing(float camX, float time);
    void DrawReflectingPool(float camX, float time);
    void DrawCongressionalFloor(float camX, float time);
    void DrawMMARing(float camX, float time);

    StageId m_stageId = STAGE_DEMOLISHED_EAST_WING;
    VenueDetails m_venues[STAGE_COUNT];
    Texture2D m_stageTextures[STAGE_COUNT]{};
    bool m_texturesLoaded = false;
};

// --- Visuals.h ---
#include <vector>

class VisualsEngine {
public:
    static void Init();
    static void Shutdown();

    static void DrawDropShadow(const Fighter& f);
    static void DrawFighterSprite(const Fighter& f);
    static void DrawProjectiles(const Fighter& f);
    static void DrawSparks(const std::vector<SparkEffect>& sparks);
    static void DrawBattleHUD(const Fighter& p1, const Fighter& p2, int roundTimer, int roundNumber, int fightIntroTimer = 0, bool p1WasKo = false, bool p2WasKo = false);
    static void DrawCinematicIntro(float timer, bool logoDropped, const Shader& flagShader, const Texture2D& flagTex);
    static void DrawStageAndCharSelect(int selectedStage, int p1Char, int p2Char, float timer, const bool defeatedFighters[CHARACTER_COUNT], bool playerLocked = false, bool choosingOpponent = false);
    static void DrawSecretSelectScreen(int step, int p1Char, int p2Char, int stage, int difficulty, float timer);
    static void DrawVersusScreen(const Fighter& p1, const Fighter& p2, float timer);
    static void DrawVictorySplash(int winnerChar, int loserChar, float timer);
    static void DrawBonusStage(float carHp, float maxCarHp, float timer, const Fighter& player, const std::vector<SparkEffect>& sparks);
    static void DrawMatchOver(const Fighter& winner, float timer);
    static void DrawEndingCelebration(float timer);
    static void DrawCthulhuEnding(float timer);
    static void DrawPauseOverlay(float timer);

    static void DrawPixelText(const char* text, int x, int y, int size, Color col);
    static Texture2D CreateProceduralFlagTexture();

private:
    static bool s_loaded;
    static Texture2D s_titleSplash;
    static Texture2D s_portraits[CHARACTER_COUNT];
    static Texture2D s_portraitsLose[CHARACTER_COUNT];

    static Texture2D s_sprites[CHARACTER_COUNT][6];
    static Texture2D s_teslaSprites[4];
    static Texture2D s_bonusStageBg;
    static Texture2D s_mapEastCoast;
    static Texture2D s_endingSavior;
    static Texture2D s_sprBillaryHillary;
    static Texture2D s_spritesHillary[6];

    static Texture2D s_sprHighBenchRoberts;
    static Texture2D s_sprHighBenchRobertsSpec;
    static Texture2D s_sprHighBenchSotomayor;
    static Texture2D s_sprHighBenchSotomayorSpec;
    static Texture2D s_portraitRoberts;
    static Texture2D s_portraitSotomayor;

    static Texture2D s_sprSleepyJoeSS;

    static Texture2D s_endingCthulhuThrow;
    static Texture2D s_endingLaserShark;
    static Texture2D s_endingCthulhuNap;
};

// --- AIController.h ---

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


// ============================================================================
// CHAMPIONSHIP EDITION (CE) UPGRADE SUITE:
// - 12-Part Articulated Hierarchical Character Rig & 5-Node Verlet Tie Physics
// - 3-Tone CPS-1 Shading, Motion-Blur Smear Arcs & Bruised HUD Portraits
// - Super Meter Bar, 30-Frame Super-Flash Cut-In, Graphic Stamps & Callouts
// - F1 Live Hitbox/Hurtbox/Pushbox & Frame-Advantage Debug Overlay
// ============================================================================
#include <cstdlib>
#include <cstdio>

namespace ChampionshipEdition {

static bool s_debugHitboxes = false;
static bool s_forceProceduralRig = false;
static int  s_superFlashTimer = 0;
static int  s_superFlashPlayer = 0;
static std::string s_superFlashMove = "OMNIBUS BILL SUPER!";

struct CEStamp {
    Vector2 pos;
    std::string text;
    Color color;
    int frame = 0;
    int maxFrames = 32;
    bool alive = false;
};

struct CEParticle {
    Vector2 pos;
    Vector2 vel;
    Color color;
    float size = 2.0f;
    float rot = 0.0f;
    float rotVel = 0.0f;
    int frame = 0;
    int maxFrames = 35;
    bool isPaper = false;
    bool alive = false;
};

struct CECalloutBanner {
    std::string text;
    std::string subtext;
    Color color;
    int playerIndex = 0;
    int timer = 0;
    int maxTimer = 65;
};

struct CEVerletTie {
    Vector2 nodes[5];
    Vector2 prev[5];
    bool inited = false;

    void Update(Vector2 anchor, Vector2 charVel, bool facingRight) {
        if (!inited) {
            for (int i = 0; i < 5; i++) {
                nodes[i] = Vector2{ anchor.x, anchor.y + i * 3.5f };
                prev[i] = nodes[i];
            }
            inited = true;
        }
        nodes[0] = anchor;
        prev[0] = anchor;
        for (int i = 1; i < 5; i++) {
            Vector2 cur = nodes[i];
            Vector2 vx = Vector2{ (cur.x - prev[i].x) * 0.84f, (cur.y - prev[i].y) * 0.84f };
            prev[i] = cur;
            float flutter = sinf((float)GetTime() * 14.0f + i * 1.3f) * 0.22f;
            nodes[i].x += vx.x - charVel.x * 0.30f + (facingRight ? -0.15f : 0.15f) + flutter;
            nodes[i].y += vx.y + 0.42f;
        }
        for (int iter = 0; iter < 3; iter++) {
            nodes[0] = anchor;
            for (int i = 0; i < 4; i++) {
                float dx = nodes[i + 1].x - nodes[i].x;
                float dy = nodes[i + 1].y - nodes[i].y;
                float dist = sqrtf(dx * dx + dy * dy);
                float rest = 3.6f;
                if (dist > 0.001f) {
                    float diff = (dist - rest) / dist;
                    if (i == 0) {
                        nodes[i + 1].x -= dx * diff;
                        nodes[i + 1].y -= dy * diff;
                    } else {
                        nodes[i].x += dx * diff * 0.5f;
                        nodes[i].y += dy * diff * 0.5f;
                        nodes[i + 1].x -= dx * diff * 0.5f;
                        nodes[i + 1].y -= dy * diff * 0.5f;
                    }
                }
            }
        }
    }

    void Draw(Color tieCol, Color trimCol) const {
        if (!inited) return;
        for (int i = 0; i < 4; i++) {
            float w = (i == 3) ? 4.2f : 3.0f;
            DrawLineEx(nodes[i], nodes[i + 1], w + 1.5f, Color{ 12, 12, 18, 230 });
            DrawLineEx(nodes[i], nodes[i + 1], w, tieCol);
        }
        DrawCircleV(nodes[4], 2.2f, trimCol);
    }
};

static CEVerletTie s_ties[2];
static std::vector<CEStamp> s_stamps;
static std::vector<CEParticle> s_particles;
static std::vector<CECalloutBanner> s_banners;
static bool s_firstStrikeAwarded = false;
static int s_prevP1Hits = 0;
static int s_prevP2Hits = 0;

inline void ResetRoundFX() {
    s_stamps.clear();
    s_particles.clear();
    s_banners.clear();
    s_superFlashTimer = 0;
    s_firstStrikeAwarded = false;
    s_prevP1Hits = 0;
    s_prevP2Hits = 0;
}

inline void PushBanner(int playerIndex, const char* text, const char* sub, Color col) {
    for (auto& b : s_banners) {
        if (b.playerIndex == playerIndex) {
            b.text = text;
            b.subtext = sub;
            b.color = col;
            b.timer = b.maxTimer;
            return;
        }
    }
    CECalloutBanner b{};
    b.playerIndex = playerIndex;
    b.text = text;
    b.subtext = sub;
    b.color = col;
    b.timer = b.maxTimer;
    s_banners.push_back(b);
}

inline void SpawnImpactJuice(Vector2 pos, bool heavy, bool isSpecial, const char* stampText) {
    int sweatCount = heavy ? 8 : 4;
    for (int i = 0; i < sweatCount; i++) {
        CEParticle p{};
        p.pos = pos;
        float ang = -PI_F * 0.85f + ((float)(rand() % 100) / 100.0f) * PI_F * 0.7f;
        float spd = 1.6f + ((float)(rand() % 100) / 100.0f) * 3.0f;
        p.vel = Vector2{ cosf(ang) * spd, sinf(ang) * spd - 1.2f };
        p.color = Color{ 130, 225, 255, 240 };
        p.size = 2.0f;
        p.maxFrames = 24 + (rand() % 10);
        p.isPaper = false;
        p.alive = true;
        s_particles.push_back(p);
    }
    if (heavy || isSpecial) {
        int docs = isSpecial ? 7 : 4;
        for (int i = 0; i < docs; i++) {
            CEParticle p{};
            p.pos = pos;
            p.vel = Vector2{ ((float)(rand() % 100) / 50.0f - 1.0f) * 3.2f, -2.0f - ((float)(rand() % 100) / 100.0f) * 2.5f };
            p.color = (i % 2 == 0) ? Color{ 250, 245, 225, 255 } : Color{ 255, 215, 60, 255 };
            p.size = 4.5f;
            p.rot = (float)(rand() % 360);
            p.rotVel = ((float)(rand() % 100) / 50.0f - 1.0f) * 9.0f;
            p.maxFrames = 42 + (rand() % 18);
            p.isPaper = true;
            p.alive = true;
            s_particles.push_back(p);
        }
    }
    if (stampText != nullptr && stampText[0] != '\0') {
        CEStamp st{};
        st.pos = Vector2{ pos.x, pos.y - 18.0f };
        st.text = stampText;
        st.color = isSpecial ? Color{ 255, 50, 50, 255 } : Color{ 255, 215, 40, 255 };
        st.frame = 0;
        st.maxFrames = 32;
        st.alive = true;
        s_stamps.push_back(st);
    }
}

// 3-Tone CPS-1 Beveled Limb Segment Helper for 12-Part Hierarchical Rig
inline void DrawLimb3Tone(Vector2 a, Vector2 b, float rA, float rB, Color baseCol) {
    Color darkCol{ (unsigned char)(baseCol.r * 0.58f), (unsigned char)(baseCol.g * 0.58f), (unsigned char)(baseCol.b * 0.58f), 255 };
    Color hiCol{ (unsigned char)std::min(255, (int)(baseCol.r * 1.28f + 18)), (unsigned char)std::min(255, (int)(baseCol.g * 1.28f + 18)), (unsigned char)std::min(255, (int)(baseCol.b * 1.28f + 18)), 255 };
    Color outCol{ 12, 12, 18, 255 };
    float rAvg = (rA + rB) * 0.5f;
    DrawLineEx(a, b, (rAvg + 1.5f) * 2.0f, outCol);
    DrawCircleV(a, rA + 1.5f, outCol);
    DrawCircleV(b, rB + 1.5f, outCol);
    DrawLineEx(a, b, rAvg * 2.0f, darkCol);
    DrawCircleV(a, rA, darkCol);
    DrawCircleV(b, rB, darkCol);
    DrawLineEx(a, b, rAvg * 1.45f, baseCol);
    DrawCircleV(a, rA * 0.82f, baseCol);
    DrawCircleV(b, rB * 0.82f, baseCol);
    Vector2 hA{ a.x, a.y - rA * 0.28f };
    Vector2 hB{ b.x, b.y - rB * 0.28f };
    DrawLineEx(hA, hB, std::max(1.0f, rAvg * 0.55f), hiCol);
}

// Articulated 12-Part Hierarchical Character Rig (Head, Jaw/Mouth, Hair/Accessory,
// Torso, Pelvis, UpperArm_L/R, Forearm_L/R, Hand_L/R, Thigh_L/R, Calf_L/R, Foot_L/R)
inline void DrawArticulated12PartRig(const Fighter& f) {
    Vector2 pos = f.GetPosition();
    bool fr = f.IsFacingRight();
    float dir = fr ? 1.0f : -1.0f;
    FighterState st = f.GetState();
    int frame = f.GetStateFrame();

    Color suit = f.GetPrimaryColor();
    Color trim = f.GetSecondaryColor();
    Color skin = f.GetSkinColor();
    Color shoes{ 24, 24, 30, 255 };

    float breath = (st == FSTATE_IDLE) ? sinf(frame * 0.14f) * 1.6f : 0.0f;
    float crouchDrop = f.IsCrouching() ? 14.0f : 0.0f;
    float walkPhase = (st == FSTATE_WALK_FWD || st == FSTATE_WALK_BACK) ? sinf(frame * 0.32f) : 0.0f;

    Vector2 pelvis{ pos.x, pos.y - 24.0f + crouchDrop + breath * 0.5f };
    Vector2 torso{ pelvis.x + dir * (st == FSTATE_ATTACK ? 4.0f : 1.0f), pelvis.y - 18.0f + crouchDrop * 0.3f };
    Vector2 head{ torso.x + dir * 2.0f, torso.y - 12.0f };

    // Legs (Thigh_L/R, Calf_L/R, Foot_L/R)
    Vector2 hipL{ pelvis.x - dir * 4.0f, pelvis.y };
    Vector2 hipR{ pelvis.x + dir * 4.0f, pelvis.y };
    Vector2 kneeL{ hipL.x - dir * 4.0f - walkPhase * 6.0f, pos.y - 12.0f };
    Vector2 kneeR{ hipR.x + dir * 5.0f + walkPhase * 6.0f, pos.y - 12.0f };
    Vector2 footL{ kneeL.x - dir * 2.0f, pos.y - 2.0f };
    Vector2 footR{ kneeR.x + dir * 3.0f, pos.y - 2.0f };

    if (st == FSTATE_ATTACK && (f.GetCurrentAttack().moveType == MOVE_LIGHT_KICK || f.GetCurrentAttack().moveType == MOVE_HEAVY_KICK)) {
        int su = f.GetCurrentAttack().startupFrames;
        int ac = f.GetCurrentAttack().activeFrames;
        if (frame >= su && frame < su + ac) {
            kneeR = Vector2{ hipR.x + dir * 14.0f, pelvis.y - 4.0f };
            footR = Vector2{ pos.x + dir * f.GetCurrentAttack().reachX, pos.y + f.GetCurrentAttack().reachY };
        }
    }

    // Arms (UpperArm_L/R, Forearm_L/R, Hand_L/R)
    Vector2 shL{ torso.x - dir * 6.0f, torso.y + 2.0f };
    Vector2 shR{ torso.x + dir * 6.0f, torso.y + 2.0f };
    Vector2 elbL{ shL.x - dir * 5.0f, shL.y + 9.0f };
    Vector2 elbR{ shR.x + dir * 7.0f, shR.y + 8.0f };
    Vector2 handL{ elbL.x + dir * 6.0f, elbL.y - 4.0f };
    Vector2 handR{ elbR.x + dir * 8.0f, elbR.y - 5.0f };

    if ((st == FSTATE_ATTACK || st == FSTATE_SPECIAL) &&
        (f.GetCurrentAttack().moveType == MOVE_LIGHT_PUNCH || f.GetCurrentAttack().moveType == MOVE_HEAVY_PUNCH || st == FSTATE_SPECIAL)) {
        int su = f.GetCurrentAttack().startupFrames;
        int ac = f.GetCurrentAttack().activeFrames;
        if (frame >= su && frame < su + ac) {
            elbR = Vector2{ shR.x + dir * (f.GetCurrentAttack().reachX * 0.5f), shR.y - 2.0f };
            handR = Vector2{ pos.x + dir * f.GetCurrentAttack().reachX, pos.y + f.GetCurrentAttack().reachY };
        }
    }

    // Back limbs
    DrawLimb3Tone(hipL, kneeL, 4.5f, 3.8f, suit);
    DrawLimb3Tone(kneeL, footL, 3.6f, 3.0f, suit);
    DrawLimb3Tone(footL, Vector2{ footL.x + dir * 5.0f, footL.y }, 3.0f, 2.5f, shoes);
    DrawLimb3Tone(shL, elbL, 3.8f, 3.2f, suit);
    DrawLimb3Tone(elbL, handL, 3.0f, 2.6f, suit);
    DrawCircleV(handL, 3.2f, skin);

    // Pelvis & Torso
    DrawLimb3Tone(pelvis, torso, 7.5f, 9.0f, suit);

    // Front leg
    DrawLimb3Tone(hipR, kneeR, 4.8f, 4.0f, suit);
    DrawLimb3Tone(kneeR, footR, 3.8f, 3.2f, suit);
    DrawLimb3Tone(footR, Vector2{ footR.x + dir * 5.5f, footR.y }, 3.2f, 2.6f, shoes);

    // Head, Jaw/Mouth & Facial Expression
    DrawCircleV(head, 8.5f, Color{ 12, 12, 18, 255 });
    DrawCircleV(head, 7.2f, skin);
    // Hair / Hat accessory
    DrawRectangle((int)(head.x - 7), (int)(head.y - 8), 14, 4, trim);
    // Eye & Jaw
    DrawCircle((int)(head.x + dir * 3.5f), (int)(head.y - 1.5f), (st == FSTATE_HITSTUN) ? 2.5f : 1.5f, WHITE);
    DrawCircle((int)(head.x + dir * 4.0f), (int)(head.y - 1.5f), 1.0f, BLACK);
    if (st == FSTATE_ATTACK || st == FSTATE_SPECIAL || st == FSTATE_HITSTUN) {
        DrawRectangle((int)(head.x + dir * 2.0f), (int)(head.y + 2.5f), 4, 3, Color{ 140, 25, 35, 255 });
    } else {
        DrawLine((int)(head.x + dir * 1.5f), (int)(head.y + 3.0f), (int)(head.x + dir * 5.0f), (int)(head.y + 2.5f), Color{ 40, 25, 20, 255 });
    }

    // Front arm
    DrawLimb3Tone(shR, elbR, 4.0f, 3.4f, suit);
    DrawLimb3Tone(elbR, handR, 3.2f, 2.8f, suit);
    DrawCircleV(handR, 3.6f, skin);
}

inline void UpdateAndTrackFighters(Fighter& p1, Fighter& p2) {
    if (IsKeyPressed(KEY_F1)) {
        s_debugHitboxes = !s_debugHitboxes;
    }
    if (IsKeyPressed(KEY_F3)) {
        s_forceProceduralRig = !s_forceProceduralRig;
    }

    // 1.25x BGM Tempo when either fighter drops below 30% HP
    bool critical = (p1.GetHealth() > 0 && p1.GetHealth() < 30) || (p2.GetHealth() > 0 && p2.GetHealth() < 30);
    AudioEngine::SetCriticalTempo(critical);

    // Update Verlet Neckties / Ribbons
    s_ties[0].Update(Vector2{ p1.GetPosition().x + (p1.IsFacingRight() ? 2.0f : -2.0f), p1.GetPosition().y - (p1.IsCrouching() ? 28.0f : 42.0f) }, p1.GetVelocity(), p1.IsFacingRight());
    s_ties[1].Update(Vector2{ p2.GetPosition().x + (p2.IsFacingRight() ? 2.0f : -2.0f), p2.GetPosition().y - (p2.IsCrouching() ? 28.0f : 42.0f) }, p2.GetVelocity(), p2.IsFacingRight());

    // Check Super Flash Trigger
    if (p1.PopSuperFlashTrigger()) {
        s_superFlashTimer = 30;
        s_superFlashPlayer = 0;
        s_superFlashMove = "OMNIBUS BILL SUPER!";
        PushBanner(0, "OMNIBUS SUPER!", p1.GetName().c_str(), Color{ 255, 215, 40, 255 });
    }
    if (p2.PopSuperFlashTrigger()) {
        s_superFlashTimer = 30;
        s_superFlashPlayer = 1;
        s_superFlashMove = "OMNIBUS BILL SUPER!";
        PushBanner(1, "OMNIBUS SUPER!", p2.GetName().c_str(), Color{ 255, 215, 40, 255 });
    }

    // Check 2-in-1 Cancel & Reversal Callouts
    if (p1.PopCancelCallout()) PushBanner(0, "2-IN-1 CANCEL!", "SPECIAL BUFFER", Color{ 90, 230, 255, 255 });
    if (p2.PopCancelCallout()) PushBanner(1, "2-IN-1 CANCEL!", "SPECIAL BUFFER", Color{ 90, 230, 255, 255 });
    if (p1.PopReversalCallout()) {
        PushBanner(0, "REVERSAL!", "TOTAL VETO", Color{ 255, 110, 50, 255 });
        SpawnImpactJuice(p1.GetPosition(), true, true, "VETO!");
    }
    if (p2.PopReversalCallout()) {
        PushBanner(1, "REVERSAL!", "TOTAL VETO", Color{ 255, 110, 50, 255 });
        SpawnImpactJuice(p2.GetPosition(), true, true, "VETO!");
    }

    // Check Combo & First Strike Callouts
    int c1 = p1.GetComboCount();
    int c2 = p2.GetComboCount();
    if (c1 > s_prevP1Hits && c1 >= 1) {
        if (!s_firstStrikeAwarded) {
            s_firstStrikeAwarded = true;
            PushBanner(0, "FIRST STRIKE!", p1.GetName().c_str(), Color{ 255, 230, 60, 255 });
        }
        if (c1 >= 2) {
            char buf[48];
            snprintf(buf, sizeof(buf), "%d HIT FILIBUSTER!", c1);
            PushBanner(0, buf, p1.GetCurrentAttack().name.c_str(), Color{ 255, 190, 40, 255 });
        }
        bool isSpec = (p1.GetState() == FSTATE_SPECIAL);
        SpawnImpactJuice(Vector2{ p2.GetPosition().x, p2.GetPosition().y - 36.0f }, p1.GetCurrentAttack().isHeavy, isSpec, isSpec ? "VETO!" : nullptr);
    }
    if (c2 > s_prevP2Hits && c2 >= 1) {
        if (!s_firstStrikeAwarded) {
            s_firstStrikeAwarded = true;
            PushBanner(1, "FIRST STRIKE!", p2.GetName().c_str(), Color{ 255, 230, 60, 255 });
        }
        if (c2 >= 2) {
            char buf[48];
            snprintf(buf, sizeof(buf), "%d HIT FILIBUSTER!", c2);
            PushBanner(1, buf, p2.GetCurrentAttack().name.c_str(), Color{ 255, 120, 60, 255 });
        }
        bool isSpec = (p2.GetState() == FSTATE_SPECIAL);
        SpawnImpactJuice(Vector2{ p1.GetPosition().x, p1.GetPosition().y - 36.0f }, p2.GetCurrentAttack().isHeavy, isSpec, isSpec ? "REDACTED!" : nullptr);
    }
    s_prevP1Hits = c1;
    s_prevP2Hits = c2;

    // Advance Particles, Stamps, and Banners
    for (auto& p : s_particles) {
        if (!p.alive) continue;
        p.pos.x += p.vel.x;
        p.pos.y += p.vel.y;
        p.vel.y += p.isPaper ? 0.08f : 0.25f;
        p.rot += p.rotVel;
        p.frame++;
        if (p.frame >= p.maxFrames || p.pos.y >= GROUND_Y + 4.0f) p.alive = false;
    }
    s_particles.erase(std::remove_if(s_particles.begin(), s_particles.end(), [](const CEParticle& p) { return !p.alive; }), s_particles.end());

    for (auto& st : s_stamps) {
        if (!st.alive) continue;
        st.pos.y -= 0.35f;
        st.frame++;
        if (st.frame >= st.maxFrames) st.alive = false;
    }
    s_stamps.erase(std::remove_if(s_stamps.begin(), s_stamps.end(), [](const CEStamp& s) { return !s.alive; }), s_stamps.end());

    for (auto& b : s_banners) {
        if (b.timer > 0) b.timer--;
    }
    s_banners.erase(std::remove_if(s_banners.begin(), s_banners.end(), [](const CECalloutBanner& b) { return b.timer <= 0; }), s_banners.end());
}

inline void DrawFighterWorldOverlays(const Fighter& f) {
    int idx = (f.GetPlayerIndex() == 0) ? 0 : 1;
    // Draw Dynamic Verlet Necktie / Stole Trail
    s_ties[idx].Draw(f.GetSecondaryColor(), Color{ 255, 220, 60, 255 });

    // Motion-Blur Smear Arcs during Active Frames
    if (f.GetState() == FSTATE_ATTACK || f.GetState() == FSTATE_SPECIAL) {
        int su = f.GetCurrentAttack().startupFrames;
        int ac = f.GetCurrentAttack().activeFrames;
        int fr = f.GetStateFrame();
        if (fr >= su && fr < su + ac) {
            float dir = f.IsFacingRight() ? 1.0f : -1.0f;
            Vector2 center{ f.GetPosition().x + dir * (f.GetCurrentAttack().reachX * 0.55f), f.GetPosition().y + f.GetCurrentAttack().reachY };
            Color arcCol = f.IsSuperMoveActive() ? Color{ 255, 80, 220, 210 } : (f.GetCurrentAttack().isHeavy ? Color{ 255, 195, 50, 195 } : Color{ 170, 235, 255, 175 });
            for (int i = 0; i < 4; i++) {
                float off = (float)i * 2.5f * dir;
                DrawCircleLines((int)(center.x + off), (int)center.y, 10.0f + i * 2.5f, arcCol);
            }
        }
    }
}

inline void DrawWorldParticlesAndDebug(const Fighter& p1, const Fighter& p2) {
    // Sweat & Paper Confetti Particles
    for (const auto& p : s_particles) {
        if (!p.alive) continue;
        if (p.isPaper) {
            DrawRectanglePro(Rectangle{ p.pos.x, p.pos.y, p.size, p.size * 0.65f }, Vector2{ p.size * 0.5f, p.size * 0.32f }, p.rot, p.color);
        } else {
            DrawCircleV(p.pos, p.size, p.color);
        }
    }

    // Graphic Stamps ("VETO!", "REDACTED!")
    for (const auto& st : s_stamps) {
        if (!st.alive) continue;
        int tw = MeasureText(st.text.c_str(), 10);
        int bx = (int)st.pos.x - tw / 2 - 4;
        int by = (int)st.pos.y - 6;
        DrawRectangle(bx, by, tw + 8, 13, Color{ 15, 10, 15, 220 });
        DrawRectangleLines(bx, by, tw + 8, 13, st.color);
        VisualsEngine::DrawPixelText(st.text.c_str(), bx + 4, by + 2, 10, st.color);
    }

    // F1 Live Hitbox / Hurtbox / Pushbox Debug Overlay
    if (s_debugHitboxes) {
        const Fighter* fighters[2] = { &p1, &p2 };
        for (int i = 0; i < 2; i++) {
            const Fighter& f = *fighters[i];
            Rectangle pb = f.GetPushbox();
            DrawRectangleRec(pb, Color{ 255, 235, 40, 50 });
            DrawRectangleLinesEx(pb, 1.0f, Color{ 255, 235, 40, 230 });

            auto hbs = f.GetHurtboxes();
            for (const auto& hb : hbs) {
                DrawRectangleRec(hb, Color{ 40, 240, 80, 55 });
                DrawRectangleLinesEx(hb, 1.0f, Color{ 40, 240, 80, 230 });
            }

            Rectangle atkBox{};
            AttackData atkData{};
            if (f.GetActiveHitbox(atkBox, atkData)) {
                DrawRectangleRec(atkBox, Color{ 255, 40, 40, 95 });
                DrawRectangleLinesEx(atkBox, 1.0f, Color{ 255, 40, 40, 255 });
            }
        }
    }
}

inline void DrawScreenHUDAndSuperFlash(const Fighter& p1, const Fighter& p2, const InputManager& in1) {
    // 1. Bottom Super Meter Bars ("OMNIBUS BILL" 0-100%)
    constexpr int METER_W = 124;
    constexpr int METER_H = 7;
    int meterY = SCREEN_HEIGHT - 13;

    // P1 Meter
    float m1 = p1.GetSuperMeter();
    int w1 = (int)((m1 / 100.0f) * METER_W);
    DrawRectangle(18, meterY, METER_W + 4, METER_H + 4, Color{ 12, 14, 22, 235 });
    Color c1 = (m1 >= 100.0f) ? (((int)(GetTime() * 16.0f) % 2 == 0) ? Color{ 255, 235, 70, 255 } : Color{ 255, 110, 30, 255 }) : Color{ 40, 175, 255, 255 };
    DrawRectangle(20, meterY + 2, w1, METER_H, c1);
    DrawRectangleLines(18, meterY, METER_W + 4, METER_H + 4, Color{ 190, 165, 60, 255 });
    VisualsEngine::DrawPixelText(m1 >= 100.0f ? "OMNIBUS READY! (236236+P)" : "OMNIBUS METER", 20, meterY - 8, 6, c1);

    // P2 Meter
    float m2 = p2.GetSuperMeter();
    int w2 = (int)((m2 / 100.0f) * METER_W);
    int p2X = SCREEN_WIDTH - 22 - METER_W;
    DrawRectangle(p2X, meterY, METER_W + 4, METER_H + 4, Color{ 12, 14, 22, 235 });
    Color c2 = (m2 >= 100.0f) ? (((int)(GetTime() * 16.0f) % 2 == 0) ? Color{ 255, 235, 70, 255 } : Color{ 255, 110, 30, 255 }) : Color{ 40, 175, 255, 255 };
    DrawRectangle(p2X + 2 + (METER_W - w2), meterY + 2, w2, METER_H, c2);
    DrawRectangleLines(p2X, meterY, METER_W + 4, METER_H + 4, Color{ 190, 165, 60, 255 });
    VisualsEngine::DrawPixelText(m2 >= 100.0f ? "OMNIBUS MAX!" : "OMNIBUS METER", SCREEN_WIDTH - 92, meterY - 8, 6, c2);

    // 2. Bruised / Sweating Critical Health Indicator (<30% HP)
    if (p1.GetHealth() > 0 && p1.GetHealth() < 30) {
        int dropY = 28 + ((int)(GetTime() * 24.0f) % 8);
        DrawCircle(15, dropY, 2, Color{ 110, 215, 255, 240 });
        VisualsEngine::DrawPixelText("CRITICAL!", 22, 29, 6, Color{ 255, 70, 70, 255 });
    }
    if (p2.GetHealth() > 0 && p2.GetHealth() < 30) {
        int dropY = 28 + ((int)(GetTime() * 24.0f) % 8);
        DrawCircle(SCREEN_WIDTH - 15, dropY, 2, Color{ 110, 215, 255, 240 });
        VisualsEngine::DrawPixelText("CRITICAL!", SCREEN_WIDTH - 64, 29, 6, Color{ 255, 70, 70, 255 });
    }

    // 3. Sliding Combo & Callout Banners
    for (const auto& b : s_banners) {
        float slide = std::min(1.0f, (float)(b.maxTimer - b.timer) / 6.0f);
        int bw = 122;
        int bh = 22;
        int bx = (b.playerIndex == 0) ? (int)(-bw + slide * (bw + 8)) : (int)(SCREEN_WIDTH - slide * (bw + 8));
        int by = 46 + b.playerIndex * 26;
        DrawRectangle(bx, by, bw, bh, Color{ 14, 16, 28, 230 });
        DrawRectangleLines(bx, by, bw, bh, b.color);
        VisualsEngine::DrawPixelText(b.text.c_str(), bx + 5, by + 3, 8, b.color);
        VisualsEngine::DrawPixelText(b.subtext.c_str(), bx + 5, by + 12, 6, WHITE);
    }

    // 4. 30-Frame Dramatic Super-Flash Cut-In Banner
    if (s_superFlashTimer > 0) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 5, 8, 22, 195 });
        int bandY = SCREEN_HEIGHT / 2 - 24;
        DrawRectangle(0, bandY, SCREEN_WIDTH, 48, Color{ 25, 45, 120, 245 });
        DrawLine(0, bandY, SCREEN_WIDTH, bandY, Color{ 255, 225, 60, 255 });
        DrawLine(0, bandY + 48, SCREEN_WIDTH, bandY + 48, Color{ 255, 225, 60, 255 });
        for (int l = 0; l < 10; l++) {
            int lx = ((s_superFlashTimer * 28 + l * 47) % SCREEN_WIDTH);
            DrawLine(lx, bandY + 4 + l * 4, lx + 65, bandY + 4 + l * 4, Color{ 120, 210, 255, 190 });
        }
        const Fighter& sf = (s_superFlashPlayer == 0) ? p1 : p2;
        VisualsEngine::DrawPixelText(sf.GetName().c_str(), 28, bandY + 8, 12, Color{ 255, 230, 70, 255 });
        VisualsEngine::DrawPixelText(s_superFlashMove.c_str(), 28, bandY + 24, 14, WHITE);
    }

    // 5. F1 Live Frame-Advantage & Input Buffer Telemetry Panel
    if (s_debugHitboxes) {
        DrawRectangle(6, SCREEN_HEIGHT - 54, SCREEN_WIDTH - 12, 36, Color{ 8, 10, 16, 225 });
        DrawRectangleLines(6, SCREEN_HEIGHT - 54, SCREEN_WIDTH - 12, 36, Color{ 80, 240, 120, 220 });
        char p1Info[128];
        snprintf(p1Info, sizeof(p1Info), "P1 F:%d ADV:%+d ST:%d %s",
                 p1.GetStateFrame(), p1.GetLastFrameAdvantage(), (int)p1.GetState(), p1.GetCurrentAttack().name.c_str());
        char p2Info[128];
        snprintf(p2Info, sizeof(p2Info), "P2 F:%d ADV:%+d ST:%d %s",
                 p2.GetStateFrame(), p2.GetLastFrameAdvantage(), (int)p2.GetState(), p2.GetCurrentAttack().name.c_str());
        VisualsEngine::DrawPixelText(p1Info, 10, SCREEN_HEIGHT - 51, 6, Color{ 110, 255, 140, 255 });
        VisualsEngine::DrawPixelText(p2Info, 10, SCREEN_HEIGHT - 41, 6, Color{ 255, 190, 100, 255 });

        std::string hist = "BUF: ";
        for (int a = 11; a >= 0; a--) {
            const auto& hf = in1.GetHistoryFrame(a);
            char c = '0' + (int)hf.relDir;
            hist.push_back(c);
            if (hf.buttonsPressed & BTN_LP) hist += "P";
            else if (hf.buttonsPressed & BTN_HP) hist += "H";
            else if (hf.buttonsPressed & BTN_LK) hist += "K";
            else if (hf.buttonsPressed & BTN_HK) hist += "S";
            hist.push_back(' ');
        }
        VisualsEngine::DrawPixelText(hist.c_str(), 10, SCREEN_HEIGHT - 30, 6, Color{ 140, 220, 255, 255 });
    }
}

} // namespace ChampionshipEdition

// --- AssetManager.cpp ---
#include <unordered_map>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <cstdint>

#ifdef _WIN32

typedef void* HRSRC;
typedef void* HGLOBAL;
typedef void* HMODULE;
typedef void* LPVOID;
typedef unsigned long DWORD;

extern "C" {
__declspec(dllimport) HRSRC __stdcall FindResourceA(HMODULE hModule, const char* lpName, const char* lpType);
__declspec(dllimport) HGLOBAL __stdcall LoadResource(HMODULE hModule, HRSRC hResInfo);
__declspec(dllimport) LPVOID __stdcall LockResource(HGLOBAL hResData);
__declspec(dllimport) DWORD __stdcall SizeofResource(HMODULE hModule, HRSRC hResInfo);
}

#define WIN_MAKEINTRESOURCEA(i) ((const char*)((uintptr_t)((uint16_t)(i))))
#define WIN_RT_RCDATA WIN_MAKEINTRESOURCEA(10)
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#endif

namespace AssetManager {

struct Entry {
    uint32_t offset;
    uint32_t size;
};

static bool s_initialized = false;
static bool s_hasEmbedded = false;
static const uint8_t* s_pakData = nullptr;
static uint32_t s_pakSize = 0;
static const uint8_t* s_payloadData = nullptr;
static uint8_t s_xorKey = 0x5A;
static std::unordered_map<std::string, Entry> s_entries;

static std::string NormalizePath(const std::string& p) {
    std::string s = p;
    std::replace(s.begin(), s.end(), '\\', '/');
    while (s.rfind("./", 0) == 0) s = s.substr(2);
    while (s.rfind("../", 0) == 0) s = s.substr(3);
    for (char& c : s) c = (char)tolower((unsigned char)c);
    return s;
}

void Init() {
    if (s_initialized) return;
    s_initialized = true;

#ifdef _WIN32

    HRSRC hRes = FindResourceA(nullptr, WIN_MAKEINTRESOURCEA(100), WIN_RT_RCDATA);
    if (hRes) {
        HGLOBAL hGlobal = LoadResource(nullptr, hRes);
        if (hGlobal) {
            s_pakData = (const uint8_t*)LockResource(hGlobal);
            s_pakSize = (uint32_t)SizeofResource(nullptr, hRes);
        }
    }
#endif

    if (!s_pakData || s_pakSize < 16) {
        const char* candidatePaths[] = {
            "assets.pak",
            "ExecutiveOrder/assets.pak",
            "../assets.pak",
            "/roms/ports/ExecutiveOrder/assets.pak",
            "/roms2/ports/ExecutiveOrder/assets.pak"
        };
        std::string foundPath;
        std::string appDirPak = std::string(GetApplicationDirectory()) + "assets.pak";
        if (::FileExists(appDirPak.c_str())) {
            foundPath = appDirPak;
        } else {
            for (const char* cp : candidatePaths) {
                if (::FileExists(cp)) {
                    foundPath = cp;
                    break;
                }
            }
        }

        if (!foundPath.empty()) {
#ifndef _WIN32
            int fd = open(foundPath.c_str(), O_RDONLY);
            if (fd >= 0) {
                struct stat st;
                if (fstat(fd, &st) == 0 && st.st_size >= 16) {
                    void* mapped = mmap(nullptr, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
                    if (mapped != MAP_FAILED) {
                        s_pakData = (const uint8_t*)mapped;
                        s_pakSize = (uint32_t)st.st_size;
                    }
                }
                close(fd);
            }
#else
            FILE* f = fopen(foundPath.c_str(), "rb");
            if (f) {
                fseek(f, 0, SEEK_END);
                long sz = ftell(f);
                fseek(f, 0, SEEK_SET);
                if (sz >= 16) {
                    static std::vector<uint8_t> fileBuf;
                    fileBuf.resize(sz);
                    if (fread(fileBuf.data(), 1, sz, f) == (size_t)sz) {
                        s_pakData = fileBuf.data();
                        s_pakSize = (uint32_t)sz;
                    }
                }
                fclose(f);
            }
#endif
        }
    }

    if (!s_pakData || s_pakSize < 16) return;

    if (memcmp(s_pakData, "EO2PAK1\0", 8) != 0) return;

    uint32_t fileCount = *(const uint32_t*)(s_pakData + 8);
    s_xorKey = s_pakData[12];

    const uint8_t* ptr = s_pakData + 16;
    for (uint32_t i = 0; i < fileCount; i++) {
        if (ptr + 2 > s_pakData + s_pakSize) break;
        uint16_t nameLen = *(const uint16_t*)ptr;
        ptr += 2;
        if (ptr + nameLen + 8 > s_pakData + s_pakSize) break;
        std::string name((const char*)ptr, nameLen);
        ptr += nameLen;
        uint32_t offset = *(const uint32_t*)ptr;
        ptr += 4;
        uint32_t size = *(const uint32_t*)ptr;
        ptr += 4;

        Entry e{ offset, size };
        s_entries[NormalizePath(name)] = e;

        if (name.rfind("assets/", 0) == 0) {
            s_entries[NormalizePath(name.substr(7))] = e;
        }
    }

    s_payloadData = ptr;
    s_hasEmbedded = !s_entries.empty();
}

bool HasEmbeddedAssets() {
    Init();
    return s_hasEmbedded;
}

bool FileExists(const char* path) {
    if (!path || !path[0]) return false;
    Init();
    if (s_hasEmbedded) {
        std::string norm = NormalizePath(path);
        if (s_entries.find(norm) != s_entries.end()) return true;
    }
    return ::FileExists(path);
}

static bool GetDecryptedData(const char* path, std::vector<uint8_t>& outBuf) {
    Init();
    if (!s_hasEmbedded || !s_payloadData) return false;
    std::string norm = NormalizePath(path);
    auto it = s_entries.find(norm);
    if (it == s_entries.end()) return false;

    const Entry& e = it->second;
    const uint8_t* src = s_payloadData + e.offset;
    outBuf.resize(e.size);
    for (uint32_t i = 0; i < e.size; i++) {
        outBuf[i] = src[i] ^ s_xorKey;
    }
    return true;
}

Texture2D LoadTexture(const char* path) {
    std::vector<uint8_t> buf;
    if (GetDecryptedData(path, buf)) {
        const char* ext = GetFileExtension(path);
        if (!ext || !ext[0]) ext = ".png";
        Image img = LoadImageFromMemory(ext, buf.data(), (int)buf.size());
        if (img.data != nullptr) {
            Texture2D tex = LoadTextureFromImage(img);
            UnloadImage(img);
            return tex;
        }
    }
    return ::LoadTexture(path);
}

Image LoadImage(const char* path) {
    std::vector<uint8_t> buf;
    if (GetDecryptedData(path, buf)) {
        const char* ext = GetFileExtension(path);
        if (!ext || !ext[0]) ext = ".png";
        return LoadImageFromMemory(ext, buf.data(), (int)buf.size());
    }
    return ::LoadImage(path);
}

}

// --- InputManager.cpp ---
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

    // Championship Edition: Check 236236 (Double QCF + Attack) for Omnibus Bill Super Combo
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

// --- AudioEngine.cpp ---
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>

static constexpr int SAMPLE_RATE = 22050;

std::atomic<int> AudioEngine::s_currentTrack{ -1 };
std::atomic<size_t> AudioEngine::s_trackPlayhead{ 0 };
static std::atomic<bool> s_ceCriticalTempo{ false };
void AudioEngine::SetCriticalTempo(bool critical) { s_ceCriticalTempo.store(critical, std::memory_order_relaxed); }
std::vector<int16_t>* AudioEngine::s_trackDataPtr = nullptr;

AudioEngine& AudioEngine::Instance() {
    static AudioEngine s_instance;
    return s_instance;
}

Sound AudioEngine::BuildWaveformSound(int sampleRate, int sampleCount, int16_t* pcmData) {

    int fadeLen = std::min(sampleCount / 4, (int)(sampleRate * 0.003f));
    if (fadeLen > 0) {
        for (int i = 0; i < fadeLen; i++) {
            float fade = (float)i / fadeLen;
            pcmData[i] = (int16_t)(pcmData[i] * fade);
            pcmData[sampleCount - 1 - i] = (int16_t)(pcmData[sampleCount - 1 - i] * fade);
        }
    }

    Wave wave{};
    wave.frameCount = (unsigned int)sampleCount;
    wave.sampleRate = (unsigned int)sampleRate;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = pcmData;

    Sound snd = LoadSoundFromWave(wave);
    return snd;
}

struct Resonator {
    float b0 = 0.0f, a1 = 0.0f, a2 = 0.0f;
    float y1 = 0.0f, y2 = 0.0f;

    void Set(float freq, float bandwidth, float sampleRate) {
        float r = expf(-PI_F * (bandwidth / sampleRate));
        float theta = 2.0f * PI_F * (freq / sampleRate);
        a1 = -2.0f * r * cosf(theta);
        a2 = r * r;
        b0 = (1.0f - r * r) * 0.5f;
        y1 = 0.0f;
        y2 = 0.0f;
    }

    float Process(float in) {
        float out = b0 * in - a1 * y1 - a2 * y2;
        y2 = y1;
        y1 = out;
        return out;
    }
};

void AudioEngine::SynthesizeAll() {
    auto RandFloat = []() -> float {
        return (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
    };

    {
        int count = (int)(SAMPLE_RATE * 0.12f);
        std::vector<int16_t> pcm(count);
        float filter = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(t * PI_F);
            float cutoff = 0.2f + 0.5f * (1.0f - t);
            float white = RandFloat();
            filter += (white - filter) * cutoff;
            float s = filter * env * 14000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_WHOOSH_LIGHT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.22f);
        std::vector<int16_t> pcm(count);
        float filter = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(t * PI_F);
            float cutoff = 0.12f + 0.35f * (1.0f - t);
            float white = RandFloat();
            filter += (white - filter) * cutoff;
            float s = filter * env * 18000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_WHOOSH_HEAVY] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.15f);
        std::vector<int16_t> pcm(count);
        float phase = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 18.0f);
            float freq = 140.0f * expf(-t * 12.0f) + 45.0f;
            phase += 2.0f * PI_F * freq / SAMPLE_RATE;
            float bass = sinf(phase);
            float snap = RandFloat() * expf(-t * 40.0f);
            float s = (bass * 0.65f + snap * 0.45f) * env * 24000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_HIT_LIGHT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.28f);
        std::vector<int16_t> pcm(count);
        float phase = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 10.0f);
            float freq = 120.0f * expf(-t * 9.0f) + 36.0f;
            phase += 2.0f * PI_F * freq / SAMPLE_RATE;
            float bass = sinf(phase) + 0.35f * sinf(phase * 0.5f);
            float snap = RandFloat() * expf(-t * 30.0f);
            float raw = (bass * 0.7f + snap * 0.5f) * env * 1.8f;
            if (raw > 0.9f) raw = 0.9f + (raw - 0.9f) * 0.2f;
            if (raw < -0.9f) raw = -0.9f + (raw + 0.9f) * 0.2f;
            pcm[i] = (int16_t)(raw * 28000.0f);
        }
        m_sounds[SFX_HIT_HEAVY] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.12f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 28.0f);
            float tone1 = sinf(2.0f * PI_F * 1150.0f * i / SAMPLE_RATE);
            float tone2 = sinf(2.0f * PI_F * 1720.0f * i / SAMPLE_RATE);
            float click = RandFloat() * expf(-t * 60.0f);
            float s = (tone1 * 0.4f + tone2 * 0.3f + click * 0.5f) * env * 22000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_BLOCK] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.28f);
        std::vector<int16_t> pcm(count);
        float pMod = 0.0f, pCar = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(t * PI_F * 0.95f) * expf(-t * 4.0f);
            float fMod = 220.0f + 300.0f * (1.0f - t);
            float fCar = 440.0f * expf(-t * 3.5f) + 160.0f;
            pMod += 2.0f * PI_F * fMod / SAMPLE_RATE;
            float mod = sinf(pMod) * 3.5f * (1.0f - t);
            pCar += 2.0f * PI_F * (fCar + mod * 120.0f) / SAMPLE_RATE;
            float s = sinf(pCar) * env * 24000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_FIREBALL_LAUNCH] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.35f);
        std::vector<int16_t> pcm(count);
        float lp = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 9.0f);
            float noise = RandFloat();
            float bass = sinf(2.0f * PI_F * (80.0f - 40.0f * t) * i / SAMPLE_RATE);
            lp += (noise - lp) * (0.3f * (1.0f - t) + 0.05f);
            float s = (lp * 0.7f + bass * 0.5f) * env * 26000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_FIREBALL_IMPACT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.32f);
        std::vector<int16_t> pcm(count);
        float phase = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(t * PI_F);
            float freq = 180.0f + 680.0f * t * t;
            phase += 2.0f * PI_F * freq / SAMPLE_RATE;
            float whoosh = RandFloat() * 0.4f * env;
            float s = (sinf(phase) * 0.6f + whoosh) * env * 25000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_UPPERCUT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.30f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 16.0f);
            float wood1 = sinf(2.0f * PI_F * 290.0f * i / SAMPLE_RATE);
            float wood2 = sinf(2.0f * PI_F * 580.0f * i / SAMPLE_RATE) * 0.5f;
            float thump = sinf(2.0f * PI_F * 90.0f * i / SAMPLE_RATE) * expf(-t * 8.0f);
            float s = (wood1 + wood2 + thump * 1.2f) * env * 25000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_GAVEL_SLAM] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.35f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(t * PI_F);
            float sq = sinf(2.0f * PI_F * (650.0f + 120.0f * sinf(t * 30.0f)) * i / SAMPLE_RATE) > 0.0f ? 1.0f : -1.0f;
            float s = sq * env * 16000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_MEGAPHONE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.25f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 6.0f);
            int step = (int)(t * 16.0f);
            float f = 400.0f + (step % 5) * 220.0f;
            float wave = sinf(2.0f * PI_F * f * i / SAMPLE_RATE);
            wave = floorf(wave * 7.0f) / 7.0f;
            float s = wave * env * 20000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_ELECTRO_GLITCH] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.22f);
        std::vector<int16_t> pcm(count);
        float filt = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(t * PI_F);
            filt += (RandFloat() - filt) * (0.15f + 0.1f * t);
            float s = filt * env * 22000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SLIDE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.90f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 4.5f);
            float sub = sinf(2.0f * PI_F * (70.0f * expf(-t * 5.0f) + 35.0f) * i / SAMPLE_RATE);
            float c1 = sinf(2.0f * PI_F * 220.0f * i / SAMPLE_RATE);
            float c2 = sinf(2.0f * PI_F * 277.18f * i / SAMPLE_RATE);
            float c3 = sinf(2.0f * PI_F * 329.63f * i / SAMPLE_RATE);
            float c4 = sinf(2.0f * PI_F * 440.0f * i / SAMPLE_RATE);
            float crash = RandFloat() * expf(-t * 12.0f) * 0.6f;
            float s = (sub * 0.6f + (c1 + c2 + c3 + c4) * 0.15f + crash) * env * 28000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_TITLE_SLAM] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.40f);
        std::vector<int16_t> pcm(count);
        int split = (int)(SAMPLE_RATE * 0.08f);
        for (int i = 0; i < count; i++) {
            float freq = (i < split) ? 987.77f : 1318.51f;
            float t = (float)(i - (i < split ? 0 : split)) / (count - split);
            float env = expf(-t * 8.0f);
            float sq = sinf(2.0f * PI_F * freq * i / SAMPLE_RATE) > 0.0f ? 1.0f : -1.0f;
            float s = sq * env * 18000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_COIN] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.06f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(t * PI_F);
            float tone = sinf(2.0f * PI_F * 920.0f * i / SAMPLE_RATE) + 0.35f * sinf(2.0f * PI_F * 1840.0f * i / SAMPLE_RATE);
            pcm[i] = (int16_t)(tone * env * 18000.0f);
        }
        m_sounds[SFX_SELECT_CURSOR] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.18f);
        std::vector<int16_t> pcm(count);
        int mid = count / 2;
        for (int i = 0; i < count; i++) {
            float freq = (i < mid) ? 587.33f : 880.0f;
            float t = (float)(i % mid) / mid;
            float env = sinf(t * PI_F);
            float s = sinf(2.0f * PI_F * freq * i / SAMPLE_RATE) * env * 22000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SELECT_CONFIRM] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 1.60f);
        std::vector<int16_t> pcm(count, 0);

        auto AddBrassHit = [&](float startSec, float durSec, float f0, float chordMult, float amp, bool hasCymbal) {
            int startIdx = (int)(startSec * SAMPLE_RATE);
            int len = (int)(durSec * SAMPLE_RATE);
            float phaseCar = 0.0f, phaseMod = 0.0f;
            for (int j = 0; j < len && (startIdx + j) < count; j++) {
                float t = (float)j / len;

                float env = (t < 0.05f) ? (t / 0.05f) : expf(-(t - 0.05f) * 4.5f);
                float modIndex = 2.4f * env;

                phaseMod += 2.0f * PI_F * (f0 * 2.0f) / SAMPLE_RATE;
                float mod = sinf(phaseMod) * modIndex;

                phaseCar += 2.0f * PI_F * (f0 + mod * f0 * 0.5f) / SAMPLE_RATE;
                float brass = sinf(phaseCar);

                float body = sinf(2.0f * PI_F * (f0 * 0.5f) * j / SAMPLE_RATE);
                float chord = chordMult > 0.0f ? sinf(2.0f * PI_F * (f0 * chordMult) * j / SAMPLE_RATE) : 0.0f;

                float cymbal = 0.0f;
                if (hasCymbal) {
                    cymbal = RandFloat() * expf(-t * 9.0f) * 0.35f;
                }

                float sample = (brass * 0.6f + body * 0.25f + chord * 0.3f + cymbal) * env * amp;
                int idx = startIdx + j;
                float current = (float)pcm[idx] + sample;
                pcm[idx] = (int16_t)std::clamp(current, -32000.0f, 32000.0f);
            }
        };

        AddBrassHit(0.00f, 0.22f, 196.0f, 1.498f, 24000.0f, false);

        AddBrassHit(0.26f, 0.25f, 233.08f, 1.26f, 26000.0f, false);

        AddBrassHit(0.56f, 1.04f, 392.0f, 1.498f, 28000.0f, true);
        AddBrassHit(0.56f, 1.04f, 493.88f, 1.0f, 20000.0f, false);
        AddBrassHit(0.56f, 0.35f, 98.0f, 0.0f, 22000.0f, false);

        m_sounds[SFX_MATCH_START_FANFARE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    auto GenerateVoiceFormant = [&](float duration, float f0_start, float f0_end,
                                    float f1, float f2, float f3,
                                    float noiseMix, float roughAmp) -> std::vector<int16_t> {
        int count = (int)(SAMPLE_RATE * duration);
        std::vector<int16_t> pcm(count);
        Resonator r1, r2, r3;
        r1.Set(f1, 80.0f, (float)SAMPLE_RATE);
        r2.Set(f2, 100.0f, (float)SAMPLE_RATE);
        r3.Set(f3, 140.0f, (float)SAMPLE_RATE);

        float phase = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(t * PI_F);
            float f0 = f0_start + (f0_end - f0_start) * t;
            phase += f0 / (float)SAMPLE_RATE;
            if (phase >= 1.0f) phase -= 1.0f;

            float pulse = (phase < 0.6f) ? sinf(phase / 0.6f * PI_F) : 0.0f;
            float source = (pulse * 2.0f - 1.0f) * (1.0f - noiseMix) + RandFloat() * noiseMix;
            float v1 = r1.Process(source);
            float v2 = r2.Process(source);
            float v3 = r3.Process(source);
            float mixed = (v1 * 0.5f + v2 * 0.35f + v3 * 0.15f) * env * roughAmp;

            float norm = mixed / 30000.0f;
            norm = tanhf(norm);
            mixed = norm * 28000.0f;

            mixed = floorf(mixed / 128.0f) * 128.0f;
            pcm[i] = (int16_t)std::clamp(mixed, -32000.0f, 32000.0f);
        }
        return pcm;
    };

    {
        auto v1 = GenerateVoiceFormant(0.35f, 130.0f, 110.0f, 500.0f, 1100.0f, 2400.0f, 0.15f, 28000.0f);
        auto v2 = GenerateVoiceFormant(0.40f, 120.0f, 145.0f, 400.0f, 1800.0f, 2600.0f, 0.2f, 30000.0f);
        std::vector<int16_t> pcm;
        pcm.insert(pcm.end(), v1.begin(), v1.end());
        pcm.insert(pcm.end(), (int)(SAMPLE_RATE * 0.05f), 0);
        pcm.insert(pcm.end(), v2.begin(), v2.end());
        m_sounds[SFX_VOICE_ROUND1] = BuildWaveformSound(SAMPLE_RATE, (int)pcm.size(), pcm.data());
    }

    {
        auto v1 = GenerateVoiceFormant(0.32f, 130.0f, 110.0f, 500.0f, 1100.0f, 2400.0f, 0.15f, 28000.0f);
        auto v2 = GenerateVoiceFormant(0.38f, 135.0f, 105.0f, 320.0f, 950.0f, 2200.0f, 0.18f, 30000.0f);
        std::vector<int16_t> pcm;
        pcm.insert(pcm.end(), v1.begin(), v1.end());
        pcm.insert(pcm.end(), (int)(SAMPLE_RATE * 0.05f), 0);
        pcm.insert(pcm.end(), v2.begin(), v2.end());
        m_sounds[SFX_VOICE_ROUND2] = BuildWaveformSound(SAMPLE_RATE, (int)pcm.size(), pcm.data());
    }

    {
        auto v1 = GenerateVoiceFormant(0.36f, 150.0f, 120.0f, 650.0f, 1600.0f, 2600.0f, 0.22f, 30000.0f);
        auto v2 = GenerateVoiceFormant(0.34f, 130.0f, 100.0f, 500.0f, 1100.0f, 2400.0f, 0.15f, 28000.0f);
        std::vector<int16_t> pcm;
        pcm.insert(pcm.end(), v1.begin(), v1.end());
        pcm.insert(pcm.end(), (int)(SAMPLE_RATE * 0.06f), 0);
        pcm.insert(pcm.end(), v2.begin(), v2.end());
        m_sounds[SFX_VOICE_FINAL_ROUND] = BuildWaveformSound(SAMPLE_RATE, (int)pcm.size(), pcm.data());
    }

    {
        auto v = GenerateVoiceFormant(0.45f, 160.0f, 120.0f, 750.0f, 1400.0f, 2500.0f, 0.28f, 34000.0f);
        m_sounds[SFX_VOICE_FIGHT] = BuildWaveformSound(SAMPLE_RATE, (int)v.size(), v.data());
    }

    {
        auto v1 = GenerateVoiceFormant(0.3f, 140.0f, 130.0f, 350.0f, 900.0f, 2400.0f, 0.1f, 28000.0f);
        auto v2 = GenerateVoiceFormant(0.45f, 130.0f, 180.0f, 450.0f, 1950.0f, 2700.0f, 0.15f, 32000.0f);
        std::vector<int16_t> pcm;
        pcm.insert(pcm.end(), v1.begin(), v1.end());
        pcm.insert(pcm.end(), (int)(SAMPLE_RATE * 0.06f), 0);
        pcm.insert(pcm.end(), v2.begin(), v2.end());
        m_sounds[SFX_VOICE_YOU_WIN] = BuildWaveformSound(SAMPLE_RATE, (int)pcm.size(), pcm.data());
    }

    {
        auto v1 = GenerateVoiceFormant(0.35f, 130.0f, 140.0f, 600.0f, 1200.0f, 2500.0f, 0.2f, 30000.0f);
        auto v2 = GenerateVoiceFormant(0.55f, 140.0f, 110.0f, 500.0f, 1500.0f, 2300.0f, 0.25f, 34000.0f);
        std::vector<int16_t> pcm;
        pcm.insert(pcm.end(), v1.begin(), v1.end());
        pcm.insert(pcm.end(), (int)(SAMPLE_RATE * 0.04f), 0);
        pcm.insert(pcm.end(), v2.begin(), v2.end());
        m_sounds[SFX_VOICE_TOTAL_VETO] = BuildWaveformSound(SAMPLE_RATE, (int)pcm.size(), pcm.data());
    }

    {
        auto v = GenerateVoiceFormant(0.45f, 150.0f, 130.0f, 700.0f, 1100.0f, 2400.0f, 0.2f, 32000.0f);
        m_sounds[SFX_VOICE_HADOUKEN] = BuildWaveformSound(SAMPLE_RATE, (int)v.size(), v.data());
    }

    {
        auto v = GenerateVoiceFormant(0.42f, 160.0f, 190.0f, 650.0f, 1350.0f, 2600.0f, 0.25f, 32000.0f);
        m_sounds[SFX_VOICE_UPPERCUT] = BuildWaveformSound(SAMPLE_RATE, (int)v.size(), v.data());
    }

    {
        auto v = GenerateVoiceFormant(0.48f, 170.0f, 135.0f, 550.0f, 1500.0f, 2500.0f, 0.22f, 33000.0f);
        m_sounds[SFX_VOICE_OBJECTION] = BuildWaveformSound(SAMPLE_RATE, (int)v.size(), v.data());
    }

    {

        auto v1 = GenerateVoiceFormant(0.18f, 160.0f, 135.0f, 400.0f, 850.0f, 2200.0f, 0.10f, 26000.0f);

        auto v2 = GenerateVoiceFormant(0.28f, 165.0f, 230.0f, 820.0f, 1700.0f, 2800.0f, 0.16f, 28000.0f);
        std::vector<int16_t> pcm;
        pcm.insert(pcm.end(), v1.begin(), v1.end());
        pcm.insert(pcm.end(), (int)(SAMPLE_RATE * 0.020f), 0);
        pcm.insert(pcm.end(), v2.begin(), v2.end());
        m_sounds[SFX_VOICE_BOO_YAA] = BuildWaveformSound(SAMPLE_RATE, (int)pcm.size(), pcm.data());
    }

    {
        m_sounds[SFX_ROUND_WIN] = m_sounds[SFX_VOICE_BOO_YAA];
    }

    {
        int count = (int)(SAMPLE_RATE * 3.0f);
        std::vector<int16_t> pcm(count, 0);
        auto AddFanfareNote = [&](float startSec, float durSec, float f0, float chordMult, float amp) {
            int startIdx = (int)(startSec * SAMPLE_RATE);
            int len = (int)(durSec * SAMPLE_RATE);
            for (int j = 0; j < len && (startIdx + j) < count; j++) {
                float t = (float)j / len;
                float env = (t < 0.04f) ? (t / 0.04f) : expf(-(t - 0.04f) * 3.2f);
                float brass = sinf(2.0f * PI_F * f0 * j / SAMPLE_RATE) * 0.6f +
                              sinf(2.0f * PI_F * (f0 * 2.0f) * j / SAMPLE_RATE) * 0.25f +
                              (chordMult > 0.0f ? sinf(2.0f * PI_F * (f0 * chordMult) * j / SAMPLE_RATE) * 0.35f : 0.0f);
                float s = brass * env * amp;
                int idx = startIdx + j;
                float cur = (float)pcm[idx] + s;
                pcm[idx] = (int16_t)std::clamp(cur, -32000.0f, 32000.0f);
            }
        };

        AddFanfareNote(0.00f, 0.28f, 261.63f, 1.25f, 22000.0f);
        AddFanfareNote(0.28f, 0.28f, 329.63f, 1.25f, 22000.0f);
        AddFanfareNote(0.56f, 0.32f, 392.00f, 1.25f, 24000.0f);
        AddFanfareNote(0.88f, 0.90f, 523.25f, 1.50f, 26000.0f);
        AddFanfareNote(1.78f, 0.28f, 392.00f, 1.25f, 22000.0f);
        AddFanfareNote(2.06f, 0.90f, 523.25f, 1.50f, 28000.0f);
        for (float beat : { 0.0f, 0.88f, 2.06f }) {
            int bIdx = (int)(beat * SAMPLE_RATE);
            int bLen = (int)(0.4f * SAMPLE_RATE);
            for (int j = 0; j < bLen && (bIdx + j) < count; j++) {
                float t = (float)j / bLen;
                float s = sinf(2.0f * PI_F * 65.0f * expf(-t * 8.0f) * j / SAMPLE_RATE) * expf(-t * 6.0f) * 20000.0f;
                int idx = bIdx + j;
                float cur = (float)pcm[idx] + s;
                pcm[idx] = (int16_t)std::clamp(cur, -32000.0f, 32000.0f);
            }
        }
        m_sounds[SFX_VICTORY_FANFARE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.85f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / SAMPLE_RATE;
            float cycle = fmodf(t, 0.21f);
            float env = (cycle < 0.12f) ? sinf(cycle / 0.12f * PI_F) : 0.0f;
            float freq = (cycle < 0.06f) ? 1240.0f : 980.0f;
            float sq = sinf(2.0f * PI_F * freq * t) > 0.0f ? 1.0f : -1.0f;
            float s = sq * env * 20000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_CAR_ALARM] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.32f);
        std::vector<int16_t> pcm(count);
        float filter = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 9.0f);
            float lowBass = sinf(2.0f * PI_F * (90.0f * expf(-t * 12.0f) + 38.0f) * i / SAMPLE_RATE);
            float white = RandFloat();
            filter += (white - filter) * 0.25f;
            float clang = sinf(2.0f * PI_F * 480.0f * i / SAMPLE_RATE) * expf(-t * 18.0f);
            float s = (lowBass * 0.6f + filter * 0.5f + clang * 0.3f) * env * 28000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_METAL_CRUNCH] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.28f);
        std::vector<int16_t> pcm(count);
        Resonator rGlass;
        rGlass.Set(3200.0f, 400.0f, (float)SAMPLE_RATE);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 14.0f);
            float raw = RandFloat();
            float filtered = rGlass.Process(raw);
            float tinkle = sinf(2.0f * PI_F * (4200.0f - t * 1800.0f) * i / SAMPLE_RATE) * 0.4f;
            float s = (filtered * 0.7f + tinkle + raw * 0.2f) * env * 26000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_GLASS_BREAK] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.22f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 8.0f);
            float buzz = ((int)(i * 0.08f) % 2 == 0) ? 1.0f : -1.0f;
            float hum = sinf(2.0f * PI_F * 120.0f * i / SAMPLE_RATE);
            float crackle = (RandFloat() > 0.6f) ? RandFloat() : 0.0f;
            float s = (buzz * 0.4f + hum * 0.3f + crackle * 0.7f) * env * 25000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_ELEC_SPARK] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.26f);
        std::vector<int16_t> pcm(count);
        float phase = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;

            float freq = 135.0f * (1.0f - t * 0.55f) + 15.0f * sinf(2.0f * PI_F * 26.0f * (float)i / SAMPLE_RATE);
            phase += 2.0f * PI_F * freq / SAMPLE_RATE;

            float flutter = 0.7f + 0.3f * sinf(2.0f * PI_F * 32.0f * (float)i / SAMPLE_RATE);

            float tone = sinf(phase) + 0.45f * sinf(phase * 2.0f) + 0.25f * (sinf(phase * 3.0f) > 0.0f ? 1.0f : -1.0f);
            float noise = RandFloat() * 0.15f;

            float env = (t < 0.08f) ? (t / 0.08f) : expf(-(t - 0.08f) * 4.5f);
            float s = (tone + noise) * flutter * env * 24000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_FART] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.35f);
        std::vector<int16_t> pcm(count);
        float phase = 0.0f;
        float filt = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;

            float squelch = sinf(2.0f * PI_F * (90.0f + 50.0f * sinf(2.0f * PI_F * 45.0f * (float)i / SAMPLE_RATE)) * (float)i / SAMPLE_RATE);

            float white = RandFloat();
            filt += (white - filt) * 0.65f;
            float hiss = (white - filt);

            float snap = 0.0f;
            if (t > 0.30f && t < 0.42f) {
                float st = (t - 0.30f) / 0.12f;
                snap = sinf(2.0f * PI_F * 1200.0f * (float)i / SAMPLE_RATE) * expf(-st * 16.0f);
            }
            float env = (t < 0.15f) ? (t / 0.15f) : expf(-(t - 0.15f) * 4.0f);
            float s = (squelch * 0.45f + hiss * 0.70f + snap * 0.55f) * env * 24000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_WORM_HISS] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.38f);
        std::vector<int16_t> pcm(count);
        float filter = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;

            float click = (t < 0.04f) ? sinf(2.0f * PI_F * 3800.0f * (float)i / SAMPLE_RATE) * expf(-t * 90.0f) : 0.0f;

            float raw = RandFloat();
            filter += (raw - filter) * 0.45f;
            float hiss = (raw - filter) * ((t > 0.03f && t < 0.32f) ? sinf((t - 0.03f) / 0.29f * PI_F) : 0.0f);

            float chirp = sinf(2.0f * PI_F * (320.0f + 1200.0f * t) * (float)i / SAMPLE_RATE) * 0.25f;
            float s = (click * 0.7f + hiss * 0.8f + chirp * 0.4f) * 26000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_INJECTION] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.65f);
        std::vector<int16_t> pcm(count);
        float phase1 = 0.0f;
        float phase2 = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;

            float f0 = 92.0f + 35.0f * sinf(PI_F * t) + 12.0f * sinf(2.0f * PI_F * 38.0f * (float)i / SAMPLE_RATE);
            phase1 += 2.0f * PI_F * f0 / SAMPLE_RATE;
            phase2 += 2.0f * PI_F * (f0 * 1.52f) / SAMPLE_RATE;
            float roar = sinf(phase1) * 0.6f + sinf(phase2) * 0.4f + (sinf(phase1 * 3.0f) > 0.0f ? 0.3f : -0.3f);
            float growl = RandFloat() * 0.25f;
            float env = (t < 0.10f) ? (t / 0.10f) : expf(-(t - 0.10f) * 2.8f);
            float s = (roar + growl) * env * 28000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_WAR_PIG_ROAR] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.45f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;

            float clatter = 0.0f;
            float pulseTime = fmodf((float)i / SAMPLE_RATE, 0.038f);
            if (pulseTime < 0.008f && t < 0.45f) {
                clatter = sinf(2.0f * PI_F * 2200.0f * pulseTime) * expf(-pulseTime * 400.0f);
            }

            float zapFreq = 160.0f + 380.0f * sinf(2.0f * PI_F * 45.0f * (float)i / SAMPLE_RATE);
            float buzz = ((int)(i * 0.12f) % 2 == 0) ? 1.0f : -1.0f;
            float spark = (RandFloat() > 0.5f) ? RandFloat() : 0.0f;
            float arc = sinf(2.0f * PI_F * zapFreq * (float)i / SAMPLE_RATE) * 0.5f;
            float env = expf(-t * 4.2f);
            float s = (clatter * 0.5f + (buzz * 0.35f + arc * 0.35f + spark * 0.65f) * (t > 0.1f ? 1.0f : t * 10.0f)) * env * 27000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_EMAIL_ZAP] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.65f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;

            float pop = (t < 0.05f) ? sinf(2.0f * PI_F * 1400.0f * expf(-t * 50.0f) * (float)i / SAMPLE_RATE) * expf(-t * 60.0f) : 0.0f;

            float chime = 0.0f;
            float baseFreq = 523.25f + 400.0f * t;
            chime += sinf(2.0f * PI_F * baseFreq * (float)i / SAMPLE_RATE) * 0.45f;
            chime += sinf(2.0f * PI_F * (baseFreq * 1.5f) * (float)i / SAMPLE_RATE) * 0.30f;
            chime += sinf(2.0f * PI_F * (baseFreq * 2.0f) * (float)i / SAMPLE_RATE) * 0.20f;
            chime += sinf(2.0f * PI_F * (baseFreq * 2.5f) * (float)i / SAMPLE_RATE) * 0.15f;

            float shimmer = 0.7f + 0.3f * sinf(2.0f * PI_F * 18.0f * (float)i / SAMPLE_RATE);
            float env = (t < 0.08f) ? (t / 0.08f) : expf(-(t - 0.08f) * 2.5f);
            float s = (pop * 0.7f + chime * shimmer * 0.85f) * env * 28000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_ADRENOCHROME] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 2.5f);
        std::vector<int16_t> pcm(count);

        struct FanfareNote { float start; float dur; float freq; };
        FanfareNote notes[] = {
            { 0.00f, 0.22f, 261.63f },
            { 0.18f, 0.22f, 329.63f },
            { 0.36f, 0.22f, 392.00f },
            { 0.54f, 0.40f, 523.25f },
            { 0.90f, 0.35f, 659.25f },
            { 1.20f, 1.30f, 783.99f },
            { 1.20f, 1.30f, 1046.50f }
        };
        for (const auto& n : notes) {
            int startIdx = (int)(n.start * SAMPLE_RATE);
            int len = (int)(n.dur * SAMPLE_RATE);
            for (int j = 0; j < len && (startIdx + j) < count; j++) {
                float t = (float)j / len;
                float env = (t < 0.08f) ? (t / 0.08f) : expf(-(t - 0.08f) * 2.2f);
                float wave = sinf(2.0f * PI_F * n.freq * j / SAMPLE_RATE) * 0.5f
                           + sinf(2.0f * PI_F * n.freq * 2.0f * j / SAMPLE_RATE) * 0.25f
                           + sinf(2.0f * PI_F * n.freq * 3.0f * j / SAMPLE_RATE) * 0.12f;

                float brass = ((int)(j * (n.freq / 22050.0f) * 8.0f) % 2 == 0 ? 0.15f : -0.15f);
                float val = (wave + brass) * env * 14000.0f;
                int idx = startIdx + j;
                float cur = (float)pcm[idx] + val;
                pcm[idx] = (int16_t)std::clamp(cur, -32000.0f, 32000.0f);
            }
        }
        m_sounds[SFX_JESUS_FANFARE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.35f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;

            float pop = (t < 0.04f) ? sinf(2.0f * PI_F * 1850.0f * expf(-t * 80.0f) * (float)i / SAMPLE_RATE) * expf(-t * 90.0f) : 0.0f;

            float noise = RandFloat();
            float fizzEnv = (t < 0.05f) ? (t / 0.05f) : expf(-(t - 0.05f) * 6.5f);
            float fizz = noise * fizzEnv * 0.7f;

            float squirt = sinf(2.0f * PI_F * (920.0f - 400.0f * t) * (float)i / SAMPLE_RATE) * fizzEnv * 0.3f;
            float s = (pop * 0.85f + fizz + squirt) * 27000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_COKE_POP] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.42f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 9.0f);

            float wood = sinf(2.0f * PI_F * 140.0f * (float)i / SAMPLE_RATE) * expf(-t * 18.0f);
            float subBass = sinf(2.0f * PI_F * 65.0f * (float)i / SAMPLE_RATE) * expf(-t * 7.0f);

            float chime1 = sinf(2.0f * PI_F * 880.0f * (float)i / SAMPLE_RATE) * expf(-t * 4.5f) * 0.35f;
            float chime2 = sinf(2.0f * PI_F * 1760.0f * (float)i / SAMPLE_RATE) * expf(-t * 6.0f) * 0.18f;
            float s = (wood * 0.9f + subBass * 0.8f + chime1 + chime2) * env * 29000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_GAVEL_STOMP] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.35f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 5.5f);
            float spark = (RandFloat() > 0.45f) ? RandFloat() : 0.0f;
            float buzz = ((int)(i * 0.16f) % 2 == 0) ? 1.0f : -1.0f;
            float flame = sinf(2.0f * PI_F * (320.0f + 180.0f * sinf(t * 22.0f)) * (float)i / SAMPLE_RATE);
            float s = (spark * 0.6f + buzz * 0.3f + flame * 0.4f) * env * 26000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_DISSENT_FLAME] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.45f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;

            float whistle = 0.0f;
            if (t < 0.15f) {
                float wFreq = 2950.0f + 220.0f * sinf(2.0f * PI_F * 45.0f * (float)i / SAMPLE_RATE);
                whistle = sinf(2.0f * PI_F * wFreq * (float)i / SAMPLE_RATE) * (1.0f - t / 0.15f);
            }

            float whoosh = RandFloat() * ((t > 0.10f && t < 0.35f) ? 0.6f : 0.1f);
            float tackleSlam = (t > 0.25f) ? sinf(2.0f * PI_F * 95.0f * (float)i / SAMPLE_RATE) * expf(-(t - 0.25f) * 16.0f) : 0.0f;
            float s = (whistle * 0.65f + whoosh * 0.5f + tackleSlam * 1.1f) * 27000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SECRET_SERVICE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.38f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;

            float snip1 = (t >= 0.02f && t < 0.08f) ? sinf(2.0f * PI_F * 2400.0f * (float)i / SAMPLE_RATE) * expf(-(t - 0.02f) * 80.0f) : 0.0f;
            float snip2 = (t >= 0.12f && t < 0.18f) ? sinf(2.0f * PI_F * 2600.0f * (float)i / SAMPLE_RATE) * expf(-(t - 0.12f) * 80.0f) : 0.0f;

            float boing = 0.0f;
            if (t >= 0.18f) {
                float bT = (t - 0.18f) / 0.20f;
                float bFreq = 220.0f + 580.0f * bT * bT;
                boing = sinf(2.0f * PI_F * bFreq * (float)i / SAMPLE_RATE) * expf(-bT * 4.0f);
            }
            float s = (snip1 * 0.8f + snip2 * 0.85f + boing * 0.75f) * 27000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_NOSE_CUT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 1.6f);
        std::vector<int16_t> pcm(count);
        float phase1 = 0.0f;
        float phase2 = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;

            float f0 = 55.0f + 65.0f * sinf(PI_F * t) + 16.0f * sinf(2.0f * PI_F * 14.0f * (float)i / SAMPLE_RATE);
            phase1 += 2.0f * PI_F * f0 / SAMPLE_RATE;
            phase2 += 2.0f * PI_F * (f0 * 1.48f) / SAMPLE_RATE;
            float roar = sinf(phase1) * 0.55f + sinf(phase2) * 0.35f + (sinf(phase1 * 2.8f) > 0.0f ? 0.3f : -0.3f);
            float growl = RandFloat() * 0.35f;
            float subBass = sinf(2.0f * PI_F * 42.0f * (float)i / SAMPLE_RATE) * 0.6f;
            float env = (t < 0.15f) ? (t / 0.15f) : expf(-(t - 0.15f) * 1.6f);
            float s = (roar + growl + subBass) * env * 29000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_CTHULHU_ROAR] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.45f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;

            float snap = (t < 0.06f) ? (sinf(2.0f * PI_F * 380.0f * (float)i / SAMPLE_RATE) + RandFloat() * 0.8f) * expf(-t * 60.0f) : 0.0f;

            float splashNoise = RandFloat() * expf(-t * 5.0f);
            float chompThud = sinf(2.0f * PI_F * 90.0f * (float)i / SAMPLE_RATE) * expf(-t * 12.0f);
            float s = (snap * 1.2f + chompThud * 0.7f + splashNoise * 0.6f) * 28000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SHARK_CHOMP] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.85f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;

            float snoreTone = 0.0f;
            if (t < 0.50f) {
                float freq = 80.0f + 30.0f * sinf(2.0f * PI_F * 12.0f * (float)i / SAMPLE_RATE);
                float buzz = ((int)(i * (freq / 22050.0f) * 4.0f) % 2 == 0) ? 0.6f : -0.6f;
                snoreTone = buzz * sinf(t / 0.50f * PI_F);
            } else {
                float exhale = RandFloat() * sinf((t - 0.50f) / 0.35f * PI_F) * 0.35f;
                snoreTone = exhale;
            }
            float s = snoreTone * 22000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_CTHULHU_SNORE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.65f);
        std::vector<int16_t> pcm(count);

        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float noteFreq = (t < 0.15f) ? 349.23f : ((t < 0.30f) ? 415.30f : 466.16f);
            if (t >= 0.30f) {
                noteFreq += 12.0f * sinf(2.0f * PI_F * 5.8f * (t - 0.30f));
            }
            float phase = 2.0f * PI_F * noteFreq * (float)i / SAMPLE_RATE;
            float wave = sinf(phase) * 0.50f
                       + sinf(phase * 2.0f) * 0.30f
                       + sinf(phase * 3.0f) * 0.20f
                       + sinf(phase * 4.0f) * 0.12f
                       + sinf(phase * 5.0f) * 0.08f;
            float buzz = ((int)(i * 0.08f) % 2 == 0) ? 0.06f : -0.06f;
            wave += buzz;
            float env = 0.0f;
            if (t < 0.04f) env = t / 0.04f;
            else if (t < 0.45f) env = 1.0f - (t - 0.04f) * 0.15f;
            else env = (1.0f - (0.45f - 0.04f) * 0.15f) * expf(-(t - 0.45f) * 12.0f);

            float s = wave * env * 28000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SAXOPHONE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
}

struct FMVoiceState {
    float carPhase = 0.0f;
    float modPhase = 0.0f;

    float Process(float carFreq, float modMult, float modIndex, float env) {
        modPhase += 2.0f * PI_F * (carFreq * modMult) / (float)SAMPLE_RATE;
        float mod = sinf(modPhase) * modIndex;
        carPhase += 2.0f * PI_F * (carFreq + mod * carFreq) / (float)SAMPLE_RATE;
        return sinf(carPhase) * env;
    }
};

void AudioEngine::SynthesizeBgmTracks() {

    constexpr int LOOP_SAMPLES = 78400;
    constexpr int STEPS = 32;
    constexpr int SAMPLES_PER_STEP = LOOP_SAMPLES / STEPS;

    for (int t = 0; t < 6; t++) {
        m_bgmTracks[t].resize(LOOP_SAMPLES);
        FMVoiceState bassVoice;
        FMVoiceState leadVoice;

        for (int step = 0; step < STEPS; step++) {
            float bassFreq = 55.0f;
            float leadFreq = 0.0f;

            if (t == 0) {

                int bassNotes[8] = { 55, 55, 65, 55, 73, 65, 55, 49 };
                bassFreq = (float)bassNotes[step % 8];
                leadFreq = bassFreq * 4.0f;
            } else if (t == 1) {

                int bassRiff[16] = { 55, 55, 110, 55, 65, 55, 98, 55, 55, 55, 110, 65, 73, 65, 49, 52 };
                bassFreq = (float)bassRiff[step % 16];
                int leadRiff[16] = { 220, 0, 261, 293, 329, 0, 293, 261, 220, 261, 293, 349, 329, 293, 261, 196 };
                leadFreq = (float)leadRiff[step % 16];
            } else if (t == 2) {

                int bassNotes[16] = { 65, 65, 65, 65, 58, 58, 58, 58, 49, 49, 49, 49, 55, 55, 62, 62 };
                bassFreq = (float)bassNotes[step % 16];
                int leadNotes[16] = { 261, 293, 329, 392, 329, 293, 261, 293, 329, 392, 440, 523, 440, 392, 329, 293 };
                leadFreq = (float)leadNotes[step % 16];
            } else if (t == 3) {

                int bassNotes[16] = { 73, 73, 82, 73, 87, 73, 98, 73, 73, 82, 87, 98, 110, 98, 87, 82 };
                bassFreq = (float)bassNotes[step % 16];
                int leadNotes[16] = { 293, 329, 349, 392, 440, 392, 349, 329, 293, 349, 440, 523, 587, 523, 440, 349 };
                leadFreq = (float)leadNotes[step % 16];
            } else if (t == 4) {

                int bassNotes[16] = { 41, 41, 82, 41, 49, 41, 73, 41, 41, 41, 82, 49, 55, 49, 41, 44 };
                bassFreq = (float)bassNotes[step % 16];
                leadFreq = bassFreq * 6.0f;
            } else {

                int bonusBass[16] = { 65, 65, 130, 65, 78, 65, 116, 65, 73, 65, 146, 73, 82, 73, 65, 78 };
                bassFreq = (float)bonusBass[step % 16];
                int bonusLead[16] = { 261, 329, 392, 523, 392, 329, 261, 392, 293, 349, 440, 587, 440, 349, 293, 440 };
                leadFreq = (float)bonusLead[step % 16];
            }

            for (int s = 0; s < SAMPLES_PER_STEP; s++) {
                int sampleIdx = step * SAMPLES_PER_STEP + s;
                float stepT = (float)s / SAMPLES_PER_STEP;

                float bassEnv = expf(-stepT * 5.0f);
                float leadEnv = sinf(stepT * PI_F * 0.9f);

                float drum = 0.0f;

                if ((step % 4) == 0) {
                    drum += sinf(2.0f * PI_F * (130.0f * expf(-stepT * 20.0f)) * ((float)s / SAMPLE_RATE)) * expf(-stepT * 10.0f) * 0.55f;
                }

                if ((step % 4) == 2) {
                    float noise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
                    drum += noise * expf(-stepT * 10.0f) * 0.38f;
                }

                float hatNoise = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f);
                drum += hatNoise * expf(-stepT * 32.0f) * 0.12f;

                float bass = bassVoice.Process(bassFreq, 2.0f, 2.0f, bassEnv) * 0.42f;
                float lead = 0.0f;
                if (leadFreq > 50.0f) {
                    lead = leadVoice.Process(leadFreq, 3.0f, 1.4f, leadEnv) * 0.22f;
                }

                float mix = (bass + lead + drum) * 0.55f;
                m_bgmTracks[t][sampleIdx] = (int16_t)std::clamp(mix * 28000.0f, -32000.0f, 32000.0f);
            }
        }
    }
}

void AudioEngine::BgmAudioCallback(void* bufferData, unsigned int frames) {
    int16_t* out = (int16_t*)bufferData;
    int track = s_currentTrack.load(std::memory_order_relaxed);

    if (s_trackDataPtr != nullptr && track >= 0 && track < 6 && !s_trackDataPtr[track].empty()) {
        const auto& pcm = s_trackDataPtr[track];
        size_t sz = pcm.size();
        size_t pos = s_trackPlayhead.load(std::memory_order_relaxed);

        bool fast = s_ceCriticalTempo.load(std::memory_order_relaxed);
        static unsigned int fracAcc = 0;
        for (unsigned int i = 0; i < frames; i++) {
            if (pos >= sz) pos = 0;
            out[i] = pcm[pos];
            if (fast) {
                // 1.25x tempo acceleration when either fighter is in critical health (<30% HP)
                fracAcc += 5;
                pos += (fracAcc >> 2);
                fracAcc &= 3;
            } else {
                pos++;
            }
        }
        s_trackPlayhead.store(pos, std::memory_order_relaxed);
    } else {
        memset(bufferData, 0, frames * sizeof(int16_t));
    }
}

void AudioEngine::Init() {
    if (m_initialized) return;
    InitAudioDevice();

    SynthesizeAll();
    SynthesizeBgmTracks();

    s_trackDataPtr = m_bgmTracks;
    s_currentTrack.store(0, std::memory_order_release);
    s_trackPlayhead.store(0, std::memory_order_relaxed);

    m_bgmStream = LoadAudioStream(SAMPLE_RATE, 16, 1);
    SetAudioStreamCallback(m_bgmStream, BgmAudioCallback);
    PlayAudioStream(m_bgmStream);
    m_streamActive = true;

    m_initialized = true;
}

void AudioEngine::Shutdown() {
    if (!m_initialized) return;
    if (m_streamActive) {
        StopAudioStream(m_bgmStream);
        UnloadAudioStream(m_bgmStream);
        m_streamActive = false;
    }
    for (int i = 0; i < SFX_COUNT; i++) {
        UnloadSound(m_sounds[i]);
    }
    CloseAudioDevice();
    m_initialized = false;
}

void AudioEngine::PlaySfx(SfxId id, float pitch, float volume) {
    if (!m_initialized || id < 0 || id >= SFX_COUNT) return;
    SetSoundPitch(m_sounds[id], pitch);
    SetSoundVolume(m_sounds[id], volume);
    ::PlaySound(m_sounds[id]);
}

void AudioEngine::SetBgmTrack(int trackId) {
    if (s_currentTrack.load(std::memory_order_relaxed) != trackId) {
        s_trackPlayhead.store(0, std::memory_order_relaxed);
        s_currentTrack.store(trackId, std::memory_order_release);
    }
}

void AudioEngine::StopBgm() {
    s_currentTrack.store(-1, std::memory_order_release);
    s_trackPlayhead.store(0, std::memory_order_relaxed);
}

void AudioEngine::Update() {

}

// --- Fighter.cpp ---
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
    AddSuperMeter(8.0f);

    if (motion == MOTION_SUPER_OMNIBUS && m_superMeter >= 100.0f) {
        m_superMeter = 0.0f;
        m_isSuperMove = true;
        m_triggerSuperFlash = true;
        m_invincibleFrames = 18;
        m_multiHitsRemaining = 5;
        m_specialIndex = 2;
        m_currentAttack = AttackData{ "OMNIBUS BILL SUPER", 4, 28, 18, 7, ATTACK_MID, 14, 30, 18, 5.2f, true, true, 44.0f, -34.0f, 46.0f, 38.0f, true, MOVE_SPECIAL };
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
            AddSuperMeter(8.0f);
            return true;
        } else if (btn == BTN_LP) {
            const char* name = (m_charId == CHAR_BILLARY) ?
                (m_facingRight ? "Presidential Point" : "Gavel Jab") :
                (crouch ? "Crouch Jab" : "Stand Jab");
            if (crouch) StartNormalAttack(MOVE_LIGHT_PUNCH, name, 3, 3, 4, 6, ATTACK_MID, 6, 11, 8, 2.0f, false, 28.0f, -14.0f, 22.0f, 14.0f);
            else StartNormalAttack(MOVE_LIGHT_PUNCH, name, 3, 3, 5, 7, ATTACK_MID, 6, 12, 8, 2.2f, false, 32.0f, -34.0f, 24.0f, 14.0f);
            AddSuperMeter(3.0f);
            return true;
        } else if (btn == BTN_HP) {
            const char* name = (m_charId == CHAR_BILLARY) ?
                (m_facingRight ? "Executive Haymaker" : "Gavel Smash") :
                (crouch ? "Crouch Haymaker" : "Stand Haymaker");
            if (crouch) StartNormalAttack(MOVE_HEAVY_PUNCH, name, 5, 4, 12, 16, ATTACK_MID, 11, 22, 14, 3.8f, true, 30.0f, -24.0f, 28.0f, 24.0f);
            else StartNormalAttack(MOVE_HEAVY_PUNCH, name, 5, 4, 12, 18, ATTACK_OVERHEAD, 11, 24, 16, 4.2f, true, 36.0f, -36.0f, 32.0f, 20.0f);
            AddSuperMeter(6.0f);
            return true;
        } else if (btn == BTN_LK) {
            const char* name = (m_charId == CHAR_BILLARY) ?
                (m_facingRight ? "Cabinet Kick" : "Low Subpoena") :
                (crouch ? "Crouch Light Kick" : "Stand Light Kick");
            if (crouch) StartNormalAttack(MOVE_LIGHT_KICK, name, 3, 3, 5, 7, ATTACK_LOW, 6, 12, 8, 2.0f, false, 32.0f, -8.0f, 26.0f, 14.0f);
            else StartNormalAttack(MOVE_LIGHT_KICK, name, 4, 3, 5, 8, ATTACK_MID, 6, 13, 9, 2.4f, false, 34.0f, -22.0f, 26.0f, 14.0f);
            AddSuperMeter(3.0f);
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
            AddSuperMeter(6.0f);
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
    AddSuperMeter(damage * 0.65f);

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
    AddSuperMeter(4.0f);

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
        if (m_superMeter >= 100.0f && crouching) {
            if (m_state == FSTATE_ATTACK) m_justCanceled = true;
            ExecuteSpecial(MOTION_SUPER_OMNIBUS, BTN_HP, opponent);
        } else if (grounded && (m_state == FSTATE_IDLE || m_state == FSTATE_WALK_FWD || m_state == FSTATE_WALK_BACK || m_state == FSTATE_CROUCH)) {
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

// --- Collision.cpp ---
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

            int remFrames = std::max(1, (attack.startupFrames + attack.activeFrames + attack.recoveryFrames) - attacker.GetStateFrame());
            if (isBlocking) {
                int chip = attack.isSpecial ? std::max(1, attack.damage / 5) : 0;
                defender.ApplyBlock(chip, 6, attack.blockstunFrames, attack.pushback);
                attacker.SetHitstop(6);
                attacker.AddSuperMeter(5.0f);
                attacker.SetLastFrameAdvantage(attack.blockstunFrames - remFrames);
                defender.SetLastFrameAdvantage(remFrames - attack.blockstunFrames);
                cameraShake = 1.2f;

                sparks.push_back(SparkEffect{ sparkPos, true, 0, 8, true });
                AudioEngine::Instance().PlaySfx(SFX_BLOCK, 1.0f);
            } else {
                int hs = attack.isSpecial ? 14 : (attack.isHeavy ? 11 : 6);
                defender.ApplyHit(attack.damage, attack.type, hs, attack.hitstunFrames, attack.pushback, attack.causesKnockdown);
                attacker.SetHitstop(hs);
                attacker.AddSuperMeter(attack.isHeavy ? 12.0f : 7.0f);
                attacker.AddComboHit();
                defender.ResetCombo();
                attacker.SetLastFrameAdvantage(attack.hitstunFrames - remFrames);
                defender.SetLastFrameAdvantage(remFrames - attack.hitstunFrames);
                cameraShake = attack.isSpecial ? 4.2f : (attack.isHeavy ? 2.8f : 1.5f);

                sparks.push_back(SparkEffect{ sparkPos, false, 0, 10, true });
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

// --- Stages.cpp ---
#include <cmath>
#include <cstdio>
#include <cstring>
#include <algorithm>

StageManager::StageManager() {
    m_venues[STAGE_DEMOLISHED_EAST_WING] = VenueDetails{
        "EAST WING DEMOLITION", "WASHINGTON, D.C. - SECTOR 1",
        Color{ 25, 20, 35, 255 }, Color{ 70, 45, 60, 255 },
        Color{ 90, 85, 95, 255 }, Color{ 50, 48, 55, 255 },
        Color{ 230, 190, 40, 255 }
    };

    m_venues[STAGE_REFLECTING_POOL] = VenueDetails{
        "THE REFLECTING POOL", "NATIONAL MALL - BASIN PLAZA",
        Color{ 10, 15, 40, 255 }, Color{ 35, 60, 100, 255 },
        Color{ 20, 35, 65, 255 }, Color{ 20, 80, 140, 255 },
        Color{ 0, 220, 255, 255 }
    };

    m_venues[STAGE_CONGRESSIONAL_FLOOR] = VenueDetails{
        "CONGRESSIONAL CHAMBER", "CAPITOL HILL - HOUSE FLOOR",
        Color{ 40, 15, 15, 255 }, Color{ 80, 35, 30, 255 },
        Color{ 110, 55, 35, 255 }, Color{ 75, 40, 25, 255 },
        Color{ 220, 180, 60, 255 }
    };

    m_venues[STAGE_WHITE_HOUSE_MMA] = VenueDetails{
        "SOUTH LAWN OCTAGON", "EXECUTIVE GROUNDS - LAWN CAGE",
        Color{ 15, 25, 30, 255 }, Color{ 35, 55, 65, 255 },
        Color{ 45, 65, 50, 255 }, Color{ 140, 140, 145, 255 },
        Color{ 200, 40, 40, 255 }
    };

    m_venues[STAGE_PRESS_BRIEFING] = VenueDetails{
        "PRESS BRIEFING ROOM", "WHITE HOUSE - JAMES BRADY ROOM",
        Color{ 15, 20, 45, 255 }, Color{ 25, 45, 95, 255 },
        Color{ 30, 50, 110, 255 }, Color{ 20, 35, 75, 255 },
        Color{ 255, 215, 0, 255 }
    };

    m_venues[STAGE_PENTAGON_ALTAR] = VenueDetails{
        "PENTAGON ALTAR OF WAR", "THE PENTAGON - BASAL SANCTUARY",
        Color{ 20, 15, 18, 255 }, Color{ 55, 25, 30, 255 },
        Color{ 75, 35, 40, 255 }, Color{ 35, 30, 38, 255 },
        Color{ 210, 40, 40, 255 }
    };

    m_venues[STAGE_WTC_PLAZA] = VenueDetails{
        "WTC PLAZA (PRE-9/11)", "NEW YORK CITY - WORLD TRADE CENTER",
        Color{ 30, 45, 80, 255 }, Color{ 70, 100, 155, 255 },
        Color{ 95, 120, 150, 255 }, Color{ 140, 140, 145, 255 },
        Color{ 255, 220, 80, 255 }
    };

    m_venues[STAGE_COMET_PING_PONG] = VenueDetails{
        "COMET PING PONG", "WASHINGTON, D.C. - BASEMENT VENUE",
        Color{ 20, 20, 25, 255 }, Color{ 40, 35, 45, 255 },
        Color{ 60, 50, 55, 255 }, Color{ 45, 45, 50, 255 },
        Color{ 255, 60, 60, 255 }
    };

    m_venues[STAGE_FLORIDA_KEYS] = VenueDetails{
        "TEMPLE ISLAND", "FLORIDA KEYS - PRIVATE ATOLL",
        Color{ 20, 60, 110, 255 }, Color{ 40, 130, 180, 255 },
        Color{ 240, 220, 130, 255 }, Color{ 225, 215, 180, 255 },
        Color{ 255, 215, 0, 255 }
    };
}

StageManager::~StageManager() {
    Shutdown();
}

void StageManager::Init() {
    if (m_texturesLoaded) return;

    AssetManager::Init();

    std::string baseDir = "assets/";
    if (!AssetManager::HasEmbeddedAssets() && !DirectoryExists(baseDir.c_str())) {
        std::string appDirAssets = std::string(GetApplicationDirectory()) + "assets/";
        std::string parentAssets = "../assets/";
        std::string appParentAssets = std::string(GetApplicationDirectory()) + "../assets/";
        if (DirectoryExists(appDirAssets.c_str())) {
            baseDir = appDirAssets;
        } else if (DirectoryExists(parentAssets.c_str())) {
            baseDir = parentAssets;
        } else if (DirectoryExists(appParentAssets.c_str())) {
            baseDir = appParentAssets;
        }
    }

    const char* namedStages[STAGE_COUNT] = {
        "stage_0.png", "stage_1.png", "stage_2.png", "stage_3.png", "stage_4.png", "stage_5.png",
        "stage_wtc_plaza.png", "stage_comet_ping_pong.png", "stage_florida_keys.png"
    };

    for (int i = 0; i < STAGE_COUNT; i++) {
        char path[128];
        snprintf(path, sizeof(path), "%sstage_%d.png", baseDir.c_str(), i);
        if (AssetManager::FileExists(path)) {
            m_stageTextures[i] = AssetManager::LoadTexture(path);
            SetTextureFilter(m_stageTextures[i], TEXTURE_FILTER_POINT);
        } else if (namedStages[i] != nullptr) {
            snprintf(path, sizeof(path), "%s%s", baseDir.c_str(), namedStages[i]);
            if (AssetManager::FileExists(path)) {
                m_stageTextures[i] = AssetManager::LoadTexture(path);
                SetTextureFilter(m_stageTextures[i], TEXTURE_FILTER_POINT);
            }
        }
    }
    m_texturesLoaded = true;
}

void StageManager::Shutdown() {
    if (!m_texturesLoaded) return;
    for (int i = 0; i < STAGE_COUNT; i++) {
        if (m_stageTextures[i].id > 0) {
            UnloadTexture(m_stageTextures[i]);
            m_stageTextures[i] = { 0 };
        }
    }
    m_texturesLoaded = false;
}

void StageManager::SetStage(StageId id) {
    if (id >= 0 && id < STAGE_COUNT) {
        m_stageId = id;
    }
}

void StageManager::DrawFarLayer(float camX, float time) {
    if (m_texturesLoaded && m_stageTextures[m_stageId].id > 0) {

        float minCam = STAGE_MIN_X;
        float maxCam = STAGE_MAX_X - SCREEN_WIDTH;
        float t = std::clamp((camX - minCam) / (maxCam - minCam), 0.0f, 1.0f);

        float srcH = 640.0f;
        float srcW = srcH * ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
        float maxScrollX = (float)m_stageTextures[m_stageId].width - srcW;
        float srcX = t * maxScrollX;
        float srcY = (float)m_stageTextures[m_stageId].height - srcH;

        Rectangle srcRec = { srcX, srcY, srcW, srcH };
        Rectangle dstRec = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
        DrawTexturePro(m_stageTextures[m_stageId], srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

        switch (m_stageId) {
        case STAGE_DEMOLISHED_EAST_WING: {

            if (fmodf(time, 4.8f) < 0.07f) {
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 210, 200, 240, 65 });
            }
            break;
        }
        case STAGE_REFLECTING_POOL: {

            float monTexX = 688.0f;
            float monScreenX = (monTexX - srcX) * ((float)SCREEN_WIDTH / srcW);
            if (fmodf(time, 1.2f) < 0.6f) {
                DrawCircle((int)monScreenX, 10, 3, Color{ 255, 40, 40, 240 });
                DrawCircle((int)monScreenX, 10, 6, Color{ 255, 60, 60, 90 });
            }
            break;
        }
        case STAGE_CONGRESSIONAL_FLOOR: {

            float chanScreenX = (float)(SCREEN_WIDTH / 2) + sinf(time * 1.5f) * 6.0f;
            DrawLine((int)chanScreenX, 0, (int)chanScreenX, 20, Color{ 200, 160, 50, 255 });
            DrawCircle((int)chanScreenX, 22, 8, Color{ 220, 180, 60, 255 });
            DrawCircle((int)chanScreenX, 22, 4, Color{ 255, 240, 180, 255 });
            break;
        }
        case STAGE_WHITE_HOUSE_MMA: {

            float heliTexX = 1140.0f;
            float heliScreenX = (heliTexX - srcX) * ((float)SCREEN_WIDTH / srcW);
            float heliScreenY = (280.0f - srcY) * ((float)SCREEN_HEIGHT / srcH);
            float rotorSweep = sinf(time * 40.0f) * 36.0f;
            DrawLine((int)heliScreenX - (int)rotorSweep, (int)heliScreenY, (int)heliScreenX + (int)rotorSweep, (int)heliScreenY, Color{ 160, 180, 160, 180 });
            break;
        }
        case STAGE_PRESS_BRIEFING: {

            float flashCycle = fmodf(time * 3.8f, 1.0f);
            if (flashCycle < 0.08f) {
                float fx = 60.0f + fmodf(time * 123.4f, 260.0f);
                float fy = 90.0f + fmodf(time * 56.7f, 50.0f);
                DrawCircle((int)fx, (int)fy, 8, Color{ 255, 255, 255, 220 });
                DrawCircle((int)fx, (int)fy, 18, Color{ 230, 240, 255, 100 });
            }
            float flashCycle2 = fmodf(time * 2.3f + 0.5f, 1.0f);
            if (flashCycle2 < 0.06f) {
                float fx2 = 120.0f + fmodf(time * 78.9f, 220.0f);
                float fy2 = 110.0f + fmodf(time * 34.5f, 40.0f);
                DrawCircle((int)fx2, (int)fy2, 6, Color{ 255, 255, 240, 200 });
                DrawCircle((int)fx2, (int)fy2, 14, Color{ 255, 250, 210, 80 });
            }
            break;
        }
        case STAGE_PENTAGON_ALTAR: {

            float flamePulse = sinf(time * 8.0f) * 4.0f;
            DrawCircle(40, 110, (int)(10 + flamePulse), Color{ 255, 100, 30, 110 });
            DrawCircle(SCREEN_WIDTH - 40, 110, (int)(10 + flamePulse), Color{ 255, 100, 30, 110 });

            if (fmodf(time, 2.0f) < 1.0f) {
                DrawTriangle(Vector2{ (float)SCREEN_WIDTH / 2.0f, 0.0f },
                             Vector2{ (float)SCREEN_WIDTH / 2.0f - 60.0f, (float)SCREEN_HEIGHT },
                             Vector2{ (float)SCREEN_WIDTH / 2.0f + 60.0f, (float)SCREEN_HEIGHT },
                             Color{ 220, 40, 60, 20 });
            }
            break;
        }
        case STAGE_WTC_PLAZA: {

            if (fmodf(time * 1.8f, 1.0f) < 0.12f) {
                float glintX = (float)(SCREEN_WIDTH / 2) + 20.0f;
                DrawCircle((int)glintX, 15, 3, WHITE);
                DrawCircle((int)glintX, 15, 6, Color{ 255, 240, 180, 120 });
            }

            if (fmodf(time * 2.5f + 0.3f, 1.0f) < 0.15f) {
                DrawCircle(48, 55, 2, Color{ 255, 230, 80, 220 });
            }
            break;
        }
        case STAGE_COMET_PING_PONG: {

            if (fmodf(time * 7.5f, 1.0f) < 0.05f) {
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 20, 20, 30, 40 });
            }

            float pizzaPulse = 0.6f + 0.4f * sinf(time * 5.0f);
            DrawCircle(SCREEN_WIDTH - 25, 25, 12, Color{ 255, 60, 40, (unsigned char)(60 * pizzaPulse) });
            break;
        }
        case STAGE_FLORIDA_KEYS: {

            float spireGlint = sinf(time * 3.0f);
            if (spireGlint > 0.8f) {
                float sx = (float)(SCREEN_WIDTH / 2) + 5.0f;
                DrawCircle((int)sx, 38, 4, Color{ 255, 245, 160, 220 });
                DrawCircle((int)sx, 38, 8, Color{ 255, 215, 0, 90 });
            }

            float waveTick = fmodf(time * 1.5f, 1.0f);
            float sparkX = 40.0f + waveTick * 180.0f;
            DrawRectangle((int)sparkX, (int)GROUND_Y - 4, 8, 1, Color{ 140, 240, 255, 180 });
            break;
        }
        default:
            break;
        }
        return;
    }

    float farOffset = -camX * 0.15f;
    const auto& v = m_venues[m_stageId];

    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, 140, v.skyTop, v.skyBottom);

    float cloudX = fmodf(time * 8.0f + farOffset * 0.5f, (float)SCREEN_WIDTH + 100.0f) - 50.0f;
    Color cloudCol = Color{ (unsigned char)(v.skyBottom.r + 30), (unsigned char)(v.skyBottom.g + 25), (unsigned char)(v.skyBottom.b + 35), 180 };
    DrawCircle((int)cloudX, 35, 24, cloudCol);
    DrawCircle((int)cloudX + 22, 30, 32, cloudCol);
    DrawCircle((int)cloudX + 50, 36, 26, cloudCol);

    float cloud2X = fmodf(time * 5.0f + farOffset * 0.3f + 180.0f, (float)SCREEN_WIDTH + 120.0f) - 60.0f;
    DrawCircle((int)cloud2X, 55, 30, cloudCol);
    DrawCircle((int)cloud2X + 28, 50, 38, cloudCol);
    DrawCircle((int)cloud2X + 60, 54, 28, cloudCol);

    switch (m_stageId) {
    case STAGE_DEMOLISHED_EAST_WING: {

        float capX = 280.0f + farOffset;
        DrawRectangle((int)capX - 25, 75, 50, 45, Color{ 35, 30, 45, 255 });
        DrawCircle((int)capX, 75, 22, Color{ 35, 30, 45, 255 });
        DrawRectangle((int)capX - 4, 45, 8, 14, Color{ 35, 30, 45, 255 });

        if (fmodf(time, 4.5f) < 0.08f) {
            DrawRectangle(0, 0, SCREEN_WIDTH, 140, Color{ 180, 170, 210, 80 });
        }
        break;
    }
    case STAGE_REFLECTING_POOL: {

        float monX = 192.0f + farOffset;
        Vector2 v1{ monX, 10.0f };
        Vector2 v2{ monX - 12.0f, 130.0f };
        Vector2 v3{ monX + 12.0f, 130.0f };
        DrawTriangle(v1, v2, v3, Color{ 140, 160, 190, 255 });
        DrawRectangle((int)monX - 12, 130, 24, 20, Color{ 120, 140, 170, 255 });

        if (fmodf(time, 1.2f) < 0.6f) {
            DrawCircle((int)monX, 10, 2, Color{ 255, 40, 40, 255 });
        }
        break;
    }
    case STAGE_CONGRESSIONAL_FLOOR: {

        for (int i = 0; i < 7; i++) {
            float archX = (float)(i * 65) + fmodf(farOffset, 65.0f);
            DrawRectangleLines((int)archX, 20, 55, 80, Color{ 90, 45, 30, 255 });
            DrawCircleLines((int)archX + 27, 20, 27, Color{ 90, 45, 30, 255 });
        }

        float chanX = 192.0f + farOffset + sinf(time * 1.5f) * 4.0f;
        DrawLine((int)chanX, 0, (int)chanX, 35, Color{ 200, 160, 50, 255 });
        DrawCircle((int)chanX, 38, 12, Color{ 220, 180, 60, 255 });
        DrawCircle((int)chanX, 38, 6, Color{ 255, 240, 180, 255 });
        break;
    }
    case STAGE_WHITE_HOUSE_MMA: {

        float whX = 140.0f + farOffset;
        DrawRectangle((int)whX, 50, 120, 65, Color{ 210, 215, 220, 255 });

        for (int c = 0; c < 5; c++) {
            DrawRectangle((int)whX + 10 + c * 24, 58, 6, 55, Color{ 175, 185, 195, 255 });
        }

        DrawTriangle(Vector2{ whX + 60, 32 }, Vector2{ whX + 5, 50 }, Vector2{ whX + 115, 50 }, Color{ 230, 235, 240, 255 });

        float heliX = 300.0f + farOffset;
        DrawRectangle((int)heliX, 90, 55, 20, Color{ 35, 55, 35, 255 });
        DrawRectangle((int)heliX + 45, 84, 25, 10, Color{ 35, 55, 35, 255 });

        float rotorSweep = sinf(time * 35.0f) * 42.0f;
        DrawLine((int)heliX + 25 - (int)rotorSweep, 82, (int)heliX + 25 + (int)rotorSweep, 82, Color{ 120, 140, 120, 180 });
        break;
    }
    default:
        break;
    }
}

void StageManager::DrawMidLayer(float camX, float time) {
    if (m_texturesLoaded && m_stageTextures[m_stageId].id > 0) {

        switch (m_stageId) {
        case STAGE_DEMOLISHED_EAST_WING: {

            float midOffset = -camX * 0.50f;
            for (int i = 0; i < 4; i++) {
                float spotX = i * 140.0f + 60.0f + fmodf(midOffset, 140.0f);
                float flicker = (fmodf(time * 15.0f + i * 2.7f, 1.0f) > 0.09f) ? 0.22f : 0.06f;
                DrawTriangle(Vector2{ spotX, 155 }, Vector2{ spotX - 32, 60 }, Vector2{ spotX + 32, 60 },
                             Color{ 255, 240, 160, (unsigned char)(flicker * 255) });
            }
            break;
        }
        case STAGE_CONGRESSIONAL_FLOOR: {

            float midOffset = -camX * 0.40f;
            for (int i = 0; i < 10; i++) {
                float npcX = i * 42.0f + fmodf(midOffset, 42.0f) + 10.0f;
                bool standing = ((i + (int)(time * 2.5f)) % 3) == 0;
                if (standing) {
                    float armY = sinf(time * 14.0f + i) * 4.0f;
                    DrawLine((int)npcX + 6, 85, (int)npcX + 10, 80 + (int)armY, Color{ 235, 190, 150, 200 });
                    if (i % 2 == 0) {
                        DrawRectangle((int)npcX + 9, 78 + (int)armY, 4, 3, Color{ 200, 150, 60, 255 });
                    }
                }
            }
            break;
        }
        case STAGE_WHITE_HOUSE_MMA: {

            float midOffset = -camX * 0.50f;
            for (int i = 0; i < 12; i++) {
                float crowdX = i * 35.0f + fmodf(midOffset, 35.0f);
                if (fmodf(time * 9.0f + i * 1.9f, 2.0f) < 0.05f) {
                    DrawCircle((int)crowdX, 120, 8, Color{ 255, 255, 255, 210 });
                    DrawCircle((int)crowdX, 120, 16, Color{ 200, 230, 255, 80 });
                }
            }
            break;
        }
        default:
            break;
        }
        return;
    }

    float midOffset = -camX * 0.50f;
    const auto& v = m_venues[m_stageId];

    switch (m_stageId) {
    case STAGE_DEMOLISHED_EAST_WING: {

        for (int i = -1; i < 7; i++) {
            float colX = i * 130.0f + midOffset;

            DrawRectangle((int)colX, 85, 26, 80, v.midArchColor);
            DrawRectangle((int)colX - 4, 80, 34, 8, Color{ 120, 115, 125, 255 });

            DrawTriangle(Vector2{ colX, 85 }, Vector2{ colX + 13, 72 }, Vector2{ colX + 26, 85 }, v.midArchColor);

            DrawLine((int)colX + 8, 72, (int)colX + 5, 58, Color{ 160, 70, 50, 255 });
            DrawLine((int)colX + 18, 72, (int)colX + 22, 54, Color{ 160, 70, 50, 255 });

            float spotX = colX + 55.0f;
            DrawRectangle((int)spotX, 150, 14, 12, Color{ 220, 160, 20, 255 });
            float flicker = (fmodf(time * 14.0f + i * 3.0f, 1.0f) > 0.08f) ? 0.35f : 0.1f;
            DrawTriangle(Vector2{ spotX + 7, 150 }, Vector2{ spotX - 35, 40 }, Vector2{ spotX + 49, 40 }, Color{ 255, 240, 150, (unsigned char)(flicker * 255) });
        }
        break;
    }
    case STAGE_REFLECTING_POOL: {

        for (int i = -1; i < 9; i++) {
            float treeX = i * 95.0f + midOffset;

            DrawRectangle((int)treeX + 15, 110, 8, 48, Color{ 30, 20, 25, 255 });

            DrawCircle((int)treeX + 19, 100, 24, Color{ 140, 50, 90, 200 });
            DrawCircle((int)treeX + 32, 95, 18, Color{ 180, 80, 120, 220 });

            float lampX = treeX + 60.0f;
            DrawRectangle((int)lampX, 115, 4, 42, Color{ 20, 25, 30, 255 });
            DrawCircle((int)lampX + 2, 112, 6, Color{ 255, 240, 160, 230 });
        }
        break;
    }
    case STAGE_CONGRESSIONAL_FLOOR: {

        DrawRectangle(0, 100, SCREEN_WIDTH, 45, v.midArchColor);
        DrawRectangle(0, 100, SCREEN_WIDTH, 5, Color{ 140, 75, 45, 255 });

        for (int i = -1; i < 14; i++) {
            float npcX = i * 55.0f + midOffset;
            bool standing = ((i + (int)(time * 3.0f)) % 4) == 0;
            int npcY = standing ? 82 : 88;

            Color suitCol = (i % 2 == 0) ? Color{ 40, 45, 60, 255 } : Color{ 55, 35, 40, 255 };
            DrawRectangle((int)npcX, npcY + 8, 14, 18, suitCol);

            DrawCircle((int)npcX + 7, npcY + 4, 6, Color{ 235, 190, 150, 255 });

            if (standing) {
                float armY = sinf(time * 12.0f + i) * 6.0f;
                DrawLine((int)npcX + 14, npcY + 10, (int)npcX + 20, npcY + 4 + (int)armY, Color{ 235, 190, 150, 255 });
                if (i % 3 == 0) {

                    DrawRectangle((int)npcX + 19, npcY + 1 + (int)armY, 6, 4, Color{ 180, 120, 40, 255 });
                }
            }
        }
        break;
    }
    case STAGE_WHITE_HOUSE_MMA: {

        DrawRectangle(0, 125, SCREEN_WIDTH, 35, Color{ 20, 30, 25, 255 });
        for (int i = -1; i < 18; i++) {
            float crowdX = i * 42.0f + midOffset;
            DrawCircle((int)crowdX, 125, 7, Color{ 50, 60, 55, 255 });

            if (fmodf(time * 8.0f + i * 1.7f, 2.0f) < 0.06f) {
                DrawCircle((int)crowdX, 118, 12, Color{ 255, 255, 255, 220 });
                DrawCircle((int)crowdX, 118, 20, Color{ 200, 230, 255, 90 });
            }
        }
        break;
    }
    default:
        break;
    }
}

void StageManager::DrawPlayfield(float camX, float time) {
    if (m_texturesLoaded && m_stageTextures[m_stageId].id > 0) {

        switch (m_stageId) {
        case STAGE_DEMOLISHED_EAST_WING: {

            float tapeY = GROUND_Y - 45.0f;
            float playOffset = -camX;
            for (int i = -2; i < 18; i++) {
                float tx = i * 28.0f + fmodf(playOffset, 28.0f);
                float sag = sinf(tx * 0.05f + time * 3.5f) * 3.0f;
                Color stripeCol = (i % 2 == 0) ? Color{ 240, 200, 30, 220 } : Color{ 20, 20, 20, 220 };
                DrawRectangle((int)tx, (int)(tapeY + sag), 28, 5, stripeCol);
            }
            break;
        }
        case STAGE_REFLECTING_POOL: {

            for (int y = (int)GROUND_Y + 12; y < SCREEN_HEIGHT; y += 5) {
                float wave = sinf(y * 0.3f + time * 5.0f) * 6.0f;
                for (int k = 0; k < 6; k++) {
                    float rx = k * 65.0f + wave + fmodf(-camX, 65.0f);
                    DrawRectangle((int)rx, y, 16, 1, Color{ 140, 230, 255, 140 });
                }
            }
            break;
        }
        case STAGE_WHITE_HOUSE_MMA: {

            float playOffset = -camX;
            DrawRectangle((int)(STAGE_MIN_X + playOffset - 10), (int)GROUND_Y - 70, 14, 70, Color{ 210, 30, 30, 255 });
            DrawRectangle((int)(STAGE_MAX_X + playOffset - 6), (int)GROUND_Y - 70, 14, 70, Color{ 30, 50, 190, 255 });
            break;
        }
        default:
            break;
        }
        return;
    }

    float playOffset = -camX;
    const auto& v = m_venues[m_stageId];

    DrawRectangle(0, (int)GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - (int)GROUND_Y, v.floorColor);

    switch (m_stageId) {
    case STAGE_DEMOLISHED_EAST_WING: {

        for (int i = 0; i < 20; i++) {
            float tileX = i * 45.0f + fmodf(playOffset, 45.0f);
            DrawLine((int)tileX, (int)GROUND_Y, (int)tileX - 15, SCREEN_HEIGHT, Color{ 35, 32, 40, 255 });
        }
        DrawLine(0, (int)GROUND_Y + 12, SCREEN_WIDTH, (int)GROUND_Y + 12, Color{ 35, 32, 40, 255 });

        float tapeY = GROUND_Y - 45.0f;
        for (int i = 0; i < 30; i++) {
            float tx = i * 28.0f + playOffset;
            float sag = sinf(tx * 0.05f + time * 3.0f) * 3.0f;
            Color stripeCol = (i % 2 == 0) ? Color{ 240, 200, 30, 240 } : Color{ 20, 20, 20, 240 };
            DrawRectangle((int)tx, (int)(tapeY + sag), 28, 6, stripeCol);
        }
        break;
    }
    case STAGE_REFLECTING_POOL: {

        for (int y = (int)GROUND_Y; y < SCREEN_HEIGHT; y += 4) {
            float wave = sinf(y * 0.25f + time * 6.0f) * 6.0f;
            Color waterShade = Color{
                (unsigned char)(15 + (y - GROUND_Y) * 2),
                (unsigned char)(80 + (y - GROUND_Y) * 4),
                (unsigned char)(180 + (y - GROUND_Y) * 2),
                255
            };
            DrawRectangle(0, y, SCREEN_WIDTH, 4, waterShade);

            for (int k = 0; k < 6; k++) {
                float rx = k * 70.0f + wave + fmodf(playOffset, 70.0f);
                DrawRectangle((int)rx, y, 18, 1, Color{ 140, 230, 255, 180 });
            }
        }
        break;
    }
    case STAGE_CONGRESSIONAL_FLOOR: {

        for (int i = 0; i < 18; i++) {
            float plankX = i * 35.0f + fmodf(playOffset, 35.0f);
            DrawLine((int)plankX, (int)GROUND_Y, (int)plankX - 25, SCREEN_HEIGHT, Color{ 45, 20, 12, 255 });
        }

        float sealX = 400.0f + playOffset;
        DrawCircle((int)sealX, (int)GROUND_Y + 14, 22, Color{ 180, 140, 40, 255 });
        DrawCircle((int)sealX, (int)GROUND_Y + 14, 18, Color{ 90, 45, 25, 255 });
        DrawCircle((int)sealX, (int)GROUND_Y + 14, 12, Color{ 220, 180, 50, 255 });
        break;
    }
    case STAGE_WHITE_HOUSE_MMA: {

        for (int i = 0; i < 16; i++) {
            float matX = i * 50.0f + fmodf(playOffset, 50.0f);
            DrawLine((int)matX, (int)GROUND_Y, (int)matX - 10, SCREEN_HEIGHT, Color{ 115, 115, 120, 255 });
        }

        float ringCenterX = 400.0f + playOffset;
        DrawCircle((int)ringCenterX, (int)GROUND_Y + 14, 26, Color{ 30, 40, 60, 255 });
        DrawCircle((int)ringCenterX, (int)GROUND_Y + 14, 22, Color{ 180, 30, 30, 255 });
        DrawCircle((int)ringCenterX, (int)GROUND_Y + 14, 16, Color{ 230, 230, 240, 255 });

        DrawRectangle((int)(STAGE_MIN_X + playOffset - 10), (int)GROUND_Y - 70, 16, 70, Color{ 210, 30, 30, 255 });
        DrawRectangle((int)(STAGE_MAX_X + playOffset - 6), (int)GROUND_Y - 70, 16, 70, Color{ 30, 50, 190, 255 });
        break;
    }
    case STAGE_PRESS_BRIEFING: {

        DrawRectangle(0, (int)GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - (int)GROUND_Y, Color{ 25, 40, 85, 255 });
        for (int i = 0; i < 18; i++) {
            float plankX = i * 40.0f + fmodf(playOffset, 40.0f);
            DrawLine((int)plankX, (int)GROUND_Y, (int)plankX - 10, SCREEN_HEIGHT, Color{ 15, 25, 60, 255 });
        }
        break;
    }
    case STAGE_PENTAGON_ALTAR: {

        DrawRectangle(0, (int)GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - (int)GROUND_Y, Color{ 30, 25, 32, 255 });
        for (int i = 0; i < 16; i++) {
            float tileX = i * 50.0f + fmodf(playOffset, 50.0f);
            DrawLine((int)tileX, (int)GROUND_Y, (int)tileX - 15, SCREEN_HEIGHT, Color{ 55, 45, 55, 255 });
        }

        float starX = 400.0f + playOffset;
        DrawCircle((int)starX, (int)GROUND_Y + 14, 20, Color{ 160, 130, 30, 255 });
        DrawCircle((int)starX, (int)GROUND_Y + 14, 14, Color{ 30, 25, 32, 255 });
        break;
    }
    default:
        break;
    }
}

void StageManager::DrawForeground(float camX, float time) {
    if (m_stageId == STAGE_WHITE_HOUSE_MMA) {

        float fenceOffset = -camX * 1.05f;
        Color wireCol = Color{ 40, 45, 50, 40 };
        for (int x = -20; x < SCREEN_WIDTH + 40; x += 18) {
            float fx = (float)x + fmodf(fenceOffset, 18.0f);
            DrawLine((int)fx, 125, (int)fx + 22, (int)GROUND_Y - 4, wireCol);
            DrawLine((int)fx + 22, 125, (int)fx, (int)GROUND_Y - 4, wireCol);
        }
    }
}

// --- Visuals.cpp ---
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>

bool VisualsEngine::s_loaded = false;
Texture2D VisualsEngine::s_titleSplash = { 0 };
Texture2D VisualsEngine::s_portraits[CHARACTER_COUNT] = { 0 };
Texture2D VisualsEngine::s_portraitsLose[CHARACTER_COUNT] = { 0 };
Texture2D VisualsEngine::s_sprites[CHARACTER_COUNT][6] = { 0 };
Texture2D VisualsEngine::s_teslaSprites[4] = { 0 };
Texture2D VisualsEngine::s_bonusStageBg = { 0 };
Texture2D VisualsEngine::s_mapEastCoast = { 0 };
Texture2D VisualsEngine::s_endingSavior = { 0 };
Texture2D VisualsEngine::s_sprBillaryHillary = { 0 };
Texture2D VisualsEngine::s_spritesHillary[6] = { 0 };
Texture2D VisualsEngine::s_sprHighBenchRoberts = { 0 };
Texture2D VisualsEngine::s_sprHighBenchRobertsSpec = { 0 };
Texture2D VisualsEngine::s_sprHighBenchSotomayor = { 0 };
Texture2D VisualsEngine::s_sprHighBenchSotomayorSpec = { 0 };
Texture2D VisualsEngine::s_portraitRoberts = { 0 };
Texture2D VisualsEngine::s_portraitSotomayor = { 0 };
Texture2D VisualsEngine::s_sprSleepyJoeSS = { 0 };
Texture2D VisualsEngine::s_endingCthulhuThrow = { 0 };
Texture2D VisualsEngine::s_endingLaserShark = { 0 };
Texture2D VisualsEngine::s_endingCthulhuNap = { 0 };

void VisualsEngine::Init() {
    if (s_loaded) return;

    AssetManager::Init();

    std::string baseDir = "assets/";
    if (!AssetManager::HasEmbeddedAssets()) {
        if (DirectoryExists("../assets/")) {
            baseDir = "../assets/";
        } else if (DirectoryExists("assets/")) {
            baseDir = "assets/";
        } else {
            std::string appParentAssets = std::string(GetApplicationDirectory()) + "../assets/";
            std::string appDirAssets = std::string(GetApplicationDirectory()) + "assets/";
            if (DirectoryExists(appParentAssets.c_str())) {
                baseDir = appParentAssets;
            } else if (DirectoryExists(appDirAssets.c_str())) {
                baseDir = appDirAssets;
            }
        }
    }

    std::string splashPath = baseDir + "title_splash.png";
    if (AssetManager::FileExists(splashPath.c_str())) {
        s_titleSplash = AssetManager::LoadTexture(splashPath.c_str());
        SetTextureFilter(s_titleSplash, TEXTURE_FILTER_POINT);
    }

    for (int i = 0; i < CHARACTER_COUNT; i++) {
        char path[128];
        snprintf(path, sizeof(path), "%sportrait_%d.png", baseDir.c_str(), i);
        if (AssetManager::FileExists(path)) {
            s_portraits[i] = AssetManager::LoadTexture(path);
            SetTextureFilter(s_portraits[i], TEXTURE_FILTER_POINT);
        }
        snprintf(path, sizeof(path), "%sportrait_lose_%d.png", baseDir.c_str(), i);
        if (AssetManager::FileExists(path)) {
            s_portraitsLose[i] = AssetManager::LoadTexture(path);
            SetTextureFilter(s_portraitsLose[i], TEXTURE_FILTER_POINT);
        } else {
            snprintf(path, sizeof(path), "%sportrait_beatup_%d.png", baseDir.c_str(), i);
            if (AssetManager::FileExists(path)) {
                s_portraitsLose[i] = AssetManager::LoadTexture(path);
                SetTextureFilter(s_portraitsLose[i], TEXTURE_FILTER_POINT);
            }
        }
    }

    const char* animNames[6] = { "idle", "lp", "hp", "lk", "hk", "spec" };
    for (int i = 0; i < CHARACTER_COUNT; i++) {
        for (int a = 0; a < 6; a++) {
            char path[128];
            snprintf(path, sizeof(path), "%sspr_%d_%s.png", baseDir.c_str(), i, animNames[a]);
            if (AssetManager::FileExists(path)) {
                s_sprites[i][a] = AssetManager::LoadTexture(path);
                SetTextureFilter(s_sprites[i][a], TEXTURE_FILTER_POINT);
            } else {

                const char* fallback = nullptr;
                if (a == 1 || a == 3) fallback = "atk1";
                else if (a == 2 || a == 4) fallback = "atk2";
                else if (a == 5) fallback = "spec";
                if (fallback) {
                    snprintf(path, sizeof(path), "%sspr_%d_%s.png", baseDir.c_str(), i, fallback);
                    if (AssetManager::FileExists(path)) {
                        s_sprites[i][a] = AssetManager::LoadTexture(path);
                        SetTextureFilter(s_sprites[i][a], TEXTURE_FILTER_POINT);
                    }
                }
            }
        }
    }

    const char* hilAnimNames[6] = { "idle", "lp", "hp", "lk", "hk", "spec" };
    for (int a = 0; a < 6; a++) {
        char path[128];
        snprintf(path, sizeof(path), "%sspr_13_hil_%s.png", baseDir.c_str(), hilAnimNames[a]);
        if (AssetManager::FileExists(path)) {
            s_spritesHillary[a] = AssetManager::LoadTexture(path);
            SetTextureFilter(s_spritesHillary[a], TEXTURE_FILTER_POINT);
        }
    }
    std::string hilPath = baseDir + "spr_13_idle_hillary.png";
    if (!AssetManager::FileExists(hilPath.c_str())) hilPath = baseDir + "spr_11_idle_hillary.png";
    if (AssetManager::FileExists(hilPath.c_str())) {
        s_sprBillaryHillary = AssetManager::LoadTexture(hilPath.c_str());
        SetTextureFilter(s_sprBillaryHillary, TEXTURE_FILTER_POINT);
    }
    if (s_spritesHillary[0].id == 0 && s_sprBillaryHillary.id > 0) {
        s_spritesHillary[0] = s_sprBillaryHillary;
    }

    std::string robPortPath = baseDir + "portrait_1_roberts.png";
    if (AssetManager::FileExists(robPortPath.c_str())) {
        s_portraitRoberts = AssetManager::LoadTexture(robPortPath.c_str());
        SetTextureFilter(s_portraitRoberts, TEXTURE_FILTER_POINT);
    }
    std::string sotoPortPath = baseDir + "portrait_1_sotomayor.png";
    if (AssetManager::FileExists(sotoPortPath.c_str())) {
        s_portraitSotomayor = AssetManager::LoadTexture(sotoPortPath.c_str());
        SetTextureFilter(s_portraitSotomayor, TEXTURE_FILTER_POINT);
    }
    std::string robIdlePath = baseDir + "spr_1_roberts_idle.png";
    if (AssetManager::FileExists(robIdlePath.c_str())) {
        s_sprHighBenchRoberts = AssetManager::LoadTexture(robIdlePath.c_str());
        SetTextureFilter(s_sprHighBenchRoberts, TEXTURE_FILTER_POINT);
    }
    std::string robSpecPath = baseDir + "spr_1_roberts_spec.png";
    if (AssetManager::FileExists(robSpecPath.c_str())) {
        s_sprHighBenchRobertsSpec = AssetManager::LoadTexture(robSpecPath.c_str());
        SetTextureFilter(s_sprHighBenchRobertsSpec, TEXTURE_FILTER_POINT);
    }
    std::string sotoIdlePath = baseDir + "spr_1_sotomayor_idle.png";
    if (AssetManager::FileExists(sotoIdlePath.c_str())) {
        s_sprHighBenchSotomayor = AssetManager::LoadTexture(sotoIdlePath.c_str());
        SetTextureFilter(s_sprHighBenchSotomayor, TEXTURE_FILTER_POINT);
    }
    std::string sotoSpecPath = baseDir + "spr_1_sotomayor_spec.png";
    if (AssetManager::FileExists(sotoSpecPath.c_str())) {
        s_sprHighBenchSotomayorSpec = AssetManager::LoadTexture(sotoSpecPath.c_str());
        SetTextureFilter(s_sprHighBenchSotomayorSpec, TEXTURE_FILTER_POINT);
    }

    std::string ssPath = baseDir + "spr_8_secret_service.png";
    if (AssetManager::FileExists(ssPath.c_str())) {
        s_sprSleepyJoeSS = AssetManager::LoadTexture(ssPath.c_str());
        SetTextureFilter(s_sprSleepyJoeSS, TEXTURE_FILTER_POINT);
    }

    std::string cthulhuThrowPath = baseDir + "ending_cthulhu_throw.png";
    if (AssetManager::FileExists(cthulhuThrowPath.c_str())) {
        s_endingCthulhuThrow = AssetManager::LoadTexture(cthulhuThrowPath.c_str());
        SetTextureFilter(s_endingCthulhuThrow, TEXTURE_FILTER_POINT);
    }
    std::string laserSharkPath = baseDir + "ending_laser_shark.png";
    if (AssetManager::FileExists(laserSharkPath.c_str())) {
        s_endingLaserShark = AssetManager::LoadTexture(laserSharkPath.c_str());
        SetTextureFilter(s_endingLaserShark, TEXTURE_FILTER_POINT);
    }
    std::string cthulhuNapPath = baseDir + "ending_cthulhu_nap.png";
    if (AssetManager::FileExists(cthulhuNapPath.c_str())) {
        s_endingCthulhuNap = AssetManager::LoadTexture(cthulhuNapPath.c_str());
        SetTextureFilter(s_endingCthulhuNap, TEXTURE_FILTER_POINT);
    }

    for (int i = 0; i < 4; i++) {
        char path[128];
        snprintf(path, sizeof(path), "%stesla_%d.png", baseDir.c_str(), i);
        if (AssetManager::FileExists(path)) {
            s_teslaSprites[i] = AssetManager::LoadTexture(path);
            SetTextureFilter(s_teslaSprites[i], TEXTURE_FILTER_POINT);
        }
    }

    std::string bonusBgPath = baseDir + "stage_bonus_tesla.png";
    if (AssetManager::FileExists(bonusBgPath.c_str())) {
        s_bonusStageBg = AssetManager::LoadTexture(bonusBgPath.c_str());
        SetTextureFilter(s_bonusStageBg, TEXTURE_FILTER_POINT);
    }

    std::string mapPath = baseDir + "map_east_coast.png";
    if (AssetManager::FileExists(mapPath.c_str())) {
        s_mapEastCoast = AssetManager::LoadTexture(mapPath.c_str());
        SetTextureFilter(s_mapEastCoast, TEXTURE_FILTER_POINT);
    }

    std::string endingPath = baseDir + "ending_savior.png";
    if (AssetManager::FileExists(endingPath.c_str())) {
        s_endingSavior = AssetManager::LoadTexture(endingPath.c_str());
        SetTextureFilter(s_endingSavior, TEXTURE_FILTER_POINT);
    }

    s_loaded = true;
}

void VisualsEngine::Shutdown() {
    if (!s_loaded) return;

    if (s_titleSplash.id > 0) {
        UnloadTexture(s_titleSplash);
        s_titleSplash = { 0 };
    }

    for (int i = 0; i < CHARACTER_COUNT; i++) {
        if (s_portraits[i].id > 0) {
            UnloadTexture(s_portraits[i]);
            s_portraits[i] = { 0 };
        }
        if (s_portraitsLose[i].id > 0) {
            UnloadTexture(s_portraitsLose[i]);
            s_portraitsLose[i] = { 0 };
        }
        for (int a = 0; a < 6; a++) {
            if (s_sprites[i][a].id > 0) {
                UnloadTexture(s_sprites[i][a]);
                s_sprites[i][a] = { 0 };
            }
        }
    }

    if (s_sprBillaryHillary.id > 0) {
        UnloadTexture(s_sprBillaryHillary);
        s_sprBillaryHillary = { 0 };
    }
    for (int a = 0; a < 6; a++) {
        if (s_spritesHillary[a].id > 0) {
            UnloadTexture(s_spritesHillary[a]);
            s_spritesHillary[a] = { 0 };
        }
    }

    for (int i = 0; i < 4; i++) {
        if (s_teslaSprites[i].id > 0) {
            UnloadTexture(s_teslaSprites[i]);
            s_teslaSprites[i] = { 0 };
        }
    }

    if (s_bonusStageBg.id > 0) {
        UnloadTexture(s_bonusStageBg);
        s_bonusStageBg = { 0 };
    }

    if (s_mapEastCoast.id > 0) {
        UnloadTexture(s_mapEastCoast);
        s_mapEastCoast = { 0 };
    }

    if (s_endingSavior.id > 0) {
        UnloadTexture(s_endingSavior);
        s_endingSavior = { 0 };
    }

    if (s_portraitRoberts.id > 0) { UnloadTexture(s_portraitRoberts); s_portraitRoberts = { 0 }; }
    if (s_portraitSotomayor.id > 0) { UnloadTexture(s_portraitSotomayor); s_portraitSotomayor = { 0 }; }
    if (s_sprHighBenchRoberts.id > 0) { UnloadTexture(s_sprHighBenchRoberts); s_sprHighBenchRoberts = { 0 }; }
    if (s_sprHighBenchRobertsSpec.id > 0) { UnloadTexture(s_sprHighBenchRobertsSpec); s_sprHighBenchRobertsSpec = { 0 }; }
    if (s_sprHighBenchSotomayor.id > 0) { UnloadTexture(s_sprHighBenchSotomayor); s_sprHighBenchSotomayor = { 0 }; }
    if (s_sprHighBenchSotomayorSpec.id > 0) { UnloadTexture(s_sprHighBenchSotomayorSpec); s_sprHighBenchSotomayorSpec = { 0 }; }
    if (s_sprSleepyJoeSS.id > 0) { UnloadTexture(s_sprSleepyJoeSS); s_sprSleepyJoeSS = { 0 }; }
    if (s_endingCthulhuThrow.id > 0) { UnloadTexture(s_endingCthulhuThrow); s_endingCthulhuThrow = { 0 }; }
    if (s_endingLaserShark.id > 0) { UnloadTexture(s_endingLaserShark); s_endingLaserShark = { 0 }; }
    if (s_endingCthulhuNap.id > 0) { UnloadTexture(s_endingCthulhuNap); s_endingCthulhuNap = { 0 }; }

    s_loaded = false;
}

Texture2D VisualsEngine::CreateProceduralFlagTexture() {
    constexpr int W = 120;
    constexpr int H = 70;
    Image img = GenImageColor(W, H, WHITE);

    float stripeH = (float)H / 13.0f;
    for (int i = 0; i < 13; i++) {
        Color col = (i % 2 == 0) ? Color{ 190, 25, 35, 255 } : Color{ 245, 245, 245, 255 };
        ImageDrawRectangle(&img, 0, (int)(i * stripeH), W, (int)(stripeH + 1), col);
    }

    int cantonW = 48;
    int cantonH = (int)(7 * stripeH);
    ImageDrawRectangle(&img, 0, 0, cantonW, cantonH, Color{ 25, 40, 110, 255 });

    for (int r = 0; r < 5; r++) {
        for (int c = 0; c < 6; c++) {
            int sx = 4 + c * 7;
            int sy = 4 + r * 6;
            ImageDrawPixel(&img, sx, sy, WHITE);
            ImageDrawPixel(&img, sx + 1, sy, WHITE);
            ImageDrawPixel(&img, sx, sy + 1, WHITE);
        }
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

void VisualsEngine::DrawPixelText(const char* text, int x, int y, int size, Color col) {

    DrawText(text, x + 1, y + 1, size, Color{ 10, 10, 15, 220 });
    DrawText(text, x, y, size, col);
}

void VisualsEngine::DrawDropShadow(const Fighter& f) {
    float dist = GROUND_Y - f.GetPosition().y;
    float w = std::max(6.0f, 26.0f - dist * 0.16f);
    float h = std::max(2.0f, 6.0f - dist * 0.04f);
    unsigned char alpha = (unsigned char)std::clamp(170 - (int)(dist * 1.4f), 30, 170);

    DrawEllipse((int)f.GetPosition().x, (int)GROUND_Y + 1, (int)w, (int)h, Color{ 10, 15, 20, alpha });
}

static void DrawCosmeticPuffs(const Fighter& f) {
    for (const auto& puff : f.GetCosmeticPuffs()) {
        float prog = std::clamp(puff.timer / puff.maxTimer, 0.0f, 1.0f);
        float r = puff.baseSize * (1.0f + prog * 1.8f);
        unsigned char alpha = (unsigned char)((1.0f - prog) * 230.0f);
        Color colPuff = Color{ 185, 225, 75, alpha };
        Color colInner = Color{ 235, 250, 160, (unsigned char)(alpha * 0.9f) };
        Color colLine = Color{ 110, 155, 40, (unsigned char)(alpha * 0.9f) };

        int cx = (int)puff.pos.x;
        int cy = (int)puff.pos.y;
        DrawCircle(cx, cy, r, colPuff);
        DrawCircle(cx - (int)(r * 0.4f), cy - (int)(r * 0.4f), r * 0.75f, colPuff);
        DrawCircle(cx - (int)(r * 0.5f), cy + (int)(r * 0.3f), r * 0.65f, colPuff);
        DrawCircle(cx + (int)(r * 0.4f), cy - (int)(r * 0.2f), r * 0.60f, colPuff);

        DrawCircle(cx, cy, r * 0.55f, colInner);
        DrawCircleLines(cx, cy, r, colLine);
        DrawCircleLines(cx - (int)(r * 0.4f), cy - (int)(r * 0.4f), r * 0.75f, colLine);

        int lx = cx - (int)(r * 1.2f);
        DrawLine(lx, cy - 3, lx - 6, cy - 6, colLine);
        DrawLine(lx, cy + 2, lx - 7, cy + 4, colLine);
    }
}

void VisualsEngine::DrawFighterSprite(const Fighter& f) {
    int charId = (int)f.GetCharacterId();
    if (charId < 0 || charId >= CHARACTER_COUNT) charId = 0;

    Vector2 pos = f.GetPosition();
    bool fr = f.IsFacingRight();
    FighterState state = f.GetState();
    int frame = f.GetStateFrame();
    float dir = fr ? 1.0f : -1.0f;

    if (!ChampionshipEdition::s_forceProceduralRig && s_loaded && s_sprites[charId][0].id > 0) {
        int animIdx = 0;
        if (state == FSTATE_ATTACK) {
            AttackMoveType mtype = f.GetCurrentAttack().moveType;
            if (mtype == MOVE_LIGHT_PUNCH) animIdx = 1;
            else if (mtype == MOVE_HEAVY_PUNCH) animIdx = 2;
            else if (mtype == MOVE_LIGHT_KICK) animIdx = 3;
            else if (mtype == MOVE_HEAVY_KICK) animIdx = 4;
            else if (mtype == MOVE_SPECIAL) animIdx = 5;
            else animIdx = f.GetCurrentAttack().isHeavy ? 2 : 1;
        } else if (state == FSTATE_SPECIAL) {
            animIdx = 5;
        } else {
            animIdx = 0;
        }

        static const float s_charScale[CHARACTER_COUNT] = {
            88.0f / 258.0f,
            84.0f / 200.0f,
            80.0f / 228.0f,
            82.0f / 250.0f,
            80.0f / 253.0f,
            78.0f / 262.0f,
            82.0f / 305.0f,
            78.0f / 196.0f,
            86.0f / 265.0f,
            100.0f / 260.0f,
            88.0f / 264.0f,
            98.4f / 302.0f,
            84.0f / 272.0f,
            84.0f / 276.0f,
            86.0f / 295.0f
        };

        Texture2D tex = s_sprites[charId][animIdx];
        if (charId == CHAR_HIGH_BENCH) {
            HighBenchStage hbStage = f.GetHighBenchStage();
            if (hbStage == HB_STAGE_ROBERTS) {
                if (animIdx == 0 && s_sprHighBenchRoberts.id > 0) tex = s_sprHighBenchRoberts;
                else if (s_sprHighBenchRobertsSpec.id > 0) tex = s_sprHighBenchRobertsSpec;
                else if (s_sprHighBenchRoberts.id > 0) tex = s_sprHighBenchRoberts;
            } else if (hbStage == HB_STAGE_SOTOMAYOR) {
                if (animIdx == 0 && s_sprHighBenchSotomayor.id > 0) tex = s_sprHighBenchSotomayor;
                else if (s_sprHighBenchSotomayorSpec.id > 0) tex = s_sprHighBenchSotomayorSpec;
                else if (s_sprHighBenchSotomayor.id > 0) tex = s_sprHighBenchSotomayor;
            }
        }

        if (charId == CHAR_BILLARY) {
            if (!fr) {

                if (s_spritesHillary[animIdx].id > 0) tex = s_spritesHillary[animIdx];
                else if (animIdx == 0 && s_sprBillaryHillary.id > 0) tex = s_sprBillaryHillary;
            } else {

                if (s_sprites[CHAR_BILLARY][animIdx].id > 0) tex = s_sprites[CHAR_BILLARY][animIdx];
            }
        }

        if (tex.id == 0) {
            if (animIdx == 1 && s_sprites[charId][2].id > 0) tex = s_sprites[charId][2];
            else if (animIdx == 2 && s_sprites[charId][1].id > 0) tex = s_sprites[charId][1];
            else if (animIdx == 3 && s_sprites[charId][4].id > 0) tex = s_sprites[charId][4];
            else if (animIdx == 4 && s_sprites[charId][3].id > 0) tex = s_sprites[charId][3];
            else if (animIdx == 5 && s_sprites[charId][2].id > 0) tex = s_sprites[charId][2];
            else tex = s_sprites[charId][0];
        }

        constexpr float CANVAS_W = 440.0f;
        constexpr float CANVAS_H = 360.0f;
        constexpr float ANCHOR_X = 180.0f;
        constexpr float BASELINE_Y = 320.0f;

        float baseScale = s_charScale[charId];
        float drawW = CANVAS_W * baseScale;
        float drawH = CANVAS_H * baseScale;

        float posX = pos.x;
        float posY = pos.y;
        float rotation = 0.0f;

        if (charId == CHAR_SLEEPY_JOE && state == FSTATE_SPECIAL) {
            posY = GROUND_Y + 4.0f;
        }

        if (state == FSTATE_IDLE) {
            float breath = sinf(frame * 0.12f) * 1.5f;
            posY += breath;
        } else if (state == FSTATE_WALK_FWD || state == FSTATE_WALK_BACK) {
            float walkBob = fabsf(sinf(frame * 0.28f)) * 2.2f;
            posY -= walkBob;

            rotation = (state == FSTATE_WALK_FWD ? 2.5f : -2.5f) * (fr ? 1.0f : -1.0f);
        } else if (state == FSTATE_TAUNT) {
            rotation = fr ? -4.5f : 4.5f;
            posY -= 1.5f;
        } else if (state == FSTATE_HITSTUN) {
            posX += (fr ? -1.0f : 1.0f) * 4.0f;
        } else if (state == FSTATE_BLOCKSTUN) {
            posX += (fr ? -1.0f : 1.0f) * 2.0f;
        } else if (state == FSTATE_KNOCKDOWN_AIR) {
            rotation = fr ? -35.0f : 35.0f;
            posY -= 10.0f;
        } else if (state == FSTATE_KNOCKDOWN_GROUND || state == FSTATE_DEFEAT) {
            rotation = fr ? -90.0f : 90.0f;
            posY = GROUND_Y - 4.0f;
        } else if (state == FSTATE_WAKEUP) {
            float riseProg = std::clamp((float)frame / 24.0f, 0.0f, 1.0f);
            rotation = (fr ? -90.0f : 90.0f) * (1.0f - riseProg);
            posY = (GROUND_Y - 4.0f) * (1.0f - riseProg) + pos.y * riseProg;
        }

        float texW = (float)tex.width;
        float texH = (float)tex.height;
        Rectangle srcRec = { 0.0f, 0.0f, texW, texH };
        if (!fr) srcRec.width = -texW;

        Color tint = WHITE;
        if (f.IsInHitstop()) {
            if ((frame % 2) == 0) tint = Color{ 255, 255, 255, 255 };
        } else if (state == FSTATE_HITSTUN) {
            if (((frame / 2) % 2) == 0) tint = Color{ 255, 100, 100, 255 };
        } else if (state == FSTATE_BLOCK_HIGH || state == FSTATE_BLOCK_LOW || state == FSTATE_BLOCKSTUN) {
            tint = Color{ 170, 215, 255, 255 };
        }

        float crouchYScale = 1.0f;
        if (f.IsCrouching()) {
            crouchYScale = 0.85f;
            drawH *= crouchYScale;
        }

        float drawX = fr ? (posX - ANCHOR_X * baseScale) : (posX - (CANVAS_W - ANCHOR_X) * baseScale);
        float drawY = posY - BASELINE_Y * baseScale * crouchYScale;
        Rectangle dstRec = { drawX, drawY, drawW, drawH };
        Vector2 origin = { 0.0f, 0.0f };

        if (fabsf(rotation) > 0.1f) {
            float pivotX = fr ? (ANCHOR_X * baseScale) : ((CANVAS_W - ANCHOR_X) * baseScale);
            float pivotY = (BASELINE_Y - 45.0f) * baseScale * crouchYScale;
            origin = { pivotX, pivotY };
            dstRec.x = posX;
            dstRec.y = posY - 45.0f * baseScale;
        }

        DrawTexturePro(tex, srcRec, dstRec, origin, rotation, tint);

        if (state == FSTATE_ATTACK) {
            int startup = f.GetCurrentAttack().startupFrames;
            int active = f.GetCurrentAttack().activeFrames;
            if (frame >= startup && frame < startup + active) {
                AttackMoveType mtype = f.GetCurrentAttack().moveType;
                float reachX = f.GetCurrentAttack().reachX;
                float reachY = f.GetCurrentAttack().reachY;
                float fxX = posX + dir * reachX;
                float fxY = posY + reachY;

                if (mtype == MOVE_LIGHT_PUNCH) {

                    DrawLineEx(Vector2{ posX + dir * 18.0f, fxY - 2.0f }, Vector2{ fxX + dir * 8.0f, fxY - 2.0f }, 2.0f, Color{ 255, 255, 230, 220 });
                    DrawLineEx(Vector2{ posX + dir * 14.0f, fxY + 4.0f }, Vector2{ fxX + dir * 4.0f, fxY + 4.0f }, 1.5f, Color{ 255, 220, 100, 180 });
                    DrawCircle((int)(fxX + dir * 6.0f), (int)fxY, 3, Color{ 255, 255, 255, 240 });
                } else if (mtype == MOVE_HEAVY_PUNCH) {

                    DrawCircleLines((int)fxX, (int)fxY, 14.0f, Color{ 255, 120, 30, 200 });
                    DrawCircle((int)fxX, (int)fxY, 5, Color{ 255, 240, 180, 255 });
                    DrawLineEx(Vector2{ posX + dir * 12.0f, fxY - 10.0f }, Vector2{ fxX + dir * 12.0f, fxY + 8.0f }, 3.5f, Color{ 255, 180, 40, 240 });
                } else if (mtype == MOVE_LIGHT_KICK) {

                    DrawLineEx(Vector2{ posX + dir * 16.0f, fxY }, Vector2{ fxX + dir * 6.0f, fxY }, 2.0f, Color{ 230, 245, 255, 220 });
                    DrawCircle((int)(fxX + dir * 4.0f), (int)fxY, 2, Color{ 255, 255, 255, 240 });
                } else if (mtype == MOVE_HEAVY_KICK) {

                    Vector2 p1{ posX + dir * 16.0f, fxY - 14.0f };
                    Vector2 p2{ fxX + dir * 14.0f, fxY };
                    Vector2 p3{ posX + dir * 20.0f, fxY + 16.0f };
                    DrawLineEx(p1, p2, 3.5f, Color{ 255, 230, 80, 240 });
                    DrawLineEx(p2, p3, 3.0f, Color{ 255, 160, 40, 220 });
                    DrawCircle((int)p2.x, (int)p2.y, 4, Color{ 255, 255, 255, 255 });
                }
            }
        }

        if (state == FSTATE_WALK_FWD || state == FSTATE_WALK_BACK) {
            float walkCycle = sinf(frame * 0.35f);
            float legStride = walkCycle * 6.5f;
            float dirSign = (state == FSTATE_WALK_FWD) ? 1.0f : -1.0f;
            int footY = (int)(GROUND_Y - 2.0f);

            int shoe1X = (int)(posX - (fr ? 7.0f : -7.0f) - legStride * dirSign);
            int shoe2X = (int)(posX + (fr ? 5.0f : -5.0f) + legStride * dirSign);
            DrawRectangle(shoe1X, footY, 9, 3, Color{ 22, 22, 28, 255 });
            DrawRectangle(shoe2X, footY, 9, 3, Color{ 36, 38, 48, 255 });

            if (fabsf(walkCycle) > 0.85f && (frame % 5 == 0)) {
                DrawCircle((int)(posX - (fr ? 10.0f : -10.0f) - legStride * dirSign), footY + 1, 2, Color{ 200, 205, 215, 120 });
            }
        }

        if (state == FSTATE_IDLE) {
            int quirkTimer = frame % 200;
            if (quirkTimer >= 100 && quirkTimer <= 170) {
                float quirkPulse = sinf((quirkTimer - 100) * 0.09f);
                switch (charId) {
                case CHAR_COMMANDER: {
                    float tieX = posX + (fr ? 4.0f : -4.0f) + sinf(frame * 0.25f) * 4.0f;
                    float tieY = pos.y - 25.0f;
                    DrawLineEx(Vector2{ posX + (fr ? 2.0f : -2.0f), pos.y - 42.0f }, Vector2{ tieX, tieY + 8.0f }, 2.5f, Color{ 230, 20, 20, 255 });
                    DrawCircle((int)tieX, (int)(tieY + 8.0f), 2, Color{ 255, 215, 0, 220 });
                    break;
                }
                case CHAR_HIGH_BENCH: {
                    int gx = (int)(posX + (fr ? 16.0f : -16.0f));
                    int gy = (int)(pos.y - 45.0f - quirkPulse * 5.0f);
                    DrawRectangle(gx - 2, gy - 2, 8, 4, Color{ 220, 180, 50, 255 });
                    DrawLine(gx + 2, gy, gx + 2, gy + 10, Color{ 140, 100, 30, 255 });
                    break;
                }
                case CHAR_REDCAP: {
                    int px = (int)(posX + (fr ? 14.0f : -14.0f));
                    int py = (int)(pos.y - 40.0f);
                    DrawCircle(px, py, 2 + (int)(quirkPulse * 2.0f), Color{ 240, 240, 245, 180 });
                    break;
                }
                case CHAR_COASTAL_GOV: {
                    int glintX = (int)(posX + (fr ? 6.0f : -6.0f));
                    int glintY = (int)(pos.y - 58.0f);
                    DrawLine(glintX - 4, glintY, glintX + 4, glintY, Color{ 255, 255, 255, 240 });
                    DrawLine(glintX, glintY - 4, glintX, glintY + 4, Color{ 255, 255, 255, 240 });
                    break;
                }
                case CHAR_TECH_VISIONARY: {
                    int dx = (int)(posX - (fr ? 18.0f : -18.0f));
                    int dy = (int)(pos.y - 68.0f + sinf(frame * 0.2f) * 3.0f);
                    DrawRectangle(dx - 4, dy - 2, 8, 4, Color{ 40, 45, 55, 255 });
                    DrawCircle(dx, dy - 1, 2, (frame % 10 < 5) ? Color{ 0, 255, 255, 255 } : Color{ 0, 120, 200, 255 });
                    DrawLine(dx - 5, dy - 3, dx + 5, dy - 3, Color{ 180, 200, 220, 220 });
                    break;
                }
                case CHAR_FILIBUSTER: {
                    int fx = (int)(posX + (fr ? 12.0f : -12.0f));
                    int fy = (int)(pos.y - 36.0f);
                    DrawRectangle(fx - 4, fy - 6, 8, 12, Color{ 160, 40, 30, 240 });
                    DrawPixelText("RULE 22", fx - 10, fy - 14, 6, Color{ 255, 220, 80, 220 });
                    break;
                }
                case CHAR_MASCOT: {
                    int mx = (int)(posX + (fr ? 14.0f : -14.0f));
                    int my = (int)(pos.y - 52.0f);
                    DrawCircle(mx, my, 3, Color{ 255, 220, 40, 230 });
                    break;
                }
                case CHAR_POPULIST: {
                    int sx = (int)(posX + (fr ? 14.0f : -14.0f));
                    int sy = (int)(pos.y - 48.0f);
                    DrawRectangle(sx - 2, sy - 4, 4, 8, Color{ 30, 30, 35, 255 });
                    if (frame % 20 < 4) {
                        DrawCircle(sx, sy - 3, 5, Color{ 255, 255, 240, 240 });
                    }
                    break;
                }
                case CHAR_SUCKRATERY_RKF: {
                    int wx = (int)(posX + (fr ? 5.0f : -5.0f));
                    int wy = (int)(pos.y - 62.0f - sinf(frame * 0.3f) * 4.0f);
                    DrawCircle(wx, wy, 3, Color{ 255, 120, 180, 255 });
                    DrawCircle(wx + (fr ? 2 : -2), wy - 1, 1, Color{ 255, 255, 255, 255 });
                    DrawLine(wx, wy, (int)(posX + (fr ? 3.0f : -3.0f)), (int)(pos.y - 56.0f), Color{ 230, 80, 150, 255 });
                    break;
                }
                case CHAR_WAR_PIG: {
                    int vx = (int)(posX + (fr ? -4.0f : 4.0f));
                    int vy = (int)(pos.y - 50.0f);
                    DrawCircle(vx, vy, 3, Color{ 255, 60, 40, (unsigned char)(quirkPulse * 220) });
                    break;
                }
                case CHAR_SOCIALIST: {
                    int ax = (int)(posX + (fr ? 16.0f : -16.0f));
                    int ay = (int)(pos.y - 40.0f - quirkPulse * 12.0f);
                    DrawCircle(ax, ay, 3, Color{ 230, 40, 40, 255 });
                    DrawLine(ax, ay - 3, ax + 1, ay - 5, Color{ 60, 160, 50, 255 });
                    break;
                }
                case CHAR_BILLARY: {
                    int ex = (int)(posX + (fr ? 12.0f : -12.0f));
                    int ey = (int)(pos.y - 50.0f);
                    DrawRectangle(ex - 4, ey - 3, 8, 6, Color{ 240, 240, 245, 230 });
                    DrawLine(ex - 4, ey - 3, ex, ey, Color{ 100, 140, 220, 240 });
                    DrawLine(ex + 4, ey - 3, ex, ey, Color{ 100, 140, 220, 240 });
                    break;
                }
                case CHAR_PALM_BEACH_PETE: {
                    int rx = (int)(posX + (fr ? 12.0f : -12.0f));
                    int ry = (int)(pos.y - 34.0f);
                    DrawCircle(rx, ry, 2, Color{ 255, 215, 0, 255 });
                    DrawLine(rx - 3, ry, rx + 3, ry, Color{ 255, 255, 255, 230 });
                    break;
                }
                case CHAR_SLEEPY_JOE: {
                    int jx = (int)(posX + (fr ? 6.0f : -6.0f));
                    int jy = (int)(pos.y - 58.0f);
                    DrawCircle(jx, jy, 2, Color{ 255, 30, 30, 240 });
                    DrawPixelText("z", jx + (fr ? 10 : -14), jy - 8, 6, Color{ 200, 220, 255, 200 });
                    break;
                }
                case CHAR_MTG: {
                    int mx = (int)(posX + (fr ? 2.0f : -2.0f));
                    int my = (int)(pos.y - 66.0f);
                    DrawPixelText("✦", mx + (int)(sinf(frame * 0.4f) * 6.0f), my - 4, 7, Color{ 255, 220, 40, 240 });
                    break;
                }
                default:
                    break;
                }
            }
        }

        if (f.GetTauntTimer() > 0 && f.HasSpeechTaunt() && !f.GetActiveTaunt().empty()) {
            const char* quote = f.GetActiveTaunt().c_str();
            int qw = MeasureText(quote, 8);
            int bubbleW = qw + 10;
            int bubbleH = 13;

            float headTopY = (f.GetCharacterId() == CHAR_WAR_PIG) ? (pos.y - 88.0f) : (pos.y - 70.0f);
            int bubbleY = (int)(headTopY - bubbleH - 4.0f);
            if (bubbleY < 12) bubbleY = 12;

            int bubbleX = (int)pos.x - bubbleW / 2;
            bubbleX = std::clamp(bubbleX, 12, SCREEN_WIDTH - 12 - bubbleW);

            DrawRectangle(bubbleX + 1, bubbleY + 1, bubbleW, bubbleH, Color{ 0, 0, 0, 70 });

            DrawRectangle(bubbleX, bubbleY, bubbleW, bubbleH, Color{ 255, 255, 235, 250 });
            DrawRectangleLines(bubbleX, bubbleY, bubbleW, bubbleH, Color{ 20, 25, 35, 255 });

            int tailX = std::clamp((int)pos.x, bubbleX + 8, bubbleX + bubbleW - 8);
            DrawTriangle(Vector2{ (float)tailX, (float)headTopY - 1.0f },
                         Vector2{ (float)tailX - 3.0f, (float)bubbleY + bubbleH },
                         Vector2{ (float)tailX + 3.0f, (float)bubbleY + bubbleH },
                         Color{ 255, 255, 235, 250 });
            DrawLine(tailX - 3, bubbleY + bubbleH, tailX, (int)headTopY - 1, Color{ 20, 25, 35, 255 });
            DrawLine(tailX + 3, bubbleY + bubbleH, tailX, (int)headTopY - 1, Color{ 20, 25, 35, 255 });

            int textX = bubbleX + (bubbleW - qw) / 2;
            DrawPixelText(quote, textX, bubbleY + 2, 8, Color{ 15, 20, 30, 255 });
        } else if ((state == FSTATE_TAUNT || f.GetTauntTimer() > 0) && !f.HasSpeechTaunt()) {
            int tickFrame = (state == FSTATE_TAUNT) ? frame : std::min(44, 90 - f.GetTauntTimer());

            switch (f.GetCharacterId()) {
            case CHAR_COMMANDER: {
                float tootProg = std::clamp((float)tickFrame / 45.0f, 0.0f, 1.0f);
                float tieWag = sinf(tickFrame * 0.45f) * 8.0f;
                DrawLineEx(Vector2{ pos.x + dir * 2, pos.y - 40 }, Vector2{ pos.x + dir * 6 + tieWag, pos.y - 18 }, 4.0f, Color{ 220, 25, 25, 255 });
                float acc = sinf(tickFrame * 0.5f) * 6.0f;
                DrawCircle((int)(pos.x + dir * (12 + acc)), (int)(pos.y - 36), 3, Color{ 255, 220, 60, 255 });
                DrawCircle((int)(pos.x - dir * (8 - acc)), (int)(pos.y - 36), 3, Color{ 255, 220, 60, 255 });
                for (int p = 0; p < 4; p++) {
                    float puffDist = 14.0f + p * 6.0f + tickFrame * 0.5f;
                    float puffY = pos.y - 24.0f - p * 3.0f - sinf(tickFrame * 0.2f + p) * 3.0f;
                    unsigned char alpha = (unsigned char)(std::clamp(1.0f - tootProg, 0.0f, 1.0f) * 190.0f);
                    DrawCircle((int)(pos.x - dir * puffDist), (int)puffY, 4 + p * 2, Color{ 220, 225, 60, alpha });
                    DrawCircle((int)(pos.x - dir * puffDist), (int)puffY, 2 + p, Color{ 245, 245, 120, alpha });
                }
                break;
            }
            case CHAR_SUCKRATERY_RKF: {
                float wormReach = sinf((tickFrame / 45.0f) * 3.14159f) * 24.0f;
                int wx = (int)(pos.x + dir * (10 + wormReach));
                int wy = (int)(pos.y - 62 - sinf(tickFrame * 0.4f) * 4.0f);
                DrawCircle(wx, wy, 4, Color{ 235, 110, 160, 255 });
                DrawCircle(wx + (int)(dir * 2), wy - 1, 1, WHITE);
                DrawLineEx(Vector2{ pos.x + dir * 6, pos.y - 62 }, Vector2{ (float)wx, (float)wy }, 3.0f, Color{ 210, 90, 140, 255 });
                DrawCircle((int)(pos.x + dir * (8 + wormReach * 0.6f)), wy + 1, 3, Color{ 245, 130, 175, 255 });
                if (tickFrame >= 15 && tickFrame <= 30) {
                    DrawCircle(wx + (int)(dir * 4), wy + (tickFrame - 15) * 2, 2, Color{ 100, 240, 80, 220 });
                }
                break;
            }
            case CHAR_SOCIALIST: {
                float arc = sinf((tickFrame / 45.0f) * 3.14159f) * 32.0f;
                float appleX = pos.x + dir * 14.0f;
                float appleY = pos.y - 50.0f - arc;
                DrawCircle((int)appleX, (int)appleY, 4, Color{ 220, 30, 30, 255 });
                DrawRectangle((int)appleX - 1, (int)appleY - 5, 2, 3, Color{ 80, 40, 15, 255 });
                DrawCircle((int)appleX + 1, (int)appleY - 4, 1, Color{ 80, 180, 40, 255 });
                if (tickFrame >= 32) {
                    DrawPixelText("✦", (int)pos.x + (int)(dir * 8), (int)pos.y - 62, 8, Color{ 255, 240, 100, 255 });
                }
                break;
            }
            case CHAR_TECH_VISIONARY: {
                float ang = (tickFrame / 45.0f) * 6.283f * 2.0f;
                float rad = 18.0f;
                float dx = pos.x + cosf(ang) * rad;
                float dy = pos.y - 74.0f + sinf(ang) * (rad * 0.6f);
                DrawRectangle((int)dx - 6, (int)dy - 2, 12, 4, Color{ 35, 45, 60, 255 });
                DrawCircle((int)dx, (int)dy, 2, Color{ 0, 240, 255, 255 });
                DrawPixelText("·", (int)dx - 4, (int)dy - 3, 6, Color{ 0, 220, 255, 180 });
                break;
            }
            case CHAR_REDCAP: {
                if (tickFrame <= 25) {
                    float spitX = pos.x + dir * (6.0f + tickFrame * 1.0f);
                    float spitY = pos.y - 48.0f + (tickFrame * tickFrame) * 0.05f;
                    DrawCircle((int)spitX, (int)spitY, 2, Color{ 240, 240, 220, 240 });
                }
                if (tickFrame >= 18 && tickFrame <= 36) {
                    DrawPixelText("✦", (int)pos.x + (int)(dir * 12), (int)pos.y - 40, 8, Color{ 255, 160, 30, 255 });
                }
                break;
            }
            case CHAR_FILIBUSTER: {
                float scrollLen = std::min(32.0f, tickFrame * 1.2f);
                int sx = (int)(pos.x + dir * 8);
                int sy = (int)(pos.y - 44);
                DrawRectangle(sx, sy, 10, (int)scrollLen, Color{ 245, 240, 215, 245 });
                DrawRectangleLines(sx, sy, 10, (int)scrollLen, Color{ 140, 110, 60, 255 });
                for (int l = 2; l < scrollLen - 2; l += 4) {
                    DrawLine(sx + 2, sy + l, sx + 8, sy + l, Color{ 100, 80, 50, 200 });
                }
                break;
            }
            case CHAR_MASCOT: {
                float wingFlap = sinf(tickFrame * 0.6f) * 8.0f;
                DrawTriangle(Vector2{ pos.x - 14, pos.y - 48 }, Vector2{ pos.x - 22, pos.y - 48 - wingFlap }, Vector2{ pos.x - 6, pos.y - 38 }, Color{ 180, 140, 40, 255 });
                DrawTriangle(Vector2{ pos.x + 14, pos.y - 48 }, Vector2{ pos.x + 22, pos.y - 48 - wingFlap }, Vector2{ pos.x + 6, pos.y - 38 }, Color{ 180, 140, 40, 255 });
                DrawPixelText("★", (int)pos.x + (int)(sinf(tickFrame * 0.3f) * 16.0f), (int)pos.y - 74, 8, Color{ 255, 220, 50, 240 });
                break;
            }
            case CHAR_PALM_BEACH_PETE: {
                int wx = (int)(pos.x + dir * 10);
                int wy = (int)(pos.y - 42);
                DrawCircle(wx, wy, 4, Color{ 255, 215, 0, 255 });
                DrawCircle(wx, wy, 2, Color{ 255, 250, 210, 255 });
                if ((tickFrame / 6) % 2 == 0) {
                    DrawPixelText("✦", wx - 3, wy - 8, 8, Color{ 255, 235, 120, 255 });
                }
                break;
            }
            default:
                break;
            }
        }

        DrawCosmeticPuffs(f);
        ChampionshipEdition::DrawFighterWorldOverlays(f);
        return;
    }
    ChampionshipEdition::DrawArticulated12PartRig(f);
    ChampionshipEdition::DrawFighterWorldOverlays(f);
    DrawCosmeticPuffs(f);
    return;

    Color c1 = f.GetPrimaryColor();
    Color c2 = f.GetSecondaryColor();
    Color skin = f.GetSkinColor();

    float breath = 0.0f;
    if (state == FSTATE_IDLE) {
        breath = sinf(frame * 0.12f) * 1.5f;
    }

    float walkStride = 0.0f;
    if (state == FSTATE_WALK_FWD || state == FSTATE_WALK_BACK) {
        walkStride = sinf(frame * 0.25f) * 7.0f;
    }

    if (state == FSTATE_KNOCKDOWN_AIR) {

        DrawRectangle((int)(pos.x - 22), (int)(pos.y - 30), 44, 18, c1);
        DrawCircle((int)(pos.x - dir * 18), (int)(pos.y - 21), 8, skin);
        DrawRectangle((int)(pos.x + dir * 10), (int)(pos.y - 28), 16, 12, c2);
        return;
    }
    if (state == FSTATE_KNOCKDOWN_GROUND || state == FSTATE_DEFEAT) {

        DrawRectangle((int)(pos.x - 28), (int)(GROUND_Y - 12), 56, 12, c1);
        DrawCircle((int)(pos.x - dir * 22), (int)(GROUND_Y - 6), 7, skin);
        return;
    }
    if (state == FSTATE_WAKEUP) {

        float riseProg = std::clamp((float)frame / 24.0f, 0.0f, 1.0f);
        int riseY = (int)((GROUND_Y - 12) * (1.0f - riseProg) + (pos.y - 42.0f) * riseProg);
        DrawRectangle((int)(pos.x - 20), riseY, 40, 20, c1);
        DrawCircle((int)(pos.x + dir * 6), riseY - 6, 7, skin);
        return;
    }

    float recoilX = 0.0f;
    if (state == FSTATE_HITSTUN) {
        recoilX = -dir * 4.0f;
    }

    int headY = (int)(pos.y - 56.0f + breath);
    int torsoY = (int)(pos.y - 42.0f + breath);
    int legsY = (int)(pos.y - 20.0f);

    if (f.IsCrouching()) {
        headY += 18;
        torsoY += 16;
        legsY += 8;
    }

    if (f.IsCrouching()) {
        DrawRectangle((int)(pos.x - 14), legsY, 28, 18, Color{ (unsigned char)(c1.r * 0.7f), (unsigned char)(c1.g * 0.7f), (unsigned char)(c1.b * 0.7f), 255 });

        DrawRectangle((int)(pos.x - dir * 8), legsY + 12, 14, 6, Color{ 30, 25, 20, 255 });
    } else {

        DrawRectangle((int)(pos.x - 10 - walkStride * 0.5f), legsY, 9, 20, Color{ (unsigned char)(c1.r * 0.65f), (unsigned char)(c1.g * 0.65f), (unsigned char)(c1.b * 0.65f), 255 });

        DrawRectangle((int)(pos.x + 2 + walkStride * 0.5f), legsY, 9, 20, c1);

        DrawRectangle((int)(pos.x - 12 - walkStride * 0.5f), legsY + 16, 11, 4, Color{ 25, 20, 15, 255 });
        DrawRectangle((int)(pos.x + 1 + walkStride * 0.5f), legsY + 16, 11, 4, Color{ 35, 30, 25, 255 });
    }

    int torsoH = f.IsCrouching() ? 18 : 24;
    DrawRectangle((int)(pos.x - 12 + recoilX), torsoY, 24, torsoH, c1);

    switch (f.GetCharacterId()) {
    case CHAR_COMMANDER:

        DrawRectangle((int)(pos.x - 3 + recoilX), torsoY + 2, 6, 18, WHITE);
        DrawRectangle((int)(pos.x - 2 + recoilX), torsoY + 4, 4, 14, c2);
        break;
    case CHAR_HIGH_BENCH:

        DrawRectangle((int)(pos.x - 4 + recoilX), torsoY + 1, 8, 8, WHITE);
        DrawCircle((int)(pos.x + recoilX), torsoY + 12, 3, c2);
        break;
    case CHAR_REDCAP:

        DrawRectangle((int)(pos.x - 4 + recoilX), torsoY + 2, 8, 16, Color{ 230, 200, 40, 255 });
        break;
    case CHAR_COASTAL_GOV:

        DrawLine((int)(pos.x - 7 + recoilX), torsoY + 2, (int)(pos.x - 1 + recoilX), torsoY + 16, c2);
        DrawLine((int)(pos.x + 7 + recoilX), torsoY + 2, (int)(pos.x + 1 + recoilX), torsoY + 16, c2);
        break;
    case CHAR_TECH_VISIONARY:

        DrawLine((int)(pos.x - 5 + recoilX), torsoY + 4, (int)(pos.x + 5 + recoilX), torsoY + 4, c2);
        DrawLine((int)(pos.x + recoilX), torsoY + 4, (int)(pos.x + recoilX), torsoY + 14, c2);
        break;
    case CHAR_FILIBUSTER:

        DrawLine((int)(pos.x - 6 + recoilX), torsoY + 2, (int)(pos.x - 6 + recoilX), torsoY + 18, c2);
        DrawLine((int)(pos.x + 6 + recoilX), torsoY + 2, (int)(pos.x + 6 + recoilX), torsoY + 18, c2);
        break;
    case CHAR_MASCOT:

        DrawCircle((int)(pos.x + recoilX), torsoY + 10, 6, c2);
        break;
    case CHAR_POPULIST:

        DrawCircle((int)(pos.x - 6 * dir + recoilX), torsoY + 6, 3, c2);
        break;
    default:
        break;
    }

    DrawCircle((int)(pos.x + recoilX), headY + 6, 7, skin);

    switch (f.GetCharacterId()) {
    case CHAR_COMMANDER:

        DrawCircle((int)(pos.x - dir * 2 + recoilX), headY + 2, 7, Color{ 210, 215, 220, 255 });
        break;
    case CHAR_HIGH_BENCH:

        DrawCircle((int)(pos.x - dir * 2 + recoilX), headY + 1, 8, Color{ 235, 235, 240, 255 });
        DrawRectangle((int)(pos.x + dir * 3 + recoilX), headY + 5, 4, 3, Color{ 220, 180, 50, 255 });
        break;
    case CHAR_REDCAP:

        DrawRectangle((int)(pos.x - 7 + recoilX), headY - 1, 14, 6, c1);
        DrawRectangle((int)(pos.x + dir * 3 + recoilX), headY + 2, 7, 3, c1);
        break;
    case CHAR_COASTAL_GOV:

        DrawRectangle((int)(pos.x - 7 + recoilX), headY, 14, 5, Color{ 25, 25, 30, 255 });
        DrawRectangle((int)(pos.x + dir * 2 + recoilX), headY + 5, 5, 3, Color{ 20, 20, 20, 255 });
        break;
    case CHAR_TECH_VISIONARY:

        DrawRectangle((int)(pos.x + dir * 1 + recoilX), headY + 4, 7, 3, c2);
        break;
    case CHAR_FILIBUSTER:

        DrawCircle((int)(pos.x - dir * 3 + recoilX), headY + 3, 6, Color{ 220, 220, 225, 255 });
        break;
    case CHAR_MASCOT:

        DrawCircle((int)(pos.x - 6 + recoilX), headY - 3, 5, c1);
        DrawCircle((int)(pos.x + 6 + recoilX), headY - 3, 5, c1);
        break;
    case CHAR_POPULIST:

        DrawCircle((int)(pos.x - dir * 5 + recoilX), headY + 1, 6, Color{ 35, 25, 20, 255 });
        DrawLine((int)(pos.x - dir * 6 + recoilX), headY + 2, (int)(pos.x - dir * 12 + recoilX), headY + 14, Color{ 35, 25, 20, 255 });
        break;
    default:
        break;
    }

    if (state == FSTATE_ATTACK || state == FSTATE_SPECIAL) {
        int startup = f.GetCurrentAttack().startupFrames;
        int active = f.GetCurrentAttack().activeFrames;
        bool isStriking = (frame >= startup && frame < startup + active);

        float reach = f.GetCurrentAttack().reachX;
        float armEndX = pos.x + dir * (isStriking ? reach : reach * 0.4f);
        float armEndY = pos.y + f.GetCurrentAttack().reachY + 8.0f;

        DrawLineEx(Vector2{ pos.x, (float)torsoY + 6 }, Vector2{ armEndX, armEndY }, 6.0f, c1);
        DrawCircle((int)armEndX, (int)armEndY, 5, skin);

        if (isStriking) {

            if (f.GetCharacterId() == CHAR_COMMANDER && f.GetSpecialIndex() == 2) {
                DrawRectangle((int)armEndX - 4, (int)armEndY - 16, 12, 16, Color{ 255, 245, 210, 255 });
                DrawCircle((int)armEndX + 2, (int)armEndY - 8, 3, Color{ 220, 180, 40, 255 });
            }

            if (f.GetCharacterId() == CHAR_HIGH_BENCH && f.GetSpecialIndex() == 1) {
                DrawRectangle((int)armEndX - 6, (int)armEndY - 20, 18, 14, Color{ 160, 95, 35, 255 });
                DrawRectangle((int)armEndX - 2, (int)armEndY - 6, 6, 18, Color{ 120, 70, 25, 255 });
            }

            if (f.GetCharacterId() == CHAR_REDCAP && f.GetSpecialIndex() == 1) {
                Vector2 h1{ armEndX, armEndY };
                Vector2 h2{ armEndX + dir * 18, armEndY - 12 };
                Vector2 h3{ armEndX + dir * 18, armEndY + 12 };
                DrawTriangle(h1, h2, h3, Color{ 230, 230, 240, 255 });
            }

            if (f.GetCharacterId() == CHAR_TECH_VISIONARY && f.GetSpecialIndex() == 1) {
                DrawRectangle((int)pos.x - 10, (int)pos.y - 75, 20, 6, Color{ 40, 45, 55, 255 });
                DrawLine((int)pos.x - 14, (int)pos.y - 76, (int)pos.x + 14, (int)pos.y - 76, c2);
            }
        }
    } else if (state == FSTATE_BLOCK_HIGH || state == FSTATE_BLOCK_LOW) {

        DrawRectangle((int)(pos.x + dir * 6), torsoY + 2, 8, 14, c1);
        DrawRectangle((int)(pos.x + dir * 8), torsoY + 4, 6, 10, skin);
    } else {

        float armSwing = walkStride * 0.8f;
        DrawLineEx(Vector2{ pos.x - 4, (float)torsoY + 4 }, Vector2{ pos.x + dir * 8 - armSwing, (float)torsoY + 16 }, 4.0f, c1);
        DrawCircle((int)(pos.x + dir * 8 - armSwing), torsoY + 16, 3, skin);
    }
    DrawCosmeticPuffs(f);
}

void VisualsEngine::DrawProjectiles(const Fighter& f) {
    const auto& projs = f.GetProjectiles();
    for (const auto& p : projs) {
        if (!p.active) continue;

        if (p.type == 0) {

            DrawCircle((int)p.pos.x, (int)p.pos.y, 10, Color{ 80, 180, 255, 240 });
            DrawCircle((int)p.pos.x, (int)p.pos.y, 6, WHITE);

            float trailDir = p.facingRight ? -1.0f : 1.0f;
            DrawTriangle(
                Vector2{ p.pos.x, p.pos.y - 8 },
                Vector2{ p.pos.x, p.pos.y + 8 },
                Vector2{ p.pos.x + trailDir * 18, p.pos.y },
                Color{ 30, 130, 240, 180 }
            );
        } else if (p.type == 1) {

            DrawRectangle((int)(p.pos.x - p.width * 0.5f), (int)(p.pos.y - p.height * 0.5f), (int)p.width, (int)p.height, Color{ 245, 245, 240, 240 });

            for (int y = -20; y < 20; y += 8) {
                DrawLine((int)p.pos.x - 5, (int)p.pos.y + y, (int)p.pos.x + 5, (int)p.pos.y + y, Color{ 80, 80, 90, 220 });
            }
        } else if (p.type == 2) {

            DrawCircle((int)p.pos.x, (int)p.pos.y, 7, Color{ 240, 40, 150, 255 });
            DrawCircle((int)p.pos.x, (int)p.pos.y, 4, Color{ 255, 240, 120, 255 });
        } else if (p.type == 3) {

            DrawRectangle((int)p.pos.x - 3, (int)GROUND_Y - 26, 6, 26, Color{ 160, 160, 170, 255 });
            DrawCircle((int)p.pos.x, (int)GROUND_Y - 28, 4, Color{ 40, 40, 45, 255 });
        } else if (p.type == 4) {

            float dir = p.facingRight ? 1.0f : -1.0f;
            DrawCircle((int)p.pos.x, (int)p.pos.y, 8, Color{ 70, 30, 15, 255 });
            DrawCircle((int)p.pos.x, (int)p.pos.y, 5, Color{ 140, 65, 30, 240 });
            DrawCircle((int)(p.pos.x - dir * 8), (int)p.pos.y - 4, 4, Color{ 255, 235, 190, 220 });
            DrawCircle((int)(p.pos.x - dir * 14), (int)p.pos.y + 3, 3, Color{ 255, 245, 220, 200 });
            DrawCircle((int)(p.pos.x - dir * 20), (int)p.pos.y - 2, 2, WHITE);
        } else if (p.type == 5) {

            DrawEllipse((int)p.pos.x, (int)GROUND_Y - 4, 16, 8, Color{ 255, 215, 50, 220 });
            DrawEllipseLines((int)p.pos.x, (int)GROUND_Y - 4, 22, 11, Color{ 255, 180, 20, 240 });
            DrawLine((int)p.pos.x - 14, (int)GROUND_Y - 4, (int)p.pos.x + 14, (int)GROUND_Y - 4, WHITE);
        } else if (p.type == 6) {

            DrawRectangle((int)(p.pos.x - 10), (int)(p.pos.y - 8), 20, 16, Color{ 255, 245, 210, 255 });
            DrawRectangleLines((int)(p.pos.x - 10), (int)(p.pos.y - 8), 20, 16, Color{ 255, 215, 40, 255 });
            DrawLine((int)p.pos.x - 14, (int)p.pos.y - 12, (int)p.pos.x + 14, (int)p.pos.y + 12, Color{ 80, 210, 255, 240 });
            DrawLine((int)p.pos.x + 12, (int)p.pos.y - 10, (int)p.pos.x - 12, (int)p.pos.y + 10, Color{ 255, 240, 60, 240 });
        } else if (p.type == 7) {

            if (s_sprSleepyJoeSS.id > 0) {
                Rectangle srcRec = { 0.0f, 0.0f, (float)s_sprSleepyJoeSS.width, (float)s_sprSleepyJoeSS.height };
                if (!p.facingRight) srcRec.width = -srcRec.width;
                Rectangle dstRec = { p.pos.x - 22.0f, p.pos.y - 28.0f, 44.0f, 54.0f };
                Rectangle trailDst = { p.pos.x - (p.facingRight ? 32.0f : 12.0f), p.pos.y - 28.0f, 44.0f, 54.0f };
                DrawTexturePro(s_sprSleepyJoeSS, srcRec, trailDst, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 30, 30, 45, 120 });
                DrawTexturePro(s_sprSleepyJoeSS, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            } else {
                DrawRectangle((int)p.pos.x - 12, (int)p.pos.y - 24, 24, 48, Color{ 15, 20, 25, 255 });
                DrawCircle((int)p.pos.x, (int)p.pos.y - 28, 8, Color{ 230, 190, 150, 255 });
            }
        } else if (p.type == 8) {

            float dir = p.facingRight ? 1.0f : -1.0f;

            DrawEllipse((int)p.pos.x, (int)p.pos.y, 14, 8, Color{ 255, 220, 60, 220 });
            DrawEllipseLines((int)p.pos.x, (int)p.pos.y, 18, 11, Color{ 255, 180, 20, 240 });
            DrawCircle((int)p.pos.x + (int)(dir * 4.0f), (int)p.pos.y, 6, WHITE);

            DrawCircle((int)(p.pos.x - dir * 10), (int)p.pos.y - 6, 4, Color{ 255, 230, 80, 255 });
            DrawLine((int)(p.pos.x - dir * 7), (int)p.pos.y - 6, (int)(p.pos.x - dir * 7), (int)p.pos.y - 14, Color{ 255, 230, 80, 255 });
            DrawCircle((int)(p.pos.x - dir * 18), (int)p.pos.y + 4, 3, Color{ 255, 200, 40, 220 });
            DrawLine((int)(p.pos.x - dir * 15), (int)p.pos.y + 4, (int)(p.pos.x - dir * 15), (int)p.pos.y - 4, Color{ 255, 200, 40, 220 });

            DrawCircle((int)(p.pos.x + dir * 12), (int)p.pos.y - 2, 2, WHITE);
        }
    }
}

void VisualsEngine::DrawSparks(const std::vector<SparkEffect>& sparks) {
    for (const auto& sp : sparks) {
        if (!sp.alive) continue;
        int step = sp.frame / 2;

        if (sp.isBlock) {

            if (step == 0) {
                DrawCircle((int)sp.pos.x, (int)sp.pos.y, 6, WHITE);
                DrawCircle((int)sp.pos.x, (int)sp.pos.y, 10, Color{ 0, 210, 255, 180 });
            } else if (step == 1) {
                DrawCircleLines((int)sp.pos.x, (int)sp.pos.y, 14, Color{ 140, 235, 255, 230 });
                DrawLine((int)sp.pos.x - 12, (int)sp.pos.y, (int)sp.pos.x + 12, (int)sp.pos.y, WHITE);
                DrawLine((int)sp.pos.x, (int)sp.pos.y - 12, (int)sp.pos.x, (int)sp.pos.y + 12, WHITE);
            } else {
                DrawCircle((int)sp.pos.x - 6, (int)sp.pos.y - 4, 2, Color{ 60, 160, 255, 160 });
                DrawCircle((int)sp.pos.x + 6, (int)sp.pos.y + 4, 2, Color{ 60, 160, 255, 160 });
            }
        } else {

            if (step == 0) {
                DrawCircle((int)sp.pos.x, (int)sp.pos.y, 8, WHITE);
                DrawCircle((int)sp.pos.x, (int)sp.pos.y, 12, Color{ 255, 230, 40, 240 });
            } else if (step == 1) {

                DrawCircle((int)sp.pos.x, (int)sp.pos.y, 10, Color{ 255, 180, 20, 255 });
                DrawCircle((int)sp.pos.x, (int)sp.pos.y, 5, WHITE);
                for (int a = 0; a < 8; a++) {
                    float ang = a * (PI_F / 4.0f);
                    float sx = sp.pos.x + cosf(ang) * 16.0f;
                    float sy = sp.pos.y + sinf(ang) * 16.0f;
                    DrawLine((int)sp.pos.x, (int)sp.pos.y, (int)sx, (int)sy, Color{ 255, 100, 20, 255 });
                }
            } else {

                DrawCircle((int)sp.pos.x - 10, (int)sp.pos.y - 8, 3, Color{ 240, 80, 20, 200 });
                DrawCircle((int)sp.pos.x + 10, (int)sp.pos.y - 6, 2, Color{ 255, 140, 20, 200 });
                DrawCircle((int)sp.pos.x + 8, (int)sp.pos.y + 9, 3, Color{ 240, 80, 20, 200 });
                DrawCircle((int)sp.pos.x - 9, (int)sp.pos.y + 7, 2, Color{ 255, 180, 40, 200 });
            }
        }
    }
}

void VisualsEngine::DrawBattleHUD(const Fighter& p1, const Fighter& p2, int roundTimer, int roundNumber, int fightIntroTimer, bool p1WasKo, bool p2WasKo) {
    constexpr int BAR_WIDTH = 140;
    constexpr int BAR_HEIGHT = 10;
    constexpr int TOP_Y = 16;

    float p1Hp = (float)p1.GetHealth();
    float p2Hp = (float)p2.GetHealth();

    if (fightIntroTimer > 50) {
        float fillProg = std::clamp((110.0f - (float)fightIntroTimer) / 50.0f, 0.0f, 1.0f);
        if (p1WasKo) p1Hp = 100.0f * fillProg;
        if (p2WasKo) p2Hp = 100.0f * fillProg;
    }

    int p1HealthW = (int)((p1Hp / 100.0f) * BAR_WIDTH);
    DrawRectangle(20, TOP_Y, BAR_WIDTH + 4, BAR_HEIGHT + 4, Color{ 20, 20, 25, 230 });
    DrawRectangle(22, TOP_Y + 2, BAR_WIDTH, BAR_HEIGHT, Color{ 160, 30, 30, 255 });
    DrawRectangle(22 + (BAR_WIDTH - p1HealthW), TOP_Y + 2, p1HealthW, BAR_HEIGHT, Color{ 240, 210, 40, 255 });

    int p2HealthW = (int)((p2Hp / 100.0f) * BAR_WIDTH);
    DrawRectangle(SCREEN_WIDTH - 24 - BAR_WIDTH, TOP_Y, BAR_WIDTH + 4, BAR_HEIGHT + 4, Color{ 20, 20, 25, 230 });
    DrawRectangle(SCREEN_WIDTH - 22 - BAR_WIDTH, TOP_Y + 2, BAR_WIDTH, BAR_HEIGHT, Color{ 160, 30, 30, 255 });
    DrawRectangle(SCREEN_WIDTH - 22 - BAR_WIDTH, TOP_Y + 2, p2HealthW, BAR_HEIGHT, Color{ 240, 210, 40, 255 });

    DrawPixelText(p1.GetName().c_str(), 22, TOP_Y - 11, 10, Color{ 255, 220, 60, 255 });
    int p2NameW = MeasureText(p2.GetName().c_str(), 10);
    DrawPixelText(p2.GetName().c_str(), SCREEN_WIDTH - 22 - p2NameW, TOP_Y - 11, 10, Color{ 255, 220, 60, 255 });

    char timerStr[8];
    snprintf(timerStr, sizeof(timerStr), "%02d", std::max(0, roundTimer));
    int tw = MeasureText(timerStr, 18);
    int timerX = (SCREEN_WIDTH - tw) / 2;
    DrawRectangle((SCREEN_WIDTH / 2) - 18, TOP_Y - 4, 36, 22, Color{ 15, 15, 20, 240 });
    DrawRectangleLines((SCREEN_WIDTH / 2) - 18, TOP_Y - 4, 36, 22, Color{ 200, 160, 40, 255 });
    DrawPixelText(timerStr, timerX, TOP_Y - 2, 18, (roundTimer <= 10) ? Color{ 255, 50, 50, 255 } : Color{ 255, 225, 60, 255 });

    for (int r = 0; r < 2; r++) {
        Color c = (r < p1.GetRoundsWon()) ? Color{ 255, 215, 0, 255 } : Color{ 60, 60, 70, 255 };
        DrawCircle(28 + r * 14, TOP_Y + 18, 5, Color{ 20, 20, 25, 255 });
        DrawCircle(28 + r * 14, TOP_Y + 18, 4, c);
        DrawPixelText("V", 26 + r * 14, TOP_Y + 14, 8, (r < p1.GetRoundsWon()) ? Color{ 50, 30, 0, 255 } : Color{ 100, 100, 110, 255 });
    }

    for (int r = 0; r < 2; r++) {
        Color c = (r < p2.GetRoundsWon()) ? Color{ 255, 215, 0, 255 } : Color{ 60, 60, 70, 255 };
        DrawCircle(SCREEN_WIDTH - 28 - r * 14, TOP_Y + 18, 5, Color{ 20, 20, 25, 255 });
        DrawCircle(SCREEN_WIDTH - 28 - r * 14, TOP_Y + 18, 4, c);
        DrawPixelText("V", SCREEN_WIDTH - 30 - r * 14, TOP_Y + 14, 8, (r < p2.GetRoundsWon()) ? Color{ 50, 30, 0, 255 } : Color{ 100, 100, 110, 255 });
    }
}

void VisualsEngine::DrawCinematicIntro(float timer, bool logoDropped, const Shader& flagShader, const Texture2D& flagTex) {
    if (s_loaded && s_titleSplash.id > 0) {

        Rectangle srcRec = { 0.0f, 0.0f, (float)s_titleSplash.width, (float)s_titleSplash.height };
        Rectangle dstRec = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
        DrawTexturePro(s_titleSplash, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

        return;
    }

    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 15, 10, 30, 255 }, Color{ 70, 35, 55, 255 });

    int lmX = 70;
    int lmY = 60;
    DrawRectangle(lmX, lmY + 30, 244, 110, Color{ 180, 185, 195, 255 });
    DrawRectangle(lmX - 10, lmY + 20, 264, 12, Color{ 200, 205, 215, 255 });
    DrawRectangle(lmX + 10, lmY + 8, 224, 14, Color{ 215, 220, 230, 255 });
    for (int c = 0; c < 12; c++) {
        DrawRectangle(lmX + 10 + c * 19, lmY + 32, 8, 98, Color{ 140, 145, 155, 255 });
    }
    DrawRectangle(lmX + 95, lmY + 55, 54, 75, Color{ 40, 35, 45, 255 });
    DrawCircle(lmX + 122, lmY + 75, 8, Color{ 220, 220, 230, 255 });
    DrawRectangle(lmX + 112, lmY + 83, 20, 36, Color{ 210, 210, 220, 255 });

    for (int s = 0; s < 6; s++) {
        unsigned char colR = (unsigned char)(130 + s * 6);
        unsigned char colG = (unsigned char)(135 + s * 6);
        unsigned char colB = (unsigned char)(145 + s * 6);
        DrawRectangle(lmX - 25 - s * 8, lmY + 130 + s * 7, 294 + s * 16, 7, Color{ colR, colG, colB, 255 });
    }

    if (flagShader.id > 0) BeginShaderMode(flagShader);
    DrawTexture(flagTex, SCREEN_WIDTH - 140, 25, WHITE);
    if (flagShader.id > 0) EndShaderMode();
    DrawRectangle(SCREEN_WIDTH - 142, 15, 4, 160, Color{ 190, 190, 200, 255 });
    DrawCircle(SCREEN_WIDTH - 140, 15, 5, Color{ 240, 200, 40, 255 });

    float logoY = logoDropped ? 28.0f : -100.0f;
    if (!logoDropped) {
        float t = std::min(1.0f, timer / 0.8f);
        logoY = -100.0f + (t * t) * 128.0f;
    }

    int plaqueW = 320;
    int plaqueH = 64;
    int plaqueX = (SCREEN_WIDTH - plaqueW) / 2;
    DrawRectangle(plaqueX, (int)logoY, plaqueW, plaqueH, Color{ 15, 20, 30, 245 });
    DrawRectangleLines(plaqueX, (int)logoY, plaqueW, plaqueH, Color{ 220, 180, 50, 255 });
    DrawRectangleLines(plaqueX + 2, (int)logoY + 2, plaqueW - 4, plaqueH - 4, Color{ 255, 235, 120, 255 });

    DrawPixelText("EXECUTIVE ORDER II", plaqueX + 28, (int)logoY + 8, 20, Color{ 255, 235, 140, 255 });
    DrawPixelText("TOTAL VETO", plaqueX + 80, (int)logoY + 34, 24, Color{ 240, 50, 50, 255 });

    if (fmodf(timer, 0.9f) < 0.55f) {
        DrawPixelText("INSERT COIN  /  PRESS START", 85, SCREEN_HEIGHT - 32, 14, Color{ 255, 235, 80, 255 });
    }

    DrawPixelText("(C) 2026 EXECUTIVE ORDER II: TOTAL VETO", 55, SCREEN_HEIGHT - 14, 10, Color{ 180, 180, 190, 255 });
}

void VisualsEngine::DrawStageAndCharSelect(int selectedStage, int p1Char, int p2Char, float timer, const bool defeatedFighters[CHARACTER_COUNT], bool playerLocked, bool choosingOpponent) {
    bool warPigBeaten = (defeatedFighters != nullptr && defeatedFighters[CHAR_WAR_PIG]);

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 8, 14, 24, 255 });

    if (warPigBeaten) {

        if (s_loaded && s_mapEastCoast.id > 0) {
            Rectangle srcRec = { 0.0f, 0.0f, (float)s_mapEastCoast.width, (float)s_mapEastCoast.height };
            Rectangle dstRec = { 0.0f, 16.0f, (float)SCREEN_WIDTH, 130.0f };
            DrawTexturePro(s_mapEastCoast, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        } else {
            DrawRectangle(0, 16, SCREEN_WIDTH, 130, Color{ 12, 22, 38, 255 });
        }

        for (int y = 16; y < 146; y += 18) {
            DrawLine(0, y, SCREEN_WIDTH, y, Color{ 25, 45, 70, 70 });
        }
        for (int x = 0; x < SCREEN_WIDTH; x += 32) {
            DrawLine(x, 16, x, 146, Color{ 25, 45, 70, 70 });
        }

        Vector2 dcPt{ 245.0f, 74.0f };
        Vector2 nyPt{ 275.0f, 48.0f };
        Vector2 flPt{ 188.0f, 124.0f };

        for (int step = 0; step <= 10; step++) {
            float f = (float)step / 10.0f;
            Vector2 pNY = { dcPt.x + (nyPt.x - dcPt.x) * f, dcPt.y + (nyPt.y - dcPt.y) * f };
            DrawCircle((int)pNY.x, (int)pNY.y, 1.5f, Color{ 255, 220, 60, 200 });
            Vector2 pFL = { dcPt.x + (flPt.x - dcPt.x) * f, dcPt.y + (flPt.y - dcPt.y) * f };
            DrawCircle((int)pFL.x, (int)pFL.y, 1.5f, Color{ 80, 220, 255, 200 });
        }

        struct MapTarget {
            Vector2 pos;
            const char* city;
            CharacterId charId;
        };
        MapTarget targets[3] = {
            { nyPt, "NEW YORK", CHAR_SOCIALIST },
            { dcPt, "D.C.", CHAR_BILLARY },
            { flPt, "FLORIDA KEYS", CHAR_PALM_BEACH_PETE }
        };

        for (int i = 0; i < 3; i++) {
            const auto& t = targets[i];
            bool isDef = (defeatedFighters != nullptr && defeatedFighters[t.charId]);
            bool isActive = (p2Char == t.charId);

            if (isDef) {

                DrawLine((int)t.pos.x - 7, (int)t.pos.y - 7, (int)t.pos.x + 7, (int)t.pos.y + 7, Color{ 230, 40, 40, 255 });
                DrawLine((int)t.pos.x - 7, (int)t.pos.y + 7, (int)t.pos.x + 7, (int)t.pos.y - 7, Color{ 230, 40, 40, 255 });
                DrawPixelText("BEATEN", (int)t.pos.x - 14, (int)t.pos.y + 8, 7, Color{ 240, 80, 80, 255 });
            } else if (isActive) {

                float ringR = 6.0f + fmodf(timer * 22.0f, 12.0f);
                DrawCircleLines((int)t.pos.x, (int)t.pos.y, ringR, Color{ 255, 60, 60, (unsigned char)(255 - ringR * 15) });
                DrawCircle((int)t.pos.x, (int)t.pos.y, 3, Color{ 255, 230, 60, 255 });
                DrawLine((int)t.pos.x - 11, (int)t.pos.y, (int)t.pos.x + 11, (int)t.pos.y, Color{ 255, 220, 60, 255 });
                DrawLine((int)t.pos.x, (int)t.pos.y - 11, (int)t.pos.x, (int)t.pos.y + 11, Color{ 255, 220, 60, 255 });
                int cw = MeasureText(t.city, 7);
                DrawRectangle((int)t.pos.x - cw / 2 - 2, (int)t.pos.y + 8, cw + 4, 10, Color{ 10, 15, 25, 220 });
                DrawPixelText(t.city, (int)t.pos.x - cw / 2, (int)t.pos.y + 9, 7, Color{ 255, 230, 80, 255 });
            } else {

                DrawCircleLines((int)t.pos.x, (int)t.pos.y, 4, Color{ 0, 210, 255, 180 });
                DrawCircle((int)t.pos.x, (int)t.pos.y, 2, Color{ 0, 210, 255, 240 });
                int cw = MeasureText(t.city, 7);
                DrawRectangle((int)t.pos.x - cw / 2 - 2, (int)t.pos.y + 7, cw + 4, 9, Color{ 10, 15, 25, 180 });
                DrawPixelText(t.city, (int)t.pos.x - cw / 2, (int)t.pos.y + 8, 7, Color{ 160, 195, 225, 220 });
            }
        }
    } else {

        for (int x = 0; x < SCREEN_WIDTH; x += 32) {
            DrawLine(x, 0, x, 145, Color{ 16, 28, 42, 255 });
        }
        for (int y = 0; y < 145; y += 24) {
            DrawLine(0, y, SCREEN_WIDTH, y, Color{ 16, 28, 42, 255 });
        }

        DrawCircleLines(185, 88, 35.0f, Color{ 22, 45, 65, 180 });
        DrawCircleLines(185, 88, 75.0f, Color{ 20, 40, 58, 140 });
        DrawCircleLines(185, 88, 115.0f, Color{ 18, 35, 52, 100 });

        float radarAngle = timer * 1.8f;
        Vector2 radarCenter{ 185.0f, 88.0f };
        Vector2 radarEnd{ radarCenter.x + cosf(radarAngle) * 125.0f, radarCenter.y + sinf(radarAngle) * 125.0f };
        DrawLine((int)radarCenter.x, (int)radarCenter.y, (int)radarEnd.x, (int)radarEnd.y, Color{ 0, 220, 160, 40 });

    Vector2 potomacPoints[] = {
        { 55, 18 }, { 68, 32 }, { 82, 48 }, { 96, 62 }, { 104, 76 },
        { 108, 92 }, { 115, 108 }, { 128, 122 }, { 145, 134 }, { 165, 145 }
    };
    for (int i = 0; i < 9; i++) {
        DrawLineEx(potomacPoints[i], potomacPoints[i + 1], 16.0f, Color{ 14, 40, 68, 255 });
        DrawCircleV(potomacPoints[i], 8.0f, Color{ 14, 40, 68, 255 });
    }
    DrawCircleV(potomacPoints[9], 8.0f, Color{ 14, 40, 68, 255 });
    for (int i = 0; i < 9; i++) {
        DrawLine((int)potomacPoints[i].x + 8, (int)potomacPoints[i].y, (int)potomacPoints[i + 1].x + 8, (int)potomacPoints[i + 1].y, Color{ 30, 90, 140, 200 });
        DrawLine((int)potomacPoints[i].x - 8, (int)potomacPoints[i].y, (int)potomacPoints[i + 1].x - 8, (int)potomacPoints[i + 1].y, Color{ 25, 75, 120, 180 });
    }

    DrawEllipse(100, 68, 5, 9, Color{ 20, 50, 35, 255 });
    DrawEllipseLines(100, 68, 5, 9, Color{ 40, 100, 60, 255 });

    DrawCircle(182, 106, 11, Color{ 14, 40, 68, 255 });
    DrawCircleLines(182, 106, 11, Color{ 35, 100, 150, 220 });

    DrawCircle(174, 98, 1, Color{ 255, 160, 200, 220 });
    DrawCircle(188, 98, 1, Color{ 255, 160, 200, 220 });
    DrawCircle(172, 110, 1, Color{ 255, 160, 200, 220 });
    DrawCircle(192, 110, 1, Color{ 255, 160, 200, 220 });
    DrawCircle(182, 117, 1, Color{ 255, 160, 200, 220 });

    Vector2 anacostiaPoints[] = {
        { 345, 105 }, { 320, 120 }, { 285, 134 }, { 250, 145 }
    };
    for (int i = 0; i < 3; i++) {
        DrawLineEx(anacostiaPoints[i], anacostiaPoints[i + 1], 10.0f, Color{ 14, 40, 68, 255 });
    }

    DrawRectangle(140, 84, 140, 8, Color{ 18, 48, 30, 240 });
    DrawRectangleLines(140, 84, 140, 8, Color{ 35, 95, 55, 200 });

    DrawRectangle(144, 86, 30, 4, Color{ 0, 190, 240, 255 });
    DrawRectangleLines(144, 86, 30, 4, WHITE);

    DrawEllipse(180, 64, 8, 10, Color{ 20, 52, 34, 255 });
    DrawEllipseLines(180, 64, 8, 10, Color{ 40, 105, 65, 255 });

    DrawRectangle(174, 38, 12, 6, Color{ 20, 52, 34, 255 });

    Color streetCol = Color{ 42, 58, 76, 255 };
    DrawLine(105, 26, 300, 26, streetCol);
    DrawLine(108, 32, 300, 32, streetCol);
    DrawLine(115, 42, 290, 42, streetCol);
    DrawLine(125, 48, 285, 48, streetCol);
    DrawLine(135, 54, 285, 54, streetCol);
    DrawLine(135, 60, 285, 60, streetCol);
    DrawLine(130, 66, 285, 66, streetCol);
    DrawLine(130, 72, 285, 72, streetCol);
    DrawLine(140, 102, 300, 102, streetCol);
    DrawLine(145, 110, 300, 110, streetCol);
    DrawLine(150, 118, 300, 118, streetCol);

    DrawLine(125, 30, 125, 80, streetCol);
    DrawLine(140, 25, 140, 84, streetCol);
    DrawLine(152, 22, 152, 84, streetCol);
    DrawLine(162, 22, 162, 84, streetCol);
    DrawLine(172, 22, 172, 80, streetCol);
    DrawLine(188, 22, 188, 80, streetCol);
    DrawLine(210, 22, 210, 115, streetCol);
    DrawLine(220, 22, 220, 115, streetCol);
    DrawLine(230, 22, 230, 115, streetCol);
    DrawLine(240, 22, 240, 115, streetCol);
    DrawLine(250, 22, 250, 115, streetCol);
    DrawLine(260, 22, 260, 115, streetCol);
    DrawLine(270, 22, 270, 115, streetCol);
    DrawLine(280, 22, 280, 115, streetCol);
    DrawLine(300, 22, 300, 115, streetCol);

    DrawLineEx(Vector2{ 180, 18 }, Vector2{ 180, 48 }, 2.0f, Color{ 140, 190, 230, 255 });

    DrawLineEx(Vector2{ 198, 18 }, Vector2{ 198, 102 }, 2.0f, Color{ 160, 190, 220, 240 });
    DrawLineEx(Vector2{ 198, 102 }, Vector2{ 175, 136 }, 2.0f, Color{ 220, 180, 60, 240 });

    DrawLineEx(Vector2{ 265, 20 }, Vector2{ 265, 125 }, 2.0f, Color{ 150, 180, 210, 240 });

    DrawLineEx(Vector2{ 290, 18 }, Vector2{ 290, 72 }, 2.0f, Color{ 170, 200, 235, 255 });
    DrawLineEx(Vector2{ 290, 84 }, Vector2{ 290, 135 }, 2.0f, Color{ 170, 200, 235, 255 });

    DrawLineEx(Vector2{ 110, 36 }, Vector2{ 285, 36 }, 2.0f, Color{ 200, 215, 235, 255 });

    DrawLineEx(Vector2{ 118, 80 }, Vector2{ 285, 80 }, 2.0f, Color{ 220, 225, 245, 255 });

    DrawLineEx(Vector2{ 132, 96 }, Vector2{ 285, 96 }, 2.0f, Color{ 200, 215, 235, 255 });

    DrawLineEx(Vector2{ 120, 32 }, Vector2{ 175, 48 }, 2.0f, Color{ 220, 210, 160, 220 });
    DrawLineEx(Vector2{ 185, 52 }, Vector2{ 285, 78 }, 2.0f, Color{ 220, 210, 160, 220 });

    DrawLineEx(Vector2{ 95, 18 }, Vector2{ 280, 68 }, 2.0f, Color{ 190, 210, 235, 240 });

    DrawLineEx(Vector2{ 106, 68 }, Vector2{ 160, 80 }, 2.0f, Color{ 175, 195, 225, 240 });

    DrawLineEx(Vector2{ 128, 54 }, Vector2{ 172, 18 }, 2.0f, Color{ 165, 190, 220, 240 });

    DrawLineEx(Vector2{ 180, 32 }, Vector2{ 280, 18 }, 2.0f, Color{ 165, 190, 220, 240 });

    DrawLineEx(Vector2{ 68, 18 }, Vector2{ 98, 62 }, 1.5f, Color{ 120, 160, 190, 200 });

    DrawLineEx(Vector2{ 68, 44 }, Vector2{ 86, 38 }, 2.5f, Color{ 210, 215, 230, 255 });

    DrawLineEx(Vector2{ 125, 18 }, Vector2{ 178, 46 }, 2.0f, Color{ 180, 200, 230, 240 });

    DrawLineEx(Vector2{ 185, 48 }, Vector2{ 310, 24 }, 2.0f, Color{ 190, 210, 230, 240 });

    DrawLineEx(Vector2{ 285, 75 }, Vector2{ 330, 48 }, 2.0f, Color{ 180, 200, 225, 240 });
    DrawLineEx(Vector2{ 285, 82 }, Vector2{ 215, 115 }, 2.0f, Color{ 180, 200, 225, 240 });

    DrawLineEx(Vector2{ 175, 136 }, Vector2{ 220, 114 }, 2.5f, Color{ 220, 135, 40, 240 });
    DrawLineEx(Vector2{ 220, 114 }, Vector2{ 315, 116 }, 2.5f, Color{ 220, 135, 40, 240 });

    DrawLineEx(Vector2{ 110, 96 }, Vector2{ 134, 90 }, 2.5f, Color{ 210, 215, 230, 255 });

    DrawLineEx(Vector2{ 98, 76 }, Vector2{ 124, 76 }, 2.0f, Color{ 200, 160, 50, 240 });

    DrawLineEx(Vector2{ 175, 136 }, Vector2{ 150, 145 }, 2.0f, Color{ 200, 165, 60, 220 });

    DrawCircleLines(152, 34, 4.0f, Color{ 0, 200, 230, 255 });
    DrawCircleLines(132, 46, 3.0f, Color{ 0, 200, 230, 255 });

    DrawRectangle(176, 48, 8, 5, Color{ 245, 245, 250, 255 });
    DrawRectangleLines(175, 47, 10, 7, Color{ 0, 220, 255, 255 });

    DrawRectangle(134, 86, 7, 7, Color{ 240, 240, 245, 255 });
    DrawRectangleLines(133, 85, 9, 9, Color{ 0, 220, 255, 255 });

    DrawRectangle(184, 87, 3, 3, WHITE);
    DrawLine(180, 88, 191, 88, Color{ 0, 230, 255, 255 });
    DrawLine(185, 83, 185, 94, Color{ 0, 230, 255, 255 });

    DrawCircle(288, 78, 4, Color{ 245, 245, 250, 255 });
    DrawRectangle(284, 72, 8, 12, Color{ 200, 205, 220, 255 });
    DrawRectangleLines(283, 71, 10, 14, Color{ 0, 220, 255, 255 });

    DrawPoly(Vector2{ 75, 115 }, 5, 15, 0.0f, Color{ 35, 55, 75, 255 });
    DrawPolyLines(Vector2{ 75, 115 }, 5, 15, 0.0f, Color{ 0, 200, 220, 255 });
    DrawPolyLines(Vector2{ 75, 115 }, 5, 8, 0.0f, Color{ 0, 160, 180, 200 });

    DrawCircle(182, 110, 4, Color{ 240, 240, 245, 255 });
    DrawCircleLines(182, 110, 4, Color{ 0, 210, 240, 255 });

    auto drawTacticalLabel = [](const char* txt, int x, int y, int sz, Color col) {
        int w = MeasureText(txt, sz);
        DrawRectangle(x - 2, y - 1, w + 4, sz + 2, Color{ 8, 14, 25, 210 });
        DrawPixelText(txt, x, y, sz, col);
    };

    drawTacticalLabel("POTOMAC R.", 56, 54, 6, Color{ 30, 135, 195, 230 });
    drawTacticalLabel("PENTAGON", 52, 130, 7, Color{ 0, 190, 220, 230 });
    drawTacticalLabel("K ST NW", 116, 28, 6, Color{ 140, 175, 205, 220 });
    drawTacticalLabel("WHITE HOUSE", 132, 26, 7, Color{ 255, 230, 100, 255 });
    drawTacticalLabel("PENN AVE", 122, 38, 6, Color{ 230, 220, 140, 255 });
    drawTacticalLabel("CONSTITUTION AVE", 196, 72, 6, Color{ 170, 205, 235, 220 });
    drawTacticalLabel("THE MALL", 195, 85, 6, Color{ 130, 230, 160, 240 });
    drawTacticalLabel("INDEPENDENCE AVE", 196, 98, 6, Color{ 170, 205, 235, 220 });
    drawTacticalLabel("CAPITOL", 268, 62, 7, Color{ 255, 230, 100, 255 });
    drawTacticalLabel("I-395", 250, 120, 6, Color{ 240, 160, 50, 240 });
    drawTacticalLabel("LINCOLN MEM", 92, 90, 6, Color{ 0, 215, 245, 220 });
    drawTacticalLabel("TIDAL BASIN", 138, 108, 6, Color{ 35, 150, 215, 220 });

    DrawPixelText("RADAR: DC SECTOR 01", 210, 22, 6, Color{ 0, 210, 190, 200 });
    DrawPixelText("38.89 N / 77.03 W", 210, 30, 6, Color{ 0, 170, 160, 180 });

    Vector2 pins[6] = {
        { 180, 48 },
        { 158, 88 },
        { 288, 78 },
        { 180, 64 },
        { 168, 42 },
        { 82, 148 }
    };

    const char* pinNames[6] = {
        "1. EAST WING DEMO",
        "2. REFLECTING POOL",
        "3. CONGRESS FLOOR",
        "4. WHITE HOUSE MMA",
        "5. PRESS BRIEFING",
        "6. PENTAGON ALTAR"
    };

    for (int i = 0; i < 6; i++) {
        bool sel = (i == selectedStage);
        Color pinCol = sel ? Color{ 255, 230, 40, 255 } : Color{ 0, 180, 220, 255 };

        DrawCircle((int)pins[i].x, (int)pins[i].y, sel ? 6 : 4, pinCol);
        if (sel) {

            float pulse = 10.0f + sinf(timer * 8.0f) * 4.0f;
            DrawCircleLines((int)pins[i].x, (int)pins[i].y, pulse, pinCol);
            DrawLine((int)pins[i].x - 14, (int)pins[i].y, (int)pins[i].x + 14, (int)pins[i].y, pinCol);
            DrawLine((int)pins[i].x, (int)pins[i].y - 14, (int)pins[i].x, (int)pins[i].y + 14, pinCol);

            int pnw = MeasureText(pinNames[i], 8);
            int boxX = (int)pins[i].x + 10;
            int boxY = (int)pins[i].y - 8;
            if (boxX + pnw + 6 > SCREEN_WIDTH - 70) {
                boxX = (int)pins[i].x - pnw - 16;
            }
            DrawRectangle(boxX, boxY, pnw + 6, 14, Color{ 8, 14, 25, 230 });
            DrawRectangleLines(boxX, boxY, pnw + 6, 14, pinCol);
            DrawPixelText(pinNames[i], boxX + 3, boxY + 3, 8, pinCol);
        }
    }
    }

    int defeatedOriginalCount = 0;
    if (defeatedFighters != nullptr) {
        for (int i = 0; i < PLAYABLE_CHARACTER_COUNT; i++) {
            if (i != p1Char && defeatedFighters[i]) defeatedOriginalCount++;
        }
    }

    DrawRectangle(0, 0, SCREEN_WIDTH, 18, Color{ 10, 12, 18, 240 });
    const char* headerTitle = "SELECT YOUR FIGHTER";
    Color headerCol = Color{ 255, 220, 80, 255 };

    if (!playerLocked) {
        headerTitle = "SELECT YOUR FIGHTER";
        headerCol = Color{ 255, 220, 80, 255 };
    } else if (warPigBeaten) {
        if (!defeatedFighters[CHAR_SOCIALIST]) {
            headerTitle = "*** EAST COAST TOUR: NEW YORK (THE SOCIALIST) ***";
            headerCol = Color{ 80, 220, 255, 255 };
        } else if (!defeatedFighters[CHAR_BILLARY]) {
            headerTitle = "*** EAST COAST TOUR: D.C. BASEMENT (BILLARY) ***";
            headerCol = Color{ 255, 120, 240, 255 };
        } else if (!defeatedFighters[CHAR_PALM_BEACH_PETE]) {
            headerTitle = "*** GRAND FINALE: FLORIDA KEYS (PALM BEACH PETE) ***";
            headerCol = Color{ 255, 215, 0, 255 };
        } else {
            headerTitle = "*** TOTAL VETO GRAND CHAMPION! ***";
            headerCol = Color{ 255, 215, 0, 255 };
        }
    } else if (defeatedOriginalCount < 9) {
        headerTitle = "CHOOSE NEXT OPPONENT";
        headerCol = Color{ 255, 120, 80, 255 };
    } else if (!defeatedFighters[CHAR_SUCKRATERY_RKF]) {
        headerTitle = "*** BOSS ENCOUNTER: SUCKRATERY RKF ***";
        headerCol = Color{ 255, 60, 60, 255 };
    } else if (!defeatedFighters[CHAR_WAR_PIG]) {
        headerTitle = "*** FINAL BOSS: WAR PIG ***";
        headerCol = Color{ 255, 40, 40, 255 };
    } else {
        headerTitle = "*** TOTAL VETO GRAND CHAMPION! ***";
        headerCol = Color{ 255, 215, 0, 255 };
    }

    int stw = MeasureText(headerTitle, 9);
    DrawPixelText(headerTitle, (SCREEN_WIDTH - stw) / 2, 2, 9, headerCol);

    const char* eggHint = "EASTER EGG: HOLD LT+RT (OR Q+E / T) FOR TESLA BONUS";
    int ehw = MeasureText(eggHint, 6);
    DrawPixelText(eggHint, (SCREEN_WIDTH - ehw) / 2, 11, 6, Color{ 80, 200, 255, 220 });

    const char* pinNamesAll[STAGE_COUNT] = {
        "EAST WING DEMO",
        "REFLECTING POOL",
        "CONGRESS FLOOR",
        "WHITE HOUSE MMA",
        "PRESS BRIEFING",
        "PENTAGON ALTAR",
        "WTC PLAZA (PRE-9/11)",
        "COMET PING PONG",
        "TEMPLE ISLAND (FL KEYS)"
    };

    char venueBanner[64];
    snprintf(venueBanner, sizeof(venueBanner), "[STAGE: %s]", pinNamesAll[selectedStage < STAGE_COUNT ? selectedStage : 0]);
    int vbw = MeasureText(venueBanner, 8);
    DrawRectangle((SCREEN_WIDTH - vbw) / 2 - 4, 132, vbw + 8, 12, Color{ 10, 14, 22, 230 });
    DrawPixelText(venueBanner, (SCREEN_WIDTH - vbw) / 2, 134, 8, Color{ 0, 220, 255, 230 });

    const char* rosterNames[CHARACTER_COUNT] = {
        "POTUS", "HIGH BENCH", "REDCAP", "GOVERNATOR",
        "TECHBRO", "FILIBUSTER", "FREAGLE", "POPULIST",
        "SLEEPY JOE", "MARJORY T.G.",
        "SUCKRATERY RKF", "WAR PIG", "THE SOCIALIST", "BILLARY",
        "PALM BEACH PETE"
    };

    const char* rosterArchetypes[CHARACTER_COUNT] = {
        "Shoto All-Round", "Three Justices", "Grappler Slam", "Agile Rushdown",
        "Zoner Traps", "Turtle Defense", "Acrobatic Fur", "Mix-Up Kicks",
        "Power Nap Counter", "Trailer Brawler",
        "Brain Worm Cobra", "Pentagon Final Boss", "Produce & Cash",
        "Dual Clinton Electro", "Adrenochrome Mogul"
    };

    int p1BoxX = 10;
    int p1BoxY = 22;
    int p1BoxSize = 58;
    DrawRectangle(p1BoxX, p1BoxY, p1BoxSize, p1BoxSize, Color{ 15, 22, 38, 255 });
    if (s_loaded && s_portraits[p1Char].id > 0) {
        Rectangle p1Src = { 0.0f, 0.0f, (float)s_portraits[p1Char].width, (float)s_portraits[p1Char].height };
        Rectangle p1Dst = { (float)p1BoxX + 2, (float)p1BoxY + 2, (float)p1BoxSize - 4, (float)p1BoxSize - 4 };
        DrawTexturePro(s_portraits[p1Char], p1Src, p1Dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    DrawRectangleLines(p1BoxX, p1BoxY, p1BoxSize, p1BoxSize, Color{ 50, 150, 255, 255 });
    DrawPixelText(playerLocked ? "1P LOCKED" : "1P SELECT", p1BoxX, p1BoxY + p1BoxSize + 3, 9, Color{ 80, 190, 255, 255 });
    DrawPixelText(rosterNames[p1Char], p1BoxX, p1BoxY + p1BoxSize + 14, 10, Color{ 255, 220, 60, 255 });
    DrawPixelText(rosterArchetypes[p1Char], p1BoxX, p1BoxY + p1BoxSize + 26, 8, Color{ 170, 185, 200, 255 });

    int p2BoxX = SCREEN_WIDTH - 68;
    int p2BoxY = 22;
    int p2BoxSize = 58;
    DrawRectangle(p2BoxX, p2BoxY, p2BoxSize, p2BoxSize, Color{ 38, 18, 22, 255 });
    if (playerLocked) {
        if (s_loaded && s_portraits[p2Char].id > 0) {

            Rectangle p2Src = { (float)s_portraits[p2Char].width, 0.0f, -(float)s_portraits[p2Char].width, (float)s_portraits[p2Char].height };
            Rectangle p2Dst = { (float)p2BoxX + 2, (float)p2BoxY + 2, (float)p2BoxSize - 4, (float)p2BoxSize - 4 };
            DrawTexturePro(s_portraits[p2Char], p2Src, p2Dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }
        DrawRectangleLines(p2BoxX, p2BoxY, p2BoxSize, p2BoxSize, Color{ 255, 60, 60, 255 });
        const char* p2Tag = (p2Char >= 10) ? "BOSS TARGET" : "2P TARGET";
        int tagW = MeasureText(p2Tag, 9);
        int nameW = MeasureText(rosterNames[p2Char], 9);
        int archW = MeasureText(rosterArchetypes[p2Char], 7);
        int alignRight = SCREEN_WIDTH - 8;
        DrawPixelText(p2Tag, alignRight - tagW, p2BoxY + p2BoxSize + 3, 9, Color{ 255, 90, 90, 255 });
        DrawPixelText(rosterNames[p2Char], alignRight - nameW, p2BoxY + p2BoxSize + 14, 9, Color{ 255, 220, 60, 255 });
        DrawPixelText(rosterArchetypes[p2Char], alignRight - archW, p2BoxY + p2BoxSize + 25, 7, Color{ 170, 185, 200, 255 });
    } else {
        DrawRectangleLines(p2BoxX, p2BoxY, p2BoxSize, p2BoxSize, Color{ 90, 45, 55, 180 });
        int tagW = MeasureText("2P TARGET", 9);
        int hintW = MeasureText("PICK 1P FIRST", 7);
        int alignRight = SCREEN_WIDTH - 8;
        DrawPixelText("2P TARGET", alignRight - tagW, p2BoxY + p2BoxSize + 3, 9, Color{ 170, 85, 85, 200 });
        DrawPixelText("[LOCKED]", p2BoxX + 6, p2BoxY + 22, 9, Color{ 150, 140, 140, 200 });
        DrawPixelText("PICK 1P FIRST", alignRight - hintW, p2BoxY + p2BoxSize + 14, 7, Color{ 160, 160, 175, 200 });
    }

    constexpr int GRID_Y = 146;
    DrawRectangle(0, GRID_Y, SCREEN_WIDTH, SCREEN_HEIGHT - GRID_Y, Color{ 16, 20, 30, 255 });
    DrawLine(0, GRID_Y, SCREEN_WIDTH, GRID_Y, Color{ 200, 160, 40, 255 });

    if (warPigBeaten) {

        int cWidth = 118;
        int cHeight = 44;
        int cY = GRID_Y + 14;

        struct ECCard {
            int x;
            CharacterId id;
            const char* header;
            const char* name;
            const char* stage;
            bool locked;
            bool defeated;
        };

        bool socDef = (defeatedFighters != nullptr && defeatedFighters[CHAR_SOCIALIST]);
        bool bilDef = (defeatedFighters != nullptr && defeatedFighters[CHAR_BILLARY]);
        bool peteDef = (defeatedFighters != nullptr && defeatedFighters[CHAR_PALM_BEACH_PETE]);

        ECCard ecCards[3] = {
            { 8, CHAR_SOCIALIST, "LEG 1: NEW YORK", "THE SOCIALIST", "WTC (Pre-9/11)", false, socDef },
            { 133, CHAR_BILLARY, "LEG 2: D.C.", "BILLARY", "Comet Ping Pong", !socDef, bilDef },
            { 258, CHAR_PALM_BEACH_PETE, "LEG 3: FL KEYS", "PALM BEACH PETE", "Temple Island", !bilDef, peteDef }
        };

        for (int k = 0; k < 3; k++) {
            const auto& cd = ecCards[k];
            bool active = (p2Char == cd.id);
            Color bgCol = cd.defeated ? Color{ 15, 15, 20, 255 } : (cd.locked ? Color{ 18, 18, 22, 255 } : Color{ 28, 32, 46, 255 });
            DrawRectangle(cd.x, cY, cWidth, cHeight, bgCol);

            if (s_loaded && s_portraits[cd.id].id > 0) {
                Rectangle srcRec = { 0.0f, 0.0f, (float)s_portraits[cd.id].width, (float)s_portraits[cd.id].height };
                Rectangle dstRec = { (float)cd.x + 2, (float)cY + 2, (float)cHeight - 4, (float)cHeight - 4 };
                Color pTint = cd.locked ? Color{ 55, 55, 60, 180 } : (cd.defeated ? Color{ 80, 80, 85, 200 } : WHITE);
                DrawTexturePro(s_portraits[cd.id], srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, pTint);
            }

            if (cd.locked) {
                DrawRectangle(cd.x + 42, cY + 2, cWidth - 44, cHeight - 4, Color{ 16, 16, 22, 250 });
                int lockW = MeasureText("[ LOCKED ]", 8);
                DrawPixelText("[ LOCKED ]", cd.x + 42 + (cWidth - 44 - lockW) / 2, cY + 11, 8, Color{ 170, 170, 185, 220 });
                const char* prevHint = "BEAT PREV LEG";
                int hintW = MeasureText(prevHint, 6);
                DrawPixelText(prevHint, cd.x + 42 + (cWidth - 44 - hintW) / 2, cY + 25, 6, Color{ 130, 130, 145, 200 });
            } else {
                Color hdrCol = (k == 2 ? Color{ 255, 215, 50, 255 } : Color{ 80, 210, 255, 255 });
                DrawPixelText(cd.header, cd.x + 44, cY + 4, 7, hdrCol);
                DrawPixelText(cd.name, cd.x + 44, cY + 15, 8, Color{ 255, 220, 60, 255 });
                DrawPixelText(cd.stage, cd.x + 44, cY + 26, 7, Color{ 170, 190, 220, 220 });
            }

            if (cd.defeated) {
                DrawRectangle(cd.x + 2, cY + 2, cWidth - 4, cHeight - 4, Color{ 35, 0, 0, 140 });
                DrawLineEx(Vector2{ (float)cd.x + 4, (float)cY + 4 }, Vector2{ (float)cd.x + cWidth - 4, (float)cY + cHeight - 4 }, 2.0f, Color{ 230, 30, 30, 240 });
                DrawLineEx(Vector2{ (float)cd.x + cWidth - 4, (float)cY + 4 }, Vector2{ (float)cd.x + 4, (float)cY + cHeight - 4 }, 2.0f, Color{ 230, 30, 30, 240 });
                DrawRectangle(cd.x + 20, cY + 16, cWidth - 40, 12, Color{ 190, 20, 20, 230 });
                int dw = MeasureText("DEFEATED", 8);
                DrawPixelText("DEFEATED", cd.x + (cWidth - dw) / 2, cY + 18, 8, WHITE);
            }

            Color borderCol = active ? Color{ 255, 50, 50, 255 } : (cd.locked ? Color{ 50, 50, 60, 255 } : Color{ 80, 140, 200, 255 });
            DrawRectangleLines(cd.x, cY, cWidth, cHeight, borderCol);
            if (active) {
                DrawRectangleLines(cd.x - 2, cY - 2, cWidth + 4, cHeight + 4, Color{ 255, 215, 0, 255 });
                int ctW = MeasureText("CURRENT TARGET", 7);
                DrawPixelText("CURRENT TARGET", cd.x + (cWidth - ctW) / 2, cY - 9, 7, Color{ 255, 220, 50, 255 });
            }
        }
    } else if (defeatedOriginalCount >= 9) {

        int bWidth = 156;
        int bHeight = 44;

        int b1X = 24;
        int b1Y = GRID_Y + 14;
        bool b1Defeated = (defeatedFighters != nullptr && defeatedFighters[CHAR_SUCKRATERY_RKF]);
        bool b1Active = (p2Char == CHAR_SUCKRATERY_RKF);

        Color b1Bg = b1Defeated ? Color{ 15, 15, 20, 255 } : Color{ 35, 25, 45, 255 };
        DrawRectangle(b1X, b1Y, bWidth, bHeight, b1Bg);

        if (s_loaded && s_portraits[CHAR_SUCKRATERY_RKF].id > 0) {
            Rectangle srcRec = { 0.0f, 0.0f, (float)s_portraits[CHAR_SUCKRATERY_RKF].width, (float)s_portraits[CHAR_SUCKRATERY_RKF].height };
            Rectangle dstRec = { (float)b1X + 2, (float)b1Y + 2, (float)bHeight - 4, (float)bHeight - 4 };
            DrawTexturePro(s_portraits[CHAR_SUCKRATERY_RKF], srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, b1Defeated ? Color{ 85, 85, 90, 200 } : WHITE);
        }
        DrawPixelText("BOSS #1", b1X + 46, b1Y + 4, 8, Color{ 255, 120, 120, 255 });
        DrawPixelText("SUCKRATERY RKF", b1X + 46, b1Y + 15, 8, Color{ 255, 220, 60, 255 });
        DrawPixelText("Press Briefing Room", b1X + 46, b1Y + 26, 7, Color{ 170, 190, 220, 220 });

        if (b1Defeated) {
            DrawRectangle(b1X + 2, b1Y + 2, bWidth - 4, bHeight - 4, Color{ 35, 0, 0, 140 });
            DrawLineEx(Vector2{ (float)b1X + 4, (float)b1Y + 4 }, Vector2{ (float)b1X + bWidth - 4, (float)b1Y + bHeight - 4 }, 2.5f, Color{ 230, 30, 30, 240 });
            DrawLineEx(Vector2{ (float)b1X + bWidth - 4, (float)b1Y + 4 }, Vector2{ (float)b1X + 4, (float)b1Y + bHeight - 4 }, 2.5f, Color{ 230, 30, 30, 240 });
            DrawRectangle(b1X + 28, b1Y + 16, bWidth - 56, 12, Color{ 190, 20, 20, 230 });
            DrawPixelText("DEFEATED", b1X + 48, b1Y + 18, 9, WHITE);
        }

        Color b1Border = b1Active ? Color{ 255, 50, 50, 255 } : (b1Defeated ? Color{ 60, 40, 40, 255 } : Color{ 160, 80, 160, 255 });
        DrawRectangleLines(b1X, b1Y, bWidth, bHeight, b1Border);
        if (b1Active) {
            DrawRectangleLines(b1X - 2, b1Y - 2, bWidth + 4, bHeight + 4, Color{ 255, 215, 0, 255 });
            int ncW = MeasureText("NEXT CHALLENGE", 8);
            DrawPixelText("NEXT CHALLENGE", b1X + (bWidth - ncW) / 2, b1Y - 9, 8, Color{ 255, 220, 50, 255 });
        }

        int b2X = SCREEN_WIDTH - b1X - bWidth;
        int b2Y = b1Y;
        bool b2Defeated = (defeatedFighters != nullptr && defeatedFighters[CHAR_WAR_PIG]);
        bool b2Locked = !b1Defeated;
        bool b2Active = (p2Char == CHAR_WAR_PIG);

        Color b2Bg = b2Defeated ? Color{ 15, 15, 20, 255 } : (b2Locked ? Color{ 20, 20, 25, 255 } : Color{ 50, 20, 25, 255 });
        DrawRectangle(b2X, b2Y, bWidth, bHeight, b2Bg);

        if (s_loaded && s_portraits[CHAR_WAR_PIG].id > 0) {
            Rectangle srcRec = { 0.0f, 0.0f, (float)s_portraits[CHAR_WAR_PIG].width, (float)s_portraits[CHAR_WAR_PIG].height };
            Rectangle dstRec = { (float)b2X + 2, (float)b2Y + 2, (float)bHeight - 4, (float)bHeight - 4 };
            Color pTint = b2Locked ? Color{ 60, 60, 65, 180 } : (b2Defeated ? Color{ 85, 85, 90, 200 } : WHITE);
            DrawTexturePro(s_portraits[CHAR_WAR_PIG], srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, pTint);
        }

        if (b2Locked) {
            DrawRectangle(b2X + 44, b2Y + 2, bWidth - 46, bHeight - 4, Color{ 16, 16, 22, 250 });
            int lockW = MeasureText("[ LOCKED ]", 9);
            DrawPixelText("[ LOCKED ]", b2X + 44 + (bWidth - 46 - lockW) / 2, b2Y + 10, 9, Color{ 180, 180, 195, 230 });
            const char* defeatHint = "DEFEAT BOSS #1";
            int hintW = MeasureText(defeatHint, 7);
            DrawPixelText(defeatHint, b2X + 44 + (bWidth - 46 - hintW) / 2, b2Y + 24, 7, Color{ 140, 140, 155, 220 });
        } else {
            DrawPixelText("FINAL BOSS", b2X + 46, b2Y + 4, 8, Color{ 255, 60, 60, 255 });
            DrawPixelText("WAR PIG", b2X + 46, b2Y + 15, 8, Color{ 255, 220, 60, 255 });
            DrawPixelText("Pentagon Altar", b2X + 46, b2Y + 26, 7, Color{ 220, 160, 160, 220 });
        }

        if (b2Defeated) {
            DrawRectangle(b2X + 2, b2Y + 2, bWidth - 4, bHeight - 4, Color{ 35, 0, 0, 140 });
            DrawLineEx(Vector2{ (float)b2X + 4, (float)b2Y + 4 }, Vector2{ (float)b2X + bWidth - 4, (float)b2Y + bHeight - 4 }, 2.5f, Color{ 230, 30, 30, 240 });
            DrawLineEx(Vector2{ (float)b2X + bWidth - 4, (float)b2Y + 4 }, Vector2{ (float)b2X + 4, (float)b2Y + bHeight - 4 }, 2.5f, Color{ 230, 30, 30, 240 });
            DrawRectangle(b2X + 28, b2Y + 16, bWidth - 56, 12, Color{ 190, 20, 20, 230 });
            DrawPixelText("DEFEATED", b2X + 48, b2Y + 18, 9, WHITE);
        }

        Color b2Border = b2Active ? Color{ 255, 40, 40, 255 } : (b2Locked ? Color{ 60, 60, 70, 255 } : Color{ 200, 50, 50, 255 });
        DrawRectangleLines(b2X, b2Y, bWidth, bHeight, b2Border);
        if (b2Active) {
            DrawRectangleLines(b2X - 2, b2Y - 2, bWidth + 4, bHeight + 4, Color{ 255, 215, 0, 255 });
            int ftW = MeasureText("FINAL TARGET", 8);
            DrawPixelText("FINAL TARGET", b2X + (bWidth - ftW) / 2, b2Y - 9, 8, Color{ 255, 220, 50, 255 });
        }
    } else {

        int boxW = 46;
        int boxH = 26;
        int startX = 33;
        int colSpacing = 68;

        const char* rosterGridNames[PLAYABLE_CHARACTER_COUNT] = {
            "POTUS", "HIGH BENCH", "REDCAP", "GOVERNATOR", "TECHBRO",
            "FILIBUSTER", "FREAGLE", "POPULIST", "SLEEPY JOE", "M.T.G."
        };

        for (int i = 0; i < PLAYABLE_CHARACTER_COUNT; i++) {
            int col = i % 5;
            int row = i / 5;
            int bx = startX + col * colSpacing;
            int by = GRID_Y + 4 + row * 36;

            bool isDefeated = (defeatedFighters != nullptr && defeatedFighters[i]);
            Color cardBg = isDefeated ? Color{ 15, 15, 20, 255 } : Color{ 25, 30, 42, 255 };
            DrawRectangle(bx, by, boxW, boxH, cardBg);

            Color portraitTint = isDefeated ? Color{ 85, 85, 90, 200 } : WHITE;

            if (s_loaded && s_portraits[i].id > 0) {
                float targetAspect = (float)(boxW - 2) / (float)(boxH - 2);
                float cropW = (float)s_portraits[i].width;
                float cropH = cropW / targetAspect;
                float cropY = ((float)s_portraits[i].height - cropH) * 0.28f;
                if (cropY < 0.0f) cropY = 0.0f;
                if (cropY + cropH > (float)s_portraits[i].height) cropH = (float)s_portraits[i].height - cropY;
                Rectangle srcRec = { 0.0f, cropY, cropW, cropH };
                Rectangle dstRec = { (float)bx + 1, (float)by + 1, (float)boxW - 2, (float)boxH - 2 };
                DrawTexturePro(s_portraits[i], srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, portraitTint);
            } else {
                DrawCircle(bx + 23, by + 13, 10, isDefeated ? Color{ 70, 70, 70, 255 } : Color{ 200, 160, 120, 255 });
            }

            if (isDefeated) {
                DrawRectangle(bx + 1, by + 1, boxW - 2, boxH - 2, Color{ 35, 0, 0, 140 });
                DrawLineEx(Vector2{ (float)bx + 3, (float)by + 3 }, Vector2{ (float)bx + boxW - 3, (float)by + boxH - 3 }, 2.5f, Color{ 230, 30, 30, 240 });
                DrawLineEx(Vector2{ (float)bx + boxW - 3, (float)by + 3 }, Vector2{ (float)bx + 3, (float)by + boxH - 3 }, 2.5f, Color{ 230, 30, 30, 240 });
                DrawRectangle(bx, by + 8, boxW, 10, Color{ 190, 20, 20, 230 });
                DrawPixelText("DEFEATED", bx + 3, by + 9, 8, WHITE);
            }

            DrawRectangleLines(bx, by, boxW, boxH, isDefeated ? Color{ 55, 40, 40, 255 } : Color{ 65, 75, 95, 255 });

            if (i == p1Char) {
                DrawRectangleLines(bx - 2, by - 2, boxW + 4, boxH + 4, Color{ 50, 170, 255, 255 });
                DrawRectangle(bx, by, 14, 9, Color{ 15, 60, 180, 230 });
                DrawPixelText("1P", bx + 2, by + 1, 8, WHITE);
            }

            if (playerLocked && i == p2Char && !isDefeated) {
                DrawRectangleLines(bx - 1, by - 1, boxW + 2, boxH + 2, Color{ 245, 50, 50, 255 });
                DrawRectangle(bx + boxW - 14, by, 14, 9, Color{ 180, 20, 20, 230 });
                DrawPixelText("2P", bx + boxW - 13, by + 1, 8, WHITE);
            }

            int nw = MeasureText(rosterGridNames[i], 7);
            DrawPixelText(rosterGridNames[i], bx + (boxW - nw) / 2, by + boxH + 2, 7, isDefeated ? Color{ 110, 110, 120, 255 } : Color{ 230, 235, 245, 255 });
        }
    }
}

void VisualsEngine::DrawSecretSelectScreen(int step, int p1Char, int p2Char, int selectedStage, int difficulty, float timer) {

    DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 8, 12, 20, 255 }, Color{ 16, 24, 38, 255 });

    for (int x = 0; x < SCREEN_WIDTH; x += 32) {
        DrawLine(x, 0, x, SCREEN_HEIGHT, Color{ 20, 32, 50, 100 });
    }
    for (int y = 0; y < SCREEN_HEIGHT; y += 24) {
        DrawLine(0, y, SCREEN_WIDTH, y, Color{ 20, 32, 50, 100 });
    }

    DrawRectangle(0, 0, SCREEN_WIDTH, 20, Color{ 6, 10, 18, 255 });
    DrawLine(0, 20, SCREEN_WIDTH, 20, Color{ 255, 215, 40, 220 });

    const char* stepTitles[4] = {
        "[ STEP 1/4: CHOOSE YOUR FIGHTER (1P) ]",
        "[ STEP 2/4: CHOOSE YOUR OPPONENT (2P) ]",
        "[ STEP 3/4: CHOOSE BATTLE VENUE (STAGE) ]",
        "[ STEP 4/4: SELECT AI DIFFICULTY ]"
    };

    Color stepColors[4] = {
        Color{ 80, 200, 255, 255 },
        Color{ 255, 90, 90, 255 },
        Color{ 255, 220, 60, 255 },
        Color{ 255, 215, 0, 255 }
    };

    const char* activeTitle = stepTitles[std::clamp(step, 0, 3)];
    Color activeTitleCol = stepColors[std::clamp(step, 0, 3)];
    int atw = MeasureText(activeTitle, 9);
    DrawPixelText(activeTitle, (SCREEN_WIDTH - atw) / 2, 4, 9, activeTitleCol);

    const char* rosterNames[CHARACTER_COUNT] = {
        "POTUS", "HIGH BENCH", "REDCAP", "GOVERNATOR",
        "TECHBRO", "FILIBUSTER", "FREAGLE", "POPULIST",
        "SLEEPY JOE", "MARJORY T.G.",
        "SUCKRATERY", "WAR PIG", "SOCIALIST", "BILLARY",
        "P.B. PETE"
    };

    const char* rosterShortNames[CHARACTER_COUNT] = {
        "POTUS", "BENCH", "REDCAP", "GOV",
        "TECHBRO", "TWEED", "FREAGLE", "POP",
        "JOE", "MTG",
        "RKF", "WAR PIG", "ZOHRAN", "BILLARY",
        "PETE"
    };

    const char* rosterArchetypes[CHARACTER_COUNT] = {
        "Presidential Shoto", "Three Justices", "Brawler Grappler", "Kickboxer Rush",
        "Drone & Laser", "Turtle Filibuster", "Direct Action Furry", "Viral Kicks",
        "Power Nap Counter", "Trailer Brawler",
        "Brain Worm Cobra", "Pentagon Juggernaut", "Groceries & Cash", "Dual Clinton Laptop",
        "Florida Keys Boss"
    };

    const char* stageNames[STAGE_COUNT] = {
        "1. EAST WING DEMO",
        "2. REFLECTING POOL",
        "3. CONGRESS FLOOR",
        "4. WHITE HOUSE MMA",
        "5. PRESS BRIEFING",
        "6. PENTAGON ALTAR",
        "7. WTC PLAZA NYC",
        "8. COMET PING PONG",
        "9. TEMPLE ISLAND"
    };

    const char* stageCardTitles[STAGE_COUNT] = {
        "1. EAST WING",
        "2. REFLECT POOL",
        "3. CONGRESS",
        "4. W.H. MMA",
        "5. PRESS ROOM",
        "6. PENTAGON",
        "7. WTC PLAZA",
        "8. COMET PING",
        "9. TEMPLE ISLE"
    };

    const char* stageCardSub[STAGE_COUNT] = {
        "White House, D.C.",
        "National Mall",
        "Capitol Chamber",
        "South Lawn Cage",
        "Press Briefing",
        "War Altar",
        "Pre-9/11 NYC",
        "Basement Club",
        "Florida Keys"
    };

    const char* stageVenues[STAGE_COUNT] = {
        "White House - East Wing",
        "National Mall - Reflect Pool",
        "U.S. Capitol - House Floor",
        "White House - South Lawn",
        "Press Briefing Room",
        "Pentagon - War Altar",
        "NYC - WTC Plaza (1990s)",
        "Comet Ping Pong Basement",
        "Florida Keys - Private Isle"
    };

    int cardW = 62;
    int cardH = 64;
    int portW = 38;
    int portH = 34;

    int p1CardX = 6;
    int p1CardY = 22;
    DrawRectangle(p1CardX, p1CardY, cardW, cardH, Color{ 14, 22, 38, 255 });
    int port1X = p1CardX + (cardW - portW) / 2;
    int port1Y = p1CardY + 3;
    if (s_loaded && s_portraits[p1Char].id > 0) {
        Rectangle p1Src = { 0.0f, 0.0f, (float)s_portraits[p1Char].width, (float)s_portraits[p1Char].height };
        Rectangle p1Dst = { (float)port1X, (float)port1Y, (float)portW, (float)portH };
        DrawTexturePro(s_portraits[p1Char], p1Src, p1Dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    DrawRectangleLines(port1X, port1Y, portW, portH, (step == 0) ? Color{ 80, 210, 255, 255 } : Color{ 50, 90, 140, 255 });
    DrawRectangleLines(p1CardX, p1CardY, cardW, cardH, (step == 0) ? Color{ 80, 210, 255, 255 } : Color{ 40, 65, 100, 200 });
    const char* p1Tag = "PLAYER 1";
    int p1tw = MeasureText(p1Tag, 7);
    DrawPixelText(p1Tag, p1CardX + (cardW - p1tw) / 2, p1CardY + 40, 7, Color{ 80, 200, 255, 255 });
    int p1nw = MeasureText(rosterShortNames[p1Char], 7);
    DrawPixelText(rosterShortNames[p1Char], p1CardX + (cardW - p1nw) / 2, p1CardY + 51, 7, Color{ 255, 220, 60, 255 });

    int p2CardX = SCREEN_WIDTH - 6 - cardW;
    int p2CardY = 22;
    DrawRectangle(p2CardX, p2CardY, cardW, cardH, Color{ 36, 16, 22, 255 });
    int port2X = p2CardX + (cardW - portW) / 2;
    int port2Y = p2CardY + 3;
    if (s_loaded && s_portraits[p2Char].id > 0) {

        Rectangle p2Src = { (float)s_portraits[p2Char].width, 0.0f, -(float)s_portraits[p2Char].width, (float)s_portraits[p2Char].height };
        Rectangle p2Dst = { (float)port2X, (float)port2Y, (float)portW, (float)portH };
        DrawTexturePro(s_portraits[p2Char], p2Src, p2Dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    DrawRectangleLines(port2X, port2Y, portW, portH, (step == 1) ? Color{ 255, 60, 60, 255 } : Color{ 140, 50, 60, 255 });
    DrawRectangleLines(p2CardX, p2CardY, cardW, cardH, (step == 1) ? Color{ 255, 60, 60, 255 } : Color{ 120, 40, 50, 200 });
    const char* p2Tag = (p2Char >= 10) ? "BOSS 2P" : "PLAYER 2";
    int p2tw = MeasureText(p2Tag, 7);
    DrawPixelText(p2Tag, p2CardX + (cardW - p2tw) / 2, p2CardY + 40, 7, Color{ 255, 90, 90, 255 });
    int p2nw = MeasureText(rosterShortNames[p2Char], 7);
    DrawPixelText(rosterShortNames[p2Char], p2CardX + (cardW - p2nw) / 2, p2CardY + 51, 7, Color{ 255, 220, 60, 255 });

    int centerInfoW = 236;
    int centerInfoH = 64;
    int centerInfoX = (SCREEN_WIDTH - centerInfoW) / 2;
    int centerInfoY = 22;
    DrawRectangle(centerInfoX, centerInfoY, centerInfoW, centerInfoH, Color{ 10, 16, 26, 230 });
    DrawRectangleLines(centerInfoX, centerInfoY, centerInfoW, centerInfoH, Color{ 40, 60, 90, 255 });

    char vsMatchup[128];
    snprintf(vsMatchup, sizeof(vsMatchup), "%s  VS  %s", rosterNames[p1Char], rosterNames[p2Char]);
    int vsw = MeasureText(vsMatchup, 8);
    DrawPixelText(vsMatchup, centerInfoX + (centerInfoW - vsw) / 2, centerInfoY + 5, 8, Color{ 255, 220, 80, 255 });

    char stageInfo[128];
    snprintf(stageInfo, sizeof(stageInfo), "STAGE: %s", stageNames[selectedStage < STAGE_COUNT ? selectedStage : 0]);
    int siw = MeasureText(stageInfo, 7);
    DrawPixelText(stageInfo, centerInfoX + (centerInfoW - siw) / 2, centerInfoY + 18, 7, Color{ 0, 220, 255, 240 });

    const char* diffLabels[3] = { "DIFFICULTY: CASUAL", "DIFFICULTY: ARCADE", "DIFFICULTY: PRESIDENTIAL" };
    Color diffColors[3] = { Color{ 80, 220, 120, 255 }, Color{ 255, 215, 0, 255 }, Color{ 255, 60, 60, 255 } };
    int dfw = MeasureText(diffLabels[difficulty], 7);
    DrawPixelText(diffLabels[difficulty], centerInfoX + (centerInfoW - dfw) / 2, centerInfoY + 31, 7, diffColors[difficulty]);

    if (step == 0) {
        char banner[128];
        snprintf(banner, sizeof(banner), "1P: %s [%s]", rosterNames[p1Char], rosterArchetypes[p1Char]);
        int bw = MeasureText(banner, 7);
        DrawPixelText(banner, centerInfoX + (centerInfoW - bw) / 2, centerInfoY + 45, 7, Color{ 80, 200, 255, 255 });
    } else if (step == 1) {
        char banner[128];
        snprintf(banner, sizeof(banner), "2P: %s [%s]", rosterNames[p2Char], rosterArchetypes[p2Char]);
        int bw = MeasureText(banner, 7);
        DrawPixelText(banner, centerInfoX + (centerInfoW - bw) / 2, centerInfoY + 45, 7, Color{ 255, 120, 120, 255 });
    } else if (step == 2) {
        char banner[128];
        snprintf(banner, sizeof(banner), "VENUE: %s", stageVenues[selectedStage]);
        int bw = MeasureText(banner, 7);
        DrawPixelText(banner, centerInfoX + (centerInfoW - bw) / 2, centerInfoY + 45, 7, Color{ 255, 215, 60, 255 });
    } else {
        const char* banner = "SELECT AI TOURNAMENT DIFFICULTY";
        int bw = MeasureText(banner, 7);
        DrawPixelText(banner, centerInfoX + (centerInfoW - bw) / 2, centerInfoY + 45, 7, Color{ 255, 215, 0, 255 });
    }

    if (step == 0 || step == 1) {

        int boxW = 42;
        int boxH = 24;

        int startX0 = 6;
        int startY0 = 94;
        for (int i = 0; i < 8; i++) {
            int bx = startX0 + i * 47;
            int by = startY0;

            DrawRectangle(bx, by, boxW, boxH, Color{ 18, 24, 36, 255 });
            if (s_loaded && s_portraits[i].id > 0) {
                float targetAspect = (float)(boxW - 2) / (float)(boxH - 2);
                float cropW = (float)s_portraits[i].width;
                float cropH = cropW / targetAspect;
                float cropY = ((float)s_portraits[i].height - cropH) * 0.28f;
                if (cropY < 0.0f) cropY = 0.0f;
                if (cropY + cropH > (float)s_portraits[i].height) cropH = (float)s_portraits[i].height - cropY;
                Rectangle src = { 0.0f, cropY, cropW, cropH };
                Rectangle dst = { (float)bx + 1, (float)by + 1, (float)boxW - 2, (float)boxH - 2 };
                DrawTexturePro(s_portraits[i], src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }
            DrawRectangleLines(bx, by, boxW, boxH, Color{ 50, 70, 100, 255 });

            if (i == p1Char) {
                DrawRectangleLines(bx - 1, by - 1, boxW + 2, boxH + 2, Color{ 80, 200, 255, 255 });
                DrawRectangle(bx + 1, by + 1, 12, 7, Color{ 20, 80, 200, 240 });
                DrawPixelText("1P", bx + 2, by + 1, 6, WHITE);
            }
            if (i == p2Char) {
                DrawRectangleLines(bx - 1, by - 1, boxW + 2, boxH + 2, Color{ 255, 60, 60, 255 });
                DrawRectangle(bx + 1, by + boxH - 8, 12, 7, Color{ 200, 20, 20, 240 });
                DrawPixelText("2P", bx + 2, by + boxH - 8, 6, WHITE);
            }

            bool isCursor = (step == 0 && i == p1Char) || (step == 1 && i == p2Char);
            if (isCursor) {
                float pulse = 2.0f + sinf(timer * 8.0f) * 1.5f;
                DrawRectangleLines(bx - (int)pulse, by - (int)pulse, boxW + (int)pulse * 2, boxH + (int)pulse * 2, Color{ 255, 215, 0, 255 });
            }

            int fSize0 = (MeasureText(rosterShortNames[i], 6) > boxW + 2) ? 5 : 6;
            int nw0 = MeasureText(rosterShortNames[i], fSize0);
            DrawPixelText(rosterShortNames[i], bx + (boxW - nw0) / 2, by + boxH + 2, fSize0, Color{ 210, 220, 240, 255 });
        }

        int startX1 = 21;
        int startY1 = 138;
        for (int i = 8; i < 15; i++) {
            int col = i - 8;
            int bx = startX1 + col * 50;
            int by = startY1;

            DrawRectangle(bx, by, boxW, boxH, (i >= 10) ? Color{ 28, 18, 22, 255 } : Color{ 18, 24, 36, 255 });
            if (s_loaded && s_portraits[i].id > 0) {
                float targetAspect = (float)(boxW - 2) / (float)(boxH - 2);
                float cropW = (float)s_portraits[i].width;
                float cropH = cropW / targetAspect;
                float cropY = ((float)s_portraits[i].height - cropH) * 0.28f;
                if (cropY < 0.0f) cropY = 0.0f;
                if (cropY + cropH > (float)s_portraits[i].height) cropH = (float)s_portraits[i].height - cropY;
                Rectangle src = { 0.0f, cropY, cropW, cropH };
                Rectangle dst = { (float)bx + 1, (float)by + 1, (float)boxW - 2, (float)boxH - 2 };
                DrawTexturePro(s_portraits[i], src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }

            DrawRectangleLines(bx, by, boxW, boxH, (i >= 10) ? Color{ 220, 45, 45, 255 } : Color{ 50, 70, 100, 255 });

            if (i >= 10) {
                DrawRectangle(bx + boxW - 8, by + 1, 7, 7, Color{ 200, 20, 20, 255 });
                DrawPixelText("B", bx + boxW - 7, by + 1, 6, Color{ 255, 220, 50, 255 });
            }

            if (i == p1Char) {
                DrawRectangleLines(bx - 1, by - 1, boxW + 2, boxH + 2, Color{ 80, 200, 255, 255 });
                DrawRectangle(bx + 1, by + 1, 12, 7, Color{ 20, 80, 200, 240 });
                DrawPixelText("1P", bx + 2, by + 1, 6, WHITE);
            }
            if (i == p2Char) {
                DrawRectangleLines(bx - 1, by - 1, boxW + 2, boxH + 2, Color{ 255, 60, 60, 255 });
                DrawRectangle(bx + 1, by + boxH - 8, 12, 7, Color{ 200, 20, 20, 240 });
                DrawPixelText("2P", bx + 2, by + boxH - 8, 6, WHITE);
            }

            bool isCursor = (step == 0 && i == p1Char) || (step == 1 && i == p2Char);
            if (isCursor) {
                float pulse = 2.0f + sinf(timer * 8.0f) * 1.5f;
                DrawRectangleLines(bx - (int)pulse, by - (int)pulse, boxW + (int)pulse * 2, boxH + (int)pulse * 2, Color{ 255, 215, 0, 255 });
            }

            int fSize1 = (MeasureText(rosterShortNames[i], 6) > boxW + 2) ? 5 : 6;
            int nw1 = MeasureText(rosterShortNames[i], fSize1);
            DrawPixelText(rosterShortNames[i], bx + (boxW - nw1) / 2, by + boxH + 2, fSize1, (i >= 10) ? Color{ 255, 120, 120, 255 } : Color{ 210, 220, 240, 255 });
        }
    } else if (step == 2) {

        int cardW = 114;
        int cardH = 26;
        for (int i = 0; i < STAGE_COUNT; i++) {
            int col = i % 3;
            int row = i / 3;
            int cx = 16 + col * 120;
            int cy = 92 + row * 30;

            bool isSelected = (i == selectedStage);
            DrawRectangle(cx, cy, cardW, cardH, isSelected ? Color{ 25, 45, 75, 255 } : Color{ 14, 20, 32, 255 });

            Color borderCol = isSelected ? Color{ 255, 215, 40, 255 } : Color{ 50, 75, 110, 255 };
            DrawRectangleLines(cx, cy, cardW, cardH, borderCol);

            if (isSelected) {
                DrawRectangleLines(cx - 2, cy - 2, cardW + 4, cardH + 4, Color{ 0, 220, 255, 255 });
                DrawRectangle(cx + 2, cy + 2, 5, cardH - 4, Color{ 255, 215, 0, 255 });
            }

            DrawPixelText(stageCardTitles[i], cx + (isSelected ? 10 : 5), cy + 3, 7, isSelected ? Color{ 255, 230, 80, 255 } : Color{ 190, 205, 230, 255 });
            DrawPixelText(stageCardSub[i], cx + (isSelected ? 10 : 5), cy + 13, 6, isSelected ? Color{ 0, 220, 255, 240 } : Color{ 120, 140, 170, 220 });
        }
    }

    if (step == 3) {

        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 160 });

        int modalW = 280;
        int modalH = 138;
        int modalX = (SCREEN_WIDTH - modalW) / 2;
        int modalY = 40;

        DrawRectangle(modalX, modalY, modalW, modalH, Color{ 10, 15, 26, 250 });
        DrawRectangleLines(modalX, modalY, modalW, modalH, Color{ 255, 215, 0, 255 });
        DrawRectangleLines(modalX + 2, modalY + 2, modalW - 4, modalH - 4, Color{ 60, 90, 140, 240 });

        const char* mTitle = "[ SELECT AI DIFFICULTY LEVEL ]";
        int mtw = MeasureText(mTitle, 9);
        DrawPixelText(mTitle, modalX + (modalW - mtw) / 2, modalY + 8, 9, Color{ 255, 220, 60, 255 });

        const char* optNames[3] = {
            "CASUAL (EASY)",
            "ARCADE (NORMAL)",
            "PRESIDENTIAL (HARD)"
        };
        const char* optDesc[3] = {
            "Relaxed pacing, forgiving windows, low block rate",
            "Authentic Capcom CPS-2 coin-op tournament combat",
            "Ruthless punishing AI, high block rate, instant anti-airs"
        };

        for (int d = 0; d < 3; d++) {
            int oy = modalY + 24 + d * 27;
            bool sel = (d == difficulty);

            Color optBg = sel ? Color{ 30, 55, 95, 255 } : Color{ 16, 22, 34, 255 };
            DrawRectangle(modalX + 12, oy, modalW - 24, 24, optBg);
            DrawRectangleLines(modalX + 12, oy, modalW - 24, 24, sel ? Color{ 255, 215, 0, 255 } : Color{ 40, 60, 85, 255 });

            if (sel) {
                float arrowPulse = sinf(timer * 8.0f) * 2.0f;
                int lx = modalX + 18 - (int)arrowPulse;
                int ly = oy + 12;
                DrawTriangle(Vector2{ (float)lx + 6, (float)ly },
                             Vector2{ (float)lx, (float)ly - 4 },
                             Vector2{ (float)lx, (float)ly + 4 },
                             Color{ 255, 215, 0, 255 });

                int rx = modalX + modalW - 18 + (int)arrowPulse;
                DrawTriangle(Vector2{ (float)rx - 6, (float)ly },
                             Vector2{ (float)rx, (float)ly + 4 },
                             Vector2{ (float)rx, (float)ly - 4 },
                             Color{ 255, 215, 0, 255 });
            }

            DrawPixelText(optNames[d], modalX + 28, oy + 4, 8, sel ? Color{ 255, 230, 80, 255 } : Color{ 180, 195, 220, 255 });
            DrawPixelText(optDesc[d], modalX + 28, oy + 13, 6, sel ? Color{ 0, 220, 255, 240 } : Color{ 120, 140, 170, 220 });
        }

        const char* confirmHint = "UP/DOWN: SELECT | ATTACK: START FIGHT!";
        int chw = MeasureText(confirmHint, 7);
        DrawPixelText(confirmHint, modalX + (modalW - chw) / 2, modalY + modalH - 18, 7, Color{ 255, 215, 50, 240 });
    }

    DrawRectangle(0, SCREEN_HEIGHT - 16, SCREEN_WIDTH, 16, Color{ 6, 8, 14, 255 });
    DrawLine(0, SCREEN_HEIGHT - 16, SCREEN_WIDTH, SCREEN_HEIGHT - 16, Color{ 50, 70, 100, 255 });

    const char* navHint = "MOVE: D-PAD | CONFIRM: ATTACK | [SELECT/TAB]: TITLE";
    int nhw = MeasureText(navHint, 7);
    DrawPixelText(navHint, (SCREEN_WIDTH - nhw) / 2, SCREEN_HEIGHT - 12, 7, Color{ 190, 210, 235, 230 });
}

void VisualsEngine::DrawVersusScreen(const Fighter& p1, const Fighter& p2, float timer) {

    DrawRectangleGradientH(0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, Color{ 15, 30, 95, 255 }, Color{ 8, 12, 35, 255 });
    DrawRectangleGradientH(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, Color{ 10, 12, 35, 255 }, Color{ 120, 15, 25, 255 });

    int p1Char = (int)p1.GetCharacterId();
    int p2Char = (int)p2.GetCharacterId();

    float slide = std::min(1.0f, timer / 0.35f);
    float ease = 1.0f - (1.0f - slide) * (1.0f - slide);

    int p1X = (int)(-80.0f + ease * 104.0f);
    int p1Y = 48;
    int portraitSize = 100;

    DrawRectangle(p1X - 2, p1Y - 2, portraitSize + 4, portraitSize + 4, Color{ 10, 25, 60, 240 });
    if (s_loaded && s_portraits[p1Char].id > 0) {
        Rectangle p1Src = { 0.0f, 0.0f, (float)s_portraits[p1Char].width, (float)s_portraits[p1Char].height };
        Rectangle p1Dst = { (float)p1X, (float)p1Y, (float)portraitSize, (float)portraitSize };
        DrawTexturePro(s_portraits[p1Char], p1Src, p1Dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    DrawRectangleLines(p1X - 2, p1Y - 2, portraitSize + 4, portraitSize + 4, Color{ 50, 160, 255, 255 });

    int p2X = (int)(SCREEN_WIDTH - 24.0f - ease * portraitSize);
    int p2Y = 48;

    DrawRectangle(p2X - 2, p2Y - 2, portraitSize + 4, portraitSize + 4, Color{ 60, 15, 20, 240 });
    if (s_loaded && s_portraits[p2Char].id > 0) {
        Rectangle p2Src = { (float)s_portraits[p2Char].width, 0.0f, -(float)s_portraits[p2Char].width, (float)s_portraits[p2Char].height };
        Rectangle p2Dst = { (float)p2X, (float)p2Y, (float)portraitSize, (float)portraitSize };
        DrawTexturePro(s_portraits[p2Char], p2Src, p2Dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }
    DrawRectangleLines(p2X - 2, p2Y - 2, portraitSize + 4, portraitSize + 4, Color{ 255, 60, 60, 255 });

    Vector2 topPt{ (float)SCREEN_WIDTH / 2 + 25.0f, 0.0f };
    Vector2 botPt{ (float)SCREEN_WIDTH / 2 - 25.0f, (float)SCREEN_HEIGHT };
    DrawLineEx(topPt, botPt, 5.0f, Color{ 255, 240, 100, 255 });
    DrawLineEx(Vector2{ topPt.x - 2, topPt.y }, Vector2{ botPt.x - 2, botPt.y }, 2.0f, WHITE);

    int vsX = (SCREEN_WIDTH / 2) - 22;
    int vsY = (SCREEN_HEIGHT / 2) - 22;
    DrawRectangle(vsX - 4, vsY - 2, 44, 40, Color{ 10, 12, 18, 220 });
    DrawRectangleLines(vsX - 4, vsY - 2, 44, 40, Color{ 255, 200, 40, 255 });
    DrawPixelText("VS", vsX + 2, vsY + 2, 38, Color{ 255, 225, 40, 255 });

    DrawPixelText("PLAYER 1", 24, 20, 13, Color{ 100, 190, 255, 255 });
    DrawPixelText(p1.GetName().c_str(), 24, p1Y + portraitSize + 8, 14, Color{ 255, 230, 80, 255 });

    int n2W = MeasureText(p2.GetName().c_str(), 14);
    DrawPixelText("PLAYER 2", SCREEN_WIDTH - 85, 20, 13, Color{ 255, 100, 100, 255 });
    DrawPixelText(p2.GetName().c_str(), SCREEN_WIDTH - 24 - n2W, p2Y + portraitSize + 8, 14, Color{ 255, 230, 80, 255 });

    DrawPixelText("GET READY TO FIGHT!", 124, SCREEN_HEIGHT - 22, 12, Color{ 255, 235, 100, 255 });
}

void VisualsEngine::DrawMatchOver(const Fighter& winner, float timer) {
    DrawRectangle(0, (SCREEN_HEIGHT / 2) - 40, SCREEN_WIDTH, 80, Color{ 10, 12, 18, 230 });
    DrawLine(0, (SCREEN_HEIGHT / 2) - 40, SCREEN_WIDTH, (SCREEN_HEIGHT / 2) - 40, Color{ 240, 190, 40, 255 });
    DrawLine(0, (SCREEN_HEIGHT / 2) + 40, SCREEN_WIDTH, (SCREEN_HEIGHT / 2) + 40, Color{ 240, 190, 40, 255 });

    std::string winText = winner.GetName() + " WINS!";
    int tw = MeasureText(winText.c_str(), 24);
    DrawPixelText(winText.c_str(), (SCREEN_WIDTH - tw) / 2, (SCREEN_HEIGHT / 2) - 24, 24, Color{ 255, 220, 60, 255 });
    DrawPixelText("TOTAL VETO EXECUTION COMPLETE", 70, (SCREEN_HEIGHT / 2) + 8, 12, Color{ 255, 80, 80, 255 });
}

void VisualsEngine::DrawVictorySplash(int winnerChar, int loserChar, float timer) {

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 255 });

    DrawRectangle(0, 0, SCREEN_WIDTH, 20, Color{ 15, 18, 28, 255 });
    DrawLine(0, 20, SCREEN_WIDTH, 20, Color{ 220, 180, 40, 255 });
    DrawPixelText("TOTAL VETO - BATTLE VERDICT", 95, 5, 11, Color{ 255, 215, 50, 255 });

    const char* rosterNames[CHARACTER_COUNT] = {
        "POTUS", "THE HIGH BENCH", "REDCAP BRAWLER", "GOVERNATOR",
        "TECHBRO", "THE FILIBUSTER", "FREAGLE", "JUNIOR POPULIST",
        "SLEEPY JOE", "MARJORY T. GREEN",
        "SUCKRATERY RKF", "WAR PIG", "THE SOCIALIST", "BILLARY",
        "PALM BEACH PETE"
    };

    int pSize = 78;
    int winX = 24;
    int winY = 28;
    DrawRectangle(winX - 2, winY - 2, pSize + 4, pSize + 4, Color{ 15, 30, 60, 255 });
    if (s_loaded && s_portraits[winnerChar].id > 0) {
        Rectangle srcRec = { 0.0f, 0.0f, (float)s_portraits[winnerChar].width, (float)s_portraits[winnerChar].height };
        Rectangle dstRec = { (float)winX, (float)winY, (float)pSize, (float)pSize };
        DrawTexturePro(s_portraits[winnerChar], srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }

    DrawRectangleLines(winX - 2, winY - 2, pSize + 4, pSize + 4, Color{ 255, 215, 0, 255 });
    DrawRectangleLines(winX - 4, winY - 4, pSize + 8, pSize + 8, Color{ 180, 140, 20, 255 });

    DrawRectangle(winX, winY + pSize + 4, pSize, 12, Color{ 20, 80, 160, 240 });
    DrawPixelText("WINNER!", winX + 16, winY + pSize + 5, 9, Color{ 255, 240, 80, 255 });
    int wnw = MeasureText(rosterNames[winnerChar], 8);
    DrawPixelText(rosterNames[winnerChar], winX + (pSize - wnw) / 2, winY + pSize + 18, 8, Color{ 255, 220, 60, 255 });

    int loseX = SCREEN_WIDTH - winX - pSize;
    int loseY = winY;
    DrawRectangle(loseX - 2, loseY - 2, pSize + 4, pSize + 4, Color{ 40, 15, 18, 255 });

    if (s_loaded && s_portraitsLose[loserChar].id > 0) {
        Rectangle srcRec = { (float)s_portraitsLose[loserChar].width, 0.0f, -(float)s_portraitsLose[loserChar].width, (float)s_portraitsLose[loserChar].height };
        Rectangle dstRec = { (float)loseX, (float)loseY, (float)pSize, (float)pSize };
        DrawTexturePro(s_portraitsLose[loserChar], srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    } else if (s_loaded && s_portraits[loserChar].id > 0) {
        Rectangle srcRec = { (float)s_portraits[loserChar].width, 0.0f, -(float)s_portraits[loserChar].width, (float)s_portraits[loserChar].height };
        Rectangle dstRec = { (float)loseX, (float)loseY, (float)pSize, (float)pSize };
        DrawTexturePro(s_portraits[loserChar], srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, Color{ 140, 100, 100, 255 });
    }

    DrawRectangleLines(loseX - 2, loseY - 2, pSize + 4, pSize + 4, Color{ 220, 40, 40, 255 });
    DrawRectangleLines(loseX - 4, loseY - 4, pSize + 8, pSize + 8, Color{ 150, 20, 20, 255 });

    DrawRectangle(loseX, loseY + pSize + 4, pSize, 12, Color{ 140, 20, 20, 240 });
    DrawPixelText("VETOED!", loseX + 16, loseY + pSize + 5, 9, Color{ 255, 120, 120, 255 });
    int lnw = MeasureText(rosterNames[loserChar], 8);
    DrawPixelText(rosterNames[loserChar], loseX + (pSize - lnw) / 2, loseY + pSize + 18, 8, Color{ 220, 160, 160, 255 });

    int centerX = SCREEN_WIDTH / 2;
    int centerY = winY + 36;
    DrawCircle(centerX, centerY, 28.0f, Color{ 20, 25, 40, 255 });
    DrawCircleLines(centerX, centerY, 28.0f, Color{ 255, 215, 0, 255 });
    DrawCircleLines(centerX, centerY, 25.0f, Color{ 200, 40, 40, 255 });
    int vtw = MeasureText("VETO", 16);
    DrawPixelText("VETO", centerX - vtw / 2, centerY - 8, 16, Color{ 255, 220, 40, 255 });

    int boxX = 18;
    int boxY = 138;
    int boxW = SCREEN_WIDTH - 36;
    int boxH = 68;

    DrawRectangle(boxX, boxY, boxW, boxH, Color{ 12, 18, 32, 250 });
    DrawRectangleLines(boxX, boxY, boxW, boxH, Color{ 220, 180, 50, 255 });
    DrawRectangleLines(boxX + 2, boxY + 2, boxW - 4, boxH - 4, Color{ 90, 130, 180, 255 });

    char speakerBuf[64];
    snprintf(speakerBuf, sizeof(speakerBuf), "[ %s ]", rosterNames[winnerChar]);
    DrawPixelText(speakerBuf, boxX + 10, boxY + 6, 8, Color{ 255, 220, 50, 255 });

    const char* line1[CHARACTER_COUNT] = {
        "\"Nobody wins bigger than me, believe me!",
        "\"By a 9-0 unanimous verdict, you have",
        "\"You can't filibuster a clothesline, brother!",
        "\"Your approval ratings just fell off a cliff!",
        "\"According to my calculations and calculator,",
        "\"Point of order! I've held this floor 35 years,",
        "\"*SCREEECH!* Freedom has spoken, and it just",
        "\"Hold that pose! This knockout is going straight",
        "\"Look, folks... not a joke, this is a big deal!",
        "\"THE SPACE LASERS PREDICTED THIS KNOCKOUT!",
        "\"My brain worm calculated every move you had!",
        "\"Maximum military aggression! You just took on",
        "\"Free groceries, subway rides, and knockouts!",
        "\"I did not have combat relations with that boxer!",
        "\"Another refreshing sip of pure Adrenochrome..."
    };

    const char* line2[CHARACTER_COUNT] = {
        "That was the greatest knockout in history!\"",
        "been sentenced to the canvas. Case closed!\"",
        "Take that sob story back to the nosebleeds!\"",
        "Better hire a crisis PR firm, pal!\"",
        "your win probability was exactly 0.00%!\"",
        "and I'm not yielding today, youngster!\"",
        "dropkicked your agenda into next week!\"",
        "to my viral livestream highlight reel!\"",
        "Now if you'll excuse me, it's naptime... Zzz...\"",
        "WWG1WGA! YOU CAN'T HANDLE THE RAW TRUTH!\"",
        "Drink some raw tonic and accept your defeat!\"",
        "the Pentagon's highest caliber firepower!\"",
        "Spread the joy and feel the progressive burn!\"",
        "Your loss was wiped from my private server!\"",
        "The island elite always triumph in paradise!\""
    };

    DrawPixelText(line1[winnerChar], boxX + 10, boxY + 20, 8, Color{ 240, 245, 255, 255 });
    DrawPixelText(line2[winnerChar], boxX + 10, boxY + 34, 8, Color{ 240, 245, 255, 255 });

    if (((int)(timer * 3.0f) % 2) == 0) {
        int cw = MeasureText("PRESS ANY BUTTON TO ADVANCE", 8);
        DrawPixelText("PRESS ANY BUTTON TO ADVANCE", (SCREEN_WIDTH - cw) / 2, boxY + 52, 8, Color{ 255, 230, 60, 255 });
    }
}

void VisualsEngine::DrawBonusStage(float carHp, float maxCarHp, float timer, const Fighter& player, const std::vector<SparkEffect>& sparks) {

    if (s_bonusStageBg.id > 0) {
        Rectangle src = { 0.0f, 0.0f, (float)s_bonusStageBg.width, (float)s_bonusStageBg.height };
        Rectangle dst = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
        DrawTexturePro(s_bonusStageBg, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 60, 80, 110, 255 });
        DrawRectangle(0, (int)GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - (int)GROUND_Y, Color{ 40, 45, 50, 255 });
    }

    float carCenterX = (float)SCREEN_WIDTH / 2.0f;
    DrawEllipse((int)carCenterX, (int)GROUND_Y + 1, 78, 11, Color{ 15, 20, 25, 170 });

    float hpRatio = carHp / maxCarHp;
    int carStage = 0;
    if (hpRatio <= 0.0f) carStage = 3;
    else if (hpRatio <= 0.25f) carStage = 3;
    else if (hpRatio <= 0.50f) carStage = 2;
    else if (hpRatio <= 0.75f) carStage = 1;
    else carStage = 0;

    float carW = 154.0f;
    float carH = 70.0f;
    Rectangle carDst = { carCenterX - carW / 2.0f, GROUND_Y - carH + 4.0f, carW, carH };

    if (s_teslaSprites[carStage].id > 0) {
        Texture2D tTex = s_teslaSprites[carStage];
        Rectangle tSrc = { 0.0f, 0.0f, (float)tTex.width, (float)tTex.height };
        DrawTexturePro(tTex, tSrc, carDst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        Color carCol = (carStage == 3) ? Color{ 90, 40, 40, 255 } : Color{ 200, 25, 35, 255 };
        DrawRectangle((int)carDst.x, (int)carDst.y + 20, (int)carW, (int)carH - 20, carCol);
    }

    if (carStage >= 2) {
        for (int p = 0; p < 4; p++) {
            float puffT = fmodf(timer * 2.0f + p * 0.4f, 1.0f);
            float smokeX = carDst.x + 35.0f + sinf(timer * 4.0f + p) * 10.0f;
            float smokeY = carDst.y + 15.0f - puffT * 40.0f;
            float smokeR = 6.0f + puffT * 12.0f;
            DrawCircle((int)smokeX, (int)smokeY, smokeR, Color{ 60, 65, 70, (unsigned char)(140 * (1.0f - puffT)) });
        }
    }
    if (carStage == 3) {
        for (int f = 0; f < 3; f++) {
            float flX = carDst.x + 28.0f + f * 12.0f;
            float flH = 8.0f + (rand() % 10);
            DrawTriangle(Vector2{ flX - 6, carDst.y + 18 }, Vector2{ flX, carDst.y + 18 - flH }, Vector2{ flX + 6, carDst.y + 18 }, Color{ 255, 140, 20, 230 });
            DrawTriangle(Vector2{ flX - 3, carDst.y + 18 }, Vector2{ flX, carDst.y + 18 - flH * 0.6f }, Vector2{ flX + 3, carDst.y + 18 }, Color{ 255, 240, 50, 255 });
        }
        if ((rand() % 4) == 0) {
            float arcX = carDst.x + 40.0f + (rand() % 70);
            DrawLineEx(Vector2{ arcX, GROUND_Y - 2 }, Vector2{ arcX + (rand()%14 - 7), GROUND_Y - 8 }, 2.0f, Color{ 0, 220, 255, 255 });
            DrawLineEx(Vector2{ arcX, GROUND_Y - 2 }, Vector2{ arcX + 6, GROUND_Y }, 1.5f, WHITE);
        }
    }

    DrawDropShadow(player);
    DrawFighterSprite(player);
    DrawProjectiles(player);

    DrawSparks(sparks);

    DrawRectangle(0, 0, SCREEN_WIDTH, 18, Color{ 10, 14, 22, 230 });
    DrawPixelText("BONUS STAGE: TESLA BEATDOWN", 12, 4, 10, Color{ 255, 220, 60, 255 });

    int sec = (int)std::max(0.0f, ceilf(timer));
    char secBuf[16];
    snprintf(secBuf, sizeof(secBuf), "%02d", sec);
    Color timerCol = (sec <= 5) ? Color{ 255, 60, 60, 255 } : Color{ 255, 230, 40, 255 };
    int tw = MeasureText(secBuf, 22);
    DrawPixelText(secBuf, (SCREEN_WIDTH - tw) / 2, 2, 22, timerCol);

    int barW = 140;
    int barH = 7;
    int barX = (SCREEN_WIDTH - barW) / 2;
    int barY = 28;

    DrawRectangle(barX - 2, barY - 2, barW + 4, barH + 4, Color{ 20, 20, 25, 255 });
    int fillW = (int)(barW * std::clamp(hpRatio, 0.0f, 1.0f));
    Color barCol = (hpRatio > 0.5f) ? Color{ 230, 40, 40, 255 } : (hpRatio > 0.25f ? Color{ 240, 160, 30, 255 } : Color{ 255, 230, 40, 255 });
    DrawRectangle(barX, barY, fillW, barH, barCol);
    DrawRectangleLines(barX - 1, barY - 1, barW + 2, barH + 2, Color{ 220, 190, 60, 255 });

    int lblW = MeasureText("TESLA MODEL S", 7);
    DrawPixelText("TESLA MODEL S", (SCREEN_WIDTH - lblW) / 2, barY + barH + 3, 7, Color{ 0, 220, 255, 240 });

    if (carHp <= 0.0f) {
        int cw = MeasureText("BONUS STAGE CLEAR!", 20);
        DrawPixelText("BONUS STAGE CLEAR!", (SCREEN_WIDTH - cw) / 2, 75, 20, Color{ 255, 225, 40, 255 });
        int pw = MeasureText("+50,000 PTS - TOTAL WRECKAGE!", 11);
        DrawPixelText("+50,000 PTS - TOTAL WRECKAGE!", (SCREEN_WIDTH - pw) / 2, 102, 11, Color{ 0, 240, 255, 255 });
    } else if (timer <= 0.0f) {
        int tuw = MeasureText("TIME UP!", 24);
        DrawPixelText("TIME UP!", (SCREEN_WIDTH - tuw) / 2, 80, 24, Color{ 255, 60, 60, 255 });
    }
}

void VisualsEngine::DrawEndingCelebration(float timer) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 255 });

    if (s_loaded && s_endingSavior.id > 0) {
        Rectangle srcRec = { 0.0f, 0.0f, (float)s_endingSavior.width, (float)s_endingSavior.height };
        Rectangle dstRec = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
        DrawTexturePro(s_endingSavior, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }

    float divinePulse = 0.5f + 0.5f * sinf(timer * 4.0f);
    DrawCircleLines(SCREEN_WIDTH / 2, 45, 30.0f + divinePulse * 8.0f, Color{ 255, 240, 120, (unsigned char)(140 * divinePulse) });
    DrawCircleLines(SCREEN_WIDTH / 2, 45, 42.0f + divinePulse * 12.0f, Color{ 255, 215, 0, (unsigned char)(100 * divinePulse) });

    for (int i = 0; i < 40; i++) {
        float cx = fmodf((float)(i * 47) + sinf(timer * 2.5f + i) * 15.0f, (float)SCREEN_WIDTH);
        float cy = fmodf((float)(i * 31) + timer * (45.0f + (i % 5) * 12.0f), (float)SCREEN_HEIGHT);
        Color cCols[5] = {
            Color{ 255, 215, 0, 240 },
            Color{ 255, 60, 60, 240 },
            Color{ 50, 180, 255, 240 },
            Color{ 255, 255, 255, 240 },
            Color{ 80, 230, 90, 240 }
        };
        DrawRectangle((int)cx, (int)cy, (i % 2 == 0 ? 3 : 2), (i % 3 == 0 ? 5 : 3), cCols[i % 5]);
    }

    if (fmodf(timer, 0.9f) < 0.55f) {
        int botY = SCREEN_HEIGHT - 18;
        DrawRectangle(0, botY, SCREEN_WIDTH, 18, Color{ 10, 14, 24, 210 });
        const char* msg = "[ PRESS START / ENTER TO RETURN TO TITLE SCREEN ]";
        int pw = MeasureText(msg, 9);
        DrawPixelText(msg, (SCREEN_WIDTH - pw) / 2, botY + 5, 9, Color{ 255, 225, 50, 255 });
    }
}

void VisualsEngine::DrawCthulhuEnding(float timer) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);

    if (timer < 5.0f) {
        if (s_loaded && s_endingCthulhuThrow.id > 0) {
            Rectangle src = { 0.0f, 0.0f, (float)s_endingCthulhuThrow.width, (float)s_endingCthulhuThrow.height };
            Rectangle dst = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
            DrawTexturePro(s_endingCthulhuThrow, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }

    } else if (timer < 10.0f) {
        if (s_loaded && s_endingLaserShark.id > 0) {
            Rectangle src = { 0.0f, 0.0f, (float)s_endingLaserShark.width, (float)s_endingLaserShark.height };
            Rectangle dst = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
            DrawTexturePro(s_endingLaserShark, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }

    } else {
        if (s_loaded && s_endingCthulhuNap.id > 0) {
            Rectangle src = { 0.0f, 0.0f, (float)s_endingCthulhuNap.width, (float)s_endingCthulhuNap.height };
            Rectangle dst = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
            DrawTexturePro(s_endingCthulhuNap, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }

        if (fmodf(timer, 0.8f) < 0.5f) {
            const char* msg = "[ PRESS START / ENTER TO RETURN TO TITLE ]";
            int pw = MeasureText(msg, 8);
            DrawRectangle((SCREEN_WIDTH - pw) / 2 - 4, SCREEN_HEIGHT - 13, pw + 8, 11, Color{ 8, 12, 18, 220 });
            DrawPixelText(msg, (SCREEN_WIDTH - pw) / 2, SCREEN_HEIGHT - 12, 8, Color{ 80, 220, 255, 240 });
        }

        if (timer >= 16.0f) {
            float fade = std::clamp((timer - 16.0f) / 2.0f, 0.0f, 1.0f);
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, (unsigned char)(fade * 255) });
        }
    }
}

void VisualsEngine::DrawPauseOverlay(float timer) {

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 110 });

    int boxW = 150;
    int boxH = 46;
    int boxX = (SCREEN_WIDTH - boxW) / 2;
    int boxY = (SCREEN_HEIGHT - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Color{ 12, 18, 30, 245 });
    DrawRectangleLines(boxX, boxY, boxW, boxH, Color{ 255, 215, 0, 255 });
    DrawRectangleLines(boxX + 2, boxY + 2, boxW - 4, boxH - 4, Color{ 60, 170, 240, 255 });

    int ptw = MeasureText("PAUSE", 18);
    DrawPixelText("PAUSE", (SCREEN_WIDTH - ptw) / 2, boxY + 7, 18, Color{ 255, 225, 40, 255 });

    if (fmodf(timer, 0.8f) < 0.5f) {
        const char* hint = "PRESS START TO RESUME";
        int hw = MeasureText(hint, 7);
        DrawPixelText(hint, (SCREEN_WIDTH - hw) / 2, boxY + 29, 7, Color{ 200, 235, 255, 240 });
    }
}

// --- AIController.cpp ---
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

// --- main.cpp ---
#include "rlgl.h"
#include <memory>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <csignal>

static volatile sig_atomic_t s_quitRequested = 0;

static void CleanExitSignalHandler(int sig) {
    (void)sig;
    s_quitRequested = 1;
}

static void CrashSignalHandler(int sig) {
    const char* sigName = "UNKNOWN";
    if (sig == SIGSEGV) sigName = "SIGSEGV (Segmentation Fault)";
    else if (sig == SIGABRT) sigName = "SIGABRT (Aborted)";
    else if (sig == SIGFPE) sigName = "SIGFPE (Floating Point Exception)";
    else if (sig == SIGILL) sigName = "SIGILL (Illegal Instruction)";

    fprintf(stderr, "\n==================================================\n");
    fprintf(stderr, "FATAL CRASH DETECTED: Signal %d - %s\n", sig, sigName);
    fprintf(stderr, "==================================================\n");
    fflush(stderr);

    FILE* f = fopen("crash_log.txt", "w");
    if (f) {
        fprintf(f, "FATAL CRASH DETECTED: Signal %d - %s\n", sig, sigName);
        fflush(f);
        fclose(f);
    }
    _Exit(sig);
}

int main(int argc, char* argv[]) {
    std::signal(SIGTERM, CleanExitSignalHandler);
    std::signal(SIGINT, CleanExitSignalHandler);
    std::signal(SIGSEGV, CrashSignalHandler);
    std::signal(SIGABRT, CrashSignalHandler);
    std::signal(SIGFPE, CrashSignalHandler);
    std::signal(SIGILL, CrashSignalHandler);

    bool windowedMode = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--windowed") == 0) windowedMode = true;
    }

    ChangeDirectory(GetApplicationDirectory());

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Executive Order II: Total Veto - Championship Edition");
    if (!IsWindowReady()) {
        TraceLog(LOG_FATAL, "ExecutiveOrder: Failed to initialize window / graphics context.");
        return 1;
    }
    SetTargetFPS(TARGET_FPS);

    static const char* s_gamepadDb =
        "19000000010000000100000001000000,ANBERNIC-keys,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,guide:b6,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,platform:Linux,\n"
        "19000000010000000100000001010000,Anbernic RG353V,platform:Linux,a:b1,b:b0,x:b2,y:b3,back:b8,start:b9,guide:b10,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,leftstick:b11,rightstick:b12,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
        "19000000010000000100000002010000,retrogame_joypad,platform:Linux,a:b1,b:b0,x:b2,y:b3,back:b8,start:b9,guide:b10,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,leftstick:b11,rightstick:b12,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
        "19000000010000000200000002000000,retrogame_joypad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,dpup:b10,dpdown:b11,dpleft:b12,dpright:b13,leftx:a0,lefty:a1,\n"
        "19000000010000000100000000010000,retrogame_joypad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,\n"
        "03000000091200000031000000010000,OpenSimHardware OSH PB Controller,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,\n"
        "19000000010000000100000000000000,GO-Advance Gamepad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,\n"
        "19000000010000000100000002000000,GO-Advance Gamepad (rev 1.1),platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,dpup:b10,dpdown:b11,dpleft:b12,dpright:b13,leftx:a0,lefty:a1,\n"
        "030000005e0400008e02000010010000,Microsoft X-Box 360 pad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b6,start:b7,guide:b8,leftshoulder:b4,rightshoulder:b5,leftstick:b9,rightstick:b10,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:a2,righttrigger:a5,dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,\n";
    SetGamepadMappings(s_gamepadDb);
    if (FileExists("gamecontrollerdb.txt")) {
        char* fileMappings = LoadFileText("gamecontrollerdb.txt");
        if (fileMappings) {
            SetGamepadMappings(fileMappings);
            UnloadFileText(fileMappings);
        }
    }

    if (!windowedMode) {
        ToggleBorderlessWindowed();
    }

    AssetManager::Init();
    const char* iconPath = AssetManager::FileExists("assets/app_icon.png") ? "assets/app_icon.png" : "../assets/app_icon.png";
    if (AssetManager::FileExists(iconPath)) {
        Image appIcon = AssetManager::LoadImage(iconPath);
        if (appIcon.data != nullptr) {
            SetWindowIcon(appIcon);
            UnloadImage(appIcon);
        }
    }

    RenderTexture2D screenTarget = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    SetTextureFilter(screenTarget.texture, TEXTURE_FILTER_POINT);

    Shader crtShader = LoadShaderFromMemory(nullptr, CRT_FRAGMENT_SHADER);
    int scanlineLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "scanlineIntensity") : -1;
    int curvatureLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "curvature") : -1;
    int bloomLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "bloom") : -1;

    float scanlineVal = 0.35f;
    float curvatureVal = 0.015f;
    float bloomVal = 0.15f;
    if (crtShader.id > 0) {
        if (scanlineLoc >= 0) SetShaderValue(crtShader, scanlineLoc, &scanlineVal, SHADER_UNIFORM_FLOAT);
        if (curvatureLoc >= 0) SetShaderValue(crtShader, curvatureLoc, &curvatureVal, SHADER_UNIFORM_FLOAT);
        if (bloomLoc >= 0) SetShaderValue(crtShader, bloomLoc, &bloomVal, SHADER_UNIFORM_FLOAT);
    }

    Shader flagShader = LoadShaderFromMemory(nullptr, FLAG_FRAGMENT_SHADER);
    int flagTimeLoc = (flagShader.id > 0) ? GetShaderLocation(flagShader, "time") : -1;

    Texture2D flagTexture = VisualsEngine::CreateProceduralFlagTexture();

    VisualsEngine::Init();

    AudioEngine::Instance().Init();

    InputManager inputP1;
    InputManager inputP2;
    StageManager stageMgr;
    stageMgr.Init();
    AIController cpuAI;

    CharacterId p1CharId = CHAR_COMMANDER;
    CharacterId p2CharId = CHAR_TECH_VISIONARY;
    StageId currentStage = STAGE_REFLECTING_POOL;

    bool defeatedFighters[CHARACTER_COUNT] = { false };
    int matchesWon = 0;
    bool bonusStagePlayed = false;
    bool playerLocked = false;
    bool choosingOpponent = false;

    static const StageId homeStages[PLAYABLE_CHARACTER_COUNT] = {
        STAGE_WHITE_HOUSE_MMA,
        STAGE_CONGRESSIONAL_FLOOR,
        STAGE_DEMOLISHED_EAST_WING,
        STAGE_REFLECTING_POOL,
        STAGE_REFLECTING_POOL,
        STAGE_CONGRESSIONAL_FLOOR,
        STAGE_WHITE_HOUSE_MMA,
        STAGE_DEMOLISHED_EAST_WING,
        STAGE_REFLECTING_POOL,
        STAGE_CONGRESSIONAL_FLOOR
    };

    auto CountDefeatedOriginals = [&]() -> int {
        int count = 0;
        for (int i = 0; i < PLAYABLE_CHARACTER_COUNT; i++) {
            if (i != (int)p1CharId && defeatedFighters[i]) {
                count++;
            }
        }
        return count;
    };

    auto GetStageForOpponent = [&](CharacterId opp) -> StageId {
        if (opp == CHAR_SUCKRATERY_RKF) return STAGE_PRESS_BRIEFING;
        if (opp == CHAR_WAR_PIG) return STAGE_PENTAGON_ALTAR;
        if (opp == CHAR_SOCIALIST) return STAGE_WTC_PLAZA;
        if (opp == CHAR_BILLARY) return STAGE_COMET_PING_PONG;
        if (opp == CHAR_PALM_BEACH_PETE) return STAGE_FLORIDA_KEYS;
        return homeStages[(int)opp];
    };

    auto PickNextAvailableOpponent = [&](int startIdx) -> CharacterId {
        int origDefeated = CountDefeatedOriginals();
        if (defeatedFighters[CHAR_WAR_PIG]) {
            if (!defeatedFighters[CHAR_SOCIALIST]) return CHAR_SOCIALIST;
            if (!defeatedFighters[CHAR_BILLARY]) return CHAR_BILLARY;
            if (!defeatedFighters[CHAR_PALM_BEACH_PETE]) return CHAR_PALM_BEACH_PETE;
            return CHAR_PALM_BEACH_PETE;
        }
        if (origDefeated >= 9) {
            if (!defeatedFighters[CHAR_SUCKRATERY_RKF]) return CHAR_SUCKRATERY_RKF;
            if (!defeatedFighters[CHAR_WAR_PIG]) return CHAR_WAR_PIG;
            return CHAR_WAR_PIG;
        }
        for (int step = 0; step < PLAYABLE_CHARACTER_COUNT; step++) {
            int idx = (startIdx + step) % PLAYABLE_CHARACTER_COUNT;
            if (idx != (int)p1CharId && !defeatedFighters[idx]) {
                return (CharacterId)idx;
            }
        }
        return CHAR_COASTAL_GOV;
    };

    const float BONUS_CAR_MAX_HP = 1200.0f;
    float bonusCarHp = BONUS_CAR_MAX_HP;
    float bonusTimer = 30.0f;
    bool bonusCarAlarmPlayed = false;
    int bonusLastStage = 0;
    float bonusPostTimer = 0.0f;
    bool bonusBooYaaPlayed = false;

    p2CharId = PickNextAvailableOpponent(1);
    currentStage = GetStageForOpponent(p2CharId);
    stageMgr.SetStage(currentStage);

    std::unique_ptr<Fighter> p1 = std::make_unique<Fighter>(0, p1CharId, 320.0f);
    std::unique_ptr<Fighter> p2 = std::make_unique<Fighter>(1, p2CharId, 480.0f);

    std::vector<SparkEffect> sparks;

    GameState gameState = STATE_SPLASH;
    float stateTimer = 0.0f;
    bool logoSlammed = false;
    bool enableCrt = true;

    int roundNumber = 1;
    int roundSeconds = 99;
    float roundTickAccum = 0.0f;
    int fightIntroTimer = 100;
    bool fightActive = false;
    bool roundOver = false;
    float matchEndTimer = 0.0f;
    bool p1WasKo = false;
    bool p2WasKo = false;
    bool bonusFromEasterEgg = false;
    bool isGamePaused = false;

    bool isSecretMode = false;
    int secretStep = 0;
    int secretP1 = 0;
    int secretP2 = 1;
    int secretStage = 0;
    int secretDifficulty = (int)AI_DIFF_NORMAL;

    float cameraX = 400.0f;
    float cameraShake = 0.0f;

    while (!WindowShouldClose() && !s_quitRequested) {
        float dt = GetFrameTime();
        if (IsKeyPressed(KEY_F2)) enableCrt = !enableCrt;
        stateTimer += dt;
        AudioEngine::Instance().Update();

            bool killRequested = WindowShouldClose() || (s_quitRequested != 0);

            if (IsKeyPressed(KEY_ESCAPE) || IsKeyDown(KEY_ESCAPE) ||
                IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE) ||
                IsKeyPressed(KEY_TAB) || IsKeyDown(KEY_TAB)) {
                killRequested = true;
            }

            for (int pad = 0; pad < 4; pad++) {
                if (IsGamepadAvailable(pad)) {
                    if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_LEFT) ||
                        IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_LEFT) ||
                        IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE) ||
                        IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE)) {
                        killRequested = true;
                        break;
                    }

                    const char* padName = GetGamepadName(pad);
                    if (padName) {
                        std::string nameLower = padName;
                        for (char& c : nameLower) c = (char)tolower((unsigned char)c);
                        if (nameLower.find("anbernic") != std::string::npos ||
                            nameLower.find("retrogame") != std::string::npos ||
                            nameLower.find("opensim") != std::string::npos ||
                            nameLower.find("go-advance") != std::string::npos) {
                            if (IsGamepadButtonPressed(pad, 8) || IsGamepadButtonDown(pad, 8) ||
                                IsGamepadButtonPressed(pad, 10) || IsGamepadButtonDown(pad, 10)) {
                                killRequested = true;
                                break;
                            }
                        }
                    }
                }
            }

            if (inputP1.IsButtonPressed(BTN_SELECT) || inputP1.IsButtonHeld(BTN_SELECT)) {
                killRequested = true;
            }

            if (killRequested) {
                break;
            }

        if (IsKeyPressed(KEY_F11) || ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER))) {
            ToggleBorderlessWindowed();
        }

        if (IsKeyPressed(KEY_F1)) {
            enableCrt = !enableCrt;
        }

        if (IsKeyPressed(KEY_F12)) {
            TakeScreenshot("screenshot.png");
        }

        switch (gameState) {
        case STATE_SPLASH: {
            if (!logoSlammed && stateTimer >= 0.8f) {
                logoSlammed = true;
                cameraShake = 4.0f;
                AudioEngine::Instance().PlaySfx(SFX_TITLE_SLAM);
            }

                inputP1.Update(0, true);

                if (inputP1.IsSecretMenuComboPressed()) {
                    AudioEngine::Instance().PlaySfx(SFX_COIN);
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_HADOUKEN, 0.8f);
                    gameState = STATE_SECRET_SELECT;
                    isSecretMode = true;
                    secretStep = 0;
                    secretP1 = (int)p1CharId;
                    secretP2 = (int)p2CharId;
                    stateTimer = 0.0f;
                    break;
                }

                bool startPressed = false;
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ONE) ||
                    IsKeyPressed(KEY_U) || IsKeyPressed(KEY_I) || IsKeyPressed(KEY_C) || IsKeyPressed(KEY_V)) {
                    startPressed = true;
                }
                for (int pad = 0; pad < 4; pad++) {
                    if (!IsGamepadAvailable(pad)) continue;
                    if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT) ||
                        IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ||
                        IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
                        IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ||
                        IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_UP)) {
                        startPressed = true;
                        break;
                    }
                }
                if (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_COIN) ||
                    inputP1.IsButtonPressed(BTN_LP) || inputP1.IsButtonPressed(BTN_HP) ||
                    inputP1.IsButtonPressed(BTN_LK) || inputP1.IsButtonPressed(BTN_HK)) {
                    startPressed = true;
                }

                if (startPressed) {
                    AudioEngine::Instance().PlaySfx(SFX_COIN);
                    gameState = STATE_SELECT_MAP;
                    isSecretMode = false;
                    playerLocked = false;
                    choosingOpponent = false;
                    p1CharId = CHAR_COMMANDER;
                    p2CharId = PickNextAvailableOpponent(1);
                    currentStage = homeStages[(int)p1CharId];
                    stageMgr.SetStage(currentStage);
                    stateTimer = 0.0f;
                }

            break;
        }

        case STATE_SELECT_MAP: {
            inputP1.Update(0, true);

            int origDefeated = CountDefeatedOriginals();

            if (defeatedFighters[CHAR_WAR_PIG]) {
                if (!defeatedFighters[CHAR_SOCIALIST]) {
                    p2CharId = CHAR_SOCIALIST;
                    currentStage = STAGE_WTC_PLAZA;
                } else if (!defeatedFighters[CHAR_BILLARY]) {
                    p2CharId = CHAR_BILLARY;
                    currentStage = STAGE_COMET_PING_PONG;
                } else if (!defeatedFighters[CHAR_PALM_BEACH_PETE]) {
                    p2CharId = CHAR_PALM_BEACH_PETE;
                    currentStage = STAGE_FLORIDA_KEYS;
                } else {

                    if (p1CharId == CHAR_COMMANDER && !isSecretMode) {
                        gameState = STATE_CTHULHU_ENDING;
                        stateTimer = 0.0f;
                        AudioEngine::Instance().StopBgm();
                        AudioEngine::Instance().PlaySfx(SFX_CTHULHU_ROAR);
                    } else {
                        gameState = STATE_ENDING_CELEBRATION;
                        stateTimer = 0.0f;
                        AudioEngine::Instance().StopBgm();
                        AudioEngine::Instance().PlaySfx(SFX_JESUS_FANFARE);
                    }
                    break;
                }
                stageMgr.SetStage(currentStage);
            } else if (origDefeated >= 9) {
                if (!defeatedFighters[CHAR_SUCKRATERY_RKF]) {
                    p2CharId = CHAR_SUCKRATERY_RKF;
                    currentStage = STAGE_PRESS_BRIEFING;
                } else if (!defeatedFighters[CHAR_WAR_PIG]) {
                    p2CharId = CHAR_WAR_PIG;
                    currentStage = STAGE_PENTAGON_ALTAR;
                } else {

                    gameState = STATE_MATCH_OVER;
                    stateTimer = 0.0f;
                    break;
                }
                stageMgr.SetStage(currentStage);
            } else if (!playerLocked) {

                int p1Col = (int)p1CharId % 5;
                int p1Row = (int)p1CharId / 5;
                bool p1Changed = false;

                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || inputP1.IsLeftPressed()) {
                    p1Col = (p1Col + 4) % 5;
                    p1Changed = true;
                }
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || inputP1.IsRightPressed()) {
                    p1Col = (p1Col + 1) % 5;
                    p1Changed = true;
                }
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || inputP1.IsUpPressed() ||
                    IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || inputP1.IsDownPressed()) {
                    p1Row = 1 - p1Row;
                    p1Changed = true;
                }

                if (p1Changed) {
                    p1CharId = (CharacterId)(p1Row * 5 + p1Col);

                    currentStage = homeStages[(int)p1CharId];
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }
            } else {

                bool p2Changed = false;

                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || inputP1.IsRightPressed()) {
                    int cur = (int)p2CharId;
                    for (int step = 1; step < PLAYABLE_CHARACTER_COUNT; step++) {
                        int next = (cur + step) % PLAYABLE_CHARACTER_COUNT;
                        if (next != (int)p1CharId && !defeatedFighters[next]) {
                            p2CharId = (CharacterId)next;
                            p2Changed = true;
                            break;
                        }
                    }
                }
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || inputP1.IsLeftPressed()) {
                    int cur = (int)p2CharId;
                    for (int step = 1; step < PLAYABLE_CHARACTER_COUNT; step++) {
                        int prev = (cur - step + PLAYABLE_CHARACTER_COUNT) % PLAYABLE_CHARACTER_COUNT;
                        if (prev != (int)p1CharId && !defeatedFighters[prev]) {
                            p2CharId = (CharacterId)prev;
                            p2Changed = true;
                            break;
                        }
                    }
                }
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || inputP1.IsUpPressed() ||
                    IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || inputP1.IsDownPressed()) {
                    int target = ((int)p2CharId + 5) % PLAYABLE_CHARACTER_COUNT;
                    for (int step = 0; step < PLAYABLE_CHARACTER_COUNT; step++) {
                        int cand = (target + step) % PLAYABLE_CHARACTER_COUNT;
                        if (cand != (int)p1CharId && !defeatedFighters[cand]) {
                            p2CharId = (CharacterId)cand;
                            p2Changed = true;
                            break;
                        }
                    }
                }

                if (p2Changed) {
                    currentStage = homeStages[(int)p2CharId];
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }
            }

            if (inputP1.IsTriggerEasterEggPressed()) {
                bonusStagePlayed = true;
                bonusFromEasterEgg = true;
                gameState = STATE_BONUS_CAR;
                stateTimer = 0.0f;
                bonusCarHp = BONUS_CAR_MAX_HP;
                bonusTimer = 30.0f;
                bonusCarAlarmPlayed = false;
                bonusLastStage = 0;
                bonusPostTimer = 0.0f;
                bonusBooYaaPlayed = false;
                p1 = std::make_unique<Fighter>(0, p1CharId, 100.0f);

                AudioEngine::Instance().StopBgm();
                AudioEngine::Instance().SetBgmTrack(5);
                AudioEngine::Instance().PlaySfx(SFX_TITLE_SLAM);
                AudioEngine::Instance().PlaySfx(SFX_VOICE_FIGHT);
                break;
            }

            bool canConfirm = (stateTimer >= 0.25f);

            if (canConfirm && (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))) {

                if (!playerLocked) {

                    playerLocked = true;
                    choosingOpponent = true;
                    p2CharId = PickNextAvailableOpponent((int)p1CharId + 1);
                    currentStage = homeStages[(int)p2CharId];
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                    stateTimer = 0.0f;
                } else {

                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);

                    currentStage = GetStageForOpponent(p2CharId);
                    stageMgr.SetStage(currentStage);

                    cpuAI.SetDifficulty((p2CharId == CHAR_WAR_PIG || p2CharId == CHAR_PALM_BEACH_PETE) ? AI_DIFF_HARD : AI_DIFF_NORMAL);
                    cpuAI.Reset();
                    inputP2.Reset();

                    p1 = std::make_unique<Fighter>(0, p1CharId, 320.0f);
                    p2 = std::make_unique<Fighter>(1, p2CharId, 480.0f);

                    AudioEngine::Instance().StopBgm();
                    AudioEngine::Instance().PlaySfx(SFX_MATCH_START_FANFARE);

                    gameState = STATE_VERSUS;
                    stateTimer = 0.0f;
                }
            }
            break;
        }

        case STATE_SECRET_SELECT: {
            inputP1.Update(0, true);

            if (inputP1.IsButtonPressed(BTN_SELECT) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
                AudioEngine::Instance().PlaySfx(SFX_BLOCK, 1.0f);
                gameState = STATE_SPLASH;
                isSecretMode = false;
                stateTimer = 0.0f;
                break;
            }

            if (secretStep == 0) {
                if (inputP1.IsLeftPressed()) {
                    secretP1 = (secretP1 - 1 + CHARACTER_COUNT) % CHARACTER_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsRightPressed()) {
                    secretP1 = (secretP1 + 1) % CHARACTER_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsUpPressed()) {
                    if (secretP1 >= 8) secretP1 = std::min(7, secretP1 - 8);
                    else secretP1 = std::min(14, secretP1 + 8);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsDownPressed()) {
                    if (secretP1 < 8) secretP1 = std::min(14, secretP1 + 8);
                    else secretP1 = secretP1 - 8;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }

                if (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                    inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                    inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                    secretStep = 1;
                    stateTimer = 0.0f;
                }
            }

            else if (secretStep == 1) {
                if (inputP1.IsLeftPressed()) {
                    secretP2 = (secretP2 - 1 + CHARACTER_COUNT) % CHARACTER_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsRightPressed()) {
                    secretP2 = (secretP2 + 1) % CHARACTER_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsUpPressed()) {
                    if (secretP2 >= 8) secretP2 = std::min(7, secretP2 - 8);
                    else secretP2 = std::min(14, secretP2 + 8);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsDownPressed()) {
                    if (secretP2 < 8) secretP2 = std::min(14, secretP2 + 8);
                    else secretP2 = secretP2 - 8;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }

                if (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                    inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                    inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                    secretStep = 2;
                    stateTimer = 0.0f;
                }
            }

            else if (secretStep == 2) {
                if (inputP1.IsLeftPressed()) {
                    secretStage = (secretStage - 1 + STAGE_COUNT) % STAGE_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsRightPressed()) {
                    secretStage = (secretStage + 1) % STAGE_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsUpPressed()) {
                    secretStage = (secretStage - 3 + STAGE_COUNT) % STAGE_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsDownPressed()) {
                    secretStage = (secretStage + 3) % STAGE_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }

                if (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                    inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                    inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                    secretStep = 3;
                    stateTimer = 0.0f;
                }
            }

            else if (secretStep == 3) {
                if (inputP1.IsUpPressed()) {
                    secretDifficulty = (secretDifficulty - 1 + 3) % 3;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsDownPressed()) {
                    secretDifficulty = (secretDifficulty + 1) % 3;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }

                if (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                    inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                    inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    AudioEngine::Instance().PlaySfx(SFX_MATCH_START_FANFARE);

                    p1CharId = (CharacterId)secretP1;
                    p2CharId = (CharacterId)secretP2;
                    currentStage = (StageId)secretStage;
                    stageMgr.SetStage(currentStage);

                    cpuAI.SetDifficulty((AIDifficulty)secretDifficulty);
                    cpuAI.Reset();
                    inputP2.Reset();

                    p1 = std::make_unique<Fighter>(0, p1CharId, 320.0f);
                    p2 = std::make_unique<Fighter>(1, p2CharId, 480.0f);

                    roundNumber = 1;
                    roundSeconds = 99;
                    fightIntroTimer = 110;
                    fightActive = false;
                    roundOver = false;
                    matchEndTimer = 0.0f;
                    p1WasKo = false;
                    p2WasKo = false;

                    gameState = STATE_FIGHT;
                    stateTimer = 0.0f;
                    int bgmTrack = (currentStage >= 4) ? ((currentStage == STAGE_PRESS_BRIEFING) ? 3 : 4) : ((int)currentStage + 1);
                    AudioEngine::Instance().SetBgmTrack(bgmTrack);
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_ROUND1);
                }
            }
            break;
        }

        case STATE_VERSUS: {

            if (stateTimer >= 1.8f) {
                gameState = STATE_FIGHT;
                stateTimer = 0.0f;
                fightIntroTimer = 110;
                fightActive = false;
                roundOver = false;
                roundSeconds = 99;
                roundNumber = 1;
                p1WasKo = false;
                p2WasKo = false;
                ChampionshipEdition::ResetRoundFX();
                p1->ResetRound(320.0f, true);
                p2->ResetRound(480.0f, false);
                cpuAI.Reset();
                inputP2.Reset();
                int bgmTrack = (currentStage >= 4) ? ((currentStage == STAGE_PRESS_BRIEFING) ? 3 : 4) : ((int)currentStage + 1);
                AudioEngine::Instance().SetBgmTrack(bgmTrack);
                AudioEngine::Instance().PlaySfx(SFX_VOICE_ROUND1);
            }
            break;
        }

        case STATE_FIGHT: {

            bool pausePressed = false;
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_P) || inputP1.IsButtonPressed(BTN_START)) {
                pausePressed = true;
            }
            for (int pad = 0; pad < 2; pad++) {
                if (IsGamepadAvailable(pad) && IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    pausePressed = true;
                }
            }

            if (pausePressed && fightIntroTimer <= 0 && !roundOver) {
                isGamePaused = !isGamePaused;
                AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
            }

            if (isGamePaused) {
                break;
            }

            if (!fightActive && !roundOver) {
                fightIntroTimer--;

                if (p1->GetState() == FSTATE_KNOCKDOWN_GROUND && fightIntroTimer <= 75) {
                    p1->SetState(FSTATE_WAKEUP);
                }
                if (p2->GetState() == FSTATE_KNOCKDOWN_GROUND && fightIntroTimer <= 75) {
                    p2->SetState(FSTATE_WAKEUP);
                }

                if (p1->GetState() == FSTATE_WAKEUP) p1->TickWakeup();
                if (p2->GetState() == FSTATE_WAKEUP) p2->TickWakeup();

                if (fightIntroTimer == 50) {
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_FIGHT);
                } else if (fightIntroTimer <= 0) {
                    fightActive = true;
                    p1WasKo = false;
                    p2WasKo = false;
                }
            }

            inputP1.Update(0, p1->IsFacingRight());
            cpuAI.Update(inputP2, *p2, *p1, dt);

            if (fightActive) {

                roundTickAccum += dt;
                if (roundTickAccum >= 1.0f) {
                    roundTickAccum -= 1.0f;
                    if (roundSeconds > 0) roundSeconds--;
                }

                if (ChampionshipEdition::s_superFlashTimer > 0) {
                    ChampionshipEdition::s_superFlashTimer--;
                } else {
                    p1->UpdatePhysics(inputP1, *p2);
                    p2->UpdatePhysics(inputP2, *p1);

                    CollisionSystem::ResolvePushboxes(*p1, *p2);
                    CollisionSystem::ResolveCombat(*p1, *p2, sparks, cameraShake);
                }
                ChampionshipEdition::UpdateAndTrackFighters(*p1, *p2);

                if (p1->GetHealth() <= 0 || p2->GetHealth() <= 0 || roundSeconds <= 0) {
                    fightActive = false;
                    roundOver = true;
                    isGamePaused = false;
                    matchEndTimer = 0.0f;

                    AudioEngine::Instance().PlaySfx(SFX_VOICE_BOO_YAA);

                    if (p1->GetHealth() > p2->GetHealth()) {
                        p1->AddRoundWin();
                    } else if (p2->GetHealth() > p1->GetHealth()) {
                        p2->AddRoundWin();
                    }
                }
            } else if (roundOver) {

                matchEndTimer += dt;
                p1->UpdatePhysics(inputP1, *p2);
                p2->UpdatePhysics(inputP2, *p1);

                if (matchEndTimer >= 1.5f) {

                    int roundsNeeded = (p2CharId == CHAR_WAR_PIG || p2CharId == CHAR_PALM_BEACH_PETE) ? 1 : 2;
                    if (p1->GetRoundsWon() >= roundsNeeded || p2->GetRoundsWon() >= roundsNeeded) {
                        stateTimer = 0.0f;
                        if (p1->GetRoundsWon() >= roundsNeeded) {
                            if (!isSecretMode) {
                                matchesWon++;
                                defeatedFighters[(int)p2CharId] = true;
                            }
                            AudioEngine::Instance().StopBgm();
                            AudioEngine::Instance().PlaySfx(SFX_VICTORY_FANFARE);
                            gameState = STATE_VICTORY_SPLASH;
                        } else {
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_TOTAL_VETO);
                            if (isSecretMode) {
                                gameState = STATE_VICTORY_SPLASH;
                            } else {
                                gameState = STATE_MATCH_OVER;
                            }
                        }
                    } else {

                        bool p1Died = (p1->GetHealth() <= 0);
                        bool p2Died = (p2->GetHealth() <= 0);
                        p1WasKo = p1Died;
                        p2WasKo = p2Died;

                        roundNumber++;
                        roundSeconds = 99;
                        fightIntroTimer = 110;
                        fightActive = false;
                        roundOver = false;
                        ChampionshipEdition::ResetRoundFX();
                        p1->ResetRound(320.0f, true, p1Died);
                        p2->ResetRound(480.0f, false, p2Died);
                        cpuAI.Reset();
                        inputP2.Reset();
                        if (roundNumber == 2) {
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_ROUND2);
                        } else {
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_FINAL_ROUND);
                        }
                    }
                }
            }

            for (auto& sp : sparks) {
                if (!sp.alive) continue;
                sp.frame++;
                if (sp.frame >= sp.maxFrames) sp.alive = false;
            }
            sparks.erase(std::remove_if(sparks.begin(), sparks.end(), [](const SparkEffect& s) { return !s.alive; }), sparks.end());

            float midX = (p1->GetPosition().x + p2->GetPosition().x) * 0.5f;
            float targetCamX = std::clamp(midX, STAGE_MIN_X + SCREEN_WIDTH * 0.5f, STAGE_MAX_X - SCREEN_WIDTH * 0.5f);
            cameraX += (targetCamX - cameraX) * 0.12f;

            if (cameraShake > 0.01f) cameraShake *= 0.88f;
            else cameraShake = 0.0f;
            break;
        }

        case STATE_VICTORY_SPLASH: {
            inputP1.Update(0, true);
            if (stateTimer >= 6.0f || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
                inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                inputP1.IsButtonPressed(BTN_HK)) {

                if (isSecretMode) {
                    gameState = STATE_SECRET_SELECT;
                    secretStep = 0;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(0);
                    break;
                }

                if (matchesWon == 3 && !bonusStagePlayed) {
                    bonusStagePlayed = true;
                    gameState = STATE_BONUS_CAR;
                    stateTimer = 0.0f;
                    bonusCarHp = BONUS_CAR_MAX_HP;
                    bonusTimer = 30.0f;
                    bonusCarAlarmPlayed = false;
                    bonusLastStage = 0;
                    bonusPostTimer = 0.0f;
                    p1->ResetRound(100.0f, true);
                    AudioEngine::Instance().SetBgmTrack(5);
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_FIGHT);
                } else if (defeatedFighters[CHAR_PALM_BEACH_PETE]) {

                    if (p1CharId == CHAR_COMMANDER && !isSecretMode) {
                        gameState = STATE_CTHULHU_ENDING;
                        stateTimer = 0.0f;
                        AudioEngine::Instance().StopBgm();
                        AudioEngine::Instance().PlaySfx(SFX_CTHULHU_ROAR);
                    } else {
                        gameState = STATE_ENDING_CELEBRATION;
                        stateTimer = 0.0f;
                        AudioEngine::Instance().StopBgm();
                        AudioEngine::Instance().PlaySfx(SFX_JESUS_FANFARE);
                    }
                } else {
                    p2CharId = PickNextAvailableOpponent((int)p2CharId + 1);
                    currentStage = GetStageForOpponent(p2CharId);
                    stageMgr.SetStage(currentStage);
                    gameState = STATE_SELECT_MAP;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(0);
                }
            }
            break;
        }

        case STATE_BONUS_CAR: {
            inputP1.Update(0, p1->IsFacingRight());

            bool pausePressed = false;
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_P) || inputP1.IsButtonPressed(BTN_START)) {
                pausePressed = true;
            }
            for (int pad = 0; pad < 2; pad++) {
                if (IsGamepadAvailable(pad) && IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    pausePressed = true;
                }
            }

            if (pausePressed && bonusCarHp > 0.0f && bonusTimer > 0.0f) {
                isGamePaused = !isGamePaused;
                AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
            }

            if (isGamePaused) {
                break;
            }

            if (bonusCarHp > 0.0f && bonusTimer > 0.0f) {
                bonusTimer = std::max(0.0f, bonusTimer - dt);

                Rectangle carHurtbox = { (float)SCREEN_WIDTH / 2.0f - 77.0f, GROUND_Y - 60.0f, 154.0f, 60.0f };

                Fighter carDummy(1, CHAR_TECH_VISIONARY, SCREEN_WIDTH / 2.0f);
                p1->UpdatePhysics(inputP1, carDummy);

                Rectangle hitbox{};
                AttackData atkData{};
                if (p1->GetActiveHitbox(hitbox, atkData) && !p1->HasHitConnected()) {
                    if (CheckCollisionRecs(hitbox, carHurtbox)) {
                        p1->SetHitConnected(true);
                        p1->SetHitstop(atkData.hitstopFrames);
                        cameraShake = atkData.isHeavy ? 5.0f : 2.5f;

                        float dmg = (float)atkData.damage;
                        bonusCarHp = std::max(0.0f, bonusCarHp - dmg);

                        SparkEffect sp{};
                        sp.pos = Vector2{ (hitbox.x + hitbox.width * 0.5f), (hitbox.y + hitbox.height * 0.5f) };
                        sp.isBlock = false;
                        sp.alive = true;
                        sp.frame = 0;
                        sp.maxFrames = 8;
                        sparks.push_back(sp);

                        AudioEngine::Instance().PlaySfx(atkData.isHeavy ? SFX_HIT_HEAVY : SFX_HIT_LIGHT);
                        AudioEngine::Instance().PlaySfx(SFX_METAL_CRUNCH, (float)(90 + (rand() % 20)) / 100.0f);

                        float hpRatio = bonusCarHp / BONUS_CAR_MAX_HP;
                        int curStage = (hpRatio <= 0.0f) ? 3 : ((hpRatio <= 0.25f) ? 3 : ((hpRatio <= 0.50f) ? 2 : ((hpRatio <= 0.75f) ? 1 : 0)));
                        if (curStage > bonusLastStage) {
                            bonusLastStage = curStage;
                            AudioEngine::Instance().PlaySfx(SFX_GLASS_BREAK);
                            if (!bonusCarAlarmPlayed) {
                                bonusCarAlarmPlayed = true;
                                AudioEngine::Instance().PlaySfx(SFX_CAR_ALARM);
                            }
                        }
                        if (curStage == 3 && (rand() % 3 == 0)) {
                            AudioEngine::Instance().PlaySfx(SFX_ELEC_SPARK);
                        }

                        if (bonusCarHp <= 0.0f) {
                            AudioEngine::Instance().PlaySfx(SFX_TITLE_SLAM);
                            AudioEngine::Instance().PlaySfx(SFX_FIREBALL_IMPACT);
                            cameraShake = 8.0f;
                        }
                    }
                }
            } else {

                if (!bonusBooYaaPlayed) {
                    bonusBooYaaPlayed = true;
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_BOO_YAA);
                }

                bonusPostTimer += dt;
                Fighter carDummy(1, CHAR_TECH_VISIONARY, SCREEN_WIDTH / 2.0f);
                p1->UpdatePhysics(inputP1, carDummy);

                if (bonusPostTimer >= 2.0f) {
                    p2CharId = PickNextAvailableOpponent((int)p2CharId + 1);
                    currentStage = GetStageForOpponent(p2CharId);
                    stageMgr.SetStage(currentStage);
                    gameState = STATE_SELECT_MAP;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(0);
                }
            }

            for (auto& sp : sparks) {
                if (!sp.alive) continue;
                sp.frame++;
                if (sp.frame >= sp.maxFrames) sp.alive = false;
            }

            if (cameraShake > 0.01f) cameraShake *= 0.88f;
            else cameraShake = 0.0f;
            break;
        }

        case STATE_MATCH_OVER: {
            inputP1.Update(0, true);
            if (stateTimer >= 5.0f || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
                inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                inputP1.IsButtonPressed(BTN_HK)) {

                if (isSecretMode) {
                    gameState = STATE_SECRET_SELECT;
                    secretStep = 0;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(0);
                    break;
                }

                matchesWon = 0;
                bonusStagePlayed = false;
                playerLocked = false;
                choosingOpponent = false;
                for (int i = 0; i < CHARACTER_COUNT; i++) defeatedFighters[i] = false;

                p1CharId = CHAR_COMMANDER;
                p2CharId = PickNextAvailableOpponent(1);
                currentStage = homeStages[(int)p2CharId];
                stageMgr.SetStage(currentStage);
                gameState = STATE_SELECT_MAP;
                stateTimer = 0.0f;
                AudioEngine::Instance().SetBgmTrack(0);
            }
            break;
        }
        case STATE_ENDING_CELEBRATION: {
            inputP1.Update(0, true);
            if (stateTimer >= 2.0f && (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))) {

                matchesWon = 0;
                bonusStagePlayed = false;
                playerLocked = false;
                choosingOpponent = false;
                for (int i = 0; i < CHARACTER_COUNT; i++) defeatedFighters[i] = false;

                p1CharId = CHAR_COMMANDER;
                p2CharId = PickNextAvailableOpponent(1);
                currentStage = homeStages[(int)p2CharId];
                stageMgr.SetStage(currentStage);
                gameState = STATE_SPLASH;
                stateTimer = 0.0f;
                logoSlammed = false;
                AudioEngine::Instance().SetBgmTrack(0);
            }
            break;
        }
        case STATE_CTHULHU_ENDING: {
            static bool sharkSfxPlayed = false;
            static bool snoreSfxPlayed = false;
            if (stateTimer < 0.1f) {
                sharkSfxPlayed = false;
                snoreSfxPlayed = false;
            }
            if (stateTimer >= 5.0f && !sharkSfxPlayed) {
                sharkSfxPlayed = true;
                AudioEngine::Instance().PlaySfx(SFX_SHARK_CHOMP);
            }
            if (stateTimer >= 10.0f && !snoreSfxPlayed) {
                snoreSfxPlayed = true;
                AudioEngine::Instance().PlaySfx(SFX_CTHULHU_SNORE);
            }

                inputP1.Update(0, true);
                bool skip = (stateTimer >= 12.0f && (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                             inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                             inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)));
                if (stateTimer >= 18.0f || skip) {

                    matchesWon = 0;
                    bonusStagePlayed = false;
                    playerLocked = false;
                    choosingOpponent = false;
                    for (int i = 0; i < CHARACTER_COUNT; i++) defeatedFighters[i] = false;

                    p1CharId = CHAR_COMMANDER;
                    p2CharId = PickNextAvailableOpponent(1);
                    currentStage = homeStages[(int)p2CharId];
                    stageMgr.SetStage(currentStage);
                    gameState = STATE_SPLASH;
                    stateTimer = 0.0f;
                    logoSlammed = false;
                    AudioEngine::Instance().SetBgmTrack(0);
                }

            break;
        }
        }

        BeginTextureMode(screenTarget);
        ClearBackground(BLACK);

        float camRenderX = cameraX - SCREEN_WIDTH * 0.5f;
        if (cameraShake > 0.1f) {
            camRenderX += ((float)(rand() % 100) / 100.0f - 0.5f) * cameraShake * 2.0f;
        }

        switch (gameState) {
        case STATE_SPLASH: {
            if (flagShader.id > 0 && flagTimeLoc >= 0) SetShaderValue(flagShader, flagTimeLoc, &stateTimer, SHADER_UNIFORM_FLOAT);
            VisualsEngine::DrawCinematicIntro(stateTimer, logoSlammed, flagShader, flagTexture);
            break;
        }
        case STATE_SELECT_MAP: {
            VisualsEngine::DrawStageAndCharSelect((int)currentStage, (int)p1CharId, (int)p2CharId, stateTimer, defeatedFighters, playerLocked, (playerLocked || CountDefeatedOriginals() >= 7));
            break;
        }
        case STATE_SECRET_SELECT: {
            VisualsEngine::DrawSecretSelectScreen(secretStep, secretP1, secretP2, secretStage, secretDifficulty, stateTimer);
            break;
        }
        case STATE_VERSUS: {
            VisualsEngine::DrawVersusScreen(*p1, *p2, stateTimer);
            break;
        }
        case STATE_VICTORY_SPLASH: {
            int winnerChar = (p1 && p2 && p2->GetRoundsWon() > p1->GetRoundsWon()) ? (int)p2CharId : (int)p1CharId;
            int loserChar = (winnerChar == (int)p1CharId) ? (int)p2CharId : (int)p1CharId;
            VisualsEngine::DrawVictorySplash(winnerChar, loserChar, stateTimer);
            break;
        }
        case STATE_BONUS_CAR: {
            VisualsEngine::DrawBonusStage(bonusCarHp, BONUS_CAR_MAX_HP, bonusTimer, *p1, sparks);
            if (isGamePaused) {
                VisualsEngine::DrawPauseOverlay(stateTimer);
            }
            break;
        }
        case STATE_FIGHT: {

            stageMgr.DrawFarLayer(camRenderX, stateTimer);

            stageMgr.DrawMidLayer(camRenderX, stateTimer);

            stageMgr.DrawPlayfield(camRenderX, stateTimer);

            rlPushMatrix();
            rlTranslatef(-camRenderX, 0.0f, 0.0f);

            VisualsEngine::DrawDropShadow(*p1);
            VisualsEngine::DrawDropShadow(*p2);

            VisualsEngine::DrawFighterSprite(*p1);
            VisualsEngine::DrawFighterSprite(*p2);

            VisualsEngine::DrawProjectiles(*p1);
            VisualsEngine::DrawProjectiles(*p2);

            VisualsEngine::DrawSparks(sparks);
            ChampionshipEdition::DrawWorldParticlesAndDebug(*p1, *p2);

            rlPopMatrix();

            stageMgr.DrawForeground(camRenderX, stateTimer);

            VisualsEngine::DrawBattleHUD(*p1, *p2, roundSeconds, roundNumber, fightIntroTimer, p1WasKo, p2WasKo);
            ChampionshipEdition::DrawScreenHUDAndSuperFlash(*p1, *p2, inputP1);

            if (!fightActive && fightIntroTimer > 0) {
                if (fightIntroTimer > 50) {
                    if (roundNumber == 3) {
                        int tw = MeasureText("FINAL ROUND", 22);
                        VisualsEngine::DrawPixelText("FINAL ROUND", (SCREEN_WIDTH - tw) / 2, (SCREEN_HEIGHT / 2) - 16, 22, Color{ 255, 230, 40, 255 });
                    } else {
                        char roundBuf[32];
                        snprintf(roundBuf, sizeof(roundBuf), "ROUND %d", roundNumber);
                        int tw = MeasureText(roundBuf, 22);
                        VisualsEngine::DrawPixelText(roundBuf, (SCREEN_WIDTH - tw) / 2, (SCREEN_HEIGHT / 2) - 16, 22, Color{ 255, 230, 40, 255 });
                    }
                } else {
                    int tw = MeasureText("FIGHT!", 28);
                    VisualsEngine::DrawPixelText("FIGHT!", (SCREEN_WIDTH - tw) / 2, (SCREEN_HEIGHT / 2) - 20, 28, Color{ 255, 50, 50, 255 });
                }
            } else if (p1->GetHealth() <= 0 || p2->GetHealth() <= 0 || roundSeconds <= 0) {
                int tw = MeasureText("TOTAL VETO!", 26);
                VisualsEngine::DrawPixelText("TOTAL VETO!", (SCREEN_WIDTH - tw) / 2, (SCREEN_HEIGHT / 2) - 18, 26, Color{ 255, 60, 60, 255 });
            }

            if (isGamePaused) {
                VisualsEngine::DrawPauseOverlay(stateTimer);
            }
            break;
        }
        case STATE_MATCH_OVER: {
            stageMgr.DrawFarLayer(camRenderX, stateTimer);
            stageMgr.DrawMidLayer(camRenderX, stateTimer);
            stageMgr.DrawPlayfield(camRenderX, stateTimer);

            rlPushMatrix();
            rlTranslatef(-camRenderX, 0.0f, 0.0f);
            VisualsEngine::DrawDropShadow(*p1);
            VisualsEngine::DrawDropShadow(*p2);
            VisualsEngine::DrawFighterSprite(*p1);
            VisualsEngine::DrawFighterSprite(*p2);
            rlPopMatrix();

            int roundsToWin = (p2CharId == CHAR_WAR_PIG) ? 1 : 2;
            const Fighter& winner = (p1->GetRoundsWon() >= roundsToWin) ? *p1 : *p2;
            VisualsEngine::DrawMatchOver(winner, stateTimer);
            break;
        }
        case STATE_ENDING_CELEBRATION: {
            VisualsEngine::DrawEndingCelebration(stateTimer);
            break;
        }
        case STATE_CTHULHU_ENDING: {
            VisualsEngine::DrawCthulhuEnding(stateTimer);
            break;
        }
        }

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        float scale = std::min((float)GetScreenWidth() / (float)SCREEN_WIDTH, (float)GetScreenHeight() / (float)SCREEN_HEIGHT);
        int scaledW = (int)(SCREEN_WIDTH * scale);
        int scaledH = (int)(SCREEN_HEIGHT * scale);
        int offsetX = (GetScreenWidth() - scaledW) / 2;
        int offsetY = (GetScreenHeight() - scaledH) / 2;

        Rectangle srcRec = { 0.0f, 0.0f, (float)screenTarget.texture.width, -(float)screenTarget.texture.height };
        Rectangle dstRec = { (float)offsetX, (float)offsetY, (float)scaledW, (float)scaledH };

        if (enableCrt && crtShader.id > 0) {
            BeginShaderMode(crtShader);
            DrawTexturePro(screenTarget.texture, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            EndShaderMode();
        } else {
            DrawTexturePro(screenTarget.texture, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }

        DrawText(ChampionshipEdition::s_debugHitboxes ? "[F1: BOXES ON] [F2: CRT] [F3: 12-PART RIG]" : "[F1: DEBUG BOXES] [F2: CRT] [F3: 12-PART RIG]", 10, 8, 12, Color{ 140, 220, 180, 220 });

        char padStatus[96];
        if (InputManager::IsGamepadActive()) {
            snprintf(padStatus, sizeof(padStatus), "[PAD: %.26s]", InputManager::GetActiveControllerName(0));
            DrawText(padStatus, 140, 8, 12, Color{ 80, 240, 140, 240 });
        } else {
            DrawText("[PAD: KEYBOARD (CONNECT GAMEPAD)]", 140, 8, 12, Color{ 180, 180, 195, 200 });
        }

        rlDrawRenderBatchActive();

        EndDrawing();
    }

    UnloadTexture(flagTexture);
    if (flagShader.id > 0) UnloadShader(flagShader);
    if (crtShader.id > 0) UnloadShader(crtShader);
    UnloadRenderTexture(screenTarget);
    AudioEngine::Instance().Shutdown();
    VisualsEngine::Shutdown();
    stageMgr.Shutdown();
    CloseWindow();

    return 0;
}

