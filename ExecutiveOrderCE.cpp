// ============================================================================
// EXECUTIVE ORDER II: TOTAL VETO - CHAMPIONSHIP EDITION (CPS-1 16-BIT UPGRADE)
// ============================================================================
// Complete Single-File C++17 / Raylib 5.0 Arcade Fighting Game Engine
// Target Executable: ExecutiveOrderCE.exe
//
// Embedded License Notices:
// ----------------------------------------------------------------------------
// raylib is licensed under an unmodified zlib/libpng license (View raylib.h)
// Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
//
// This software is provided "as-is", without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//   1. The origin of this software must not be misrepresented; you must not
//      claim that you wrote the original software.
//   2. Altered source versions must be plainly marked as such, and must not be
//      misrepresented as being the original software.
//   3. This notice may not be removed or altered from any source distribution.
// ============================================================================

#include "raylib.h"
#include "rlgl.h"
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <atomic>

// ----------------------------------------------------------------------------
// CORE ARCADE CONSTANTS & DISPLAY TIMING
// ----------------------------------------------------------------------------
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
constexpr float MAX_FIGHTER_DISTANCE = 320.0f;

// ----------------------------------------------------------------------------
// ENUMS: ROSTER (8 FIGHTERS), VENUES (4 DC STAGES), MOVES & STATES
// ----------------------------------------------------------------------------
enum CharacterId : int {
    CHAR_COMMANDER = 0,       // POTUS - Presidential Shoto
    CHAR_HIGH_BENCH = 1,      // THE HIGH BENCH - Supreme Court Zoning / Gavel
    CHAR_REDCAP = 2,          // THE REDCAP - Rally Brawler / Truck Tackle
    CHAR_COASTAL_GOV = 3,     // GOVERNATOR - Slick Kickboxer / Flash Kick
    CHAR_TECH_VISIONARY = 4,  // TECHBRO - Algorithm Drone / Quantum Dash
    CHAR_FILIBUSTER = 5,      // THE FILIBUSTER - Old Guard Senator / Podium
    CHAR_MASCOT = 6,          // FREAGLE - Patriotic Mascot / Dive Talons
    CHAR_POPULIST = 7,        // THE POPULIST - Viral Firebrand / Blitz Kicks
    CHARACTER_COUNT = 8
};

enum StageId : int {
    STAGE_DEMOLISHED_EAST_WING = 0,
    STAGE_REFLECTING_POOL = 1,
    STAGE_CONGRESSIONAL_FLOOR = 2,
    STAGE_WHITE_HOUSE_MMA = 3,
    STAGE_COUNT = 4
};

enum GameState : int {
    STATE_SPLASH = 0,
    STATE_SELECT_MAP,
    STATE_VERSUS,
    STATE_FIGHT,
    STATE_VICTORY_SPLASH,
    STATE_MATCH_OVER
};

enum AttackHeight : int {
    ATK_NONE = 0,
    ATK_HIGH,
    ATK_MID,
    ATK_LOW,
    ATK_OVERHEAD,
    ATK_THROW,
    ATK_PROJECTILE
};

enum MoveId : int {
    MOVE_NONE = 0,
    MOVE_ST_LP,
    MOVE_ST_HP,
    MOVE_ST_LK,
    MOVE_ST_HK,
    MOVE_CR_LP,
    MOVE_CR_HP,
    MOVE_CR_LK,
    MOVE_CR_HK,        // Down + HK low sweep (causes hard knockdown)
    MOVE_FWD_HP,       // Forward + HP overhead (beats crouching block)
    MOVE_AIR_LP,
    MOVE_AIR_HP,
    MOVE_AIR_LK,
    MOVE_AIR_HK,
    MOVE_THROW,        // LP + LK close-range unblockable throw
    MOVE_SPEC_QCF,     // 236 + Attack (Signature Projectile)
    MOVE_SPEC_DP,      // 623 / Down-Up + Attack (Invincible Anti-Air Reversal)
    MOVE_SPEC_QCB,     // 214 + Attack (Advancing Dash / Counter Tackle)
    MOVE_SUPER_OMNIBUS,// 236236 + Attack at 100% Meter ("Omnibus Bill" Super)
    MOVE_COUNT
};

enum AnimId : int {
    ANIM_IDLE = 0,
    ANIM_WALK_FWD,
    ANIM_WALK_BACK,
    ANIM_CROUCH,
    ANIM_JUMP,
    ANIM_BLOCK_HIGH,
    ANIM_BLOCK_LOW,
    ANIM_HITSTUN,
    ANIM_BLOCKSTUN,
    ANIM_KNOCKDOWN,
    ANIM_WAKEUP,
    ANIM_VICTORY,
    ANIM_DEFEAT,
    ANIM_ST_LP,
    ANIM_ST_HP,
    ANIM_ST_LK,
    ANIM_ST_HK,
    ANIM_CR_LP,
    ANIM_CR_HP,
    ANIM_CR_LK,
    ANIM_CR_HK,
    ANIM_FWD_HP,
    ANIM_AIR_ATK,
    ANIM_THROW,
    ANIM_SPEC_QCF,
    ANIM_SPEC_DP,
    ANIM_SPEC_QCB,
    ANIM_SUPER_OMNIBUS,
    ANIM_COUNT
};

enum FighterState : int {
    FSTATE_IDLE = 0,
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
    FSTATE_THROW_GRAB,
    FSTATE_THROWN,
    FSTATE_VICTORY,
    FSTATE_DEFEAT
};

enum FacialState : int {
    FACE_IDLE_SMIRK = 0,
    FACE_EFFORT_GRIT,
    FACE_ANGRY_SHOUT,
    FACE_HIT_SHOCK,
    FACE_KO_DIZZY,
    FACE_VICTORY_GRIN
};

enum InputButtons : uint16_t {
    BTN_NONE    = 0,
    BTN_LP      = (1 << 0),
    BTN_HP      = (1 << 1),
    BTN_LK      = (1 << 2),
    BTN_HK      = (1 << 3),
    DIR_UP      = (1 << 4),
    DIR_DOWN    = (1 << 5),
    DIR_LEFT    = (1 << 6),
    DIR_RIGHT   = (1 << 7),
    BTN_START   = (1 << 8),
    BTN_COIN    = (1 << 9),
    BTN_SPECIAL = (1 << 10),
    BTN_SELECT  = (1 << 11)
};

// ----------------------------------------------------------------------------
// CRT & FLAG GLSL SHADERS (EMBEDDED)
// ----------------------------------------------------------------------------
static const char* CRT_FRAGMENT_SHADER = R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float scanlineIntensity;
uniform float curvature;
uniform float bloom;

void main()
{
    vec2 uv = fragTexCoord;
    vec2 dc = uv - 0.5;
    float dist = dot(dc, dc);
    uv = uv + dc * (dist * curvature);

    if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
        finalColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec4 col = texture(texture0, uv);

    float scanline = sin(uv.y * 224.0 * 3.14159265 * 2.0);
    float scanMod = 1.0 - (scanlineIntensity * (0.5 + 0.5 * scanline));
    col.rgb *= scanMod;

    float subpixel = mod(floor(uv.x * 384.0 * 3.0), 3.0);
    vec3 mask = vec3(0.92, 0.92, 0.92);
    if (subpixel < 1.0) mask = vec3(1.16, 0.88, 0.88);
    else if (subpixel < 2.0) mask = vec3(0.88, 1.16, 0.88);
    else mask = vec3(0.88, 0.88, 1.16);
    col.rgb *= mix(vec3(1.0), mask, bloom);

    float vx = uv.x * (1.0 - uv.x);
    float vy = uv.y * (1.0 - uv.y);
    float vig = clamp(16.0 * vx * vy, 0.0, 1.0);
    col.rgb *= (0.78 + 0.22 * vig);

    finalColor = col * fragColor;
}
)";

static const char* FLAG_FRAGMENT_SHADER = R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;

void main()
{
    vec2 uv = fragTexCoord;
    float ripple = sin(uv.x * 12.0 - time * 6.0) * 0.04 * (uv.x + 0.2);
    float rippleY = cos(uv.x * 8.0 - time * 5.0) * 0.02 * (uv.x + 0.2);
    uv.y += ripple;
    uv.x += rippleY;

    vec4 col = texture(texture0, uv);
    float shadow = 1.0 + (cos(uv.x * 12.0 - time * 6.0) * 0.15 * uv.x);
    col.rgb *= shadow;

    finalColor = col * fragColor;
}
)";

// ----------------------------------------------------------------------------
// COLOR & PIXEL DRAWING HELPERS (3-TONE CPS-1 SHADING + SILHOUETTES)
// ----------------------------------------------------------------------------
inline Color ShadeColor(Color c, float factor) {
    int r = (int)std::clamp(c.r * factor, 0.0f, 255.0f);
    int g = (int)std::clamp(c.g * factor, 0.0f, 255.0f);
    int b = (int)std::clamp(c.b * factor, 0.0f, 255.0f);
    return Color{ (unsigned char)r, (unsigned char)g, (unsigned char)b, c.a };
}

inline Color BevelHighlight(Color c, int boost = 48) {
    return Color{
        (unsigned char)std::min(255, (int)c.r + boost),
        (unsigned char)std::min(255, (int)c.g + boost),
        (unsigned char)std::min(255, (int)c.b + boost),
        c.a
    };
}

inline void DrawPixelText(const char* text, int x, int y, int size, Color col) {
    DrawText(text, x + 1, y + 1, size, Color{ 8, 10, 16, 235 });
    DrawText(text, x - 1, y, size, Color{ 8, 10, 16, 180 });
    DrawText(text, x, y, size, col);
}

inline void DrawCapsuleLimb(Vector2 a, Vector2 b, float radius, Color baseCol, float outlinePx = 1.5f) {
    Color outline = Color{ 12, 14, 20, 255 };
    Color darkTone = ShadeColor(baseCol, 0.68f);
    Color lightTone = BevelHighlight(baseCol, 42);

    // 1. Outer Dark Silhouette
    DrawLineEx(a, b, (radius + outlinePx) * 2.0f, outline);
    DrawCircleV(a, radius + outlinePx, outline);
    DrawCircleV(b, radius + outlinePx, outline);

    // 2. Shadow / Dark Tone Core
    DrawLineEx(a, b, radius * 2.0f, darkTone);
    DrawCircleV(a, radius, darkTone);
    DrawCircleV(b, radius, darkTone);

    // 3. Base Color Mid-Tone
    Vector2 offsetMid{ 0.0f, -radius * 0.18f };
    DrawLineEx(Vector2{ a.x + offsetMid.x, a.y + offsetMid.y },
               Vector2{ b.x + offsetMid.x, b.y + offsetMid.y },
               radius * 1.45f, baseCol);
    DrawCircleV(Vector2{ a.x + offsetMid.x, a.y + offsetMid.y }, radius * 0.72f, baseCol);
    DrawCircleV(Vector2{ b.x + offsetMid.x, b.y + offsetMid.y }, radius * 0.72f, baseCol);

    // 4. Directional Top/Front Bevel Highlight
    Vector2 offsetHi{ 0.0f, -radius * 0.38f };
    DrawLineEx(Vector2{ a.x + offsetHi.x, a.y + offsetHi.y },
               Vector2{ b.x + offsetHi.x, b.y + offsetHi.y },
                std::max(1.0f, radius * 0.65f), lightTone);
}

inline void DrawShadedBox(float x, float y, float w, float h, Color baseCol) {
    Color outline = Color{ 12, 14, 20, 255 };
    Color darkTone = ShadeColor(baseCol, 0.68f);
    Color lightTone = BevelHighlight(baseCol, 45);
    int ix = (int)std::round(x);
    int iy = (int)std::round(y);
    int iw = std::max(2, (int)std::round(w));
    int ih = std::max(2, (int)std::round(h));

    DrawRectangle(ix - 1, iy - 1, iw + 2, ih + 2, outline);
    DrawRectangle(ix, iy, iw, ih, darkTone);
    DrawRectangle(ix, iy, std::max(1, iw - 2), std::max(1, ih - 2), baseCol);
    DrawRectangle(ix + 1, iy + 1, std::max(1, iw - 3), std::max(1, ih / 3), lightTone);
}

// ----------------------------------------------------------------------------
// 3. PROCEDURAL ARCADE SYNTHESIZER & MULTI-VOICE AUDIO ENGINE (YM2151 / OKI6295)
// ----------------------------------------------------------------------------
enum SfxId : int {
    SFX_WHOOSH_LIGHT = 0,
    SFX_WHOOSH_HEAVY,
    SFX_HIT_LIGHT,
    SFX_HIT_HEAVY,
    SFX_BLOCK_THUD,
    SFX_THROW_GRAB,
    SFX_THROW_SLAM,
    SFX_PROJ_LAUNCH,
    SFX_PROJ_IMPACT,
    SFX_DP_REVERSAL,
    SFX_SUPER_FLASH,
    SFX_SUPER_HIT,
    SFX_COIN_INSERT,
    SFX_MENU_CURSOR,
    SFX_MENU_CONFIRM,
    SFX_VOICE_ROUND1,
    SFX_VOICE_ROUND2,
    SFX_VOICE_FINAL_ROUND,
    SFX_VOICE_DEBATE,
    SFX_VOICE_VETO,
    SFX_VOICE_OVERRULED,
    SFX_VOICE_FILIBUSTER,
    SFX_VOICE_OMNIBUS,
    SFX_VOICE_KO,
    SFX_VOICE_YOU_WIN,
    SFX_VOICE_GRUNT_LIGHT,
    SFX_VOICE_GRUNT_HEAVY,
    SFX_VOICE_GRUNT_HIT,
    SFX_COUNT
};

class ArcadeAudioEngine {
public:
    static ArcadeAudioEngine& Instance() {
        static ArcadeAudioEngine inst;
        return inst;
    }

    void Init() {
        if (m_initialized) return;
        InitAudioDevice();
        SetAudioStreamBufferSizeDefault(2048);

        SynthesizeAllSfx();

        m_bgmStream = LoadAudioStream(44100, 16, 1);
        SetAudioStreamCallback(m_bgmStream, &ArcadeAudioEngine::TrackerAudioCallback);
        PlayAudioStream(m_bgmStream);
        m_initialized = true;
    }

    void Shutdown() {
        if (!m_initialized) return;
        StopAudioStream(m_bgmStream);
        UnloadAudioStream(m_bgmStream);
        for (int i = 0; i < SFX_COUNT; i++) {
            if (m_sounds[i].frameCount > 0) UnloadSound(m_sounds[i]);
        }
        CloseAudioDevice();
        m_initialized = false;
    }

    void PlaySfx(SfxId id, float pitch = 1.0f, float volume = 0.9f) {
        if (!m_initialized || id < 0 || id >= SFX_COUNT) return;
        SetSoundPitch(m_sounds[id], pitch);
        SetSoundVolume(m_sounds[id], volume);
        PlaySound(m_sounds[id]);
    }

    void PlayFighterGrunt(CharacterId cid, bool heavy, bool isHit = false) {
        static const float charPitch[CHARACTER_COUNT] = {
            0.92f, // POTUS (deep blustery)
            0.85f, // High Bench (solemn baritone)
            0.96f, // Redcap (raspy brawler)
            1.06f, // Governator (smooth tenor)
            1.12f, // Tech Visionary (nasal mid-high)
            0.78f, // Filibuster (gravelly old senator)
            1.32f, // Mascot (eagle screech pitch)
            1.24f  // Populist (energetic higher pitch)
        };
        float p = charPitch[(int)cid % CHARACTER_COUNT];
        if (isHit) PlaySfx(SFX_VOICE_GRUNT_HIT, p, 0.95f);
        else if (heavy) PlaySfx(SFX_VOICE_GRUNT_HEAVY, p, 0.95f);
        else PlaySfx(SFX_VOICE_GRUNT_LIGHT, p, 0.85f);
    }

    void SetTrack(int trackIndex, bool criticalHealthTempo) {
        s_activeTrack.store(trackIndex);
        s_criticalTempo.store(criticalHealthTempo);
    }

    void SetCriticalTempo(bool critical) {
        s_criticalTempo.store(critical);
    }

private:
    ArcadeAudioEngine() = default;
    bool m_initialized = false;
    Sound m_sounds[SFX_COUNT]{};
    AudioStream m_bgmStream{};

    static std::atomic<int> s_activeTrack;
    static std::atomic<bool> s_criticalTempo;
    static double s_trackerTime;
    static uint32_t s_lfsr;

    static float NextNoise() {
        uint32_t bit = ((s_lfsr >> 0) ^ (s_lfsr >> 2) ^ (s_lfsr >> 3) ^ (s_lfsr >> 5)) & 1u;
        s_lfsr = (s_lfsr >> 1) | (bit << 15);
        return ((float)(s_lfsr & 0xFFFF) / 32768.0f) - 1.0f;
    }

    Sound MakeSoundFromPcm(const std::vector<int16_t>& pcm, int sampleRate = 44100) {
        Wave w{};
        w.frameCount = (unsigned int)pcm.size();
        w.sampleRate = sampleRate;
        w.sampleSize = 16;
        w.channels = 1;
        int16_t* buf = (int16_t*)MemAlloc((unsigned int)(pcm.size() * sizeof(int16_t)));
        memcpy(buf, pcm.data(), pcm.size() * sizeof(int16_t));
        w.data = buf;
        Sound s = LoadSoundFromWave(w);
        UnloadWave(w);
        return s;
    }

    // Procedural Vocal Formant Synthesizer (OKI6295 Arcade Speech Style)
    struct FormantPhoneme {
        float duration;
        float f0;       // Fundamental glottal pitch
        float f1, f2, f3; // 3 vocal tract formant frequencies
        float noiseMix; // Unvoiced sibilant/plosive noise
        float amp;
    };

    Sound BuildFormantVoice(const std::vector<FormantPhoneme>& seq, bool echoTail = false) {
        const int sr = 44100;
        float totalDur = 0.0f;
        for (const auto& p : seq) totalDur += p.duration;
        if (echoTail) totalDur += 0.38f;

        int totalSamples = (int)(totalDur * sr);
        std::vector<float> floatBuf(totalSamples, 0.0f);

        float phase0 = 0.0f, phase1 = 0.0f, phase2 = 0.0f, phase3 = 0.0f;
        int cursor = 0;

        for (const auto& ph : seq) {
            int count = (int)(ph.duration * sr);
            for (int i = 0; i < count && (cursor + i) < totalSamples; i++) {
                float t = (float)i / (float)count;
                float env = std::sin(t * PI_F);
                env = std::min(1.0f, env * 1.6f);

                float vibrato = 1.0f + 0.018f * std::sin(2.0f * PI_F * 6.5f * ((cursor + i) / (float)sr));
                phase0 += (ph.f0 * vibrato) / sr;
                if (phase0 >= 1.0f) phase0 -= 1.0f;

                // Glottal pulse train rich in harmonics
                float glottal = (phase0 < 0.35f) ? std::sin(phase0 / 0.35f * PI_F) : -0.35f;

                // Reset formant phases on glottal cycle for crisp vocal resonance
                phase1 += ph.f1 / sr;
                phase2 += ph.f2 / sr;
                phase3 += ph.f3 / sr;

                float formants = 0.55f * std::sin(2.0f * PI_F * phase1) * (0.6f + 0.4f * glottal)
                               + 0.32f * std::sin(2.0f * PI_F * phase2) * (0.5f + 0.5f * glottal)
                               + 0.13f * std::sin(2.0f * PI_F * phase3);

                float n = NextNoise() * ph.noiseMix;
                float sample = (formants * (1.0f - ph.noiseMix * 0.5f) + n) * env * ph.amp;

                // 8-bit ADPCM quantization grit
                sample = std::round(sample * 64.0f) / 64.0f;
                floatBuf[cursor + i] = sample;
            }
            cursor += count;
        }

        // Optional stadium slapback echo (for K.O., VETO, ROUND calls)
        if (echoTail) {
            int delay1 = (int)(0.11f * sr);
            int delay2 = (int)(0.22f * sr);
            for (int i = totalSamples - 1; i >= 0; i--) {
                float v = floatBuf[i];
                if (i >= delay1) v += floatBuf[i - delay1] * 0.42f;
                if (i >= delay2) v += floatBuf[i - delay2] * 0.22f;
                floatBuf[i] = std::clamp(v, -0.98f, 0.98f);
            }
        }

        std::vector<int16_t> pcm(totalSamples);
        for (int i = 0; i < totalSamples; i++) {
            pcm[i] = (int16_t)(std::clamp(floatBuf[i], -0.98f, 0.98f) * 28000.0f);
        }
        return MakeSoundFromPcm(pcm, sr);
    }

    void SynthesizeAllSfx() {
        const int sr = 44100;

        // 1. Light Whoosh
        {
            int n = (int)(0.11f * sr);
            std::vector<int16_t> pcm(n);
            for (int i = 0; i < n; i++) {
                float t = (float)i / n;
                float env = std::sin(t * PI_F);
                float freq = 520.0f - t * 320.0f;
                float s = (NextNoise() * 0.65f + std::sin(2.0f * PI_F * freq * (i / (float)sr)) * 0.35f) * env * 0.55f;
                pcm[i] = (int16_t)(s * 26000.0f);
            }
            m_sounds[SFX_WHOOSH_LIGHT] = MakeSoundFromPcm(pcm);
        }

        // 2. Heavy Whoosh
        {
            int n = (int)(0.18f * sr);
            std::vector<int16_t> pcm(n);
            for (int i = 0; i < n; i++) {
                float t = (float)i / n;
                float env = std::sin(t * PI_F);
                float freq = 340.0f - t * 220.0f;
                float s = (NextNoise() * 0.72f + std::sin(2.0f * PI_F * freq * (i / (float)sr)) * 0.45f) * env * 0.75f;
                pcm[i] = (int16_t)(std::clamp(s, -0.95f, 0.95f) * 28000.0f);
            }
            m_sounds[SFX_WHOOSH_HEAVY] = MakeSoundFromPcm(pcm);
        }

        // 3. Light Hit Impact (Crisp slap + low punch envelope)
        {
            int n = (int)(0.14f * sr);
            std::vector<int16_t> pcm(n);
            float ph = 0.0f;
            for (int i = 0; i < n; i++) {
                float t = (float)i / n;
                float env = std::exp(-t * 14.0f);
                float freq = 260.0f * std::exp(-t * 9.0f) + 55.0f;
                ph += freq / sr;
                float sq = (std::fmod(ph, 1.0f) < 0.5f) ? 1.0f : -1.0f;
                float s = (sq * 0.55f + NextNoise() * 0.65f) * env;
                pcm[i] = (int16_t)(std::clamp(s, -0.95f, 0.95f) * 29000.0f);
            }
            m_sounds[SFX_HIT_LIGHT] = MakeSoundFromPcm(pcm);
        }

        // 4. Heavy Smack Impact (CPS-1 crunch + sub-bass drop + sawtooth bite)
        {
            int n = (int)(0.26f * sr);
            std::vector<int16_t> pcm(n);
            float ph = 0.0f;
            for (int i = 0; i < n; i++) {
                float t = (float)i / n;
                float env = std::exp(-t * 8.5f);
                float freq = 320.0f * std::exp(-t * 11.0f) + 42.0f;
                ph += freq / sr;
                float saw = 2.0f * std::fmod(ph, 1.0f) - 1.0f;
                float sub = std::sin(2.0f * PI_F * ph * 0.5f);
                float s = (saw * 0.45f + sub * 0.55f + NextNoise() * 0.70f) * env;
                pcm[i] = (int16_t)(std::clamp(s, -0.98f, 0.98f) * 31000.0f);
            }
            m_sounds[SFX_HIT_HEAVY] = MakeSoundFromPcm(pcm);
        }

        // 5. Block Thud (Metallic hollow guard resonance)
        {
            int n = (int)(0.13f * sr);
            std::vector<int16_t> pcm(n);
            for (int i = 0; i < n; i++) {
                float t = (float)i / n;
                float env = std::exp(-t * 16.0f);
                float tm = i / (float)sr;
                float s = (std::sin(2.0f * PI_F * 195.0f * tm) * 0.6f
                         + std::sin(2.0f * PI_F * 580.0f * tm) * 0.35f
                         + NextNoise() * 0.25f) * env;
                pcm[i] = (int16_t)(std::clamp(s, -0.95f, 0.95f) * 27000.0f);
            }
            m_sounds[SFX_BLOCK_THUD] = MakeSoundFromPcm(pcm);
        }

        // 6. Throw Grab & Throw Slam
        {
            int n = (int)(0.12f * sr);
            std::vector<int16_t> pcm(n);
            for (int i = 0; i < n; i++) {
                float t = (float)i / n;
                float freq = 180.0f + t * 420.0f;
                float s = std::sin(2.0f * PI_F * freq * (i / (float)sr)) * (1.0f - t) * 0.8f;
                pcm[i] = (int16_t)(s * 26000.0f);
            }
            m_sounds[SFX_THROW_GRAB] = MakeSoundFromPcm(pcm);
        }
        {
            int n = (int)(0.35f * sr);
            std::vector<int16_t> pcm(n);
            float ph = 0.0f;
            for (int i = 0; i < n; i++) {
                float t = (float)i / n;
                float env = std::exp(-t * 6.5f);
                float freq = 180.0f * std::exp(-t * 8.0f) + 34.0f;
                ph += freq / sr;
                float s = (std::sin(2.0f * PI_F * ph) * 0.65f + NextNoise() * 0.75f) * env;
                pcm[i] = (int16_t)(std::clamp(s, -0.98f, 0.98f) * 31500.0f);
            }
            m_sounds[SFX_THROW_SLAM] = MakeSoundFromPcm(pcm);
        }

        // 7. Projectile Launch & Impact
        {
            int n = (int)(0.24f * sr);
            std::vector<int16_t> pcm(n);
            float ph = 0.0f;
            for (int i = 0; i < n; i++) {
                float t = (float)i / n;
                float env = (1.0f - t);
                float freq = 240.0f + std::sin(t * PI_F * 6.0f) * 140.0f + t * 380.0f;
                ph += freq / sr;
                float fm = std::sin(2.0f * PI_F * ph + 1.8f * std::sin(2.0f * PI_F * ph * 2.0f));
                float s = (fm * 0.65f + NextNoise() * 0.25f) * env;
                pcm[i] = (int16_t)(std::clamp(s, -0.95f, 0.95f) * 27000.0f);
            }
            m_sounds[SFX_PROJ_LAUNCH] = MakeSoundFromPcm(pcm);
            m_sounds[SFX_PROJ_IMPACT] = m_sounds[SFX_HIT_HEAVY];
        }

        // 8. DP Reversal Rising Jet
        {
            int n = (int)(0.28f * sr);
            std::vector<int16_t> pcm(n);
            float ph = 0.0f;
            for (int i = 0; i < n; i++) {
                float t = (float)i / n;
                float env = std::sin(t * PI_F);
                float freq = 160.0f + t * 720.0f;
                ph += freq / sr;
                float sq = (std::fmod(ph, 1.0f) < 0.5f) ? 0.8f : -0.8f;
                float s = (sq * 0.55f + NextNoise() * 0.45f) * env;
                pcm[i] = (int16_t)(std::clamp(s, -0.95f, 0.95f) * 28000.0f);
            }
            m_sounds[SFX_DP_REVERSAL] = MakeSoundFromPcm(pcm);
        }

        // 9. Super Combo Dramatic Flash Chime ("Omnibus Bill" Activation!)
        {
            int n = (int)(0.52f * sr);
            std::vector<int16_t> pcm(n);
            for (int i = 0; i < n; i++) {
                float t = (float)i / n;
                float tm = i / (float)sr;
                float env = std::exp(-t * 3.8f);
                float f1 = 523.25f + t * 260.0f;
                float f2 = 783.99f + t * 390.0f;
                float f3 = 1046.50f + t * 520.0f;
                float fm = std::sin(2.0f * PI_F * f1 * tm + 2.0f * std::sin(2.0f * PI_F * f1 * 2.0f * tm)) * 0.4f
                         + std::sin(2.0f * PI_F * f2 * tm) * 0.35f
                         + std::sin(2.0f * PI_F * f3 * tm) * 0.25f;
                float s = (fm + NextNoise() * (t < 0.12f ? 0.45f : 0.05f)) * env;
                pcm[i] = (int16_t)(std::clamp(s, -0.98f, 0.98f) * 30000.0f);
            }
            m_sounds[SFX_SUPER_FLASH] = MakeSoundFromPcm(pcm);
            m_sounds[SFX_SUPER_HIT] = m_sounds[SFX_THROW_SLAM];
        }

        // 10. Coin Insert, Menu Cursor & Confirm
        {
            int n = (int)(0.22f * sr);
            std::vector<int16_t> pcm(n);
            for (int i = 0; i < n; i++) {
                float tm = i / (float)sr;
                float freq = (tm < 0.06f) ? 987.77f : 1318.51f;
                float env = std::exp(-tm * 12.0f);
                pcm[i] = (int16_t)(std::sin(2.0f * PI_F * freq * tm) * env * 26000.0f);
            }
            m_sounds[SFX_COIN_INSERT] = MakeSoundFromPcm(pcm);
        }
        {
            int n = (int)(0.05f * sr);
            std::vector<int16_t> pcm(n);
            for (int i = 0; i < n; i++) {
                float tm = i / (float)sr;
                pcm[i] = (int16_t)(std::sin(2.0f * PI_F * 880.0f * tm) * (1.0f - (float)i / n) * 20000.0f);
            }
            m_sounds[SFX_MENU_CURSOR] = MakeSoundFromPcm(pcm);
        }
        {
            int n = (int)(0.16f * sr);
            std::vector<int16_t> pcm(n);
            for (int i = 0; i < n; i++) {
                float tm = i / (float)sr;
                float freq = (tm < 0.07f) ? 659.25f : 880.0f;
                pcm[i] = (int16_t)(std::sin(2.0f * PI_F * freq * tm) * (1.0f - (float)i / n) * 25000.0f);
            }
            m_sounds[SFX_MENU_CONFIRM] = MakeSoundFromPcm(pcm);
        }

        // 11. Procedural Formant Voice Callouts & Grunts
        m_sounds[SFX_VOICE_ROUND1] = BuildFormantVoice({
            { 0.14f, 125.0f, 450.0f, 1350.0f, 2400.0f, 0.12f, 0.90f }, // "ROU-"
            { 0.12f, 120.0f, 320.0f,  950.0f, 2200.0f, 0.08f, 0.85f }, // "-ND"
            { 0.20f, 138.0f, 650.0f, 1200.0f, 2500.0f, 0.06f, 0.95f }  // "ONE!"
        }, true);

        m_sounds[SFX_VOICE_ROUND2] = BuildFormantVoice({
            { 0.14f, 125.0f, 450.0f, 1350.0f, 2400.0f, 0.12f, 0.90f }, // "ROU-"
            { 0.12f, 120.0f, 320.0f,  950.0f, 2200.0f, 0.08f, 0.85f }, // "-ND"
            { 0.22f, 145.0f, 300.0f,  870.0f, 2250.0f, 0.25f, 0.95f }  // "TWO!"
        }, true);

        m_sounds[SFX_VOICE_FINAL_ROUND] = BuildFormantVoice({
            { 0.15f, 135.0f, 700.0f, 1300.0f, 2500.0f, 0.22f, 0.92f }, // "FI-"
            { 0.12f, 125.0f, 450.0f, 1100.0f, 2300.0f, 0.08f, 0.88f }, // "-NAL"
            { 0.22f, 140.0f, 480.0f, 1300.0f, 2400.0f, 0.12f, 0.95f }  // "ROUND!"
        }, true);

        m_sounds[SFX_VOICE_DEBATE] = BuildFormantVoice({
            { 0.11f, 135.0f, 380.0f, 1950.0f, 2600.0f, 0.15f, 0.90f }, // "DE-"
            { 0.24f, 155.0f, 520.0f, 2050.0f, 2750.0f, 0.18f, 1.00f }  // "-BATE!"
        }, true);

        m_sounds[SFX_VOICE_VETO] = BuildFormantVoice({
            { 0.16f, 150.0f, 290.0f, 2250.0f, 2900.0f, 0.20f, 0.98f }, // "VEE-"
            { 0.24f, 125.0f, 450.0f,  850.0f, 2300.0f, 0.15f, 0.95f }  // "-TO!"
        }, true);

        m_sounds[SFX_VOICE_OVERRULED] = BuildFormantVoice({
            { 0.12f, 130.0f, 450.0f,  850.0f, 2300.0f, 0.08f, 0.92f }, // "O-"
            { 0.12f, 122.0f, 500.0f, 1350.0f, 2200.0f, 0.14f, 0.88f }, // "-VER-"
            { 0.22f, 142.0f, 320.0f,  900.0f, 2150.0f, 0.12f, 0.98f }  // "-RULED!"
        }, true);

        m_sounds[SFX_VOICE_FILIBUSTER] = BuildFormantVoice({
            { 0.09f, 138.0f, 380.0f, 1950.0f, 2550.0f, 0.20f, 0.90f }, // "FI-"
            { 0.09f, 132.0f, 360.0f, 1850.0f, 2500.0f, 0.08f, 0.88f }, // "-LI-"
            { 0.12f, 148.0f, 620.0f, 1200.0f, 2400.0f, 0.15f, 0.95f }, // "-BUS-"
            { 0.16f, 124.0f, 480.0f, 1350.0f, 2200.0f, 0.18f, 0.92f }  // "-TER!"
        }, true);

        m_sounds[SFX_VOICE_OMNIBUS] = BuildFormantVoice({
            { 0.14f, 145.0f, 550.0f,  920.0f, 2300.0f, 0.08f, 0.96f }, // "OM-"
            { 0.10f, 138.0f, 320.0f, 2100.0f, 2700.0f, 0.10f, 0.92f }, // "-NI-"
            { 0.22f, 160.0f, 620.0f, 1220.0f, 2500.0f, 0.24f, 1.00f }  // "-BUS!"
        }, true);

        m_sounds[SFX_VOICE_KO] = BuildFormantVoice({
            { 0.22f, 158.0f, 520.0f, 2050.0f, 2750.0f, 0.22f, 1.00f }, // "KAY!"
            { 0.30f, 128.0f, 450.0f,  820.0f, 2300.0f, 0.08f, 1.00f }  // "OHHH!"
        }, true);

        m_sounds[SFX_VOICE_YOU_WIN] = BuildFormantVoice({
            { 0.16f, 136.0f, 310.0f,  880.0f, 2250.0f, 0.08f, 0.94f }, // "YOU"
            { 0.24f, 152.0f, 390.0f, 1980.0f, 2650.0f, 0.12f, 0.98f }  // "WIN!"
        }, true);

        m_sounds[SFX_VOICE_GRUNT_LIGHT] = BuildFormantVoice({
            { 0.09f, 142.0f, 620.0f, 1350.0f, 2450.0f, 0.22f, 0.88f }  // "HAH!"
        }, false);

        m_sounds[SFX_VOICE_GRUNT_HEAVY] = BuildFormantVoice({
            { 0.07f, 155.0f, 700.0f, 1450.0f, 2550.0f, 0.25f, 0.95f },
            { 0.12f, 128.0f, 540.0f, 1200.0f, 2350.0f, 0.15f, 0.95f }  // "HYAAH!"
        }, false);

        m_sounds[SFX_VOICE_GRUNT_HIT] = BuildFormantVoice({
            { 0.14f, 118.0f, 500.0f, 1050.0f, 2150.0f, 0.18f, 0.92f }  // "UUGH!"
        }, false);
    }

    // Real-time 16-Bit Chiptune Step-Sequencer Callback (1.25x Tempo on Critical Health!)
    static void TrackerAudioCallback(void* bufferData, unsigned int frames) {
        int16_t* out = static_cast<int16_t*>(bufferData);
        int track = s_activeTrack.load();
        bool crit = s_criticalTempo.load();
        double speedMul = crit ? 1.25 : 1.0;
        const double sr = 44100.0;

        // 4 distinct DC Stage themes + Title/Menu theme (MIDI-style semitone offsets from A2=110Hz)
        static const int bassPatterns[5][16] = {
            { 0, 0, 12, 0,  3, 3, 15, 3,  5, 5, 17, 5,  7, 10, 7, 5 }, // Stage 0: East Wing
            { 2, 2, 14, 2,  5, 5, 17, 5,  9, 9, 21, 9,  7,  7, 5, 2 }, // Stage 1: Reflecting Pool
            { 4, 4, 16, 4,  7, 7, 19, 7,  2, 2, 14, 2,  9,  7, 4, 2 }, // Stage 2: Congressional Floor
            { 0, 12, 0, 12, 5, 17, 5, 17, 7, 19, 7, 19, 10, 7, 5, 3 }, // Stage 3: South Lawn Octagon
            { 0,  7, 12, 7, 5, 12, 17, 12, 3, 10, 15, 10, 7, 14, 12, 7 } // Menu / Select
        };
        static const int leadPatterns[5][16] = {
            { 12, 15, 19, 24, 22, 19, 15, 12, 17, 20, 24, 27, 26, 24, 19, 15 },
            { 14, 17, 21, 26, 24, 21, 17, 14, 21, 24, 26, 29, 28, 26, 21, 17 },
            { 16, 19, 23, 28, 26, 23, 19, 16, 14, 17, 21, 26, 23, 19, 16, 12 },
            { 12, 24, 22, 19, 17, 29, 27, 24, 19, 31, 29, 26, 22, 19, 15, 12 },
            { 24, 19, 15, 12, 29, 24, 20, 17, 27, 22, 19, 15, 26, 22, 19, 14 }
        };

        int patIdx = std::clamp(track, 0, 4);

        for (unsigned int i = 0; i < frames; i++) {
            s_trackerTime += (1.0 / sr) * speedMul;
            double stepRate = 7.5; // 16th-note groove rate
            double stepPos = s_trackerTime * stepRate;
            int step = ((int)stepPos) & 15;
            double frac = stepPos - std::floor(stepPos);

            // 1. Driving FM Slap Bass
            int bSemi = bassPatterns[patIdx][step];
            double bFreq = 55.0 * std::pow(2.0, bSemi / 12.0);
            double bEnv = std::exp(-frac * 4.2);
            double bMod = std::sin(2.0 * PI_F * bFreq * 2.0 * s_trackerTime) * 2.2 * bEnv;
            double bass = std::sin(2.0 * PI_F * bFreq * s_trackerTime + bMod) * bEnv * 0.36;

            // 2. CPS-1 YM2151 Brass/Lead Synth
            int lSemi = leadPatterns[patIdx][step];
            double lFreq = 220.0 * std::pow(2.0, lSemi / 12.0);
            double lEnv = std::exp(-frac * 2.6);
            double lMod = std::sin(2.0 * PI_F * lFreq * 3.0 * s_trackerTime) * 1.1;
            double lead = std::sin(2.0 * PI_F * lFreq * s_trackerTime + lMod) * lEnv * 0.24;

            // 3. Arcade Percussion (Kick on 0,4,8,12; Snare on 4,12; Hi-hat on odd steps)
            double drm = 0.0;
            if ((step & 3) == 0) {
                double kFreq = 150.0 * std::exp(-frac * 18.0) + 42.0;
                drm += std::sin(2.0 * PI_F * kFreq * frac) * std::exp(-frac * 9.0) * 0.42;
            }
            if ((step & 7) == 4) {
                drm += NextNoise() * std::exp(-frac * 11.0) * 0.30;
            } else if ((step & 1) == 1) {
                drm += NextNoise() * std::exp(-frac * 26.0) * 0.12;
            }

            double mix = std::clamp(bass + lead + drm, -0.92, 0.92);
            out[i] = (int16_t)(mix * 19500.0);
        }
    }
};

std::atomic<int> ArcadeAudioEngine::s_activeTrack{ 4 };
std::atomic<bool> ArcadeAudioEngine::s_criticalTempo{ false };
double ArcadeAudioEngine::s_trackerTime = 0.0;
uint32_t ArcadeAudioEngine::s_lfsr = 0xACE1u;

// ----------------------------------------------------------------------------
// DIRECTIONAL INPUT BUFFER & MOTION PARSER (QCF, DP, QCB, 236236 SUPER, THROW)
// ----------------------------------------------------------------------------
enum RelativeDir : uint8_t {
    RDIR_NEUTRAL = 5,
    RDIR_UP = 8,
    RDIR_DOWN = 2,
    RDIR_FWD = 6,
    RDIR_BACK = 4,
    RDIR_UP_FWD = 9,
    RDIR_UP_BACK = 7,
    RDIR_DOWN_FWD = 3,
    RDIR_DOWN_BACK = 1
};

enum MotionType : int {
    MOT_NONE = 0,
    MOT_SUPER_236236, // 236236 + Attack ("Omnibus Bill" Super Combo)
    MOT_DP_623,       // 623 or Down-Up + Attack (Anti-Air Reversal)
    MOT_QCF_236,      // 236 + Attack (Signature Projectile)
    MOT_QCB_214       // 214 + Attack (Advancing Dash / Counter Tackle)
};

struct RawInputState {
    bool up = false, down = false, left = false, right = false;
    bool lp = false, hp = false, lk = false, hk = false;
    bool special = false, start = false, select = false, coin = false;
};

struct InputFrameEntry {
    RelativeDir relDir = RDIR_NEUTRAL;
    uint16_t held = 0;
    uint16_t pressed = 0;
};

class InputBuffer {
public:
    static constexpr int HISTORY_LEN = 60;

    InputBuffer() { Reset(); }

    void Reset() {
        m_prev = RawInputState{};
        m_curr = RawInputState{};
        for (auto& f : m_ring) f = InputFrameEntry{};
        m_head = 0;
        m_chargeDown = 0;
    }

    static RawInputState PollHardware(int playerIndex) {
        RawInputState s{};
        if (playerIndex == 0) {
            s.up    = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
            s.down  = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
            s.left  = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
            s.right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);
            s.lp    = IsKeyDown(KEY_U) || IsKeyDown(KEY_J) || IsKeyDown(KEY_Z);
            s.hp    = IsKeyDown(KEY_I) || IsKeyDown(KEY_K) || IsKeyDown(KEY_X);
            s.lk    = IsKeyDown(KEY_O) || IsKeyDown(KEY_L) || IsKeyDown(KEY_C);
            s.hk    = IsKeyDown(KEY_P) || IsKeyDown(KEY_SEMICOLON) || IsKeyDown(KEY_V);
            s.special = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_B);
            s.start = IsKeyDown(KEY_ENTER);
            s.coin  = IsKeyDown(KEY_FIVE) || IsKeyDown(KEY_RIGHT_SHIFT);
            s.select = IsKeyDown(KEY_BACKSPACE) || IsKeyDown(KEY_ESCAPE);
        }

        int pad = (playerIndex == 0) ? 0 : 1;
        if (IsGamepadAvailable(pad)) {
            float ax = GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_X);
            float ay = GetGamepadAxisMovement(pad, GAMEPAD_AXIS_LEFT_Y);
            if (ay < -0.45f || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_UP)) s.up = true;
            if (ay >  0.45f || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) s.down = true;
            if (ax < -0.45f || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) s.left = true;
            if (ax >  0.45f || IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) s.right = true;

            if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))  s.lp = true;
            if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_UP))    s.hp = true;
            if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))  s.lk = true;
            if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) s.hk = true;
            if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ||
                IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1))   s.special = true;
            if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT))     s.start = true;
            if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_LEFT))      s.select = true;
        }
        return s;
    }

    void Update(const RawInputState& raw, bool facingRight) {
        m_prev = m_curr;
        m_curr = raw;

        uint16_t held = 0;
        if (raw.lp) held |= BTN_LP;
        if (raw.hp) held |= BTN_HP;
        if (raw.lk) held |= BTN_LK;
        if (raw.hk) held |= BTN_HK;
        if (raw.up) held |= DIR_UP;
        if (raw.down) held |= DIR_DOWN;
        if (raw.left) held |= DIR_LEFT;
        if (raw.right) held |= DIR_RIGHT;
        if (raw.start) held |= BTN_START;
        if (raw.coin) held |= BTN_COIN;
        if (raw.special) held |= BTN_SPECIAL;
        if (raw.select) held |= BTN_SELECT;

        uint16_t prevHeld = m_ring[m_head].held;
        uint16_t pressed = held & (~prevHeld);

        m_head = (m_head + 1) % HISTORY_LEN;
        m_ring[m_head].relDir = ComputeRelDir(raw, facingRight);
        m_ring[m_head].held = held;
        m_ring[m_head].pressed = pressed;

        if (m_ring[m_head].relDir == RDIR_DOWN || m_ring[m_head].relDir == RDIR_DOWN_BACK || m_ring[m_head].relDir == RDIR_DOWN_FWD) {
            m_chargeDown = std::min(60, m_chargeDown + 1);
        } else if (m_chargeDown > 0) {
            m_chargeDown--;
        }
    }

    const RawInputState& Raw() const { return m_curr; }
    RelativeDir RelDir() const { return m_ring[m_head].relDir; }
    uint16_t Held() const { return m_ring[m_head].held; }
    uint16_t Pressed() const { return m_ring[m_head].pressed; }
    bool IsPressed(uint16_t mask) const { return (m_ring[m_head].pressed & mask) != 0; }
    bool IsHeld(uint16_t mask) const { return (m_ring[m_head].held & mask) != 0; }

    // Detect 236236 Super, 623 DP / Down-Up, 236 QCF, and 214 QCB motions
    MotionType DetectMotion(int windowFrames = 26) const {
        if (MatchSequence({ RDIR_DOWN, RDIR_DOWN_FWD, RDIR_FWD, RDIR_DOWN, RDIR_DOWN_FWD, RDIR_FWD }, 32) ||
            MatchSequence({ RDIR_DOWN, RDIR_FWD, RDIR_DOWN, RDIR_FWD }, 28)) {
            return MOT_SUPER_236236;
        }
        if (MatchSequence({ RDIR_FWD, RDIR_DOWN, RDIR_DOWN_FWD }, windowFrames) ||
            MatchSequence({ RDIR_FWD, RDIR_DOWN, RDIR_FWD }, windowFrames) ||
            MatchSequence({ RDIR_DOWN, RDIR_UP }, 16)) {
            return MOT_DP_623;
        }
        if (MatchSequence({ RDIR_DOWN, RDIR_DOWN_FWD, RDIR_FWD }, windowFrames) ||
            MatchSequence({ RDIR_DOWN, RDIR_FWD }, 16)) {
            return MOT_QCF_236;
        }
        if (MatchSequence({ RDIR_DOWN, RDIR_DOWN_BACK, RDIR_BACK }, windowFrames) ||
            MatchSequence({ RDIR_DOWN, RDIR_BACK }, 16)) {
            return MOT_QCB_214;
        }
        return MOT_NONE;
    }

    void ConsumeMotions() {
        for (auto& f : m_ring) f.relDir = RDIR_NEUTRAL;
    }

    std::string GetRecentBufferString(int count = 10) const {
        static const char* numpadChar[10] = { "5","1","2","3","4","5","6","7","8","9" };
        std::string s;
        for (int i = count - 1; i >= 0; i--) {
            int idx = (m_head - i + HISTORY_LEN) % HISTORY_LEN;
            int d = (int)m_ring[idx].relDir;
            if (d >= 1 && d <= 9) s += numpadChar[d];
            if (m_ring[idx].pressed & BTN_LP) s += "[LP]";
            if (m_ring[idx].pressed & BTN_HP) s += "[HP]";
            if (m_ring[idx].pressed & BTN_LK) s += "[LK]";
            if (m_ring[idx].pressed & BTN_HK) s += "[HK]";
        }
        return s;
    }

private:
    RawInputState m_prev{};
    RawInputState m_curr{};
    std::array<InputFrameEntry, HISTORY_LEN> m_ring{};
    int m_head = 0;
    int m_chargeDown = 0;

    static RelativeDir ComputeRelDir(const RawInputState& s, bool facingRight) {
        bool fwd  = facingRight ? s.right : s.left;
        bool back = facingRight ? s.left  : s.right;
        if (s.up && fwd)    return RDIR_UP_FWD;
        if (s.up && back)   return RDIR_UP_BACK;
        if (s.up)           return RDIR_UP;
        if (s.down && fwd)  return RDIR_DOWN_FWD;
        if (s.down && back) return RDIR_DOWN_BACK;
        if (s.down)         return RDIR_DOWN;
        if (fwd)            return RDIR_FWD;
        if (back)           return RDIR_BACK;
        return RDIR_NEUTRAL;
    }

    bool MatchSequence(const std::vector<RelativeDir>& seq, int maxFramesBack) const {
        int seqIdx = (int)seq.size() - 1;
        for (int f = 0; f < maxFramesBack && seqIdx >= 0; f++) {
            int idx = (m_head - f + HISTORY_LEN) % HISTORY_LEN;
            if (m_ring[idx].relDir == seq[seqIdx]) {
                seqIdx--;
            }
        }
        return (seqIdx < 0);
    }
};

// ----------------------------------------------------------------------------
// 1 & 2. HIERARCHICAL 12-PART RIG, 8-12 KEYFRAME ANIMATIONS & DATA-DRIVEN MOVES
// ----------------------------------------------------------------------------
struct RigKeyframe {
    float normTime = 0.0f;       // 0.0 .. 1.0 normalized progress through clip
    float pelvisOffsetX = 0.0f;  // Root horizontal shift
    float pelvisOffsetY = 0.0f;  // Root vertical drop/lift
    float torsoTilt = 0.0f;      // Degrees forward(+) / back(-)
    float headTilt = 0.0f;       // Degrees nod
    float jawOpen = 0.0f;        // 0.0 (closed) .. 1.0 (wide shout)
    float hairSway = 0.0f;       // Accessory/hair recoil angle
    float uArmL = 25.0f, fArmL = -45.0f, handL = 0.0f;
    float uArmR = -20.0f, fArmR = -55.0f, handR = 0.0f;
    float thighL = -15.0f, calfL = 20.0f, footL = 0.0f;
    float thighR = 18.0f,  calfR = 15.0f, footR = 0.0f;
    bool drawSmearArc = false;
    bool smearIsKick = false;
};

inline float LerpAngle(float a, float b, float t) {
    return a + (b - a) * t;
}

inline RigKeyframe InterpolatePose(const RigKeyframe& a, const RigKeyframe& b, float t) {
    RigKeyframe r{};
    r.normTime      = LerpAngle(a.normTime, b.normTime, t);
    r.pelvisOffsetX = LerpAngle(a.pelvisOffsetX, b.pelvisOffsetX, t);
    r.pelvisOffsetY = LerpAngle(a.pelvisOffsetY, b.pelvisOffsetY, t);
    r.torsoTilt     = LerpAngle(a.torsoTilt, b.torsoTilt, t);
    r.headTilt      = LerpAngle(a.headTilt, b.headTilt, t);
    r.jawOpen       = LerpAngle(a.jawOpen, b.jawOpen, t);
    r.hairSway      = LerpAngle(a.hairSway, b.hairSway, t);
    r.uArmL         = LerpAngle(a.uArmL, b.uArmL, t);
    r.fArmL         = LerpAngle(a.fArmL, b.fArmL, t);
    r.handL         = LerpAngle(a.handL, b.handL, t);
    r.uArmR         = LerpAngle(a.uArmR, b.uArmR, t);
    r.fArmR         = LerpAngle(a.fArmR, b.fArmR, t);
    r.handR         = LerpAngle(a.handR, b.handR, t);
    r.thighL        = LerpAngle(a.thighL, b.thighL, t);
    r.calfL         = LerpAngle(a.calfL, b.calfL, t);
    r.footL         = LerpAngle(a.footL, b.footL, t);
    r.thighR        = LerpAngle(a.thighR, b.thighR, t);
    r.calfR         = LerpAngle(a.calfR, b.calfR, t);
    r.footR         = LerpAngle(a.footR, b.footR, t);
    r.drawSmearArc  = (t < 0.65f) ? a.drawSmearArc : b.drawSmearArc;
    r.smearIsKick   = a.smearIsKick || b.smearIsKick;
    return r;
}

// Generate 10-Keyframe Interpolated Animation Table for every AnimId
inline std::array<RigKeyframe, 10> BuildAnimKeyframes(AnimId id) {
    std::array<RigKeyframe, 10> keys{};
    for (int i = 0; i < 10; i++) {
        float u = (float)i / 9.0f;
        RigKeyframe& k = keys[i];
        k.normTime = u;

        switch (id) {
        case ANIM_IDLE: {
            float ph = u * 2.0f * PI_F;
            k.pelvisOffsetY = std::sin(ph) * 2.2f;
            k.torsoTilt = 4.0f + std::sin(ph) * 2.5f;
            k.headTilt = -2.0f + std::cos(ph) * 2.0f;
            k.hairSway = std::sin(ph - 0.6f) * 5.0f;
            k.uArmL = 28.0f + std::sin(ph) * 6.0f;  k.fArmL = -68.0f;
            k.uArmR = -18.0f + std::cos(ph) * 6.0f; k.fArmR = -78.0f;
            k.thighL = -16.0f; k.calfL = 18.0f;
            k.thighR = 18.0f;  k.calfR = 16.0f;
            break;
        }
        case ANIM_WALK_FWD:
        case ANIM_WALK_BACK: {
            float dir = (id == ANIM_WALK_FWD) ? 1.0f : -1.0f;
            float ph = u * 2.0f * PI_F * dir;
            k.pelvisOffsetY = -std::fabs(std::sin(ph)) * 3.2f;
            k.torsoTilt = (id == ANIM_WALK_FWD) ? 9.0f : -6.0f;
            k.hairSway = -dir * 6.0f + std::sin(ph) * 4.0f;
            k.uArmL = 20.0f - std::sin(ph) * 22.0f; k.fArmL = -60.0f;
            k.uArmR = -10.0f + std::sin(ph) * 22.0f; k.fArmR = -65.0f;
            k.thighL = std::sin(ph) * 32.0f;
            k.calfL = (std::sin(ph) < 0.0f) ? 35.0f : 10.0f;
            k.thighR = -std::sin(ph) * 32.0f;
            k.calfR = (-std::sin(ph) < 0.0f) ? 35.0f : 10.0f;
            break;
        }
        case ANIM_CROUCH:
        case ANIM_BLOCK_LOW: {
            k.pelvisOffsetY = 14.0f + std::sin(u * 2.0f * PI_F) * 1.0f;
            k.torsoTilt = 16.0f;
            k.uArmL = 35.0f; k.fArmL = -85.0f;
            k.uArmR = (id == ANIM_BLOCK_LOW) ? 48.0f : 15.0f;
            k.fArmR = (id == ANIM_BLOCK_LOW) ? -105.0f : -80.0f;
            k.thighL = -58.0f; k.calfL = 88.0f;
            k.thighR = 45.0f;  k.calfR = 78.0f;
            break;
        }
        case ANIM_JUMP: {
            k.pelvisOffsetY = -4.0f;
            k.torsoTilt = 8.0f;
            k.hairSway = -12.0f;
            k.uArmL = -35.0f; k.fArmL = -60.0f;
            k.uArmR = 40.0f;  k.fArmR = -70.0f;
            k.thighL = 42.0f; k.calfL = 65.0f;
            k.thighR = -25.0f; k.calfR = 55.0f;
            break;
        }
        case ANIM_BLOCK_HIGH:
        case ANIM_BLOCKSTUN: {
            float shake = (id == ANIM_BLOCKSTUN) ? std::sin(u * PI_F * 4.0f) * 3.0f : 0.0f;
            k.pelvisOffsetX = -3.0f + shake;
            k.torsoTilt = -8.0f;
            k.uArmL = 55.0f; k.fArmL = -105.0f;
            k.uArmR = 62.0f; k.fArmR = -112.0f;
            k.thighL = -22.0f; k.calfL = 26.0f;
            k.thighR = 22.0f;  k.calfR = 24.0f;
            break;
        }
        case ANIM_HITSTUN: {
            float arch = std::sin(u * PI_F);
            k.pelvisOffsetX = -6.0f * arch;
            k.torsoTilt = -24.0f * arch;
            k.headTilt = -28.0f * arch;
            k.jawOpen = 0.85f * arch;
            k.hairSway = -25.0f * arch;
            k.uArmL = -45.0f * arch; k.fArmL = -20.0f;
            k.uArmR = 35.0f * arch;  k.fArmR = -30.0f;
            k.thighL = -18.0f; k.calfL = 25.0f;
            k.thighR = 14.0f;  k.calfR = 22.0f;
            break;
        }
        case ANIM_KNOCKDOWN:
        case ANIM_DEFEAT: {
            float fall = std::min(1.0f, u * 1.4f);
            k.pelvisOffsetY = 26.0f * fall;
            k.torsoTilt = -78.0f * fall;
            k.headTilt = -20.0f * fall;
            k.jawOpen = 0.7f;
            k.uArmL = -70.0f * fall; k.fArmL = -10.0f;
            k.uArmR = -50.0f * fall; k.fArmR = -15.0f;
            k.thighL = 40.0f * fall; k.calfL = 20.0f;
            k.thighR = 65.0f * fall; k.calfR = 15.0f;
            break;
        }
        case ANIM_WAKEUP: {
            float rise = 1.0f - u;
            k.pelvisOffsetY = 24.0f * rise;
            k.torsoTilt = -65.0f * rise;
            k.uArmL = 25.0f; k.fArmL = -55.0f;
            k.uArmR = -15.0f; k.fArmR = -60.0f;
            k.thighL = -20.0f; k.calfL = 30.0f;
            k.thighR = 20.0f;  k.calfR = 25.0f;
            break;
        }
        case ANIM_VICTORY: {
            float pump = std::sin(u * PI_F * 2.0f);
            k.pelvisOffsetY = -std::fabs(pump) * 3.0f;
            k.torsoTilt = -6.0f;
            k.jawOpen = 0.45f;
            k.uArmL = 135.0f + pump * 18.0f; k.fArmL = -25.0f;
            k.uArmR = 145.0f - pump * 18.0f; k.fArmR = -20.0f;
            k.thighL = -18.0f; k.calfL = 12.0f;
            k.thighR = 18.0f;  k.calfR = 12.0f;
            break;
        }
        case ANIM_ST_LP:
        case ANIM_CR_LP: {
            float crouchY = (id == ANIM_CR_LP) ? 14.0f : 0.0f;
            bool startup = (i <= 2);
            bool active  = (i >= 3 && i <= 5);
            k.pelvisOffsetY = crouchY;
            k.pelvisOffsetX = active ? 5.0f : (startup ? -2.0f : 1.0f);
            k.torsoTilt = active ? 14.0f : (startup ? -6.0f : 6.0f);
            k.jawOpen = active ? 0.35f : 0.0f;
            k.uArmR = active ? 88.0f : (startup ? -30.0f : 20.0f);
            k.fArmR = active ? 0.0f  : -85.0f;
            k.uArmL = -25.0f; k.fArmL = -70.0f;
            k.thighL = (id == ANIM_CR_LP) ? -55.0f : -18.0f;
            k.calfL  = (id == ANIM_CR_LP) ? 85.0f  : 20.0f;
            k.thighR = (id == ANIM_CR_LP) ? 45.0f  : 22.0f;
            k.calfR  = (id == ANIM_CR_LP) ? 75.0f  : 18.0f;
            k.drawSmearArc = active;
            k.smearIsKick = false;
            break;
        }
        case ANIM_ST_HP:
        case ANIM_CR_HP:
        case ANIM_FWD_HP: {
            float crouchY = (id == ANIM_CR_HP) ? 13.0f : ((id == ANIM_FWD_HP) ? -4.0f : 0.0f);
            bool startup = (i <= 3);
            bool active  = (i >= 4 && i <= 6);
            k.pelvisOffsetY = crouchY;
            k.pelvisOffsetX = active ? 10.0f : (startup ? -6.0f : 3.0f);
            k.torsoTilt = active ? ((id == ANIM_FWD_HP) ? 28.0f : 22.0f) : (startup ? -16.0f : 8.0f);
            k.jawOpen = active ? 0.85f : 0.2f;
            k.hairSway = active ? -18.0f : 10.0f;
            k.uArmR = active ? ((id == ANIM_FWD_HP) ? 62.0f : 94.0f) : (startup ? -55.0f : 25.0f);
            k.fArmR = active ? -4.0f : -95.0f;
            k.uArmL = active ? -48.0f : 35.0f; k.fArmL = -65.0f;
            k.thighL = (id == ANIM_CR_HP) ? -55.0f : -24.0f;
            k.calfL  = (id == ANIM_CR_HP) ? 85.0f  : 26.0f;
            k.thighR = (id == ANIM_CR_HP) ? 45.0f  : 28.0f;
            k.calfR  = (id == ANIM_CR_HP) ? 75.0f  : 22.0f;
            k.drawSmearArc = active;
            k.smearIsKick = false;
            break;
        }
        case ANIM_ST_LK:
        case ANIM_CR_LK: {
            float crouchY = (id == ANIM_CR_LK) ? 15.0f : 0.0f;
            bool startup = (i <= 2);
            bool active  = (i >= 3 && i <= 5);
            k.pelvisOffsetY = crouchY;
            k.torsoTilt = active ? -8.0f : 4.0f;
            k.uArmL = 30.0f; k.fArmL = -70.0f;
            k.uArmR = -20.0f; k.fArmR = -65.0f;
            k.thighL = (id == ANIM_CR_LK) ? -55.0f : -12.0f;
            k.calfL  = (id == ANIM_CR_LK) ? 85.0f  : 16.0f;
            k.thighR = active ? ((id == ANIM_CR_LK) ? 75.0f : 84.0f) : (startup ? 45.0f : 20.0f);
            k.calfR  = active ? 4.0f : 65.0f;
            k.drawSmearArc = active;
            k.smearIsKick = true;
            break;
        }
        case ANIM_ST_HK:
        case ANIM_CR_HK:
        case ANIM_AIR_ATK: {
            bool isSweep = (id == ANIM_CR_HK);
            bool startup = (i <= 3);
            bool active  = (i >= 4 && i <= 6);
            k.pelvisOffsetY = isSweep ? 18.0f : 0.0f;
            k.pelvisOffsetX = active ? 8.0f : (startup ? -4.0f : 2.0f);
            k.torsoTilt = active ? (isSweep ? -18.0f : -22.0f) : 6.0f;
            k.jawOpen = active ? 0.75f : 0.1f;
            k.uArmL = -40.0f; k.fArmL = -50.0f;
            k.uArmR = 45.0f;  k.fArmR = -60.0f;
            k.thighL = isSweep ? -65.0f : -18.0f;
            k.calfL  = isSweep ? 95.0f  : 20.0f;
            k.thighR = active ? (isSweep ? 78.0f : 112.0f) : (startup ? 55.0f : 24.0f);
            k.calfR  = active ? 0.0f : 75.0f;
            k.drawSmearArc = active;
            k.smearIsKick = true;
            break;
        }
        case ANIM_THROW: {
            bool grab = (i <= 3);
            k.pelvisOffsetX = grab ? 8.0f : -6.0f;
            k.torsoTilt = grab ? 24.0f : -28.0f;
            k.jawOpen = 0.85f;
            k.uArmL = grab ? 85.0f : 135.0f; k.fArmL = -15.0f;
            k.uArmR = grab ? 90.0f : 140.0f; k.fArmR = -10.0f;
            k.thighL = -25.0f; k.calfL = 28.0f;
            k.thighR = 30.0f;  k.calfR = 24.0f;
            k.drawSmearArc = !grab;
            break;
        }
        case ANIM_SPEC_QCF: {
            bool startup = (i <= 3);
            bool active  = (i >= 4 && i <= 7);
            k.pelvisOffsetX = active ? 9.0f : -6.0f;
            k.pelvisOffsetY = 4.0f;
            k.torsoTilt = active ? 25.0f : -14.0f;
            k.jawOpen = active ? 1.0f : 0.2f;
            k.hairSway = active ? -22.0f : 12.0f;
            k.uArmL = active ? 88.0f : -45.0f; k.fArmL = active ? 0.0f : -80.0f;
            k.uArmR = active ? 92.0f : -50.0f; k.fArmR = active ? 0.0f : -85.0f;
            k.thighL = -30.0f; k.calfL = 35.0f;
            k.thighR = 36.0f;  k.calfR = 28.0f;
            k.drawSmearArc = active;
            break;
        }
        case ANIM_SPEC_DP: {
            bool startup = (i <= 2);
            bool active  = (i >= 3 && i <= 7);
            k.pelvisOffsetY = startup ? 12.0f : -8.0f;
            k.torsoTilt = active ? -16.0f : 22.0f;
            k.jawOpen = 1.0f;
            k.hairSway = -28.0f;
            k.uArmR = active ? 162.0f : -35.0f;
            k.fArmR = active ? -8.0f  : -95.0f;
            k.uArmL = -45.0f; k.fArmL = -70.0f;
            k.thighL = active ? -35.0f : -50.0f; k.calfL = 45.0f;
            k.thighR = active ? 65.0f  : 40.0f;  k.calfR = active ? 75.0f : 60.0f;
            k.drawSmearArc = active;
            break;
        }
        case ANIM_SPEC_QCB:
        case ANIM_SUPER_OMNIBUS:
        default: {
            bool active = (i >= 2 && i <= 7);
            float fl = (id == ANIM_SUPER_OMNIBUS) ? std::sin(u * PI_F * 6.0f) * 35.0f : 0.0f;
            k.pelvisOffsetX = active ? 12.0f : -4.0f;
            k.torsoTilt = active ? 26.0f : -10.0f;
            k.jawOpen = 1.0f;
            k.hairSway = -25.0f;
            k.uArmR = active ? (95.0f + fl) : -40.0f; k.fArmR = -5.0f;
            k.uArmL = active ? (85.0f - fl) : -35.0f; k.fArmL = -10.0f;
            k.thighL = -28.0f; k.calfL = 32.0f;
            k.thighR = 38.0f;  k.calfR = 24.0f;
            k.drawSmearArc = active;
            break;
        }
        }
    }
    return keys;
}

// ----------------------------------------------------------------------------
// 2. DETERMINISTIC MOVEDEF & FRAMEDATA ARCHITECTURE
// ----------------------------------------------------------------------------
struct MoveDef {
    MoveId id = MOVE_NONE;
    const char* name = "Idle";
    AnimId anim = ANIM_IDLE;
    int startupFrames = 4;
    int activeFrames = 4;
    int recoveryFrames = 8;
    int damage = 8;
    int chipDamage = 2;
    AttackHeight height = ATK_MID;
    int hitstopFrames = 6;       // 6 Light, 11 Heavy, 14 Special/Super
    int hitstunFrames = 14;
    int blockstunFrames = 10;
    float pushback = 3.2f;
    bool causesKnockdown = false;
    bool cancelable = false;     // 2-in-1 Special/Super cancel window
    int invincibleFrames = 0;    // Startup invincibility (DP Reversals & Supers)
    int armorHits = 0;           // Advancing tackle armor
    float velX = 0.0f;
    float velY = 0.0f;
    int meterGain = 8;
    int multiHits = 1;
    Rectangle localHitbox{ 16.0f, -52.0f, 32.0f, 18.0f };
};

struct Projectile {
    Vector2 pos{ 0.0f, 0.0f };
    Vector2 vel{ 0.0f, 0.0f };
    float width = 22.0f;
    float height = 16.0f;
    int damage = 12;
    int lifetime = 150;
    int animFrame = 0;
    bool active = false;
    bool facingRight = true;
    CharacterId ownerChar = CHAR_COMMANDER;
    bool isSuperProj = false;
};

struct VerletNode {
    Vector2 pos{ 0.0f, 0.0f };
    Vector2 prev{ 0.0f, 0.0f };
};

// ----------------------------------------------------------------------------
// CHARACTER PROFILE & MOVE TABLE BUILDER FOR ALL 8 POLITICIANS
// ----------------------------------------------------------------------------
struct CharacterProfile {
    const char* shortName;
    const char* fullTitle;
    const char* archetype;
    const char* qcfName;
    const char* dpName;
    const char* qcbName;
    const char* superName;
    const char* stampText;
    const char* victoryQuote1;
    const char* victoryQuote2;
    Color suitPrimary;
    Color accentSecondary;
    Color skinColor;
    Color hairColor;
    float walkFwdSpeed;
    float walkBackSpeed;
    float jumpVelY;
    StageId homeStage;
};

static const CharacterProfile g_RosterProfiles[CHARACTER_COUNT] = {
    {
        "POTUS", "THE COMMANDER", "Presidential Shoto",
        "EXECUTIVE ORDER", "TOTAL VETO UPPERCUT", "RED TIE LARIAT", "OMNIBUS MEGA-BILL", "VETOED!",
        "\"Nobody cancels normals into Supers bigger than me!",
        "That Omnibus Veto was the greatest in arcade history!\"",
        Color{ 22, 42, 118, 255 }, Color{ 230, 25, 25, 255 }, Color{ 245, 152, 82, 255 }, Color{ 245, 215, 70, 255 },
        2.05f, 1.50f, -9.2f, STAGE_WHITE_HOUSE_MMA
    },
    {
        "HIGH BENCH", "SUPREME JUSTICE", "Judicial Zoner",
        "REDACTED SUBPOENA", "SUPREME GAVEL SLAM", "JUDICIAL IMMUNITY", "9-0 CONSTITUTIONAL WRIT", "OVERRULED!",
        "\"By a unanimous 9-0 verdict, your defense is",
        "ruled unconstitutional. Case dismissed!\"",
        Color{ 24, 24, 32, 255 }, Color{ 215, 175, 45, 255 }, Color{ 125, 82, 55, 255 }, Color{ 225, 225, 232, 255 },
        1.65f, 1.30f, -8.8f, STAGE_CONGRESSIONAL_FLOOR
    },
    {
        "REDCAP", "RALLY BRAWLER", "Armored Grappler",
        "BULLHORN SONIC WAVE", "PATRIOT UPPERCUT", "LIFTED TRUCK TACKLE", "CONVOY STAMPEDE", "REJECTED!",
        "\"You can't filibuster a bumper-to-bumper tackle,",
        "brother! Take that back to committee!\"",
        Color{ 205, 28, 32, 255 }, Color{ 38, 62, 130, 255 }, Color{ 242, 185, 142, 255 }, Color{ 165, 115, 55, 255 },
        1.85f, 1.35f, -8.9f, STAGE_DEMOLISHED_EAST_WING
    },
    {
        "GOVERNATOR", "COASTAL GOV", "Rushdown Kickboxer",
        "PRESS BRIEFING MEMO", "HAIR-GEL FLASH KICK", "SLICK PR SLIDE", "EXECUTIVE BUDGET CUT", "RECALLED!",
        "\"Your focus-group approval rating just cratered!",
        "Never bring bad optics to a prime-time debate!\"",
        Color{ 28, 78, 118, 255 }, Color{ 235, 205, 95, 255 }, Color{ 238, 188, 150, 255 }, Color{ 30, 32, 38, 255 },
        2.35f, 1.75f, -9.5f, STAGE_REFLECTING_POOL
    },
    {
        "TECHBRO", "TECH VISIONARY", "Cybernetic Zoner",
        "ALGORITHM LASER DRONE", "STARSHIP THRUSTER", "QUANTUM HYPER-DASH", "ORBITAL SATELLITE BEAM", "DISRUPTED!",
        "\"First-principles physics dictated your knockout",
        "within 16.67 milliseconds. Disruption complete!\"",
        Color{ 42, 68, 128, 255 }, Color{ 0, 225, 255, 255 }, Color{ 240, 205, 178, 255 }, Color{ 65, 52, 42, 255 },
        2.10f, 1.55f, -9.3f, STAGE_REFLECTING_POOL
    },
    {
        "FILIBUSTER", "OLD GUARD SENATOR", "Defensive Counter",
        "RULE 22 TOME TOSS", "CLOTURE PODIUM SLAM", "MARATHON GUARD RUSH", "24-HOUR FLOOR BLOCKADE", "STRICKEN!",
        "\"Point of personal privilege! I have held this floor",
        "since 1987 and I will not yield a single frame!\"",
        Color{ 64, 66, 76, 255 }, Color{ 168, 118, 52, 255 }, Color{ 235, 198, 172, 255 }, Color{ 235, 235, 240, 255 },
        1.55f, 1.25f, -8.6f, STAGE_CONGRESSIONAL_FLOOR
    },
    {
        "FREAGLE", "CAPITOL MASCOT", "Aerial Dive Brawler",
        "FREEDOM FEATHER DART", "SKY-TALON SCREECH", "RAPTOR SWOOP POUNCE", "E PLURIBUS EAGLE STORM", "LIBERTY!",
        "\"*SCREEEECH!* The bald eagle has landed right",
        "on your health bar! Sweet land of combo liberty!\"",
        Color{ 235, 125, 28, 255 }, Color{ 45, 95, 210, 255 }, Color{ 250, 225, 170, 255 }, Color{ 248, 248, 252, 255 },
        2.25f, 1.65f, -9.8f, STAGE_WHITE_HOUSE_MMA
    },
    {
        "POPULIST", "THE POPULIST", "High-Speed Combo",
        "VIRAL HASHTAG SPARK", "GRASSROOTS RISING", "SQUAD BLITZ LUNGE", "GREEN NEW HURRICANE", "TRENDING!",
        "\"Ratioed, clipped, and livestreamed in 60fps!",
        "The grassroots movement never drops a 2-in-1 cancel!\"",
        Color{ 238, 238, 245, 255 }, Color{ 215, 25, 95, 255 }, Color{ 215, 162, 124, 255 }, Color{ 32, 24, 22, 255 },
        2.30f, 1.70f, -9.4f, STAGE_DEMOLISHED_EAST_WING
    }
};

// ----------------------------------------------------------------------------
// FIGHTER CLASS: COMBAT STATE MACHINE, 2-IN-1 CANCELS & 12-PART RIG RENDERER
// ----------------------------------------------------------------------------
class Fighter {
public:
    Fighter(int playerIdx, CharacterId cid, float startX)
        : m_playerIdx(playerIdx), m_charId(cid) {
        InitMoveTable();
        ResetRound(startX, playerIdx == 0);
    }

    void ResetRound(float startX, bool faceRight) {
        m_pos = Vector2{ startX, GROUND_Y };
        m_vel = Vector2{ 0.0f, 0.0f };
        m_facingRight = faceRight;
        m_state = FSTATE_IDLE;
        m_anim = ANIM_IDLE;
        m_stateFrame = 0;
        m_hitstopFrames = 0;
        m_hitstunFrames = 0;
        m_blockstunFrames = 0;
        m_invincibleTimer = 0;
        m_armorLeft = 0;
        m_pushbackVel = 0.0f;
        m_currentMove = MoveDef{};
        m_hitConnected = false;
        m_multiHitsLeft = 0;
        m_health = 100;
        m_comboCount = 0;
        m_lastFrameAdvantage = 0;
        m_projectiles.clear();

        // Initialize 5-node verlet chain for necktie / lapel / scarf / cape
        for (int i = 0; i < 5; i++) {
            Vector2 pt{ startX, GROUND_Y - 46.0f + i * 4.5f };
            m_clothNodes[i] = VerletNode{ pt, pt };
        }
    }

    void InitMoveTable() {
        const auto& prof = g_RosterProfiles[(int)m_charId];

        // Standing Normals (Cancelable on active/early recovery frames for 2-in-1 cancels!)
        m_moves[MOVE_ST_LP] = MoveDef{
            MOVE_ST_LP, "Standing Jab (5LP)", ANIM_ST_LP,
            3, 3, 6, 6, 1, ATK_HIGH, 6, 11, 8, 2.4f, false, true, 0, 0, 0.0f, 0.0f, 6, 1,
            Rectangle{ 12.0f, -54.0f, 28.0f, 14.0f }
        };
        m_moves[MOVE_ST_HP] = MoveDef{
            MOVE_ST_HP, "Standing Fierce (5HP)", ANIM_ST_HP,
            6, 4, 12, 13, 2, ATK_HIGH, 11, 18, 13, 4.4f, false, true, 0, 0, 1.4f, 0.0f, 11, 1,
            Rectangle{ 14.0f, -56.0f, 36.0f, 18.0f }
        };
        m_moves[MOVE_ST_LK] = MoveDef{
            MOVE_ST_LK, "Standing Short (5LK)", ANIM_ST_LK,
            4, 3, 7, 7, 1, ATK_MID, 6, 12, 9, 2.6f, false, true, 0, 0, 0.0f, 0.0f, 6, 1,
            Rectangle{ 12.0f, -32.0f, 30.0f, 14.0f }
        };
        m_moves[MOVE_ST_HK] = MoveDef{
            MOVE_ST_HK, "Standing Roundhouse (5HK)", ANIM_ST_HK,
            7, 5, 14, 14, 2, ATK_HIGH, 11, 20, 14, 5.0f, false, false, 0, 0, 1.8f, 0.0f, 12, 1,
            Rectangle{ 14.0f, -60.0f, 38.0f, 22.0f }
        };

        // Crouching Normals (including 2HK Hard Knockdown Sweep)
        m_moves[MOVE_CR_LP] = MoveDef{
            MOVE_CR_LP, "Crouching Jab (2LP)", ANIM_CR_LP,
            3, 3, 6, 5, 1, ATK_MID, 6, 11, 8, 2.2f, false, true, 0, 0, 0.0f, 0.0f, 5, 1,
            Rectangle{ 12.0f, -38.0f, 27.0f, 13.0f }
        };
        m_moves[MOVE_CR_HP] = MoveDef{
            MOVE_CR_HP, "Crouching Anti-Air Fierce (2HP)", ANIM_CR_HP,
            5, 5, 13, 13, 2, ATK_MID, 11, 19, 13, 3.8f, false, true, 0, 0, 0.8f, 0.0f, 11, 1,
            Rectangle{ 12.0f, -64.0f, 30.0f, 28.0f }
        };
        m_moves[MOVE_CR_LK] = MoveDef{
            MOVE_CR_LK, "Crouching Low Short (2LK)", ANIM_CR_LK,
            3, 3, 7, 6, 1, ATK_LOW, 6, 12, 9, 2.3f, false, true, 0, 0, 0.0f, 0.0f, 6, 1,
            Rectangle{ 12.0f, -16.0f, 30.0f, 13.0f }
        };
        m_moves[MOVE_CR_HK] = MoveDef{
            MOVE_CR_HK, "Subcommittee Sweep (2HK)", ANIM_CR_HK,
            6, 5, 15, 13, 2, ATK_LOW, 11, 26, 14, 4.6f, true, false, 0, 0, 2.2f, 0.0f, 12, 1,
            Rectangle{ 14.0f, -16.0f, 40.0f, 15.0f }
        };

        // Command Overhead (6HP - Beats Crouching Block!)
        m_moves[MOVE_FWD_HP] = MoveDef{
            MOVE_FWD_HP, "Executive Gavel Overhead (6HP)", ANIM_FWD_HP,
            13, 4, 11, 12, 2, ATK_OVERHEAD, 11, 17, 11, 3.4f, false, true, 0, 0, 2.0f, 0.0f, 10, 1,
            Rectangle{ 14.0f, -52.0f, 32.0f, 22.0f }
        };

        // Jumping Normals
        m_moves[MOVE_AIR_LP] = MoveDef{
            MOVE_AIR_LP, "Jumping Jab (j.LP)", ANIM_AIR_ATK,
            3, 8, 6, 7, 1, ATK_OVERHEAD, 6, 14, 9, 2.5f, false, false, 0, 0, 0.0f, 0.0f, 6, 1,
            Rectangle{ 10.0f, -44.0f, 26.0f, 18.0f }
        };
        m_moves[MOVE_AIR_HP] = MoveDef{
            MOVE_AIR_HP, "Jumping Fierce (j.HP)", ANIM_AIR_ATK,
            5, 7, 8, 13, 2, ATK_OVERHEAD, 11, 18, 12, 3.8f, false, false, 0, 0, 0.0f, 0.0f, 10, 1,
            Rectangle{ 12.0f, -42.0f, 32.0f, 22.0f }
        };
        m_moves[MOVE_AIR_LK] = MoveDef{
            MOVE_AIR_LK, "Jumping Cross-Up (j.LK)", ANIM_AIR_ATK,
            4, 9, 6, 8, 1, ATK_OVERHEAD, 6, 14, 9, 2.6f, false, false, 0, 0, 0.0f, 0.0f, 6, 1,
            Rectangle{ -8.0f, -38.0f, 34.0f, 18.0f }
        };
        m_moves[MOVE_AIR_HK] = MoveDef{
            MOVE_AIR_HK, "Jumping Dive Kick (j.HK)", ANIM_AIR_ATK,
            5, 8, 8, 14, 2, ATK_OVERHEAD, 11, 19, 13, 4.2f, false, false, 0, 0, 0.0f, 0.0f, 11, 1,
            Rectangle{ 12.0f, -36.0f, 36.0f, 22.0f }
        };

        // Close-Range Unblockable Throw (LP + LK)
        m_moves[MOVE_THROW] = MoveDef{
            MOVE_THROW, "Bipartisan Lobbyist Toss", ANIM_THROW,
            3, 3, 16, 18, 0, ATK_THROW, 14, 30, 0, 6.5f, true, false, 0, 0, 0.0f, 0.0f, 14, 1,
            Rectangle{ 8.0f, -56.0f, 30.0f, 44.0f }
        };

        // Signature Special 1: 236 / QCF + Attack (Projectile)
        m_moves[MOVE_SPEC_QCF] = MoveDef{
            MOVE_SPEC_QCF, prof.qcfName, ANIM_SPEC_QCF,
            9, 6, 16, 12, 3, ATK_MID, 14, 18, 12, 3.5f, false, false, 0, 0, 0.0f, 0.0f, 14, 1,
            Rectangle{ 16.0f, -52.0f, 28.0f, 20.0f }
        };

        // Signature Special 2: 623 / DP or Down-Up + Attack (Invincible Startup Anti-Air Reversal)
        m_moves[MOVE_SPEC_DP] = MoveDef{
            MOVE_SPEC_DP, prof.dpName, ANIM_SPEC_DP,
            3, 12, 19, 18, 4, ATK_MID, 14, 28, 14, 4.8f, true, false, 9, 0, 2.2f, -8.4f, 16, 1,
            Rectangle{ 10.0f, -72.0f, 30.0f, 54.0f }
        };

        // Signature Special 3: 214 / QCB + Attack (Advancing Dash / Counter Tackle)
        AttackHeight qcbHeight = (m_charId == CHAR_COASTAL_GOV) ? ATK_LOW : ATK_MID;
        int qcbArmor = (m_charId == CHAR_REDCAP || m_charId == CHAR_FILIBUSTER || m_charId == CHAR_HIGH_BENCH) ? 1 : 0;
        float qcbVy = (m_charId == CHAR_MASCOT) ? -5.2f : 0.0f;
        m_moves[MOVE_SPEC_QCB] = MoveDef{
            MOVE_SPEC_QCB, prof.qcbName, ANIM_SPEC_QCB,
            6, 12, 13, 16, 3, qcbHeight, 14, 24, 12, 5.2f, true, false, 0, qcbArmor, 4.8f, qcbVy, 15, 1,
            Rectangle{ 12.0f, -52.0f, 36.0f, 34.0f }
        };

        // "Omnibus Bill" Super Combo (236236 + Attack at 100% Meter)
        m_moves[MOVE_SUPER_OMNIBUS] = MoveDef{
            MOVE_SUPER_OMNIBUS, prof.superName, ANIM_SUPER_OMNIBUS,
            2, 22, 18, 9, 2, ATK_MID, 14, 32, 16, 2.8f, true, false, 18, 1, 5.4f, 0.0f, 0, 4,
            Rectangle{ 12.0f, -64.0f, 46.0f, 50.0f }
        };
    }

    // Check if the current normal frame is inside the 2-in-1 Special/Super cancel window
    bool CanSpecialCancel() const {
        if (m_state != FSTATE_ATTACK) return false;
        if (!m_currentMove.cancelable) return false;
        if (!m_hitConnected) return false;
        int start = m_currentMove.startupFrames;
        int endCancel = start + m_currentMove.activeFrames + 6;
        return (m_stateFrame >= start && m_stateFrame <= endCancel);
    }

    void StartMove(MoveId id, bool& outTriggeredSuperFlash) {
        outTriggeredSuperFlash = false;
        m_currentMove = m_moves[id];
        m_state = (id == MOVE_THROW) ? FSTATE_THROW_GRAB : FSTATE_ATTACK;
        m_anim = m_currentMove.anim;
        m_stateFrame = 0;
        m_hitConnected = false;
        m_multiHitsLeft = m_currentMove.multiHits;
        m_invincibleTimer = m_currentMove.invincibleFrames;
        m_armorLeft = m_currentMove.armorHits;
        m_superMeter = std::min(100, m_superMeter + m_currentMove.meterGain / 2);

        float dir = m_facingRight ? 1.0f : -1.0f;
        if (std::fabs(m_currentMove.velX) > 0.01f) m_vel.x = dir * m_currentMove.velX;
        if (std::fabs(m_currentMove.velY) > 0.01f) m_vel.y = m_currentMove.velY;

        if (id == MOVE_SUPER_OMNIBUS) {
            m_superMeter = 0;
            outTriggeredSuperFlash = true;
            ArcadeAudioEngine::Instance().PlaySfx(SFX_SUPER_FLASH);
            ArcadeAudioEngine::Instance().PlaySfx(SFX_VOICE_OMNIBUS);
        } else if (id == MOVE_SPEC_DP) {
            ArcadeAudioEngine::Instance().PlaySfx(SFX_DP_REVERSAL);
            ArcadeAudioEngine::Instance().PlaySfx(SFX_VOICE_VETO);
        } else if (id == MOVE_SPEC_QCF) {
            ArcadeAudioEngine::Instance().PlayFighterGrunt(m_charId, true, false);
        } else if (id == MOVE_SPEC_QCB) {
            ArcadeAudioEngine::Instance().PlaySfx(SFX_WHOOSH_HEAVY);
            ArcadeAudioEngine::Instance().PlayFighterGrunt(m_charId, true, false);
        } else {
            bool heavy = (m_currentMove.hitstopFrames >= 10);
            ArcadeAudioEngine::Instance().PlaySfx(heavy ? SFX_WHOOSH_HEAVY : SFX_WHOOSH_LIGHT);
            ArcadeAudioEngine::Instance().PlayFighterGrunt(m_charId, heavy, false);
        }
    }

    void Update(const InputBuffer& input, Fighter& opponent, bool& outTriggeredSuper, bool& outCancelledNormal) {
        outTriggeredSuper = false;
        outCancelledNormal = false;

        // Tick active projectiles even during fighter hitstop
        for (auto& p : m_projectiles) {
            if (!p.active) continue;
            p.pos.x += p.vel.x;
            p.pos.y += p.vel.y;
            if (p.ownerChar == CHAR_FILIBUSTER) p.vel.y += 0.18f; // Arcing lawbook tome
            p.animFrame++;
            p.lifetime--;
            if (p.lifetime <= 0 || p.pos.x < STAGE_MIN_X - 40.0f || p.pos.x > STAGE_MAX_X + 40.0f || p.pos.y > GROUND_Y + 6.0f) {
                p.active = false;
            }
        }

        if (m_hitstopFrames > 0) {
            m_hitstopFrames--;
            UpdateClothVerlet();
            return;
        }

        if (m_invincibleTimer > 0) m_invincibleTimer--;

        const auto& prof = g_RosterProfiles[(int)m_charId];
        RelativeDir rdir = input.RelDir();
        bool holdingBack = (rdir == RDIR_BACK || rdir == RDIR_UP_BACK || rdir == RDIR_DOWN_BACK);
        bool holdingDown = (rdir == RDIR_DOWN || rdir == RDIR_DOWN_BACK || rdir == RDIR_DOWN_FWD);
        bool holdingFwd  = (rdir == RDIR_FWD || rdir == RDIR_DOWN_FWD);
        m_holdingBack = holdingBack;
        m_holdingDownBack = (rdir == RDIR_DOWN_BACK);

        bool atkBtnPressed = input.IsPressed(BTN_LP | BTN_HP | BTN_LK | BTN_HK | BTN_SPECIAL);
        MotionType mot = atkBtnPressed ? input.DetectMotion() : MOT_NONE;

        // Easy 1-button shortcut on BTN_SPECIAL for accessibility alongside arcade motions
        if (input.IsPressed(BTN_SPECIAL) && mot == MOT_NONE) {
            if (m_superMeter >= 100) mot = MOT_SUPER_236236;
            else if (holdingDown) mot = MOT_DP_623;
            else if (holdingBack) mot = MOT_QCB_214;
            else mot = MOT_QCF_236;
        }

        bool neutralControl = (m_state == FSTATE_IDLE || m_state == FSTATE_WALK_FWD ||
                               m_state == FSTATE_WALK_BACK || m_state == FSTATE_CROUCH ||
                               m_state == FSTATE_BLOCK_HIGH || m_state == FSTATE_BLOCK_LOW);
        bool inCancelWindow = CanSpecialCancel();

        // 1. Check Special & Super Triggers (from Neutral OR 2-in-1 Normal Cancel!)
        if ((neutralControl || inCancelWindow) && !IsAirborne() && mot != MOT_NONE) {
            if (mot == MOT_SUPER_236236 && m_superMeter >= 100) {
                if (inCancelWindow) outCancelledNormal = true;
                StartMove(MOVE_SUPER_OMNIBUS, outTriggeredSuper);
                return;
            } else if (mot == MOT_DP_623) {
                if (inCancelWindow) outCancelledNormal = true;
                StartMove(MOVE_SPEC_DP, outTriggeredSuper);
                return;
            } else if (mot == MOT_QCB_214) {
                if (inCancelWindow) outCancelledNormal = true;
                StartMove(MOVE_SPEC_QCB, outTriggeredSuper);
                return;
            } else if (mot == MOT_QCF_236) {
                if (inCancelWindow) outCancelledNormal = true;
                StartMove(MOVE_SPEC_QCF, outTriggeredSuper);
                return;
            }
        }

        // 2. Close-Range Unblockable Throw (LP + LK simultaneous press)
        if (neutralControl && !IsAirborne() &&
            ((input.IsPressed(BTN_LP) && input.IsHeld(BTN_LK)) || (input.IsPressed(BTN_LK) && input.IsHeld(BTN_LP)))) {
            StartMove(MOVE_THROW, outTriggeredSuper);
            return;
        }

        // 3. Standing, Crouching & Command Overhead Normals
        if (neutralControl && !IsAirborne()) {
            if (input.IsPressed(BTN_HP)) {
                if (holdingDown) StartMove(MOVE_CR_HP, outTriggeredSuper);
                else if (holdingFwd) StartMove(MOVE_FWD_HP, outTriggeredSuper);
                else StartMove(MOVE_ST_HP, outTriggeredSuper);
                return;
            }
            if (input.IsPressed(BTN_LP)) {
                StartMove(holdingDown ? MOVE_CR_LP : MOVE_ST_LP, outTriggeredSuper);
                return;
            }
            if (input.IsPressed(BTN_HK)) {
                StartMove(holdingDown ? MOVE_CR_HK : MOVE_ST_HK, outTriggeredSuper);
                return;
            }
            if (input.IsPressed(BTN_LK)) {
                StartMove(holdingDown ? MOVE_CR_LK : MOVE_ST_LK, outTriggeredSuper);
                return;
            }
        } else if (m_state == FSTATE_JUMP && IsAirborne()) {
            if (input.IsPressed(BTN_HP)) { StartMove(MOVE_AIR_HP, outTriggeredSuper); return; }
            if (input.IsPressed(BTN_HK)) { StartMove(MOVE_AIR_HK, outTriggeredSuper); return; }
            if (input.IsPressed(BTN_LP)) { StartMove(MOVE_AIR_LP, outTriggeredSuper); return; }
            if (input.IsPressed(BTN_LK)) { StartMove(MOVE_AIR_LK, outTriggeredSuper); return; }
        }

        // 4. Advance Current State Machine
        m_stateFrame++;

        switch (m_state) {
        case FSTATE_IDLE:
        case FSTATE_WALK_FWD:
        case FSTATE_WALK_BACK:
        case FSTATE_CROUCH:
        case FSTATE_BLOCK_HIGH:
        case FSTATE_BLOCK_LOW: {
            m_facingRight = (opponent.GetPos().x >= m_pos.x);
            bool oppAttacking = (opponent.GetState() == FSTATE_ATTACK || !opponent.GetProjectiles().empty());

            if (rdir == RDIR_UP || rdir == RDIR_UP_FWD || rdir == RDIR_UP_BACK) {
                m_state = FSTATE_JUMP;
                m_anim = ANIM_JUMP;
                m_stateFrame = 0;
                m_vel.y = prof.jumpVelY;
                float jx = (rdir == RDIR_UP_FWD) ? 2.45f : ((rdir == RDIR_UP_BACK) ? -2.25f : 0.0f);
                m_vel.x = m_facingRight ? jx : -jx;
            } else if (holdingDown) {
                m_vel.x = 0.0f;
                if (holdingBack && oppAttacking) {
                    m_state = FSTATE_BLOCK_LOW;
                    m_anim = ANIM_BLOCK_LOW;
                } else {
                    m_state = FSTATE_CROUCH;
                    m_anim = ANIM_CROUCH;
                }
            } else if (holdingFwd) {
                m_state = FSTATE_WALK_FWD;
                m_anim = ANIM_WALK_FWD;
                m_vel.x = (m_facingRight ? 1.0f : -1.0f) * prof.walkFwdSpeed;
            } else if (holdingBack) {
                if (oppAttacking) {
                    m_state = FSTATE_BLOCK_HIGH;
                    m_anim = ANIM_BLOCK_HIGH;
                    m_vel.x = 0.0f;
                } else {
                    m_state = FSTATE_WALK_BACK;
                    m_anim = ANIM_WALK_BACK;
                    m_vel.x = (m_facingRight ? -1.0f : 1.0f) * prof.walkBackSpeed;
                }
            } else {
                m_state = FSTATE_IDLE;
                m_anim = ANIM_IDLE;
                m_vel.x = 0.0f;
            }
            break;
        }
        case FSTATE_JUMP: {
            // Aerial movement handled in physics step below
            break;
        }
        case FSTATE_ATTACK:
        case FSTATE_THROW_GRAB: {
            // Spawn projectile on first active frame of MOVE_SPEC_QCF
            if (m_currentMove.id == MOVE_SPEC_QCF && m_stateFrame == m_currentMove.startupFrames) {
                SpawnCharacterProjectile(false);
            }
            // Multi-hit re-arming during active frames (for Super Combo & multi-hit specials)
            if (m_currentMove.multiHits > 1 && m_multiHitsLeft > 1) {
                int interval = std::max(3, m_currentMove.activeFrames / m_currentMove.multiHits);
                int rel = m_stateFrame - m_currentMove.startupFrames;
                if (rel > 0 && (rel % interval) == 0) {
                    m_hitConnected = false;
                    m_multiHitsLeft--;
                }
            }
            // Friction on grounded advancing moves
            if (!IsAirborne() && m_stateFrame > m_currentMove.startupFrames + m_currentMove.activeFrames) {
                m_vel.x *= 0.78f;
            }

            int total = m_currentMove.startupFrames + m_currentMove.activeFrames + m_currentMove.recoveryFrames;
            if (m_stateFrame >= total && !IsAirborne()) {
                m_state = FSTATE_IDLE;
                m_anim = ANIM_IDLE;
                m_stateFrame = 0;
                m_vel.x = 0.0f;
                m_currentMove = MoveDef{};
            }
            break;
        }
        case FSTATE_HITSTUN: {
            m_pos.x += m_pushbackVel;
            m_pushbackVel *= 0.84f;
            m_hitstunFrames--;
            if (m_hitstunFrames <= 0) {
                m_state = FSTATE_IDLE;
                m_anim = ANIM_IDLE;
                m_stateFrame = 0;
            }
            break;
        }
        case FSTATE_BLOCKSTUN: {
            m_pos.x += m_pushbackVel;
            m_pushbackVel *= 0.84f;
            m_blockstunFrames--;
            if (m_blockstunFrames <= 0) {
                m_state = holdingDown ? FSTATE_CROUCH : FSTATE_IDLE;
                m_anim = holdingDown ? ANIM_CROUCH : ANIM_IDLE;
                m_stateFrame = 0;
            }
            break;
        }
        case FSTATE_THROWN: {
            if (m_stateFrame == 10) {
                m_state = FSTATE_KNOCKDOWN_AIR;
                m_anim = ANIM_KNOCKDOWN;
                m_vel.x = m_facingRight ? -4.8f : 4.8f;
                m_vel.y = -6.8f;
            }
            break;
        }
        case FSTATE_KNOCKDOWN_AIR: {
            // Handled when landing on ground below
            break;
        }
        case FSTATE_KNOCKDOWN_GROUND: {
            m_vel.x *= 0.8f;
            if (m_stateFrame >= 32) {
                if (m_health > 0) {
                    m_state = FSTATE_WAKEUP;
                    m_anim = ANIM_WAKEUP;
                    m_stateFrame = 0;
                    m_invincibleTimer = 10;
                } else {
                    m_state = FSTATE_DEFEAT;
                    m_anim = ANIM_DEFEAT;
                }
            }
            break;
        }
        case FSTATE_WAKEUP: {
            if (m_stateFrame >= 18) {
                m_state = FSTATE_IDLE;
                m_anim = ANIM_IDLE;
                m_stateFrame = 0;
            }
            break;
        }
        default:
            break;
        }

        // 5. Physics & Gravity Integration
        m_pos.x += m_vel.x;
        m_pos.y += m_vel.y;

        if (m_pos.y < GROUND_Y || m_vel.y < 0.0f) {
            m_vel.y += 0.48f;
            if (m_pos.y >= GROUND_Y) {
                m_pos.y = GROUND_Y;
                m_vel.y = 0.0f;
                m_vel.x = 0.0f;
                if (m_state == FSTATE_KNOCKDOWN_AIR) {
                    m_state = FSTATE_KNOCKDOWN_GROUND;
                    m_anim = ANIM_KNOCKDOWN;
                    m_stateFrame = 0;
                    ArcadeAudioEngine::Instance().PlaySfx(SFX_THROW_SLAM, 1.0f, 0.8f);
                } else if (m_state == FSTATE_JUMP || m_state == FSTATE_ATTACK) {
                    m_state = FSTATE_IDLE;
                    m_anim = ANIM_IDLE;
                    m_stateFrame = 0;
                    m_currentMove = MoveDef{};
                }
            }
        }

        m_pos.x = std::clamp(m_pos.x, STAGE_MIN_X + 18.0f, STAGE_MAX_X - 18.0f);
        UpdateClothVerlet();
    }

    void SpawnCharacterProjectile(bool isSuper) {
        Projectile p{};
        float dir = m_facingRight ? 1.0f : -1.0f;
        p.pos = Vector2{ m_pos.x + dir * 26.0f, m_pos.y - 44.0f };
        float spd = isSuper ? 5.6f : 3.65f;
        if (m_charId == CHAR_TECH_VISIONARY || m_charId == CHAR_POPULIST) spd = 4.4f;
        p.vel = Vector2{ dir * spd, (m_charId == CHAR_FILIBUSTER) ? -3.2f : 0.0f };
        p.width = isSuper ? 34.0f : 24.0f;
        p.height = isSuper ? 26.0f : 16.0f;
        p.damage = isSuper ? 22 : 12;
        p.lifetime = 140;
        p.active = true;
        p.facingRight = m_facingRight;
        p.ownerChar = m_charId;
        p.isSuperProj = isSuper;
        m_projectiles.push_back(p);
        ArcadeAudioEngine::Instance().PlaySfx(SFX_PROJ_LAUNCH);
    }

    bool CanBlockAttack(AttackHeight h) const {
        if (h == ATK_THROW) return false;
        if (m_state != FSTATE_IDLE && m_state != FSTATE_WALK_BACK &&
            m_state != FSTATE_CROUCH && m_state != FSTATE_BLOCK_HIGH &&
            m_state != FSTATE_BLOCK_LOW && m_state != FSTATE_BLOCKSTUN) {
            return false;
        }
        if (!m_holdingBack && !m_holdingDownBack) return false;
        if (h == ATK_LOW) return m_holdingDownBack;
        if (h == ATK_OVERHEAD) return (m_holdingBack && !m_holdingDownBack);
        return true;
    }

    void ReceiveHit(int dmg, int hitstop, int hitstun, float pushback, bool knockdown, bool opponentFacingRight) {
        if (m_armorLeft > 0 && !knockdown) {
            m_armorLeft--;
            m_health = std::max(1, m_health - dmg / 2);
            m_hitstopFrames = hitstop / 2;
            return;
        }
        m_health = std::max(0, m_health - dmg);
        m_superMeter = std::min(100, m_superMeter + 10);
        m_hitstopFrames = hitstop;
        float pushDir = opponentFacingRight ? 1.0f : -1.0f;

        if (m_health <= 0 || knockdown || IsAirborne()) {
            m_state = FSTATE_KNOCKDOWN_AIR;
            m_anim = ANIM_KNOCKDOWN;
            m_stateFrame = 0;
            m_vel.x = pushDir * 3.6f;
            m_vel.y = -6.2f;
        } else {
            m_state = FSTATE_HITSTUN;
            m_anim = ANIM_HITSTUN;
            m_stateFrame = 0;
            m_hitstunFrames = hitstun;
            m_pushbackVel = pushDir * pushback;
            m_vel.x = 0.0f;
        }
        ArcadeAudioEngine::Instance().PlayFighterGrunt(m_charId, false, true);
    }

    void ReceiveBlock(int chipDmg, int hitstop, int blockstun, float pushback, bool opponentFacingRight) {
        m_health = std::max(1, m_health - chipDmg);
        m_superMeter = std::min(100, m_superMeter + 4);
        m_hitstopFrames = hitstop;
        m_blockstunFrames = blockstun;
        m_state = FSTATE_BLOCKSTUN;
        m_anim = m_holdingDownBack ? ANIM_BLOCK_LOW : ANIM_BLOCKSTUN;
        m_stateFrame = 0;
        m_pushbackVel = (opponentFacingRight ? 1.0f : -1.0f) * pushback * 0.85f;
    }

    // Per-Frame Box Queries (Pushbox, Hurtboxes, Active Hitbox)
    Rectangle GetPushbox() const {
        float h = IsCrouching() ? 42.0f : 62.0f;
        return Rectangle{ m_pos.x - 15.0f, m_pos.y - h, 30.0f, h };
    }

    std::vector<Rectangle> GetHurtboxes() const {
        std::vector<Rectangle> boxes;
        if (m_invincibleTimer > 0 || m_state == FSTATE_KNOCKDOWN_GROUND || m_state == FSTATE_WAKEUP || m_state == FSTATE_DEFEAT) {
            return boxes;
        }
        float dir = m_facingRight ? 1.0f : -1.0f;
        if (IsCrouching()) {
            boxes.push_back(Rectangle{ m_pos.x - 16.0f, m_pos.y - 46.0f, 32.0f, 46.0f });
            boxes.push_back(Rectangle{ m_pos.x - 10.0f, m_pos.y - 58.0f, 20.0f, 16.0f });
        } else {
            // Head, Torso & Legs local-to-world hurtboxes
            boxes.push_back(Rectangle{ m_pos.x - 10.0f, m_pos.y - 76.0f, 20.0f, 18.0f });
            boxes.push_back(Rectangle{ m_pos.x - 15.0f, m_pos.y - 58.0f, 30.0f, 30.0f });
            boxes.push_back(Rectangle{ m_pos.x - 14.0f, m_pos.y - 28.0f, 28.0f, 28.0f });
        }
        // Extended limb hurtbox during active/recovery frames of attacks
        if (m_state == FSTATE_ATTACK && m_stateFrame >= m_currentMove.startupFrames) {
            Rectangle hb = GetWorldRect(m_currentMove.localHitbox, dir);
            hb.y += 3.0f;
            hb.height = std::max(10.0f, hb.height - 6.0f);
            boxes.push_back(hb);
        }
        return boxes;
    }

    bool GetActiveHitbox(Rectangle& outRect, MoveDef& outMove) const {
        if (m_state != FSTATE_ATTACK && m_state != FSTATE_THROW_GRAB) return false;
        if (m_hitConnected) return false;
        int start = m_currentMove.startupFrames;
        int end = start + m_currentMove.activeFrames;
        if (m_stateFrame >= start && m_stateFrame < end) {
            float dir = m_facingRight ? 1.0f : -1.0f;
            outRect = GetWorldRect(m_currentMove.localHitbox, dir);
            outMove = m_currentMove;
            return true;
        }
        return false;
    }

    // Evaluate current interpolated 12-part skeletal pose
    RigKeyframe EvaluateCurrentPose() const {
        auto table = BuildAnimKeyframes(m_anim);
        float norm = 0.0f;
        if (m_state == FSTATE_ATTACK || m_state == FSTATE_THROW_GRAB) {
            int total = std::max(1, m_currentMove.startupFrames + m_currentMove.activeFrames + m_currentMove.recoveryFrames);
            norm = std::clamp((float)m_stateFrame / (float)total, 0.0f, 1.0f);
        } else if (m_state == FSTATE_HITSTUN || m_state == FSTATE_BLOCKSTUN) {
            norm = std::clamp((float)m_stateFrame / 18.0f, 0.0f, 1.0f);
        } else if (m_state == FSTATE_KNOCKDOWN_AIR || m_state == FSTATE_KNOCKDOWN_GROUND || m_state == FSTATE_DEFEAT) {
            norm = std::clamp((float)m_stateFrame / 20.0f, 0.0f, 1.0f);
        } else if (m_state == FSTATE_WAKEUP) {
            norm = std::clamp((float)m_stateFrame / 18.0f, 0.0f, 1.0f);
        } else {
            norm = std::fmod((float)m_stateFrame * 0.035f, 1.0f);
        }

        float scaled = norm * 9.0f;
        int idx0 = std::clamp((int)std::floor(scaled), 0, 8);
        int idx1 = idx0 + 1;
        float frac = scaled - (float)idx0;
        return InterpolatePose(table[idx0], table[idx1], frac);
    }

    FacialState GetFacialState() const {
        if (m_state == FSTATE_DEFEAT || m_state == FSTATE_KNOCKDOWN_GROUND) return FACE_KO_DIZZY;
        if (m_state == FSTATE_HITSTUN || m_state == FSTATE_KNOCKDOWN_AIR || m_state == FSTATE_THROWN) return FACE_HIT_SHOCK;
        if (m_state == FSTATE_VICTORY) return FACE_VICTORY_GRIN;
        if (m_state == FSTATE_ATTACK) {
            if (m_currentMove.id >= MOVE_SPEC_QCF) return FACE_ANGRY_SHOUT;
            return FACE_EFFORT_GRIT;
        }
        return FACE_IDLE_SMIRK;
    }

    // Accessors
    Vector2 GetPos() const { return m_pos; }
    void SetPosX(float x) { m_pos.x = std::clamp(x, STAGE_MIN_X + 18.0f, STAGE_MAX_X - 18.0f); }
    bool IsFacingRight() const { return m_facingRight; }
    void SetFacingRight(bool fr) { m_facingRight = fr; }
    FighterState GetState() const { return m_state; }
    void SetState(FighterState s, AnimId a) { m_state = s; m_anim = a; m_stateFrame = 0; }
    CharacterId GetCharId() const { return m_charId; }
    int GetHealth() const { return m_health; }
    int GetSuperMeter() const { return m_superMeter; }
    void AddSuperMeter(int amt) { m_superMeter = std::clamp(m_superMeter + amt, 0, 100); }
    int GetRoundsWon() const { return m_roundsWon; }
    void AddRoundWin() { m_roundsWon++; }
    bool IsCrouching() const { return m_state == FSTATE_CROUCH || m_state == FSTATE_BLOCK_LOW || (m_state == FSTATE_ATTACK && (m_currentMove.id >= MOVE_CR_LP && m_currentMove.id <= MOVE_CR_HK)); }
    bool IsAirborne() const { return m_pos.y < GROUND_Y - 0.5f; }
    void SetHitstop(int f) { m_hitstopFrames = f; }
    int GetHitstop() const { return m_hitstopFrames; }
    void SetHitConnected(bool c) { m_hitConnected = c; }
    int GetStateFrame() const { return m_stateFrame; }
    const MoveDef& GetCurrentMove() const { return m_currentMove; }
    int GetComboCount() const { return m_comboCount; }
    void SetComboCount(int c) { m_comboCount = c; }
    int GetFrameAdvantage() const { return m_lastFrameAdvantage; }
    void SetFrameAdvantage(int adv) { m_lastFrameAdvantage = adv; }
    int GetInvincibleTimer() const { return m_invincibleTimer; }
    std::vector<Projectile>& GetProjectiles() { return m_projectiles; }
    const std::vector<Projectile>& GetProjectiles() const { return m_projectiles; }
    const std::array<VerletNode, 5>& GetClothNodes() const { return m_clothNodes; }

private:
    int m_playerIdx = 0;
    CharacterId m_charId = CHAR_COMMANDER;
    Vector2 m_pos{ 0.0f, GROUND_Y };
    Vector2 m_vel{ 0.0f, 0.0f };
    bool m_facingRight = true;
    FighterState m_state = FSTATE_IDLE;
    AnimId m_anim = ANIM_IDLE;
    int m_stateFrame = 0;
    int m_hitstopFrames = 0;
    int m_hitstunFrames = 0;
    int m_blockstunFrames = 0;
    int m_invincibleTimer = 0;
    int m_armorLeft = 0;
    float m_pushbackVel = 0.0f;
    bool m_holdingBack = false;
    bool m_holdingDownBack = false;
    bool m_hitConnected = false;
    int m_multiHitsLeft = 0;

    int m_health = 100;
    int m_superMeter = 0;
    int m_roundsWon = 0;
    int m_comboCount = 0;
    int m_lastFrameAdvantage = 0;

    MoveDef m_moves[MOVE_COUNT]{};
    MoveDef m_currentMove{};
    std::vector<Projectile> m_projectiles;
    std::array<VerletNode, 5> m_clothNodes{};

    Rectangle GetWorldRect(const Rectangle& local, float dir) const {
        float wx = (dir > 0.0f) ? (m_pos.x + local.x) : (m_pos.x - local.x - local.width);
        float wy = m_pos.y + local.y;
        return Rectangle{ wx, wy, local.width, local.height };
    }

    void UpdateClothVerlet() {
        float dir = m_facingRight ? 1.0f : -1.0f;
        RigKeyframe pose = EvaluateCurrentPose();
        Vector2 anchor{
            m_pos.x + dir * (pose.pelvisOffsetX + 4.0f),
            m_pos.y - 48.0f + pose.pelvisOffsetY
        };
        m_clothNodes[0].pos = anchor;
        m_clothNodes[0].prev = anchor;

        for (int i = 1; i < 5; i++) {
            Vector2 cur = m_clothNodes[i].pos;
            Vector2 vx = { (cur.x - m_clothNodes[i].prev.x) * 0.86f - m_vel.x * 0.35f - dir * 0.25f,
                           (cur.y - m_clothNodes[i].prev.y) * 0.86f + 0.55f };
            m_clothNodes[i].prev = cur;
            m_clothNodes[i].pos = Vector2{ cur.x + vx.x, cur.y + vx.y };
        }
        // Distance constraint pass
        const float segLen = 4.5f;
        for (int iter = 0; iter < 3; iter++) {
            m_clothNodes[0].pos = anchor;
            for (int i = 1; i < 5; i++) {
                float dx = m_clothNodes[i].pos.x - m_clothNodes[i - 1].pos.x;
                float dy = m_clothNodes[i].pos.y - m_clothNodes[i - 1].pos.y;
                float d = std::sqrt(dx * dx + dy * dy);
                if (d > 0.001f) {
                    float diff = (d - segLen) / d;
                    m_clothNodes[i].pos.x -= dx * diff;
                    m_clothNodes[i].pos.y -= dy * diff;
                }
            }
        }
    }
};

// ----------------------------------------------------------------------------
// HIERARCHICAL 12-PART RIG RENDERER, CARICATURE PORTRAITS & 4 UPGRADED STAGES
// ----------------------------------------------------------------------------
inline Vector2 PolarStep(Vector2 origin, float deg, float len, float dirSign) {
    float rad = (deg * PI_F) / 180.0f;
    return Vector2{
        origin.x + std::sin(rad) * len * dirSign,
        origin.y + std::cos(rad) * len
    };
}

inline void DrawFighterRig(const Fighter& f, float globalTime) {
    Vector2 root = f.GetPos();
    bool fr = f.IsFacingRight();
    float dir = fr ? 1.0f : -1.0f;
    CharacterId cid = f.GetCharId();
    const auto& prof = g_RosterProfiles[(int)cid];
    RigKeyframe pose = f.EvaluateCurrentPose();
    FacialState face = f.GetFacialState();

    // 1. Floor Drop Shadow
    float airDist = std::max(0.0f, GROUND_Y - root.y);
    float shW = std::max(8.0f, 26.0f - airDist * 0.16f);
    float shH = std::max(2.5f, 6.0f - airDist * 0.04f);
    DrawEllipse((int)root.x, (int)GROUND_Y + 1, shW + 2.0f, shH + 1.0f, Color{ 8, 10, 16, 95 });
    DrawEllipse((int)root.x, (int)GROUND_Y + 1, shW, shH, Color{ 5, 6, 10, 175 });

    // Invincibility / Hitstun Flash Tint
    Color suitCol = prof.suitPrimary;
    Color accCol  = prof.accentSecondary;
    Color skinCol = prof.skinColor;
    if (f.GetInvincibleTimer() > 0 && ((int)(globalTime * 30.0f) % 2 == 0)) {
        suitCol = BevelHighlight(suitCol, 85);
        accCol  = Color{ 255, 245, 130, 255 };
    } else if (f.GetState() == FSTATE_HITSTUN && (f.GetStateFrame() % 4 < 2)) {
        suitCol = Color{ 255, 95, 95, 255 };
    }

    // 2. Compute Hierarchical Joint World Coordinates
    Vector2 pelvis{ root.x + dir * pose.pelvisOffsetX, root.y - 26.0f + pose.pelvisOffsetY };
    Vector2 hipL{ pelvis.x - dir * 4.5f, pelvis.y + 2.0f };
    Vector2 hipR{ pelvis.x + dir * 4.5f, pelvis.y + 2.0f };

    // Back Leg (L) & Front Leg (R)
    Vector2 kneeL  = PolarStep(hipL,  pose.thighL, 12.5f, dir);
    Vector2 ankleL = PolarStep(kneeL, pose.thighL - pose.calfL, 12.0f, dir);
    Vector2 toeL   = Vector2{ ankleL.x + dir * 6.5f, ankleL.y + 1.0f };

    Vector2 kneeR  = PolarStep(hipR,  pose.thighR, 12.5f, dir);
    Vector2 ankleR = PolarStep(kneeR, pose.thighR - pose.calfR, 12.0f, dir);
    Vector2 toeR   = Vector2{ ankleR.x + dir * 7.0f, ankleR.y + 1.0f };

    // Torso & Shoulders
    float torsoRad = (pose.torsoTilt * PI_F) / 180.0f;
    Vector2 chest{
        pelvis.x + std::sin(torsoRad) * 20.0f * dir,
        pelvis.y - std::cos(torsoRad) * 20.0f
    };
    Vector2 shoulderL{ chest.x - dir * 5.5f, chest.y + 2.0f };
    Vector2 shoulderR{ chest.x + dir * 5.5f, chest.y + 2.0f };

    // Back Arm (L) & Front Arm (R)
    Vector2 elbowL = PolarStep(shoulderL, pose.uArmL + pose.torsoTilt, 11.5f, dir);
    Vector2 handL  = PolarStep(elbowL,    pose.uArmL + pose.torsoTilt - pose.fArmL, 11.0f, dir);

    Vector2 elbowR = PolarStep(shoulderR, pose.uArmR + pose.torsoTilt, 11.5f, dir);
    Vector2 handR  = PolarStep(elbowR,    pose.uArmR + pose.torsoTilt - pose.fArmR, 11.5f, dir);

    // Head & Jaw
    Vector2 headCenter{
        chest.x + std::sin(torsoRad) * 9.0f * dir + dir * (pose.headTilt * 0.08f),
        chest.y - std::cos(torsoRad) * 10.5f
    };

    Color backSuit = ShadeColor(suitCol, 0.76f);
    Color shoeCol  = (cid == CHAR_MASCOT) ? Color{ 240, 180, 30, 255 } : Color{ 24, 24, 30, 255 };

    // 3. Draw Back Arm (UpperArm_L, Forearm_L, Hand_L)
    DrawCapsuleLimb(shoulderL, elbowL, 3.6f, backSuit);
    DrawCapsuleLimb(elbowL, handL, 3.2f, backSuit);
    DrawCapsuleLimb(handL, handL, 3.4f, ShadeColor(skinCol, 0.85f));

    // 4. Draw Back Leg (Thigh_L, Calf_L, Foot_L)
    DrawCapsuleLimb(hipL, kneeL, 4.4f, backSuit);
    DrawCapsuleLimb(kneeL, ankleL, 3.8f, backSuit);
    DrawCapsuleLimb(ankleL, toeL, 3.2f, shoeCol);

    // 5. Draw Pelvis & Torso with 3-Tone Suit / Robe Detail
    DrawCapsuleLimb(pelvis, chest, 8.2f, suitCol, 1.8f);
    DrawCapsuleLimb(hipL, hipR, 5.2f, ShadeColor(suitCol, 0.85f), 1.5f);

    // Shirt V-neck / Collar & Character Torso Insignia
    Vector2 collarTop{ chest.x + dir * 2.5f, chest.y + 1.0f };
    Vector2 collarBot{ pelvis.x + dir * 2.0f, pelvis.y - 4.0f };
    DrawLineEx(collarTop, collarBot, 4.5f, WHITE);

    // 6. Draw Dynamic Verlet Necktie / Stole / Jacket Trail (5-Node Physics Chain)
    const auto& cloth = f.GetClothNodes();
    for (int i = 0; i < 4; i++) {
        float w = 3.6f - i * 0.45f;
        DrawLineEx(cloth[i].pos, cloth[i + 1].pos, w + 2.0f, Color{ 12, 14, 20, 255 });
        DrawLineEx(cloth[i].pos, cloth[i + 1].pos, w, accCol);
    }

    // 7. Draw Front Leg (Thigh_R, Calf_R, Foot_R)
    DrawCapsuleLimb(hipR, kneeR, 4.6f, suitCol);
    DrawCapsuleLimb(kneeR, ankleR, 4.0f, suitCol);
    DrawCapsuleLimb(ankleR, toeR, 3.5f, shoeCol);

    // 8. Draw Head, Jaw/Mouth, Facial Expression & Hair/Accessory
    float headR = 8.2f;
    DrawCircleV(headCenter, headR + 1.6f, Color{ 12, 14, 20, 255 });
    DrawCircleV(headCenter, headR, ShadeColor(skinCol, 0.78f));
    DrawCircleV(Vector2{ headCenter.x + dir * 1.0f, headCenter.y - 0.8f }, headR * 0.85f, skinCol);
    DrawCircleV(Vector2{ headCenter.x + dir * 2.2f, headCenter.y - 2.6f }, headR * 0.42f, BevelHighlight(skinCol, 36));

    // Jaw / Mouth Articulation
    float jawDrop = pose.jawOpen * 3.8f;
    Vector2 jawPos{ headCenter.x + dir * 2.5f, headCenter.y + 4.5f + jawDrop * 0.5f };
    DrawCircleV(jawPos, 4.2f, skinCol);

    if (face == FACE_ANGRY_SHOUT || pose.jawOpen > 0.45f) {
        DrawRectangle((int)(headCenter.x + dir * 2.0f - 2.0f), (int)(headCenter.y + 3.0f), 5, (int)(3.0f + jawDrop), Color{ 45, 10, 15, 255 });
        DrawLine((int)(headCenter.x + dir * 1.5f), (int)(headCenter.y + 3.0f), (int)(headCenter.x + dir * 5.0f), (int)(headCenter.y + 3.0f), WHITE);
    } else if (face == FACE_HIT_SHOCK) {
        DrawCircle((int)(headCenter.x + dir * 4.0f), (int)(headCenter.y + 4.5f), 2.5f, Color{ 30, 10, 15, 255 });
    } else {
        // Smirk / Effort Grit line
        DrawLine((int)(headCenter.x + dir * 1.5f), (int)(headCenter.y + 4.0f), (int)(headCenter.x + dir * 5.5f), (int)(headCenter.y + 3.2f), Color{ 40, 25, 20, 255 });
    }

    // Eyes & Eyebrows
    int eyeX = (int)(headCenter.x + dir * 3.8f);
    int eyeY = (int)(headCenter.y - 1.2f);
    if (face == FACE_HIT_SHOCK) {
        DrawCircle(eyeX, eyeY, 2.8f, WHITE);
        DrawPixel(eyeX, eyeY, BLACK);
    } else if (face == FACE_KO_DIZZY) {
        DrawLine(eyeX - 2, eyeY - 2, eyeX + 2, eyeY + 2, BLACK);
        DrawLine(eyeX - 2, eyeY + 2, eyeX + 2, eyeY - 2, BLACK);
    } else {
        DrawRectangle(eyeX - 1, eyeY - 1, 3, 2, WHITE);
        DrawPixel(eyeX + (fr ? 1 : -1), eyeY, BLACK);
        // Furrowed brow
        DrawLine(eyeX - 2, eyeY - 2, eyeX + 2, eyeY - (face == FACE_ANGRY_SHOUT ? 1 : 2), Color{ 30, 22, 18, 255 });
    }

    // Character-Specific Hair & Signature Head Accessories
    float sway = pose.hairSway * 0.12f * dir;
    switch (cid) {
    case CHAR_COMMANDER: {
        // Swooping Golden Presidential Pompadour
        Vector2 h1{ headCenter.x - dir * 7.0f + sway, headCenter.y - 5.0f };
        Vector2 h2{ headCenter.x + dir * 9.0f + sway, headCenter.y - 8.5f };
        DrawCapsuleLimb(h1, h2, 3.8f, prof.hairColor, 1.2f);
        break;
    }
    case CHAR_HIGH_BENCH: {
        // Supreme Justice Silver Hair + Gold-Rimmed Glasses
        DrawCapsuleLimb(Vector2{ headCenter.x - dir * 5.0f, headCenter.y - 6.0f },
                        Vector2{ headCenter.x + dir * 3.0f, headCenter.y - 6.5f }, 3.5f, prof.hairColor, 1.0f);
        DrawRectangleLines(eyeX - 2, eyeY - 2, 5, 4, Color{ 235, 195, 55, 255 });
        break;
    }
    case CHAR_REDCAP: {
        // Iconic Red Rally Trucker Cap + Visor
        DrawCapsuleLimb(Vector2{ headCenter.x - dir * 5.0f, headCenter.y - 6.5f },
                        Vector2{ headCenter.x + dir * 4.0f, headCenter.y - 6.5f }, 4.0f, suitCol, 1.2f);
        DrawLineEx(Vector2{ headCenter.x + dir * 2.0f, headCenter.y - 4.5f },
                   Vector2{ headCenter.x + dir * 11.5f, headCenter.y - 4.0f }, 3.0f, suitCol);
        break;
    }
    case CHAR_COASTAL_GOV: {
        // Slicked-back Jet Black Hair + Aviator Shades
        DrawCapsuleLimb(Vector2{ headCenter.x - dir * 6.5f + sway, headCenter.y - 6.5f },
                        Vector2{ headCenter.x + dir * 5.0f, headCenter.y - 7.5f }, 3.4f, prof.hairColor, 1.2f);
        DrawRectangle(eyeX - 2, eyeY - 2, 6, 3, Color{ 15, 20, 28, 255 });
        break;
    }
    case CHAR_TECH_VISIONARY: {
        // Cybernetic Neural Visor + Cropped Hair
        DrawCapsuleLimb(Vector2{ headCenter.x - dir * 5.0f, headCenter.y - 6.5f },
                        Vector2{ headCenter.x + dir * 4.0f, headCenter.y - 6.5f }, 3.2f, prof.hairColor, 1.0f);
        DrawRectangle(eyeX - 2, eyeY - 2, 7, 3, Color{ 0, 235, 255, 255 });
        break;
    }
    case CHAR_FILIBUSTER: {
        // Classic Senator Silver Comb-over + Thick Horn-Rim Spectacles
        DrawCapsuleLimb(Vector2{ headCenter.x - dir * 6.0f, headCenter.y - 5.5f },
                        Vector2{ headCenter.x + dir * 2.0f, headCenter.y - 6.5f }, 3.5f, prof.hairColor, 1.0f);
        DrawRectangleLines(eyeX - 2, eyeY - 2, 5, 4, Color{ 30, 25, 25, 255 });
        break;
    }
    case CHAR_MASCOT: {
        // Bald Eagle White Feathered Crest + Hooked Golden Beak
        DrawCapsuleLimb(Vector2{ headCenter.x - dir * 7.0f + sway, headCenter.y - 7.0f },
                        Vector2{ headCenter.x + dir * 5.0f, headCenter.y - 7.5f }, 4.2f, WHITE, 1.2f);
        DrawTriangle(Vector2{ headCenter.x + dir * 6.0f, headCenter.y - 2.0f },
                     Vector2{ headCenter.x + dir * 6.0f, headCenter.y + 3.5f },
                     Vector2{ headCenter.x + dir * 13.5f, headCenter.y + 1.5f },
                     Color{ 255, 195, 25, 255 });
        break;
    }
    case CHAR_POPULIST: {
        // High Activist Bun + Gold Hoop Earring
        DrawCircleV(Vector2{ headCenter.x - dir * 6.5f + sway, headCenter.y - 6.5f }, 4.5f, prof.hairColor);
        DrawCircleLines((int)(headCenter.x - dir * 1.0f), (int)(headCenter.y + 3.0f), 2.5f, Color{ 255, 215, 40, 255 });
        break;
    }
    default:
        break;
    }

    // 9. Draw Front Arm (UpperArm_R, Forearm_R, Hand_R)
    DrawCapsuleLimb(shoulderR, elbowR, 3.8f, suitCol);
    DrawCapsuleLimb(elbowR, handR, 3.4f, suitCol);
    DrawCapsuleLimb(handR, handR, 3.8f, skinCol);

    // Character Hand Props on Active Attacks (Gavel, Bullhorn, Binder)
    if (f.GetState() == FSTATE_ATTACK) {
        if (cid == CHAR_HIGH_BENCH) {
            DrawShadedBox(handR.x + dir * 2.0f, handR.y - 7.0f, 10.0f, 6.0f, Color{ 150, 95, 35, 255 });
        } else if (cid == CHAR_REDCAP && f.GetCurrentMove().id == MOVE_SPEC_QCF) {
            DrawTriangle(handR, Vector2{ handR.x + dir * 12.0f, handR.y - 6.0f }, Vector2{ handR.x + dir * 12.0f, handR.y + 6.0f }, WHITE);
        } else if (cid == CHAR_FILIBUSTER) {
            DrawShadedBox(handR.x, handR.y - 5.0f, 9.0f, 11.0f, Color{ 165, 45, 35, 255 });
        }
    }

    // 10. Motion-Blur Smear Arcs on Active Frames
    if (pose.drawSmearArc) {
        Vector2 tip = pose.smearIsKick ? toeR : handR;
        Color smearOuter = (f.GetCurrentMove().id >= MOVE_SPEC_QCF) ? Color{ 255, 90, 30, 195 } : Color{ 255, 235, 110, 185 };
        Color smearCore  = Color{ 255, 255, 255, 225 };
        for (int s = 0; s < 4; s++) {
            float trailAngle = (s + 1) * 0.22f;
            Vector2 pA{
                chest.x + (tip.x - chest.x) * std::cos(trailAngle),
                tip.y - std::sin(trailAngle) * (pose.smearIsKick ? 18.0f : 12.0f)
            };
            Vector2 pB{
                chest.x + (tip.x - chest.x) * std::cos(trailAngle + 0.18f),
                tip.y - std::sin(trailAngle + 0.18f) * (pose.smearIsKick ? 18.0f : 12.0f)
            };
            DrawLineEx(tip, pA, 5.0f - s, smearOuter);
            DrawLineEx(pA, pB, 3.5f - s * 0.6f, smearCore);
        }
        DrawCircleV(tip, 4.5f, smearCore);
    }
}

// Render Multi-Frame Projectiles for All 8 Characters
inline void DrawCharacterProjectiles(const Fighter& f) {
    for (const auto& p : f.GetProjectiles()) {
        if (!p.active) continue;
        float dir = p.facingRight ? 1.0f : -1.0f;
        float pulse = std::sin(p.animFrame * 0.45f) * 2.5f;
        int x = (int)p.pos.x;
        int y = (int)p.pos.y;

        switch (p.ownerChar) {
        case CHAR_COMMANDER: {
            // Executive Order Scroll + Golden Seal Wave
            DrawCircle(x, y, 10.0f + pulse, Color{ 255, 200, 40, 180 });
            DrawShadedBox(p.pos.x - 10.0f, p.pos.y - 6.0f, 20.0f, 12.0f, Color{ 250, 242, 215, 255 });
            DrawCircle(x + (int)(dir * 3.0f), y, 3.5f, Color{ 225, 35, 35, 255 });
            break;
        }
        case CHAR_HIGH_BENCH: {
            // Redacted Subpoena Writ
            DrawShadedBox(p.pos.x - 11.0f, p.pos.y - 9.0f, 22.0f, 18.0f, Color{ 240, 240, 245, 255 });
            DrawRectangle(x - 8, y - 5, 16, 3, BLACK);
            DrawRectangle(x - 8, y + 1, 14, 3, BLACK);
            break;
        }
        case CHAR_REDCAP: {
            // Bullhorn Sonic Rings
            for (int r = 0; r < 3; r++) {
                float rad = 6.0f + r * 4.5f + std::fmod(p.animFrame * 0.8f, 4.0f);
                DrawCircleLines(x - (int)(dir * r * 5.0f), y, rad, (r % 2 == 0) ? Color{ 255, 60, 60, 240 } : Color{ 255, 225, 60, 240 });
            }
            break;
        }
        case CHAR_COASTAL_GOV: {
            // Spinning Press Briefing Disc
            DrawEllipse(x, y, 13.0f, 5.0f + std::fabs(pulse), Color{ 40, 190, 255, 230 });
            DrawEllipseLines(x, y, 14.0f, 6.0f, WHITE);
            break;
        }
        case CHAR_TECH_VISIONARY: {
            // Algorithm Plasma Beam + Binary Trail
            DrawShadedBox(p.pos.x - 12.0f, p.pos.y - 4.0f, 24.0f, 8.0f, Color{ 0, 235, 255, 255 });
            DrawPixelText((p.animFrame & 2) ? "101" : "010", x - (int)(dir * 18.0f), y - 4, 6, Color{ 120, 250, 255, 220 });
            break;
        }
        case CHAR_FILIBUSTER: {
            // Heavy Arcing Senate Tome
            DrawShadedBox(p.pos.x - 9.0f, p.pos.y - 8.0f, 18.0f, 16.0f, Color{ 155, 45, 35, 255 });
            DrawRectangle(x - 6, y - 2, 12, 3, Color{ 240, 200, 65, 255 });
            break;
        }
        case CHAR_MASCOT: {
            // Twin Patriotic Eagle Feathers
            DrawTriangle(Vector2{ p.pos.x + dir * 12.0f, p.pos.y },
                         Vector2{ p.pos.x - dir * 10.0f, p.pos.y - 6.0f },
                         Vector2{ p.pos.x - dir * 10.0f, p.pos.y + 6.0f },
                         Color{ 255, 220, 55, 255 });
            DrawLineEx(Vector2{ p.pos.x - dir * 12.0f, p.pos.y }, Vector2{ p.pos.x + dir * 12.0f, p.pos.y }, 2.0f, WHITE);
            break;
        }
        case CHAR_POPULIST:
        default: {
            // Viral Livestream Spark Orb
            DrawCircle(x, y, 9.0f + pulse * 0.5f, Color{ 245, 40, 145, 220 });
            DrawCircle(x, y, 5.0f, WHITE);
            DrawPixelText("#", x - 3, y - 4, 8, Color{ 255, 240, 70, 255 });
            break;
        }
        }
    }
}

// Render Procedural Caricature Portrait (Supports Normal, Shouting & <30% HP Bruised/Sweating HUD State)
inline void DrawProceduralPortrait(CharacterId cid, int x, int y, int w, int h, bool bruisedLowHp, bool flipX = false) {
    const auto& prof = g_RosterProfiles[(int)cid];
    DrawShadedBox((float)x, (float)y, (float)w, (float)h, ShadeColor(prof.suitPrimary, 0.55f));

    int cx = x + w / 2;
    int cy = y + h / 2 + 2;
    float dir = flipX ? -1.0f : 1.0f;

    // Shoulders & Suit Collar
    DrawShadedBox((float)(cx - w * 0.38f), (float)(y + h * 0.68f), (float)(w * 0.76f), (float)(h * 0.30f), prof.suitPrimary);
    DrawTriangle(Vector2{ (float)cx, (float)(y + h * 0.96f) },
                 Vector2{ (float)(cx + 5), (float)(y + h * 0.68f) },
                 Vector2{ (float)(cx - 5), (float)(y + h * 0.68f) }, WHITE);
    DrawLine(cx, (int)(y + h * 0.70f), cx, (int)(y + h * 0.95f), prof.accentSecondary);

    // Head Silhouette & 3-Tone Skin
    float headR = std::min(w, h) * 0.28f;
    DrawCircle(cx, cy - 2, headR + 1.5f, Color{ 12, 14, 20, 255 });
    DrawCircle(cx, cy - 2, headR, prof.skinColor);
    DrawCircle(cx + (int)(dir * 2.0f), cy - 4, headR * 0.55f, BevelHighlight(prof.skinColor, 32));

    // Hair / Hat / Visor
    DrawCapsuleLimb(Vector2{ (float)(cx - headR * 0.8f), (float)(cy - headR * 0.85f) },
                    Vector2{ (float)(cx + headR * 0.8f), (float)(cy - headR * 0.90f) },
                    headR * 0.36f, (cid == CHAR_REDCAP) ? prof.suitPrimary : prof.hairColor, 1.0f);

    // Eyes & Bruised Low-HP Details (<30% HP)
    int ex = cx + (int)(dir * headR * 0.32f);
    int ey = cy - 3;
    if (bruisedLowHp) {
        // Swollen black-and-blue eye, white cross band-aid, and flying sweat drop!
        DrawCircle(ex, ey, 3.5f, Color{ 75, 35, 95, 220 });
        DrawLine(ex - 2, ey, ex + 2, ey + 1, BLACK);
        DrawRectangle(cx - (int)(dir * 6.0f), cy - 6, 6, 3, Color{ 245, 240, 225, 255 });
        DrawCircle(cx - (int)(dir * headR * 0.65f), cy - 1, 2.2f, Color{ 90, 210, 255, 240 });
        DrawRectangle(cx - 3, cy + (int)(headR * 0.35f), 7, 3, Color{ 45, 15, 20, 255 });
    } else {
        DrawRectangle(ex - 1, ey - 1, 3, 2, WHITE);
        DrawPixel(ex, ey, BLACK);
        DrawLine(cx - 2, cy + (int)(headR * 0.42f), cx + 4, cy + (int)(headR * 0.36f), Color{ 40, 20, 18, 255 });
    }
}

// ----------------------------------------------------------------------------
// UPGRADED 4 DC PARALLAX VENUES (WITH CROWDS, LIGHTING, PETALS & PERSPECTIVE)
// ----------------------------------------------------------------------------
class StageRenderer {
public:
    static void DrawVenue(StageId stage, float camX, float time) {
        float farOff  = -camX * 0.18f;
        float midOff  = -camX * 0.48f;
        float playOff = -camX;

        switch (stage) {
        case STAGE_DEMOLISHED_EAST_WING: {
            // Sky & Capitol Silhouette
            DrawRectangleGradientV(0, 0, SCREEN_WIDTH, (int)GROUND_Y, Color{ 24, 18, 36, 255 }, Color{ 78, 44, 58, 255 });
            float capX = 210.0f + farOff;
            DrawRectangle((int)capX - 38, 68, 76, 52, Color{ 40, 34, 50, 255 });
            DrawCircle((int)capX, 68, 28.0f, Color{ 48, 42, 60, 255 });
            DrawRectangle((int)capX - 3, 34, 6, 14, Color{ 58, 50, 70, 255 });

            // Sweeping Searchlights
            for (int s = 0; s < 3; s++) {
                float sx = 70.0f + s * 130.0f + std::fmod(midOff, 130.0f);
                float sway = std::sin(time * 1.8f + s * 2.1f) * 42.0f;
                DrawTriangle(Vector2{ sx, GROUND_Y },
                             Vector2{ sx + sway - 28.0f, 0.0f },
                             Vector2{ sx + sway + 28.0f, 0.0f },
                             Color{ 255, 240, 150, 36 });
            }

            // Midground Ruined Columns & Animated Secret Service / Hardhat Crew
            for (int i = -1; i < 7; i++) {
                float colX = i * 110.0f + std::fmod(midOff, 110.0f);
                DrawShadedBox(colX, 82.0f, 24.0f, 114.0f, Color{ 96, 92, 104, 255 });
                float bob = std::fabs(std::sin(time * 5.0f + i)) * 3.0f;
                DrawShadedBox(colX + 42.0f, GROUND_Y - 26.0f - bob, 12.0f, 18.0f, Color{ 25, 30, 42, 255 });
                DrawCircle((int)(colX + 48.0f), (int)(GROUND_Y - 30.0f - bob), 5.0f, Color{ 240, 200, 40, 255 });
            }

            // Perspective Floor & Caution Tape
            DrawPerspectiveFloor(playOff, Color{ 54, 50, 60, 255 }, Color{ 36, 32, 42, 255 });
            for (int i = -1; i < 16; i++) {
                float tx = i * 28.0f + std::fmod(playOff, 28.0f);
                float sag = std::sin(tx * 0.05f + time * 4.0f) * 2.5f;
                DrawRectangle((int)tx, (int)(GROUND_Y - 42.0f + sag), 28, 5, (i & 1) ? Color{ 20, 20, 25, 230 } : Color{ 245, 205, 30, 230 });
            }
            break;
        }
        case STAGE_REFLECTING_POOL: {
            // Twilight Mall Sky & Washington Monument Obelisk
            DrawRectangleGradientV(0, 0, SCREEN_WIDTH, (int)GROUND_Y, Color{ 12, 22, 52, 255 }, Color{ 48, 88, 138, 255 });
            float monX = 192.0f + farOff;
            DrawTriangle(Vector2{ monX, 12.0f }, Vector2{ monX - 13.0f, 138.0f }, Vector2{ monX + 13.0f, 138.0f }, Color{ 175, 190, 212, 255 });
            if (std::fmod(time, 1.0f) < 0.5f) DrawCircle((int)monX, 12, 3.0f, Color{ 255, 45, 45, 255 });

            // Cherry Blossom Trees & Animated Spectators
            for (int i = -1; i < 8; i++) {
                float tx = i * 90.0f + std::fmod(midOff, 90.0f);
                DrawShadedBox(tx + 14.0f, 118.0f, 8.0f, 78.0f, Color{ 52, 34, 30, 255 });
                DrawCircle((int)(tx + 18.0f), 106, 22.0f, Color{ 225, 115, 155, 220 });
                DrawCircle((int)(tx + 30.0f), 100, 17.0f, Color{ 245, 155, 185, 230 });
            }

            // Perspective Basin Floor & Shimmering Water
            DrawPerspectiveFloor(playOff, Color{ 28, 92, 152, 255 }, Color{ 85, 185, 235, 255 });

            // Drifting Cherry Blossom Petals
            for (int p = 0; p < 22; p++) {
                float px = std::fmod(p * 37.0f + time * (18.0f + (p % 4) * 5.0f) + playOff * 0.4f + 800.0f, (float)SCREEN_WIDTH);
                float py = std::fmod(p * 23.0f + time * 24.0f, GROUND_Y);
                DrawCircle((int)px, (int)py, 2.0f, Color{ 255, 185, 210, 220 });
            }
            break;
        }
        case STAGE_CONGRESSIONAL_FLOOR: {
            // Capitol Chamber Arches & Swinging Chandelier
            DrawRectangleGradientV(0, 0, SCREEN_WIDTH, (int)GROUND_Y, Color{ 46, 18, 18, 255 }, Color{ 92, 42, 32, 255 });
            float chanX = 192.0f + farOff + std::sin(time * 2.0f) * 8.0f;
            DrawLine((int)chanX, 0, (int)chanX, 34, Color{ 230, 190, 60, 255 });
            DrawCircle((int)chanX, 38, 11.0f, Color{ 255, 220, 90, 255 });

            // Animated Gallery of Cheering/Gaveling Congressmembers
            DrawShadedBox(0.0f, 108.0f, (float)SCREEN_WIDTH, 88.0f, Color{ 112, 58, 38, 255 });
            for (int i = -1; i < 12; i++) {
                float nx = i * 44.0f + std::fmod(midOff, 44.0f);
                bool cheering = (((int)(time * 3.5f) + i) % 3 == 0);
                int ny = cheering ? 84 : 90;
                DrawShadedBox(nx, (float)(ny + 8), 14.0f, 18.0f, (i & 1) ? Color{ 32, 45, 78, 255 } : Color{ 68, 32, 38, 255 });
                DrawCircle((int)(nx + 7.0f), ny + 3, 5.5f, Color{ 235, 190, 152, 255 });
            }

            // Perspective Mahogany Floor & Drifting Legislative Documents
            DrawPerspectiveFloor(playOff, Color{ 82, 42, 26, 255 }, Color{ 135, 82, 45, 255 });
            for (int d = 0; d < 12; d++) {
                float dx = std::fmod(d * 53.0f + time * 22.0f + std::sin(time * 3.0f + d) * 12.0f, (float)SCREEN_WIDTH);
                float dy = std::fmod(d * 29.0f + time * 19.0f, GROUND_Y - 10.0f);
                DrawRectangle((int)dx, (int)dy, 6, 4, Color{ 245, 240, 220, 210 });
            }
            break;
        }
        case STAGE_WHITE_HOUSE_MMA:
        default: {
            // South Lawn Night Sky, White House Portico & Marine One Helicopter
            DrawRectangleGradientV(0, 0, SCREEN_WIDTH, (int)GROUND_Y, Color{ 14, 24, 34, 255 }, Color{ 38, 64, 72, 255 });
            float whX = 130.0f + farOff;
            DrawShadedBox(whX, 54.0f, 124.0f, 68.0f, Color{ 225, 230, 238, 255 });
            DrawTriangle(Vector2{ whX + 62.0f, 34.0f }, Vector2{ whX + 4.0f, 54.0f }, Vector2{ whX + 120.0f, 54.0f }, WHITE);

            float heliX = 290.0f + farOff + std::sin(time * 1.4f) * 18.0f;
            float heliY = 44.0f + std::cos(time * 2.2f) * 5.0f;
            DrawShadedBox(heliX, heliY, 42.0f, 15.0f, Color{ 38, 58, 42, 255 });
            float rotor = std::sin(time * 42.0f) * 34.0f;
            DrawLine((int)(heliX + 20.0f - rotor), (int)heliY - 3, (int)(heliX + 20.0f + rotor), (int)heliY - 3, Color{ 200, 220, 200, 210 });

            // Press Corps Crowd & Continuous Camera Flashes
            for (int i = -1; i < 14; i++) {
                float cx = i * 36.0f + std::fmod(midOff, 36.0f);
                float bob = std::fabs(std::sin(time * 6.0f + i)) * 3.0f;
                DrawCircle((int)cx, (int)(GROUND_Y - 16.0f - bob), 6.5f, Color{ 30, 38, 46, 255 });
                if (std::fmod(time * 7.0f + i * 1.37f, 2.4f) < 0.08f) {
                    DrawCircle((int)cx, (int)(GROUND_Y - 18.0f), 9.0f, WHITE);
                    DrawCircle((int)cx, (int)(GROUND_Y - 18.0f), 16.0f, Color{ 200, 235, 255, 105 });
                }
            }

            // Perspective Octagon Mat & Cage Posts
            DrawPerspectiveFloor(playOff, Color{ 125, 128, 136, 255 }, Color{ 85, 88, 96, 255 });
            for (int x = -16; x < SCREEN_WIDTH + 32; x += 20) {
                float fx = x + std::fmod(playOff, 20.0f);
                DrawLine((int)fx, (int)GROUND_Y - 58, (int)fx + 20, (int)GROUND_Y, Color{ 50, 58, 66, 75 });
                DrawLine((int)fx + 20, (int)GROUND_Y - 58, (int)fx, (int)GROUND_Y, Color{ 50, 58, 66, 75 });
            }
            break;
        }
        }
    }

private:
    static void DrawPerspectiveFloor(float playOff, Color baseCol, Color lineCol) {
        int gy = (int)GROUND_Y;
        int fh = SCREEN_HEIGHT - gy;
        DrawRectangle(0, gy, SCREEN_WIDTH, fh, baseCol);

        // Horizontal depth scanlines
        for (int r = 1; r <= 4; r++) {
            int y = gy + (r * r * fh) / 16;
            DrawLine(0, y, SCREEN_WIDTH, y, lineCol);
        }
        // Perspective fan lines radiating from stage horizon
        for (int i = -6; i < 18; i++) {
            float topX = i * 36.0f + std::fmod(playOff, 36.0f);
            float botX = (topX - SCREEN_WIDTH * 0.5f) * 1.65f + SCREEN_WIDTH * 0.5f;
            DrawLine((int)topX, gy, (int)botX, SCREEN_HEIGHT, lineCol);
        }
        DrawLine(0, gy, SCREEN_WIDTH, gy, BevelHighlight(lineCol, 40));
    }
};

// ----------------------------------------------------------------------------
// 4. ARCADE POLISH, HIT-SPARKS, STAMPS, COMBO CALLOUTS, F1 DEBUG & MAIN LOOP
// ----------------------------------------------------------------------------
struct SparkParticle {
    Vector2 pos{ 0.0f, 0.0f };
    Vector2 vel{ 0.0f, 0.0f };
    int frame = 0;
    int maxFrames = 14;
    int kind = 0; // 0=Hit Starburst, 1=Block Blue Ring, 2=Stamp ("VETO!"), 3=Sweat Drop, 4=Paper/Confetti
    const char* stampText = "VETO!";
    Color col{ 255, 220, 40, 255 };
    bool alive = false;
};

struct CalloutBanner {
    std::string text;
    std::string subText;
    int playerSide = 0; // 0 = Left (P1), 1 = Right (P2)
    int timer = 0;
    int maxTimer = 75;
    Color accent{ 255, 215, 40, 255 };
};

inline void SpawnHitEffects(std::vector<SparkParticle>& sparks, Vector2 pos, bool blocked, bool heavy, bool isStamp, const char* stampStr) {
    // Primary Starburst or Block Ring
    SparkParticle sp{};
    sp.pos = pos;
    sp.frame = 0;
    sp.maxFrames = isStamp ? 26 : (heavy ? 16 : 11);
    sp.kind = isStamp ? 2 : (blocked ? 1 : 0);
    sp.stampText = stampStr;
    sp.col = blocked ? Color{ 60, 215, 255, 255 } : (heavy ? Color{ 255, 140, 25, 255 } : Color{ 255, 235, 70, 255 });
    sp.alive = true;
    sparks.push_back(sp);

    // Flying Sweat Droplets
    int sweatCount = blocked ? 2 : (heavy ? 7 : 4);
    for (int i = 0; i < sweatCount; i++) {
        SparkParticle sw{};
        sw.pos = pos;
        float ang = -PI_F * 0.85f + (std::rand() % 100) * 0.007f * PI_F;
        float spd = 1.6f + (std::rand() % 30) * 0.1f;
        sw.vel = Vector2{ std::cos(ang) * spd, std::sin(ang) * spd - 1.5f };
        sw.maxFrames = 18 + (std::rand() % 8);
        sw.kind = 3;
        sw.col = Color{ 110, 215, 255, 240 };
        sw.alive = true;
        sparks.push_back(sw);
    }

    // Drifting Paper / Subpoena / Confetti Particles on Heavy or Special Impacts
    if (heavy && !blocked) {
        for (int i = 0; i < 6; i++) {
            SparkParticle pp{};
            pp.pos = pos;
            pp.vel = Vector2{ ((std::rand() % 100) / 25.0f) - 2.0f, -2.5f - (std::rand() % 20) * 0.1f };
            pp.maxFrames = 38 + (std::rand() % 16);
            pp.kind = 4;
            pp.col = (i % 2 == 0) ? Color{ 245, 242, 225, 255 } : Color{ 255, 215, 50, 255 };
            pp.alive = true;
            sparks.push_back(pp);
        }
    }
}

inline void UpdateAndDrawSparks(std::vector<SparkParticle>& sparks) {
    for (auto& s : sparks) {
        if (!s.alive) continue;
        s.pos.x += s.vel.x;
        s.pos.y += s.vel.y;
        if (s.kind == 3) s.vel.y += 0.28f; // Sweat gravity
        if (s.kind == 4) {
            s.vel.y += 0.08f;              // Fluttering paper gravity
            s.vel.x += std::sin(s.frame * 0.35f) * 0.12f;
        }

        float prog = (float)s.frame / (float)std::max(1, s.maxFrames);
        int x = (int)s.pos.x;
        int y = (int)s.pos.y;

        if (s.kind == 0) {
            // Multi-frame 8-point Starburst Hit-Spark
            float r = 6.0f + prog * 16.0f;
            DrawCircle(x, y, (1.0f - prog) * 9.0f, WHITE);
            for (int a = 0; a < 8; a++) {
                float ang = a * (PI_F * 0.25f) + prog * 0.4f;
                Vector2 tip{ s.pos.x + std::cos(ang) * r, s.pos.y + std::sin(ang) * r };
                DrawLineEx(s.pos, tip, std::max(1.0f, (1.0f - prog) * 3.5f), s.col);
            }
        } else if (s.kind == 1) {
            // Multi-frame Blue Guard Ring
            float r = 5.0f + prog * 14.0f;
            DrawCircleLines(x, y, r, s.col);
            DrawCircleLines(x, y, r * 0.65f, WHITE);
        } else if (s.kind == 2) {
            // Giant Graphic Stamp ("VETO!", "OVERRULED!", "REDACTED")
            float scale = (prog < 0.25f) ? (prog / 0.25f) * 1.2f : 1.0f;
            int tw = MeasureText(s.stampText, 10);
            int bx = x - tw / 2 - 4;
            int by = y - 16 - (int)(prog * 10.0f);
            DrawRectangle(bx - 1, by - 1, (int)((tw + 8) * scale), 15, Color{ 15, 10, 10, 230 });
            DrawRectangleLines(bx, by, (int)((tw + 6) * scale), 13, Color{ 255, 55, 55, 255 });
            DrawPixelText(s.stampText, bx + 3, by + 2, 10, Color{ 255, 230, 65, 255 });
        } else if (s.kind == 3) {
            // Flying Sweat Droplet
            DrawCircle(x, y, 2.0f, s.col);
            DrawPixel(x, y - 1, WHITE);
        } else if (s.kind == 4) {
            // Drifting Paper / Confetti
            DrawRectangle(x, y, 5, 3, s.col);
            DrawPixel(x + 1, y + 1, Color{ 90, 90, 100, 200 });
        }

        s.frame++;
        if (s.frame >= s.maxFrames) s.alive = false;
    }
}

// Draw F1 Debug Overlay: Green Hurtboxes, Red Hitboxes, Yellow Pushboxes & Live Frame Advantage
inline void DrawF1DebugOverlay(const Fighter& p1, const Fighter& p2, float camX, const InputBuffer& in1) {
    auto drawBoxesFor = [&](const Fighter& f) {
        // 1. Yellow Pushbox
        Rectangle pb = f.GetPushbox();
        pb.x -= camX;
        DrawRectangleRec(pb, Color{ 255, 235, 40, 45 });
        DrawRectangleLinesEx(pb, 1.0f, Color{ 255, 235, 40, 230 });

        // 2. Green Hurtboxes
        for (const auto& hb : f.GetHurtboxes()) {
            Rectangle r = hb;
            r.x -= camX;
            DrawRectangleRec(r, Color{ 40, 245, 80, 55 });
            DrawRectangleLinesEx(r, 1.0f, Color{ 40, 245, 80, 240 });
        }

        // 3. Red Active Hitbox
        Rectangle atkBox{};
        MoveDef md{};
        if (f.GetActiveHitbox(atkBox, md)) {
            atkBox.x -= camX;
            DrawRectangleRec(atkBox, Color{ 255, 35, 35, 110 });
            DrawRectangleLinesEx(atkBox, 1.5f, Color{ 255, 40, 40, 255 });
        }
        // Active Projectiles Hitboxes
        for (const auto& p : f.GetProjectiles()) {
            if (!p.active) continue;
            Rectangle pr{ p.pos.x - p.width * 0.5f - camX, p.pos.y - p.height * 0.5f, p.width, p.height };
            DrawRectangleRec(pr, Color{ 255, 35, 35, 95 });
            DrawRectangleLinesEx(pr, 1.0f, Color{ 255, 50, 50, 255 });
        }
    };

    drawBoxesFor(p1);
    drawBoxesFor(p2);

    // Top-center Frame Data Telemetry Bar
    DrawRectangle(4, 34, SCREEN_WIDTH - 8, 24, Color{ 8, 12, 20, 215 });
    DrawRectangleLines(4, 34, SCREEN_WIDTH - 8, 24, Color{ 80, 220, 120, 220 });

    char p1Info[128];
    const auto& m1 = p1.GetCurrentMove();
    std::snprintf(p1Info, sizeof(p1Info), "P1: %s | F:%d (%d/%d/%d) | ADV:%+d",
                  m1.name, p1.GetStateFrame(), m1.startupFrames, m1.activeFrames, m1.recoveryFrames, p1.GetFrameAdvantage());
    DrawPixelText(p1Info, 8, 37, 6, Color{ 120, 255, 150, 255 });

    std::string bufStr = "INPUT: " + in1.GetRecentBufferString(10);
    DrawPixelText(bufStr.c_str(), 8, 48, 6, Color{ 255, 230, 90, 255 });
}

// Draw Battle HUD (With Low-HP Bruised Portraits, Super Meter Bars & Combo Callouts)
inline void DrawChampionshipHUD(const Fighter& p1, const Fighter& p2, int roundTimer,
                                const std::vector<CalloutBanner>& banners, bool debugF1) {
    const auto& prof1 = g_RosterProfiles[(int)p1.GetCharId()];
    const auto& prof2 = g_RosterProfiles[(int)p2.GetCharId()];

    // HUD Portraits (Automatically switches to Bruised/Sweating state when HP < 30%!)
    DrawProceduralPortrait(p1.GetCharId(), 6, 6, 26, 26, p1.GetHealth() < 30, false);
    DrawRectangleLines(6, 6, 26, 26, Color{ 255, 215, 50, 255 });

    DrawProceduralPortrait(p2.GetCharId(), SCREEN_WIDTH - 32, 6, 26, 26, p2.GetHealth() < 30, true);
    DrawRectangleLines(SCREEN_WIDTH - 32, 6, 26, 26, Color{ 255, 215, 50, 255 });

    // Health Bars
    constexpr int BAR_W = 132;
    constexpr int BAR_H = 9;
    int p1X = 35;
    int p2X = SCREEN_WIDTH - 35 - BAR_W;
    int barY = 14;

    DrawShadedBox((float)p1X - 1, (float)barY - 1, (float)BAR_W + 2, (float)BAR_H + 2, Color{ 165, 25, 25, 255 });
    int w1 = (std::clamp(p1.GetHealth(), 0, 100) * BAR_W) / 100;
    Color c1 = (p1.GetHealth() < 30) ? Color{ 255, 95, 35, 255 } : Color{ 245, 215, 40, 255 };
    DrawRectangle(p1X + (BAR_W - w1), barY, w1, BAR_H, c1);

    DrawShadedBox((float)p2X - 1, (float)barY - 1, (float)BAR_W + 2, (float)BAR_H + 2, Color{ 165, 25, 25, 255 });
    int w2 = (std::clamp(p2.GetHealth(), 0, 100) * BAR_W) / 100;
    Color c2 = (p2.GetHealth() < 30) ? Color{ 255, 95, 35, 255 } : Color{ 245, 215, 40, 255 };
    DrawRectangle(p2X, barY, w2, BAR_H, c2);

    // Fighter Names & Round Win Medals
    DrawPixelText(prof1.shortName, p1X, 4, 8, Color{ 255, 230, 80, 255 });
    int n2w = MeasureText(prof2.shortName, 8);
    DrawPixelText(prof2.shortName, SCREEN_WIDTH - 35 - n2w, 4, 8, Color{ 255, 230, 80, 255 });

    for (int r = 0; r < 2; r++) {
        DrawCircle(p1X + 6 + r * 12, barY + 14, 3.5f, (r < p1.GetRoundsWon()) ? Color{ 255, 215, 0, 255 } : Color{ 50, 55, 65, 255 });
        DrawCircle(SCREEN_WIDTH - 41 - r * 12, barY + 14, 3.5f, (r < p2.GetRoundsWon()) ? Color{ 255, 215, 0, 255 } : Color{ 50, 55, 65, 255 });
    }

    // Center Round Timer
    char tbuf[16];
    std::snprintf(tbuf, sizeof(tbuf), "%02d", std::max(0, roundTimer));
    DrawShadedBox((float)(SCREEN_WIDTH / 2 - 15), 6.0f, 30.0f, 20.0f, Color{ 18, 22, 34, 255 });
    int tw = MeasureText(tbuf, 16);
    DrawPixelText(tbuf, (SCREEN_WIDTH - tw) / 2, 8, 16, (roundTimer <= 10) ? Color{ 255, 60, 60, 255 } : Color{ 255, 230, 60, 255 });

    // Bottom Super Combo Meter Bars ("OMNIBUS BILL" 100% Gauge)
    constexpr int SUP_W = 124;
    constexpr int SUP_H = 7;
    int supY = SCREEN_HEIGHT - 13;
    DrawShadedBox(10.0f, (float)supY, (float)SUP_W, (float)SUP_H, Color{ 18, 24, 38, 255 });
    int sw1 = (p1.GetSuperMeter() * SUP_W) / 100;
    Color sc1 = (p1.GetSuperMeter() >= 100) ? Color{ 255, 215, 35, 255 } : Color{ 40, 185, 255, 255 };
    DrawRectangle(10, supY, sw1, SUP_H, sc1);
    DrawPixelText((p1.GetSuperMeter() >= 100) ? "OMNIBUS READY! [236236+P]" : "VETO METER", 12, supY - 8, 6, sc1);

    int sup2X = SCREEN_WIDTH - 10 - SUP_W;
    DrawShadedBox((float)sup2X, (float)supY, (float)SUP_W, (float)SUP_H, Color{ 18, 24, 38, 255 });
    int sw2 = (p2.GetSuperMeter() * SUP_W) / 100;
    Color sc2 = (p2.GetSuperMeter() >= 100) ? Color{ 255, 215, 35, 255 } : Color{ 255, 75, 95, 255 };
    DrawRectangle(sup2X + (SUP_W - sw2), supY, sw2, SUP_H, sc2);
    DrawPixelText((p2.GetSuperMeter() >= 100) ? "OMNIBUS READY!" : "VETO METER", sup2X + 42, supY - 8, 6, sc2);

    if (debugF1) {
        DrawPixelText("[F1 DEBUG ON]", SCREEN_WIDTH / 2 - 26, SCREEN_HEIGHT - 11, 6, Color{ 80, 255, 120, 255 });
    }

    // Sliding Arcade Combo Counter & Callout Banners
    for (const auto& b : banners) {
        if (b.timer <= 0) continue;
        float slide = std::min(1.0f, (b.maxTimer - b.timer) / 8.0f);
        int bw = 128;
        int bx = (b.playerSide == 0) ? (int)(-bw + slide * (bw + 8)) : (int)(SCREEN_WIDTH - slide * (bw + 8));
        int by = 64;
        DrawShadedBox((float)bx, (float)by, (float)bw, 24.0f, Color{ 16, 22, 38, 240 });
        DrawRectangleLines(bx, by, bw, 24, b.accent);
        DrawPixelText(b.text.c_str(), bx + 6, by + 3, 9, b.accent);
        DrawPixelText(b.subText.c_str(), bx + 6, by + 14, 6, WHITE);
    }
}

// Draw 30-Frame Dramatic Dark Super-Flash Cut-In ("OMNIBUS BILL" Activation)
inline void DrawSuperFlashCutIn(const Fighter& caster, int flashTimer) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 4, 8, 18, 210 });
    const auto& prof = g_RosterProfiles[(int)caster.GetCharId()];

    // Speedlines across center band
    int bandY = 66;
    int bandH = 74;
    DrawRectangle(0, bandY, SCREEN_WIDTH, bandH, ShadeColor(prof.suitPrimary, 0.85f));
    DrawLine(0, bandY, SCREEN_WIDTH, bandY, Color{ 255, 225, 60, 255 });
    DrawLine(0, bandY + bandH, SCREEN_WIDTH, bandY + bandH, Color{ 255, 225, 60, 255 });

    for (int i = 0; i < 18; i++) {
        int lx = ((i * 53 + flashTimer * 28) % (SCREEN_WIDTH + 80)) - 40;
        int ly = bandY + 4 + (i * 11) % (bandH - 8);
        DrawLine(lx, ly, lx + 48, ly, Color{ 255, 245, 160, 190 });
    }

    int portX = caster.IsFacingRight() ? 24 : (SCREEN_WIDTH - 92);
    DrawProceduralPortrait(caster.GetCharId(), portX, bandY + 5, 64, 64, false, !caster.IsFacingRight());
    DrawRectangleLines(portX, bandY + 5, 64, 64, Color{ 255, 225, 60, 255 });

    int tx = caster.IsFacingRight() ? 102 : 24;
    DrawPixelText("OMNIBUS SUPER COMBO!", tx, bandY + 14, 14, Color{ 255, 230, 55, 255 });
    DrawPixelText(prof.superName, tx, bandY + 34, 12, Color{ 80, 230, 255, 255 });
    DrawPixelText(prof.fullTitle, tx, bandY + 52, 8, WHITE);
}

// ----------------------------------------------------------------------------
// CHAMPIONSHIP AI CONTROLLER (ANTI-AIRS, ZONING, 2-IN-1 CANCELS & SUPERS)
// ----------------------------------------------------------------------------
class ChampionshipAI {
public:
    RawInputState ComputeInput(const Fighter& ai, const Fighter& opp, int frameCounter) {
        RawInputState s{};
        float dx = opp.GetPos().x - ai.GetPos().x;
        float dist = std::fabs(dx);
        bool faceRight = ai.IsFacingRight();

        // 1. Defensive Blocking against incoming attacks or projectiles
        bool incomingThreat = (opp.GetState() == FSTATE_ATTACK && dist < 95.0f) || !opp.GetProjectiles().empty();
        if (incomingThreat && (frameCounter % 3 != 0)) {
            if (faceRight) s.left = true; else s.right = true;
            if (opp.IsCrouching()) s.down = true;
            return s;
        }

        // 2. 2-in-1 Cancel straight into Special or Super when a Normal connects!
        if (ai.CanSpecialCancel() && (frameCounter % 2 == 0)) {
            s.special = true;
            if (ai.GetSuperMeter() < 100 && (frameCounter % 4 == 0)) {
                if (faceRight) s.left = true; else s.right = true;
            }
            return s;
        }

        // 3. Anti-Air 623 / DP Reversal if opponent jumps in close
        if (opp.IsAirborne() && dist < 72.0f && (frameCounter % 8 == 0)) {
            s.down = true;
            s.special = true;
            return s;
        }

        // 4. Neutral Spacing, Poke, Throw & Projectile Zoning
        if (dist > 135.0f) {
            if ((frameCounter % 45) == 0) {
                s.special = true; // Launch signature QCF projectile
            } else {
                if (faceRight) s.right = true; else s.left = true;
            }
        } else if (dist > 54.0f) {
            int r = (frameCounter / 6) % 7;
            if (r == 0) { if (faceRight) s.right = true; else s.left = true; }
            else if (r == 1 && (frameCounter % 6 == 0)) s.hp = true;
            else if (r == 2 && (frameCounter % 6 == 0)) { s.down = true; s.hk = true; } // 2HK Sweep
            else if (r == 3 && (frameCounter % 6 == 0)) { if (faceRight) s.right = true; else s.left = true; s.hp = true; } // 6HP Overhead
        } else {
            // Close range pressure & throws
            if ((frameCounter % 28) == 0) {
                s.lp = true; s.lk = true; // Unblockable Throw attempt
            } else if ((frameCounter % 9) == 0) {
                s.lp = true;
            } else if ((frameCounter % 15) == 0) {
                s.down = true; s.lk = true;
            }
        }
        return s;
    }
};

// ----------------------------------------------------------------------------
// MAIN ENTRY POINT & FULL ARCADE STATE MACHINE
// ----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    bool startWindowed = false;
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--windowed") == 0) startWindowed = true;
    }

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Executive Order II: Total Veto - Championship Edition");
    if (!IsWindowReady()) return 1;
    SetTargetFPS(TARGET_FPS);
    SetExitKey(KEY_NULL);

    if (!startWindowed) {
        ToggleBorderlessWindowed();
    }

    RenderTexture2D target = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    Shader crtShader = LoadShaderFromMemory(nullptr, CRT_FRAGMENT_SHADER);
    int scanLoc = GetShaderLocation(crtShader, "scanlineIntensity");
    int curvLoc = GetShaderLocation(crtShader, "curvature");
    int bloomLoc = GetShaderLocation(crtShader, "bloom");
    float scanVal = 0.32f, curvVal = 0.015f, bloomVal = 0.15f;
    if (crtShader.id > 0) {
        SetShaderValue(crtShader, scanLoc, &scanVal, SHADER_UNIFORM_FLOAT);
        SetShaderValue(crtShader, curvLoc, &curvVal, SHADER_UNIFORM_FLOAT);
        SetShaderValue(crtShader, bloomLoc, &bloomVal, SHADER_UNIFORM_FLOAT);
    }

    Shader flagShader = LoadShaderFromMemory(nullptr, FLAG_FRAGMENT_SHADER);
    int flagTimeLoc = GetShaderLocation(flagShader, "time");

    // Generate Procedural American Flag Texture
    Image flagImg = GenImageColor(110, 65, WHITE);
    for (int i = 0; i < 13; i++) {
        ImageDrawRectangle(&flagImg, 0, i * 5, 110, 5, (i & 1) ? Color{ 245, 245, 245, 255 } : Color{ 195, 25, 35, 255 });
    }
    ImageDrawRectangle(&flagImg, 0, 0, 44, 35, Color{ 22, 40, 112, 255 });
    Texture2D flagTex = LoadTextureFromImage(flagImg);
    UnloadImage(flagImg);

    ArcadeAudioEngine::Instance().Init();
    ArcadeAudioEngine::Instance().SetTrack(4, false);

    InputBuffer inputP1, inputP2;
    ChampionshipAI cpuAI;

    GameState state = STATE_SPLASH;
    float stateTimer = 0.0f;
    float globalTime = 0.0f;
    int frameCounter = 0;

    int p1Char = CHAR_COMMANDER;
    int p2Char = CHAR_HIGH_BENCH;
    int selectedStage = STAGE_DEMOLISHED_EAST_WING;
    bool defeated[CHARACTER_COUNT] = { false };

    std::unique_ptr<Fighter> p1 = std::make_unique<Fighter>(0, (CharacterId)p1Char, 320.0f);
    std::unique_ptr<Fighter> p2 = std::make_unique<Fighter>(1, (CharacterId)p2Char, 480.0f);

    float cameraX = 208.0f;
    int shakeFrames = 0;
    float shakeMag = 0.0f;
    int superFlashFrames = 0;
    int superCasterIdx = 0;
    int roundNumber = 1;
    int roundTimer = 99;
    float roundSecondAccum = 0.0f;
    int introTimer = 0;
    bool firstStrikeAwarded = false;
    bool debugF1 = false;
    bool crtEnabled = true;

    std::vector<SparkParticle> sparks;
    std::vector<CalloutBanner> banners;

    auto pushBanner = [&](int side, const char* title, const char* sub, Color col) {
        for (auto& b : banners) {
            if (b.playerSide == side) {
                b.text = title;
                b.subText = sub;
                b.timer = b.maxTimer;
                b.accent = col;
                return;
            }
        }
        banners.push_back(CalloutBanner{ title, sub, side, 75, 75, col });
    };

    while (!WindowShouldClose()) {
        globalTime += FIXED_DT;
        stateTimer += FIXED_DT;
        frameCounter++;

        if (IsKeyPressed(KEY_F1)) debugF1 = !debugF1;
        if (IsKeyPressed(KEY_F2)) crtEnabled = !crtEnabled;
        if (IsKeyPressed(KEY_F11)) ToggleBorderlessWindowed();

        RawInputState raw1 = InputBuffer::PollHardware(0);
        if (raw1.select || IsKeyPressed(KEY_ESCAPE)) break;

        inputP1.Update(raw1, p1->IsFacingRight());
        for (auto& b : banners) if (b.timer > 0) b.timer--;

        switch (state) {
        case STATE_SPLASH: {
            if (inputP1.IsPressed(BTN_START | BTN_COIN | BTN_LP | BTN_HP)) {
                ArcadeAudioEngine::Instance().PlaySfx(SFX_COIN_INSERT);
                state = STATE_SELECT_MAP;
                stateTimer = 0.0f;
            }
            break;
        }
        case STATE_SELECT_MAP: {
            if (inputP1.IsPressed(DIR_LEFT)) {
                p1Char = (p1Char + CHARACTER_COUNT - 1) % CHARACTER_COUNT;
                ArcadeAudioEngine::Instance().PlaySfx(SFX_MENU_CURSOR);
            } else if (inputP1.IsPressed(DIR_RIGHT)) {
                p1Char = (p1Char + 1) % CHARACTER_COUNT;
                ArcadeAudioEngine::Instance().PlaySfx(SFX_MENU_CURSOR);
            } else if (inputP1.IsPressed(DIR_UP)) {
                selectedStage = (selectedStage + STAGE_COUNT - 1) % STAGE_COUNT;
                ArcadeAudioEngine::Instance().PlaySfx(SFX_MENU_CURSOR);
            } else if (inputP1.IsPressed(DIR_DOWN)) {
                selectedStage = (selectedStage + 1) % STAGE_COUNT;
                ArcadeAudioEngine::Instance().PlaySfx(SFX_MENU_CURSOR);
            } else if (inputP1.IsPressed(BTN_START | BTN_LP | BTN_HP | BTN_LK | BTN_HK)) {
                ArcadeAudioEngine::Instance().PlaySfx(SFX_MENU_CONFIRM);
                // Pick next undefeated opponent
                p2Char = (p1Char + 1) % CHARACTER_COUNT;
                for (int i = 0; i < CHARACTER_COUNT; i++) {
                    int cand = (p1Char + 1 + i) % CHARACTER_COUNT;
                    if (!defeated[cand] && cand != p1Char) { p2Char = cand; break; }
                }
                p1 = std::make_unique<Fighter>(0, (CharacterId)p1Char, 320.0f);
                p2 = std::make_unique<Fighter>(1, (CharacterId)p2Char, 480.0f);
                roundNumber = 1;
                state = STATE_VERSUS;
                stateTimer = 0.0f;
            }
            break;
        }
        case STATE_VERSUS: {
            if (stateTimer >= 2.6f || inputP1.IsPressed(BTN_START | BTN_LP | BTN_HP)) {
                p1->ResetRound(320.0f, true);
                p2->ResetRound(480.0f, false);
                sparks.clear();
                banners.clear();
                roundTimer = 99;
                roundSecondAccum = 0.0f;
                introTimer = 90;
                firstStrikeAwarded = false;
                ArcadeAudioEngine::Instance().SetTrack(selectedStage, false);
                ArcadeAudioEngine::Instance().PlaySfx(
                    (roundNumber == 1) ? SFX_VOICE_ROUND1 : ((roundNumber == 2) ? SFX_VOICE_ROUND2 : SFX_VOICE_FINAL_ROUND));
                state = STATE_FIGHT;
                stateTimer = 0.0f;
            }
            break;
        }
        case STATE_FIGHT: {
            bool critHp = (p1->GetHealth() < 30 || p2->GetHealth() < 30);
            ArcadeAudioEngine::Instance().SetCriticalTempo(critHp);

            if (introTimer > 0) {
                introTimer--;
                if (introTimer == 35) ArcadeAudioEngine::Instance().PlaySfx(SFX_VOICE_DEBATE);
                break;
            }

            // 30-Frame Super-Flash Freeze
            if (superFlashFrames > 0) {
                superFlashFrames--;
                break;
            }

            // Timer Countdown
            roundSecondAccum += FIXED_DT;
            if (roundSecondAccum >= 1.0f && roundTimer > 0) {
                roundSecondAccum -= 1.0f;
                roundTimer--;
            }

            RawInputState raw2 = cpuAI.ComputeInput(*p2, *p1, frameCounter);
            inputP2.Update(raw2, p2->IsFacingRight());

            bool sup1 = false, can1 = false;
            bool sup2 = false, can2 = false;
            p1->Update(inputP1, *p2, sup1, can1);
            p2->Update(inputP2, *p1, sup2, can2);

            if (can1) pushBanner(0, "2-IN-1 CANCEL!", p1->GetCurrentMove().name, Color{ 80, 235, 255, 255 });
            if (can2) pushBanner(1, "2-IN-1 CANCEL!", p2->GetCurrentMove().name, Color{ 80, 235, 255, 255 });
            if (sup1) { superFlashFrames = 30; superCasterIdx = 0; }
            if (sup2) { superFlashFrames = 30; superCasterIdx = 1; }

            // Pushbox Separation & Max Leash Constraint
            Rectangle pb1 = p1->GetPushbox();
            Rectangle pb2 = p2->GetPushbox();
            if (CheckCollisionRecs(pb1, pb2)) {
                float overlap = (pb1.width * 0.5f + pb2.width * 0.5f) - std::fabs(p1->GetPos().x - p2->GetPos().x);
                if (overlap > 0.0f) {
                    float s = (p1->GetPos().x <= p2->GetPos().x) ? -1.0f : 1.0f;
                    p1->SetPosX(p1->GetPos().x + s * overlap * 0.5f);
                    p2->SetPosX(p2->GetPos().x - s * overlap * 0.5f);
                }
            }
            float dist = std::fabs(p1->GetPos().x - p2->GetPos().x);
            if (dist > MAX_FIGHTER_DISTANCE) {
                float mid = (p1->GetPos().x + p2->GetPos().x) * 0.5f;
                float s = (p1->GetPos().x < p2->GetPos().x) ? -1.0f : 1.0f;
                p1->SetPosX(mid + s * (MAX_FIGHTER_DISTANCE * 0.5f));
                p2->SetPosX(mid - s * (MAX_FIGHTER_DISTANCE * 0.5f));
            }

            // Resolve Melee Strikes & Throws for Both Fighters
            auto resolveStrike = [&](Fighter& atk, Fighter& def, int side) {
                Rectangle hb{};
                MoveDef md{};
                if (!atk.GetActiveHitbox(hb, md)) return;

                for (const auto& hurt : def.GetHurtboxes()) {
                    if (CheckCollisionRecs(hb, hurt)) {
                        atk.SetHitConnected(true);
                        atk.SetHitstop(md.hitstopFrames);
                        Vector2 contact{ (hb.x + hurt.x + hurt.width * 0.5f) * 0.5f, hb.y + hb.height * 0.5f };
                        const auto& prof = g_RosterProfiles[(int)atk.GetCharId()];

                        if (md.height == ATK_THROW) {
                            if (!def.IsAirborne()) {
                                def.SetState(FSTATE_THROWN, ANIM_HITSTUN);
                                def.ReceiveHit(md.damage, md.hitstopFrames, md.hitstunFrames, md.pushback, true, atk.IsFacingRight());
                                ArcadeAudioEngine::Instance().PlaySfx(SFX_THROW_SLAM);
                                shakeFrames = 14; shakeMag = 4.5f;
                                SpawnHitEffects(sparks, contact, false, true, true, "LOBBYIST TOSS!");
                                pushBanner(side, "BIPARTISAN TOSS!", "UNBLOCKABLE THROW", Color{ 255, 185, 40, 255 });
                            }
                            return;
                        }

                        bool blocked = def.CanBlockAttack(md.height);
                        int remRecovery = (md.startupFrames + md.activeFrames + md.recoveryFrames) - atk.GetStateFrame();

                        if (blocked) {
                            def.ReceiveBlock(md.chipDamage, md.hitstopFrames, md.blockstunFrames, md.pushback, atk.IsFacingRight());
                            atk.SetFrameAdvantage(md.blockstunFrames - remRecovery);
                            def.SetComboCount(0);
                            ArcadeAudioEngine::Instance().PlaySfx(SFX_BLOCK_THUD);
                            SpawnHitEffects(sparks, contact, true, md.hitstopFrames >= 11, false, prof.stampText);
                        } else {
                            bool wasHitstun = (def.GetState() == FSTATE_HITSTUN || def.GetState() == FSTATE_KNOCKDOWN_AIR);
                            int newCombo = wasHitstun ? (atk.GetComboCount() + 1) : 1;
                            atk.SetComboCount(newCombo);
                            atk.AddSuperMeter(md.meterGain);
                            def.ReceiveHit(md.damage, md.hitstopFrames, md.hitstunFrames, md.pushback, md.causesKnockdown, atk.IsFacingRight());
                            atk.SetFrameAdvantage(md.hitstunFrames - remRecovery);

                            bool isHeavy = (md.hitstopFrames >= 11);
                            bool isSpec = (md.id >= MOVE_SPEC_QCF);
                            ArcadeAudioEngine::Instance().PlaySfx(isHeavy ? SFX_HIT_HEAVY : SFX_HIT_LIGHT);
                            shakeFrames = isSpec ? 14 : (isHeavy ? 9 : 4);
                            shakeMag = isSpec ? 4.2f : (isHeavy ? 2.6f : 1.2f);

                            SpawnHitEffects(sparks, contact, false, isHeavy, isSpec, prof.stampText);

                            if (!firstStrikeAwarded) {
                                firstStrikeAwarded = true;
                                pushBanner(side, "FIRST STRIKE!", md.name, Color{ 255, 230, 50, 255 });
                            } else if (md.id == MOVE_SPEC_DP) {
                                pushBanner(side, "REVERSAL!", prof.dpName, Color{ 255, 95, 45, 255 });
                            } else if (newCombo >= 2) {
                                char ctext[64];
                                std::snprintf(ctext, sizeof(ctext), "%d HIT FILIBUSTER!", newCombo);
                                pushBanner(side, ctext, md.name, Color{ 255, 215, 40, 255 });
                                if (newCombo == 3) ArcadeAudioEngine::Instance().PlaySfx(SFX_VOICE_FILIBUSTER);
                            }
                        }
                        return;
                    }
                }
            };

            resolveStrike(*p1, *p2, 0);
            resolveStrike(*p2, *p1, 1);

            // Resolve Projectiles vs Opponent Hurtboxes & Projectile Clashes
            auto resolveProjectiles = [&](Fighter& owner, Fighter& target, int side) {
                for (auto& pr : owner.GetProjectiles()) {
                    if (!pr.active) continue;
                    Rectangle prBox{ pr.pos.x - pr.width * 0.5f, pr.pos.y - pr.height * 0.5f, pr.width, pr.height };
                    for (const auto& hurt : target.GetHurtboxes()) {
                        if (CheckCollisionRecs(prBox, hurt)) {
                            pr.active = false;
                            bool blocked = target.CanBlockAttack(ATK_MID);
                            if (blocked) {
                                target.ReceiveBlock(3, 8, 12, 3.0f, pr.facingRight);
                                ArcadeAudioEngine::Instance().PlaySfx(SFX_BLOCK_THUD);
                                SpawnHitEffects(sparks, pr.pos, true, false, false, "GUARD");
                            } else {
                                target.ReceiveHit(pr.damage, 10, 18, 4.0f, pr.isSuperProj, pr.facingRight);
                                owner.AddSuperMeter(10);
                                ArcadeAudioEngine::Instance().PlaySfx(SFX_PROJ_IMPACT);
                                SpawnHitEffects(sparks, pr.pos, false, true, true, g_RosterProfiles[(int)owner.GetCharId()].stampText);
                                shakeFrames = 8; shakeMag = 2.4f;
                            }
                            break;
                        }
                    }
                }
            };
            resolveProjectiles(*p1, *p2, 0);
            resolveProjectiles(*p2, *p1, 1);

            // Camera Tracking
            float midX = (p1->GetPos().x + p2->GetPos().x) * 0.5f;
            float targetCam = std::clamp(midX - SCREEN_WIDTH * 0.5f, STAGE_MIN_X, STAGE_MAX_X - SCREEN_WIDTH);
            cameraX += (targetCam - cameraX) * 0.18f;

            // Check Round End (K.O. or Time Over)
            if (p1->GetHealth() <= 0 || p2->GetHealth() <= 0 || roundTimer <= 0) {
                ArcadeAudioEngine::Instance().PlaySfx(SFX_VOICE_KO);
                if (p1->GetHealth() >= p2->GetHealth()) {
                    p1->AddRoundWin();
                    p1->SetState(FSTATE_VICTORY, ANIM_VICTORY);
                    p2->SetState(FSTATE_DEFEAT, ANIM_DEFEAT);
                } else {
                    p2->AddRoundWin();
                    p2->SetState(FSTATE_VICTORY, ANIM_VICTORY);
                    p1->SetState(FSTATE_DEFEAT, ANIM_DEFEAT);
                }

                if (p1->GetRoundsWon() >= 2 || p2->GetRoundsWon() >= 2) {
                    if (p1->GetRoundsWon() >= 2) {
                        defeated[p2Char] = true;
                        ArcadeAudioEngine::Instance().PlaySfx(SFX_VOICE_YOU_WIN);
                    }
                    state = STATE_VICTORY_SPLASH;
                    stateTimer = 0.0f;
                } else {
                    roundNumber++;
                    state = STATE_VERSUS;
                    stateTimer = 0.0f;
                }
            }
            break;
        }
        case STATE_VICTORY_SPLASH: {
            if (stateTimer >= 4.0f || inputP1.IsPressed(BTN_START | BTN_LP | BTN_HP)) {
                state = STATE_SELECT_MAP;
                stateTimer = 0.0f;
                ArcadeAudioEngine::Instance().SetTrack(4, false);
            }
            break;
        }
        default:
            break;
        }

        // --------------------------------------------------------------------
        // RENDER TO 384x224 CPS-1 FRAMEBUFFER
        // --------------------------------------------------------------------
        BeginTextureMode(target);
        ClearBackground(BLACK);

        if (state == STATE_SPLASH) {
            DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 14, 18, 38, 255 }, Color{ 68, 28, 48, 255 });
            if (flagShader.id > 0) {
                SetShaderValue(flagShader, flagTimeLoc, &globalTime, SHADER_UNIFORM_FLOAT);
                BeginShaderMode(flagShader);
            }
            DrawTexture(flagTex, SCREEN_WIDTH - 130, 22, WHITE);
            if (flagShader.id > 0) EndShaderMode();

            DrawShadedBox(32.0f, 24.0f, 320.0f, 76.0f, Color{ 16, 24, 46, 245 });
            DrawRectangleLines(32, 24, 320, 76, Color{ 255, 215, 50, 255 });
            DrawPixelText("EXECUTIVE ORDER II", 64, 32, 20, Color{ 255, 235, 130, 255 });
            DrawPixelText("TOTAL VETO", 112, 54, 24, Color{ 245, 50, 50, 255 });
            DrawShadedBox(88.0f, 82.0f, 208.0f, 14.0f, Color{ 215, 165, 25, 255 });
            DrawPixelText("CHAMPIONSHIP EDITION - CPS-1", 100, 85, 8, Color{ 15, 15, 20, 255 });

            // Showcase P1 & P2 Rigs on Title Screen
            p1->SetPosX(110.0f); p1->SetFacingRight(true);
            p2->SetPosX(274.0f); p2->SetFacingRight(false);
            DrawFighterRig(*p1, globalTime);
            DrawFighterRig(*p2, globalTime);

            if (std::fmod(globalTime, 0.8f) < 0.5f) {
                DrawPixelText("INSERT COIN / PRESS START OR ATTACK", 68, SCREEN_HEIGHT - 28, 10, Color{ 255, 235, 70, 255 });
            }
            DrawPixelText("POWERED BY RAYLIB (C) RAMON SANTAMARIA - ZLIB LICENSE", 38, SCREEN_HEIGHT - 12, 7, Color{ 170, 180, 200, 255 });
        } else if (state == STATE_SELECT_MAP) {
            DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 10, 16, 30, 255 }, Color{ 22, 34, 58, 255 });
            DrawPixelText("CHAMPIONSHIP EDITION - SELECT FIGHTER & VENUE", 42, 8, 10, Color{ 255, 225, 60, 255 });

            static const char* venueNames[STAGE_COUNT] = {
                "1. EAST WING DEMOLITION", "2. THE REFLECTING POOL",
                "3. CONGRESSIONAL CHAMBER", "4. SOUTH LAWN OCTAGON"
            };
            DrawShadedBox(62.0f, 24.0f, 260.0f, 18.0f, Color{ 24, 42, 74, 255 });
            DrawPixelText(venueNames[selectedStage], 92, 29, 8, Color{ 80, 230, 255, 255 });

            for (int i = 0; i < CHARACTER_COUNT; i++) {
                int col = i % 4;
                int row = i / 4;
                int bx = 28 + col * 84;
                int by = 52 + row * 68;
                bool sel = (i == p1Char);
                DrawShadedBox((float)bx, (float)by, 72.0f, 56.0f, sel ? Color{ 38, 65, 115, 255 } : Color{ 18, 26, 42, 255 });
                DrawProceduralPortrait((CharacterId)i, bx + 16, by + 4, 40, 36, defeated[i], false);
                DrawRectangleLines(bx, by, 72, 56, sel ? Color{ 255, 220, 45, 255 } : Color{ 60, 80, 110, 255 });
                int nw = MeasureText(g_RosterProfiles[i].shortName, 7);
                DrawPixelText(g_RosterProfiles[i].shortName, bx + (72 - nw) / 2, by + 44, 7, sel ? Color{ 255, 235, 80, 255 } : WHITE);
            }
            DrawPixelText("LEFT/RIGHT: FIGHTER | UP/DOWN: STAGE | ATTACK/START: FIGHT", 24, SCREEN_HEIGHT - 14, 7, Color{ 200, 220, 245, 255 });
        } else if (state == STATE_VERSUS) {
            DrawRectangleGradientH(0, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, Color{ 18, 38, 105, 255 }, Color{ 10, 14, 30, 255 });
            DrawRectangleGradientH(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, Color{ 10, 14, 30, 255 }, Color{ 115, 20, 30, 255 });
            DrawProceduralPortrait((CharacterId)p1Char, 28, 44, 96, 96, false, false);
            DrawRectangleLines(28, 44, 96, 96, Color{ 80, 200, 255, 255 });
            DrawProceduralPortrait((CharacterId)p2Char, SCREEN_WIDTH - 124, 44, 96, 96, false, true);
            DrawRectangleLines(SCREEN_WIDTH - 124, 44, 96, 96, Color{ 255, 75, 75, 255 });

            DrawPixelText("VS", SCREEN_WIDTH / 2 - 18, 78, 30, Color{ 255, 225, 50, 255 });
            DrawPixelText(g_RosterProfiles[p1Char].fullTitle, 24, 150, 10, Color{ 255, 230, 80, 255 });
            int w2 = MeasureText(g_RosterProfiles[p2Char].fullTitle, 10);
            DrawPixelText(g_RosterProfiles[p2Char].fullTitle, SCREEN_WIDTH - 24 - w2, 150, 10, Color{ 255, 230, 80, 255 });
        } else if (state == STATE_FIGHT) {
            float sx = 0.0f, sy = 0.0f;
            if (shakeFrames > 0) {
                shakeFrames--;
                sx = ((std::rand() % 200) / 100.0f - 1.0f) * shakeMag;
                sy = ((std::rand() % 200) / 100.0f - 1.0f) * shakeMag;
            }

            StageRenderer::DrawVenue((StageId)selectedStage, cameraX + sx, globalTime);

            rlPushMatrix();
            rlTranslatef(-cameraX + sx, sy, 0.0f);
            DrawFighterRig(*p1, globalTime);
            DrawFighterRig(*p2, globalTime);
            DrawCharacterProjectiles(*p1);
            DrawCharacterProjectiles(*p2);
            UpdateAndDrawSparks(sparks);
            rlPopMatrix();

            if (debugF1) {
                DrawF1DebugOverlay(*p1, *p2, cameraX, inputP1);
            }

            DrawChampionshipHUD(*p1, *p2, roundTimer, banners, debugF1);

            if (superFlashFrames > 0) {
                DrawSuperFlashCutIn((superCasterIdx == 0) ? *p1 : *p2, superFlashFrames);
            }
            if (introTimer > 0) {
                const char* msg = (introTimer > 40) ? "ROUND START" : "DEBATE!";
                int mw = MeasureText(msg, 22);
                DrawPixelText(msg, (SCREEN_WIDTH - mw) / 2, 92, 22, Color{ 255, 225, 50, 255 });
            }
        } else if (state == STATE_VICTORY_SPLASH) {
            int winId = (p1->GetRoundsWon() >= 2) ? p1Char : p2Char;
            int loseId = (p1->GetRoundsWon() >= 2) ? p2Char : p1Char;
            DrawRectangleGradientV(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 12, 18, 36, 255 }, Color{ 28, 14, 24, 255 });
            DrawProceduralPortrait((CharacterId)winId, 34, 26, 84, 84, false, false);
            DrawRectangleLines(34, 26, 84, 84, Color{ 255, 215, 0, 255 });
            DrawProceduralPortrait((CharacterId)loseId, SCREEN_WIDTH - 118, 26, 84, 84, true, true);
            DrawRectangleLines(SCREEN_WIDTH - 118, 26, 84, 84, Color{ 225, 45, 45, 255 });

            DrawPixelText("WINNER!", 48, 114, 10, Color{ 255, 230, 60, 255 });
            DrawPixelText("VETOED!", SCREEN_WIDTH - 102, 114, 10, Color{ 255, 90, 90, 255 });

            DrawShadedBox(20.0f, 136.0f, 344.0f, 68.0f, Color{ 14, 20, 34, 250 });
            DrawRectangleLines(20, 136, 344, 68, Color{ 235, 195, 50, 255 });
            DrawPixelText(g_RosterProfiles[winId].fullTitle, 30, 142, 9, Color{ 255, 220, 50, 255 });
            DrawPixelText(g_RosterProfiles[winId].victoryQuote1, 30, 158, 8, WHITE);
            DrawPixelText(g_RosterProfiles[winId].victoryQuote2, 30, 172, 8, WHITE);
        }

        EndTextureMode();

        // --------------------------------------------------------------------
        // PRESENT UPSCALED CRT FRAMEBUFFER TO DESKTOP WINDOW
        // --------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLACK);

        float scale = std::min((float)GetScreenWidth() / SCREEN_WIDTH, (float)GetScreenHeight() / SCREEN_HEIGHT);
        float dw = SCREEN_WIDTH * scale;
        float dh = SCREEN_HEIGHT * scale;
        Rectangle srcRec{ 0.0f, 0.0f, (float)SCREEN_WIDTH, -(float)SCREEN_HEIGHT };
        Rectangle dstRec{ (GetScreenWidth() - dw) * 0.5f, (GetScreenHeight() - dh) * 0.5f, dw, dh };

        if (crtEnabled && crtShader.id > 0) BeginShaderMode(crtShader);
        DrawTexturePro(target.texture, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        if (crtEnabled && crtShader.id > 0) EndShaderMode();

        EndDrawing();
    }

    UnloadTexture(flagTex);
    if (crtShader.id > 0) UnloadShader(crtShader);
    if (flagShader.id > 0) UnloadShader(flagShader);
    UnloadRenderTexture(target);
    ArcadeAudioEngine::Instance().Shutdown();
    CloseWindow();
    return 0;
}
