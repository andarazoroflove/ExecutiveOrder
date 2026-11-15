#include "AudioEngine.h"
#include "AssetManager.h"
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <string>

static constexpr int SAMPLE_RATE = 44100;

std::atomic<int> AudioEngine::s_currentTrack{ -1 };
std::atomic<size_t> AudioEngine::s_trackPlayhead{ 0 };
inline std::atomic<bool> s_ceCriticalTempo{ false };
void AudioEngine::SetCriticalTempo(bool critical) { s_ceCriticalTempo.store(critical, std::memory_order_relaxed); }
std::vector<int16_t>* AudioEngine::s_trackDataPtr = nullptr;

static inline float FastRand(uint32_t& state) {
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return (float)(state & 0xFFFFu) / 32768.0f - 1.0f;
}

static inline float PolyBlep(float t, float dt) {
    if (t < dt) {
        t /= dt;
        return t + t - t * t - 1.0f;
    } else if (t > 1.0f - dt) {
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }
    return 0.0f;
}

static inline float OscSaw(float& phase, float freq) {
    float dt = std::clamp(freq / (float)SAMPLE_RATE, 0.00001f, 0.48f);
    phase += dt;
    if (phase >= 1.0f) phase -= 1.0f;
    float v = 2.0f * phase - 1.0f;
    v -= PolyBlep(phase, dt);
    return v;
}

static inline float OscPulse(float& phase, float freq, float pw = 0.5f) {
    float dt = std::clamp(freq / (float)SAMPLE_RATE, 0.00001f, 0.48f);
    phase += dt;
    if (phase >= 1.0f) phase -= 1.0f;
    float v = (phase < pw) ? 1.0f : -1.0f;
    v += PolyBlep(phase, dt);
    float p2 = phase - pw;
    if (p2 < 0.0f) p2 += 1.0f;
    v -= PolyBlep(p2, dt);
    return v;
}

static inline float FastTanh(float x) {
    if (x <= -3.0f) return -1.0f;
    if (x >= 3.0f) return 1.0f;
    float x2 = x * x;
    return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}

static inline float FastDecay(float x) {
    if (x <= 0.0f) return 1.0f;
    if (x >= 16.0f) return 0.0f;
    float x2 = x * x;
    return 1.0f / (1.0f + x + 0.48f * x2 + 0.235f * x2 * x);
}

struct SvfFilter {
    float low = 0.0f;
    float band = 0.0f;
    float high = 0.0f;

    inline void Step(float in, float cutoffHz, float resQ) {
        float wc = PI_F * std::clamp(cutoffHz, 28.0f, 15500.0f) / (float)SAMPLE_RATE;
        float f = std::clamp(2.0f * wc * (1.0f - 0.1666667f * wc * wc), 0.001f, 0.82f);
        float q = std::clamp(1.0f / std::max(0.55f, resQ), 0.14f, 1.80f);
        low += f * band;
        high = in - low - q * band;
        band += f * high;

        band = std::clamp(band, -4.0f, 4.0f);
        low = std::clamp(low, -4.0f, 4.0f);
    }
};

AudioEngine& AudioEngine::Instance() {
    static AudioEngine s_instance;
    return s_instance;
}

Sound AudioEngine::BuildWaveformSound(int sampleRate, int frameCount, const int16_t* pcmData, int channels) {
    std::vector<int16_t> buf(pcmData, pcmData + (size_t)frameCount * channels);
    int fadeLen = std::min(frameCount / 4, (int)(sampleRate * 0.003f));
    if (fadeLen > 0) {
        for (int i = 0; i < fadeLen; i++) {
            float fade = (float)i / (float)fadeLen;
            for (int c = 0; c < channels; c++) {
                buf[i * channels + c] = (int16_t)(buf[i * channels + c] * fade);
                buf[(frameCount - 1 - i) * channels + c] = (int16_t)(buf[(frameCount - 1 - i) * channels + c] * fade);
            }
        }
    }

    std::vector<int16_t> stereo;
    if (channels == 1) {
        stereo.resize((size_t)frameCount * 2);
        int haasSamples = (int)(sampleRate * 0.00052f);
        int slapSamples = (int)(sampleRate * 0.018f);
        for (int i = 0; i < frameCount; i++) {
            float dry = (float)buf[i];
            float delayed = (i >= haasSamples) ? (float)buf[i - haasSamples] : dry;
            float slap = (i >= slapSamples) ? (float)buf[i - slapSamples] * 0.075f : 0.0f;
            stereo[i * 2 + 0] = (int16_t)std::clamp(dry * 0.96f + delayed * 0.09f - slap, -32000.0f, 32000.0f);
            stereo[i * 2 + 1] = (int16_t)std::clamp(delayed * 0.96f + dry * 0.09f + slap, -32000.0f, 32000.0f);
        }
    } else {
        stereo = std::move(buf);
    }

    Wave wave{};
    wave.frameCount = (unsigned int)frameCount;
    wave.sampleRate = (unsigned int)sampleRate;
    wave.sampleSize = 16;
    wave.channels = 2;
    wave.data = stereo.data();

    return LoadSoundFromWave(wave);
}

Sound AudioEngine::BuildStereoSound(int frameCount, const int16_t* interleavedStereo) {
    return BuildWaveformSound(SAMPLE_RATE, frameCount, interleavedStereo, 2);
}

void AudioEngine::LoadAnnouncerVoices() {
    AssetManager::Init();
    std::string baseDir = "assets/";
    if (!AssetManager::HasEmbeddedAssets()) {
        if (DirectoryExists("../assets/")) baseDir = "../assets/";
        else if (DirectoryExists("assets/")) baseDir = "assets/";
        else if (DirectoryExists("turbo/assets/")) baseDir = "turbo/assets/";
    }

    struct VoiceEntry {
        SfxId id;
        const char* file;
    };
    static const VoiceEntry kVoices[] = {
        { SFX_VOICE_ROUND1,          "voice_round1.wav" },
        { SFX_VOICE_ROUND2,          "voice_round2.wav" },
        { SFX_VOICE_ROUND3,          "voice_round3.wav" },
        { SFX_VOICE_FINAL_ROUND,     "voice_final_round.wav" },
        { SFX_VOICE_FIGHT,           "voice_fight.wav" },
        { SFX_VOICE_FINISH_HIM,      "voice_finish_him.wav" },
        { SFX_VOICE_VETOED,          "voice_vetoed.wav" },
        { SFX_VOICE_TOTAL_VETO,      "voice_total_veto.wav" },
        { SFX_VOICE_YOU_WIN,         "voice_you_win.wav" },
        { SFX_VOICE_PERFECT,         "voice_perfect.wav" },
        { SFX_VOICE_SUPER_VETO,      "voice_super_combo.wav" },
        { SFX_VOICE_TITLE,           "voice_title.wav" },
        { SFX_VOICE_GLOBAL_CONFLICT, "voice_global.wav" },
        { SFX_DICK_LAUGH,            "voice_dick_laugh.wav" },
    };

    for (const auto& ve : kVoices) {
        std::string p1 = baseDir + ve.file;
        if (AssetManager::FileExists(p1.c_str())) {
            Sound s = AssetManager::LoadSound(p1.c_str());
            if (s.frameCount > 0) {
                if (m_sounds[ve.id].frameCount > 0) UnloadSound(m_sounds[ve.id]);
                m_sounds[ve.id] = s;
            }
        }
    }
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
    uint32_t rng = 0x9E3779B9u;
    auto RandFloat = [&]() -> float { return FastRand(rng); };

    {
        int count = (int)(SAMPLE_RATE * 0.115f);
        std::vector<int16_t> pcm(count);
        SvfFilter svf{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(powf(t, 0.72f) * PI_F);
            svf.Step(RandFloat(), 2600.0f - 1700.0f * t, 2.2f);
            float tone = sinf(2.0f * PI_F * (560.0f - 310.0f * t) * i / SAMPLE_RATE) * 0.24f;
            pcm[i] = (int16_t)std::clamp((svf.band * 0.95f + tone) * env * 19500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_WHOOSH_LIGHT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.21f);
        std::vector<int16_t> pcm(count);
        SvfFilter svf{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(powf(t, 0.68f) * PI_F);
            svf.Step(RandFloat(), 1550.0f - 1050.0f * t, 2.4f);
            float swish = sinf(2.0f * PI_F * (340.0f - 205.0f * t) * i / SAMPLE_RATE) * 0.36f;
            pcm[i] = (int16_t)std::clamp((svf.band * 0.95f + swish) * env * 22500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_WHOOSH_HEAVY] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.18f);
        std::vector<int16_t> pcm(count);
        float phase = 0.0f, fmPhase = 0.0f;
        SvfFilter snap{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 14.0f);
            float freq = 360.0f * expf(-t * 20.0f) + 58.0f;
            fmPhase += 2.0f * PI_F * (freq * 3.5f) / SAMPLE_RATE;
            phase += 2.0f * PI_F * (freq + sinf(fmPhase) * 240.0f * expf(-t * 26.0f)) / SAMPLE_RATE;
            snap.Step(RandFloat(), 2800.0f * expf(-t * 8.0f) + 600.0f, 1.8f);
            float s = tanhf((sinf(phase) * 0.85f + snap.band * expf(-t * 18.0f) * 0.95f) * 1.45f) * env * 30800.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_HIT_LIGHT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.34f);
        std::vector<int16_t> st(count * 2);
        float phase = 0.0f, gPh = 0.0f;
        SvfFilter crunchL{}, crunchR{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 7.8f);
            float freq = 420.0f * expf(-t * 22.0f) + 43.0f;
            phase += 2.0f * PI_F * freq / SAMPLE_RATE;
            float sub = sinf(phase);
            float grit = OscSaw(gPh, 110.0f * expf(-t * 6.0f) + 55.0f) * expf(-t * 14.0f);
            crunchL.Step(RandFloat(), 2400.0f * expf(-t * 5.0f) + 420.0f, 2.0f);
            crunchR.Step(RandFloat(), 2600.0f * expf(-t * 5.0f) + 460.0f, 2.0f);
            float core = sub * 0.78f + grit * 0.42f;
            float l = tanhf((core + crunchL.band * 0.85f * expf(-t * 9.0f)) * 1.65f) * env * 31500.0f;
            float r = tanhf((core + crunchR.band * 0.85f * expf(-t * 9.0f)) * 1.65f) * env * 31500.0f;
            st[i * 2 + 0] = (int16_t)std::clamp(l, -32000.0f, 32000.0f);
            st[i * 2 + 1] = (int16_t)std::clamp(r, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_HIT_HEAVY] = BuildStereoSound(count, st.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.19f);
        std::vector<int16_t> pcm(count);
        float p1 = 0.0f, p2 = 0.0f, p3 = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 14.5f);
            p1 += 2.0f * PI_F * 880.0f / SAMPLE_RATE;
            p2 += 2.0f * PI_F * 1420.0f / SAMPLE_RATE;
            p3 += 2.0f * PI_F * 2240.0f / SAMPLE_RATE;
            float fm = sinf(p1 + sinf(p2) * 2.4f * expf(-t * 6.0f)) + sinf(p3) * 0.55f;
            float thud = sinf(2.0f * PI_F * (180.0f * expf(-t * 18.0f) + 70.0f) * i / SAMPLE_RATE) * 0.45f;
            float s = (fm * 0.62f + thud + RandFloat() * expf(-t * 32.0f) * 0.30f) * env * 29000.0f;
            pcm[i] = (int16_t)std::clamp(s, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_BLOCK] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.30f);
        std::vector<int16_t> pcm(count);
        float pSaw = 0.0f, pSub = 0.0f;
        SvfFilter svf{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(t * PI_F * 0.95f) * expf(-t * 3.4f);
            float fCar = 560.0f * expf(-t * 3.2f) + 165.0f;
            float raw = OscSaw(pSaw, fCar) * 0.65f + OscPulse(pSub, fCar * 0.5f, 0.35f) * 0.45f + RandFloat() * 0.25f;
            svf.Step(raw, 4200.0f * expf(-t * 2.4f) + 520.0f, 3.2f);
            pcm[i] = (int16_t)std::clamp(tanhf((svf.low + svf.band * 0.6f) * 1.5f) * env * 27500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_FIREBALL_LAUNCH] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.35f);
        std::vector<int16_t> pcm(count);
        SvfFilter svf{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 8.2f);
            svf.Step(RandFloat(), 2600.0f * (1.0f - t * 0.8f) + 220.0f, 2.2f);
            float bass = sinf(2.0f * PI_F * (105.0f * expf(-t * 8.0f) + 42.0f) * i / SAMPLE_RATE);
            pcm[i] = (int16_t)std::clamp(tanhf((svf.low * 0.8f + svf.band * 0.5f + bass * 0.72f) * 1.4f) * env * 28500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_FIREBALL_IMPACT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.33f);
        std::vector<int16_t> pcm(count);
        float phase = 0.0f;
        SvfFilter svf{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(t * PI_F);
            float freq = 180.0f + 840.0f * t * t;
            float saw = OscSaw(phase, freq);
            svf.Step(saw * 0.65f + RandFloat() * 0.45f, 600.0f + 4200.0f * t, 2.8f);
            pcm[i] = (int16_t)std::clamp((svf.low * 0.7f + svf.band * 0.6f) * env * 27000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_UPPERCUT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.30f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 14.0f);
            float wood = sinf(2.0f * PI_F * 290.0f * i / SAMPLE_RATE) + 0.55f * sinf(2.0f * PI_F * 595.0f * i / SAMPLE_RATE);
            float thump = sinf(2.0f * PI_F * (110.0f * expf(-t * 12.0f) + 52.0f) * i / SAMPLE_RATE) * expf(-t * 7.0f);
            pcm[i] = (int16_t)std::clamp((wood + thump * 1.35f + RandFloat() * expf(-t * 30.0f) * 0.35f) * env * 27000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_GAVEL_SLAM] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.32f);
        std::vector<int16_t> pcm(count);
        float ph = 0.0f;
        SvfFilter bp{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float raw = OscPulse(ph, 640.0f + 130.0f * sinf(t * 30.0f), 0.42f);
            bp.Step(raw, 1650.0f, 2.5f);
            pcm[i] = (int16_t)std::clamp(bp.band * sinf(t * PI_F) * 24000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_MEGAPHONE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.25f);
        std::vector<int16_t> pcm(count);
        float ph = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            int step = (int)(t * 18.0f);
            float f = 320.0f + (step * 197 % 7) * 210.0f;
            float wave = floorf(OscSaw(ph, f) * 6.0f) / 6.0f;
            pcm[i] = (int16_t)std::clamp(wave * expf(-t * 5.5f) * 22500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_ELECTRO_GLITCH] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.22f);
        std::vector<int16_t> pcm(count);
        SvfFilter filt{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            filt.Step(RandFloat(), 900.0f + 1400.0f * t, 1.9f);
            pcm[i] = (int16_t)std::clamp(filt.band * sinf(t * PI_F) * 24500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SLIDE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 1.15f);
        std::vector<int16_t> st(count * 2, 0);
        float g1 = 0.0f, g2 = 0.0f, g3 = 0.0f, g4 = 0.0f;
        SvfFilter cabL{}, cabR{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = (t < 0.008f) ? (t / 0.008f) : expf(-t * 3.1f);
            float sub = sinf(2.0f * PI_F * (92.0f * expf(-t * 6.5f) + 36.71f) * i / SAMPLE_RATE) * 0.72f;

            float chL = OscSaw(g1, 73.42f) * 0.45f + OscSaw(g2, 110.00f) * 0.40f;
            float chR = OscSaw(g3, 73.68f) * 0.45f + OscSaw(g4, 146.83f) * 0.40f;
            cabL.Step(tanhf(chL * 2.8f), 2600.0f * expf(-t * 1.4f) + 650.0f, 1.6f);
            cabR.Step(tanhf(chR * 2.8f), 2750.0f * expf(-t * 1.4f) + 680.0f, 1.6f);
            float crash = RandFloat() * expf(-t * 9.0f) * 0.48f;
            st[i * 2 + 0] = (int16_t)std::clamp(tanhf((sub + cabL.low * 0.65f + crash) * 1.35f) * env * 30500.0f, -32000.0f, 32000.0f);
            st[i * 2 + 1] = (int16_t)std::clamp(tanhf((sub + cabR.low * 0.65f - crash * 0.8f) * 1.35f) * env * 30500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_TITLE_SLAM] = BuildStereoSound(count, st.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.40f);
        std::vector<int16_t> st(count * 2, 0);
        int split = (int)(SAMPLE_RATE * 0.072f);
        float p1 = 0.0f, p2 = 0.0f;
        for (int i = 0; i < count; i++) {
            float freq = (i < split) ? 987.77f : 1479.98f;
            float t = (float)(i < split ? i : (i - split)) / (float)(count - split);
            float bell = sinf(2.0f * PI_F * freq * i / SAMPLE_RATE) * 0.65f +
                         OscPulse(p1, freq * 2.0f, 0.25f) * 0.22f +
                         OscSaw(p2, freq * 0.5f) * 0.16f;
            float env = expf(-t * 6.8f);
            st[i * 2 + 0] = (int16_t)std::clamp(bell * env * 22000.0f, -32000.0f, 32000.0f);
            st[i * 2 + 1] = (int16_t)std::clamp(bell * env * 22000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_COIN] = BuildStereoSound(count, st.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.060f);
        std::vector<int16_t> pcm(count);
        float ph = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float f = 920.0f + 420.0f * (1.0f - t);
            float tone = sinf(2.0f * PI_F * f * i / SAMPLE_RATE) * 0.7f + OscPulse(ph, f * 2.0f, 0.3f) * 0.28f;
            pcm[i] = (int16_t)std::clamp(tone * sinf(powf(t, 0.6f) * PI_F) * 19500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SELECT_CURSOR] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.22f);
        std::vector<int16_t> st(count * 2, 0);
        int mid = (int)(count * 0.38f);
        float pL = 0.0f, pR = 0.0f;
        for (int i = 0; i < count; i++) {
            float freq = (i < mid) ? 587.33f : 880.0f;
            float t = (float)(i < mid ? i : (i - mid)) / (float)(i < mid ? mid : (count - mid));
            float env = expf(-t * 5.2f);
            float l = OscSaw(pL, freq) * 0.35f + sinf(2.0f * PI_F * freq * i / SAMPLE_RATE) * 0.65f;
            float r = OscSaw(pR, freq * 1.005f) * 0.35f + sinf(2.0f * PI_F * freq * 1.5f * i / SAMPLE_RATE) * 0.45f;
            st[i * 2 + 0] = (int16_t)std::clamp(l * env * 23500.0f, -32000.0f, 32000.0f);
            st[i * 2 + 1] = (int16_t)std::clamp(r * env * 23500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SELECT_CONFIRM] = BuildStereoSound(count, st.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 1.68f);
        std::vector<int16_t> st(count * 2, 0);
        auto AddPowerStab = [&](float startSec, float durSec, float rootF, float amp, bool crashHit) {
            int s0 = (int)(startSec * SAMPLE_RATE);
            int len = (int)(durSec * SAMPLE_RATE);
            float p1 = 0.0f, p2 = 0.0f, p3 = 0.0f, p4 = 0.0f;
            SvfFilter cabL{}, cabR{};
            for (int j = 0; j < len && (s0 + j) < count; j++) {
                float u = (float)j / len;
                float env = (u < 0.025f) ? (u / 0.025f) : expf(-(u - 0.025f) * 3.4f);
                float gL = OscSaw(p1, rootF) * 0.52f + OscSaw(p2, rootF * 1.4983f) * 0.44f;
                float gR = OscSaw(p3, rootF * 1.006f) * 0.52f + OscPulse(p4, rootF * 2.0f, 0.4f) * 0.38f;
                cabL.Step(tanhf(gL * 3.1f), 2900.0f - 900.0f * u, 1.7f);
                cabR.Step(tanhf(gR * 3.1f), 3050.0f - 900.0f * u, 1.7f);
                float sub = sinf(2.0f * PI_F * (rootF * 0.5f) * j / SAMPLE_RATE) * 0.48f;
                float kick = (j < (int)(SAMPLE_RATE * 0.14f))
                    ? sinf(2.0f * PI_F * (160.0f * expf(-j / (SAMPLE_RATE * 0.03f)) + 45.0f) * j / SAMPLE_RATE) * expf(-j / (SAMPLE_RATE * 0.08f)) * 0.65f
                    : 0.0f;
                float cym = crashHit ? (RandFloat() * expf(-u * 6.5f) * 0.42f) : 0.0f;
                int idx = (s0 + j) * 2;
                st[idx + 0] = (int16_t)std::clamp((float)st[idx + 0] + (cabL.low * 0.62f + cabL.band * 0.32f + sub + kick + cym) * env * amp, -32000.0f, 32000.0f);
                st[idx + 1] = (int16_t)std::clamp((float)st[idx + 1] + (cabR.low * 0.62f + cabR.band * 0.32f + sub + kick - cym * 0.8f) * env * amp, -32000.0f, 32000.0f);
            }
        };
        AddPowerStab(0.00f, 0.22f, 164.81f, 23500.0f, true);
        AddPowerStab(0.21f, 0.22f, 196.00f, 24500.0f, false);
        AddPowerStab(0.42f, 0.20f, 220.00f, 25500.0f, false);
        AddPowerStab(0.60f, 1.05f, 246.94f, 27500.0f, true);
        AddPowerStab(0.60f, 1.05f, 329.63f, 16500.0f, false);
        m_sounds[SFX_MATCH_START_FANFARE] = BuildStereoSound(count, st.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.32f);
        std::vector<int16_t> pcm(count);
        float p1 = 0.0f, p2 = 0.0f;
        SvfFilter svf{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(t * PI_F * 0.92f) * expf(-t * 2.8f);
            float f = 310.0f + 510.0f * (1.0f - expf(-t * 6.5f));
            svf.Step(OscSaw(p1, f) * 0.6f + OscSaw(p2, f * 1.498f) * 0.45f, 3400.0f, 2.4f);
            pcm[i] = (int16_t)std::clamp(svf.low * env * 27000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_VOICE_HADOUKEN] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.36f);
        std::vector<int16_t> pcm(count);
        float ph = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(t * PI_F);
            float s = OscSaw(ph, 240.0f + 1020.0f * t) * 0.65f + RandFloat() * 0.30f;
            pcm[i] = (int16_t)std::clamp(tanhf(s * 1.4f) * env * 26500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_VOICE_UPPERCUT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.30f);
        std::vector<int16_t> pcm(count);
        float p1 = 0.0f, p2 = 0.0f, p3 = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = expf(-t * 7.5f);
            float s = OscSaw(p1, 440.0f) * 0.45f + OscSaw(p2, 554.37f) * 0.35f + OscSaw(p3, 659.25f) * 0.35f;
            pcm[i] = (int16_t)std::clamp(tanhf(s * 1.5f) * env * 25000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_VOICE_OBJECTION] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.46f);
        std::vector<int16_t> pcm(count);
        int mid = (int)(count * 0.36f);
        float p1 = 0.0f, p2 = 0.0f;
        for (int i = 0; i < count; i++) {
            float f = (i < mid) ? 523.25f : 783.99f;
            float t = (float)(i < mid ? i : (i - mid)) / (float)(i < mid ? mid : (count - mid));
            float env = expf(-t * 4.6f);
            float s = sinf(2.0f * PI_F * f * i / SAMPLE_RATE) * 0.55f + OscSaw(p1, f) * 0.28f + OscSaw(p2, f * 1.5f) * 0.22f;
            pcm[i] = (int16_t)std::clamp(s * env * 25500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_VOICE_BOO_YAA] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
        m_sounds[SFX_ROUND_WIN] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 2.85f);
        std::vector<int16_t> st(count * 2, 0);
        auto AddRiffNote = [&](float stSec, float dur, float f0, float amp) {
            int s0 = (int)(stSec * SAMPLE_RATE), len = (int)(dur * SAMPLE_RATE);
            float p1 = 0.0f, p2 = 0.0f, p3 = 0.0f;
            SvfFilter cabL{}, cabR{};
            for (int j = 0; j < len && (s0 + j) < count; j++) {
                float u = (float)j / len;
                float env = (u < 0.03f) ? (u / 0.03f) : expf(-(u - 0.03f) * 2.8f);
                float vib = 1.0f + 0.007f * sinf(2.0f * PI_F * 6.2f * j / SAMPLE_RATE);
                float gL = OscSaw(p1, f0 * vib) * 0.55f + OscSaw(p2, f0 * 0.5f) * 0.35f;
                float gR = OscSaw(p3, f0 * vib * 1.4983f) * 0.48f + sinf(2.0f * PI_F * f0 * 2.0f * j / SAMPLE_RATE) * 0.30f;
                cabL.Step(tanhf(gL * 2.4f), 3200.0f, 1.8f);
                cabR.Step(tanhf(gR * 2.4f), 3350.0f, 1.8f);
                int idx = (s0 + j) * 2;
                st[idx + 0] = (int16_t)std::clamp((float)st[idx + 0] + cabL.low * env * amp, -32000.0f, 32000.0f);
                st[idx + 1] = (int16_t)std::clamp((float)st[idx + 1] + cabR.low * env * amp, -32000.0f, 32000.0f);
            }
        };
        AddRiffNote(0.00f, 0.22f, 293.66f, 22500.0f);
        AddRiffNote(0.22f, 0.22f, 349.23f, 22500.0f);
        AddRiffNote(0.44f, 0.26f, 440.00f, 24000.0f);
        AddRiffNote(0.70f, 0.78f, 587.33f, 26500.0f);
        AddRiffNote(1.50f, 0.24f, 523.25f, 23500.0f);
        AddRiffNote(1.74f, 1.05f, 587.33f, 28000.0f);
        int dly = (int)(SAMPLE_RATE * 0.165f);
        for (int i = dly; i < count; i++) {
            st[i * 2 + 0] = (int16_t)std::clamp((float)st[i * 2 + 0] + st[(i - dly) * 2 + 1] * 0.26f, -32000.0f, 32000.0f);
            st[i * 2 + 1] = (int16_t)std::clamp((float)st[i * 2 + 1] + st[(i - dly) * 2 + 0] * 0.24f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_VICTORY_FANFARE] = BuildStereoSound(count, st.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.75f);
        std::vector<int16_t> pcm(count);
        float ph = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / SAMPLE_RATE;
            float cyc = fmodf(t, 0.20f);
            float env = (cyc < 0.12f) ? sinf(cyc / 0.12f * PI_F) : 0.0f;
            float freq = (cyc < 0.06f) ? 1240.0f : 980.0f;
            pcm[i] = (int16_t)(OscPulse(ph, freq, 0.45f) * env * 18500.0f);
        }
        m_sounds[SFX_CAR_ALARM] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.30f);
        std::vector<int16_t> pcm(count);
        SvfFilter filt{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            filt.Step(RandFloat(), 1450.0f * expf(-t * 4.0f) + 280.0f, 2.1f);
            float bass = sinf(2.0f * PI_F * (105.0f * expf(-t * 12.0f) + 38.0f) * i / SAMPLE_RATE);
            pcm[i] = (int16_t)std::clamp(tanhf((bass * 0.7f + filt.band * 0.8f) * 1.5f) * expf(-t * 8.5f) * 29000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_METAL_CRUNCH] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.26f);
        std::vector<int16_t> pcm(count);
        Resonator rg; rg.Set(3400.0f, 450.0f, (float)SAMPLE_RATE);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float s = rg.Process(RandFloat()) * 0.75f + sinf(2.0f * PI_F * (4300.0f - t * 1800.0f) * i / SAMPLE_RATE) * 0.35f;
            pcm[i] = (int16_t)std::clamp(s * expf(-t * 13.0f) * 27000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_GLASS_BREAK] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.22f);
        std::vector<int16_t> pcm(count);
        float ph = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float buzz = OscSaw(ph, 145.0f + 95.0f * sinf(t * 90.0f)) * 0.48f;
            float crackle = (RandFloat() > 0.50f) ? RandFloat() * 0.68f : 0.0f;
            pcm[i] = (int16_t)std::clamp((buzz + crackle) * expf(-t * 7.5f) * 25500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_ELEC_SPARK] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.26f);
        std::vector<int16_t> pcm(count);
        float phase = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float freq = 135.0f * (1.0f - t * 0.55f) + 15.0f * sinf(2.0f * PI_F * 26.0f * i / SAMPLE_RATE);
            phase += 2.0f * PI_F * freq / SAMPLE_RATE;
            float env = (t < 0.08f) ? (t / 0.08f) : expf(-(t - 0.08f) * 4.5f);
            pcm[i] = (int16_t)std::clamp((sinf(phase) + 0.45f * sinf(phase * 2.0f)) * env * 22000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_FART] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.32f);
        std::vector<int16_t> pcm(count);
        float filt = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float w = RandFloat(); filt += (w - filt) * 0.6f;
            float sq = sinf(2.0f * PI_F * (110.0f + 50.0f * sinf(t * 40.0f)) * i / SAMPLE_RATE);
            pcm[i] = (int16_t)std::clamp((sq * 0.45f + (w - filt) * 0.65f) * expf(-t * 4.5f) * 24000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_WORM_HISS] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.34f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float chirp = sinf(2.0f * PI_F * (360.0f + 1300.0f * t) * i / SAMPLE_RATE) * expf(-t * 5.0f);
            pcm[i] = (int16_t)std::clamp((chirp * 0.6f + RandFloat() * expf(-t * 9.0f) * 0.4f) * 24000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_INJECTION] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.62f);
        std::vector<int16_t> pcm(count);
        float p1 = 0.0f, p2 = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float f = 92.0f + 34.0f * sinf(PI_F * t);
            p1 += 2.0f * PI_F * f / SAMPLE_RATE;
            float roar = tanhf((sinf(p1) * 0.7f + OscSaw(p2, f * 0.5f) * 0.45f + RandFloat() * 0.25f) * 1.5f);
            pcm[i] = (int16_t)std::clamp(roar * expf(-t * 2.8f) * 27500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_WAR_PIG_ROAR] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.40f);
        std::vector<int16_t> pcm(count);
        float ph = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float arc = OscSaw(ph, 240.0f + 460.0f * sinf(t * 52.0f));
            pcm[i] = (int16_t)std::clamp((arc * 0.55f + RandFloat() * 0.45f) * expf(-t * 4.5f) * 25000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_EMAIL_ZAP] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.60f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float bf = 523.25f + 420.0f * t;
            float c = sinf(2.0f * PI_F * bf * i / SAMPLE_RATE) * 0.5f + sinf(2.0f * PI_F * bf * 1.5f * i / SAMPLE_RATE) * 0.35f;
            pcm[i] = (int16_t)std::clamp(c * expf(-t * 2.8f) * 26000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_ADRENOCHROME] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 2.6f);
        std::vector<int16_t> st(count * 2, 0);
        float freqs[5] = { 261.63f, 329.63f, 392.00f, 523.25f, 783.99f };
        for (int n = 0; n < 5; n++) {
            int s0 = (int)(n * 0.24f * SAMPLE_RATE), len = (int)((n == 4 ? 1.45f : 0.38f) * SAMPLE_RATE);
            float pL = 0.0f, pR = 0.0f;
            SvfFilter lpL{}, lpR{};
            for (int j = 0; j < len && (s0 + j) < count; j++) {
                float u = (float)j / len;
                float env = (u < 0.08f) ? (u / 0.08f) : expf(-(u - 0.08f) * 1.8f);
                float l = OscSaw(pL, freqs[n]) * 0.45f + sinf(2.0f * PI_F * freqs[n] * 2.0f * j / SAMPLE_RATE) * 0.35f;
                float r = OscSaw(pR, freqs[n] * 1.004f) * 0.45f + sinf(2.0f * PI_F * freqs[n] * 1.5f * j / SAMPLE_RATE) * 0.35f;
                lpL.Step(l, 2800.0f, 1.3f);
                lpR.Step(r, 2900.0f, 1.3f);
                st[(s0 + j) * 2 + 0] = (int16_t)std::clamp((float)st[(s0 + j) * 2 + 0] + lpL.low * env * 19500.0f, -32000.0f, 32000.0f);
                st[(s0 + j) * 2 + 1] = (int16_t)std::clamp((float)st[(s0 + j) * 2 + 1] + lpR.low * env * 19500.0f, -32000.0f, 32000.0f);
            }
        }
        m_sounds[SFX_JESUS_FANFARE] = BuildStereoSound(count, st.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.32f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float pop = (t < 0.04f) ? sinf(2.0f * PI_F * 1850.0f * expf(-t * 80.0f) * i / SAMPLE_RATE) : 0.0f;
            pcm[i] = (int16_t)std::clamp((pop * 0.8f + RandFloat() * expf(-t * 7.0f) * 0.6f) * 26000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_COKE_POP] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    m_sounds[SFX_GAVEL_STOMP]    = m_sounds[SFX_GAVEL_SLAM];
    m_sounds[SFX_DISSENT_FLAME]  = m_sounds[SFX_FIREBALL_LAUNCH];
    m_sounds[SFX_SECRET_SERVICE] = m_sounds[SFX_SLIDE];
    {
        int count = (int)(SAMPLE_RATE * 0.35f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float f = (t < 0.2f) ? 2400.0f : (240.0f + 560.0f * t);
            pcm[i] = (int16_t)std::clamp(sinf(2.0f * PI_F * f * i / SAMPLE_RATE) * expf(-t * 6.0f) * 24000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_NOSE_CUT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 1.45f);
        std::vector<int16_t> st(count * 2, 0);
        float p1 = 0.0f, p2 = 0.0f;
        SvfFilter fL{}, fR{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float f = 54.0f + 46.0f * sinf(PI_F * t);
            p1 += 2.0f * PI_F * f / SAMPLE_RATE;
            float raw = sinf(p1) * 0.6f + OscSaw(p2, f * 0.5f) * 0.45f + RandFloat() * 0.32f;
            fL.Step(raw, 900.0f + 800.0f * sinf(t * PI_F), 2.2f);
            fR.Step(raw, 960.0f + 800.0f * sinf(t * PI_F), 2.2f);
            float env = expf(-t * 1.65f);
            st[i * 2 + 0] = (int16_t)std::clamp(tanhf(fL.low * 1.6f) * env * 29000.0f, -32000.0f, 32000.0f);
            st[i * 2 + 1] = (int16_t)std::clamp(tanhf(fR.low * 1.6f) * env * 29000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_CTHULHU_ROAR] = BuildStereoSound(count, st.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.46f);
        std::vector<int16_t> pcm(count);
        SvfFilter splash{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            splash.Step(RandFloat(), 2200.0f * (1.0f - t * 0.65f) + 320.0f, 2.0f);
            float crunch = sinf(2.0f * PI_F * (260.0f * expf(-t * 16.0f) + 48.0f) * i / SAMPLE_RATE) * expf(-t * 8.0f);
            float laser = (t > 0.18f) ? sinf(2.0f * PI_F * (1700.0f * expf(-(t - 0.18f) * 7.0f) + 280.0f) * i / SAMPLE_RATE) * 0.35f : 0.0f;
            pcm[i] = (int16_t)std::clamp((crunch * 0.8f + splash.band * 0.7f + laser) * expf(-t * 3.5f) * 29500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SHARK_CHOMP] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.88f);
        std::vector<int16_t> pcm(count);
        float ph = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float rumble = (t < 0.55f)
                ? OscSaw(ph, 52.0f + 12.0f * sinf(t * 48.0f)) * sinf(t / 0.55f * PI_F) * 0.65f
                : sinf(2.0f * PI_F * (680.0f + 180.0f * (t - 0.55f)) * i / SAMPLE_RATE) * sinf((t - 0.55f) / 0.45f * PI_F) * 0.38f;
            pcm[i] = (int16_t)std::clamp(rumble * 23500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_CTHULHU_SNORE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.65f);
        std::vector<int16_t> pcm(count);
        float ph = 0.0f;
        SvfFilter body{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float nf = (t < 0.16f) ? 349.23f : ((t < 0.32f) ? 415.30f : 466.16f + 9.0f * sinf(t * 36.0f));
            float raw = OscPulse(ph, nf, 0.36f);
            body.Step(raw + RandFloat() * 0.08f, 1950.0f, 2.2f);
            pcm[i] = (int16_t)std::clamp((body.low * 0.7f + body.band * 0.5f) * expf(-t * 2.2f) * 26500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SAXOPHONE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.60f);
        std::vector<int16_t> st(count * 2);
        float pL = 0.0f, pR = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = (t < 0.018f) ? (t / 0.018f) : expf(-t * 3.4f);
            float sweep = 210.0f + 1780.0f * powf(t, 0.62f);
            pL += 2.0f * PI_F * sweep / SAMPLE_RATE;
            pR += 2.0f * PI_F * (sweep * 1.010f) / SAMPLE_RATE;
            float gong = sinf(2.0f * PI_F * 110.0f * i / SAMPLE_RATE) * expf(-t * 5.5f) * 0.58f;
            float chime = sinf(2.0f * PI_F * 1760.0f * i / SAMPLE_RATE) * expf(-t * 8.5f) * 0.38f;
            st[i * 2 + 0] = (int16_t)std::clamp((sinf(pL) * 0.56f + gong + chime) * env * 30500.0f, -32000.0f, 32000.0f);
            st[i * 2 + 1] = (int16_t)std::clamp((sinf(pR) * 0.56f + gong - chime) * env * 30500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SUPER_FREEZE] = BuildStereoSound(count, st.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.14f);
        std::vector<int16_t> pcm(count);
        SvfFilter filt{};
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            filt.Step(RandFloat(), 2600.0f - 1600.0f * t, 2.0f);
            float zip = sinf(2.0f * PI_F * (440.0f + 380.0f * (1.0f - t)) * i / SAMPLE_RATE) * 0.35f;
            pcm[i] = (int16_t)std::clamp((filt.band * 0.85f + zip) * sinf(t * PI_F) * 24500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_DASH] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.22f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float ring = sinf(2.0f * PI_F * 980.0f * i / SAMPLE_RATE) + 0.6f * sinf(2.0f * PI_F * 1470.0f * i / SAMPLE_RATE);
            pcm[i] = (int16_t)std::clamp((ring * 0.55f + RandFloat() * expf(-t * 25.0f) * 0.45f) * expf(-t * 11.0f) * 26000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_THROW_TECH] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.24f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float spin = sinf(2.0f * PI_F * (380.0f + 160.0f * sinf(t * 38.0f)) * i / SAMPLE_RATE);
            pcm[i] = (int16_t)std::clamp((spin * 0.55f + RandFloat() * 0.35f) * sinf(t * PI_F) * 23500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SHAWARMA_TOSS] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.42f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float hitT = fmodf(t * 4.0f, 1.0f);
            float doum = sinf(2.0f * PI_F * (185.0f * expf(-hitT * 6.0f) + 95.0f) * i / SAMPLE_RATE) * expf(-hitT * 8.0f);
            float jingle = RandFloat() * sinf(2.0f * PI_F * 3600.0f * i / SAMPLE_RATE) * expf(-hitT * 10.0f);
            pcm[i] = (int16_t)std::clamp((doum * 0.65f + jingle * 0.5f) * 26000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SHAKIRA_SHAKE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.48f);
        std::vector<int16_t> pcm(count);
        float ph = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float thud = sinf(2.0f * PI_F * (95.0f * expf(-t * 14.0f) + 42.0f) * i / SAMPLE_RATE) * expf(-t * 10.0f);
            float springFreq = 195.0f + 95.0f * sinf(2.0f * PI_F * 19.0f * t);
            ph += 2.0f * PI_F * springFreq / SAMPLE_RATE;
            float boing = sinf(ph) * expf(-t * 4.5f) * 0.65f;
            pcm[i] = (int16_t)std::clamp((thud * 0.85f + boing) * 27000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_COUCH_SPRING] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.44f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float honk = (t < 0.24f) ? (sinf(2.0f * PI_F * 485.0f * i / SAMPLE_RATE) + 0.5f * sinf(2.0f * PI_F * 610.0f * i / SAMPLE_RATE)) * sinf(t / 0.24f * PI_F) : 0.0f;
            float squeak = sinf(2.0f * PI_F * (1480.0f + 320.0f * sinf(t * 42.0f)) * i / SAMPLE_RATE) * 0.22f;
            pcm[i] = (int16_t)std::clamp((honk * 0.55f + squeak + RandFloat() * 0.15f) * expf(-t * 2.5f) * 25000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_WHEELCHAIR_ROLL] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.65f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            int step = (int)(t * 14.0f);
            float bellF = (step % 2 == 0) ? 1567.98f : 2093.00f;
            float stepT = fmodf(t * 14.0f, 1.0f);
            float bell = sinf(2.0f * PI_F * bellF * i / SAMPLE_RATE) * expf(-stepT * 5.0f);
            float sub = sinf(2.0f * PI_F * 110.0f * i / SAMPLE_RATE) * expf(-t * 6.0f) * 0.45f;
            pcm[i] = (int16_t)std::clamp((bell * 0.65f + sub) * 28000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_ATM_JACKPOT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.36f);
        std::vector<int16_t> pcm(count);
        float ph = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float freq = 1950.0f * expf(-t * 4.2f) + 260.0f + 90.0f * sinf(t * 95.0f);
            pcm[i] = (int16_t)std::clamp(OscSaw(ph, freq) * expf(-t * 3.4f) * 26500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_LASER_BEAM] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.55f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float boom = sinf(2.0f * PI_F * (130.0f * expf(-t * 6.0f) + 44.0f) * i / SAMPLE_RATE);
            float shimmer = sinf(2.0f * PI_F * (520.0f + 220.0f * t) * i / SAMPLE_RATE) * 0.35f;
            pcm[i] = (int16_t)std::clamp((boom * 0.75f + shimmer + RandFloat() * 0.25f) * expf(-t * 3.8f) * 29000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SUPREME_WAVE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.34f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float anvil = sinf(2.0f * PI_F * 1120.0f * i / SAMPLE_RATE) * 0.55f +
                          sinf(2.0f * PI_F * 1695.0f * i / SAMPLE_RATE) * 0.40f +
                          sinf(2.0f * PI_F * 2490.0f * i / SAMPLE_RATE) * 0.25f;
            pcm[i] = (int16_t)std::clamp((anvil + RandFloat() * expf(-t * 28.0f) * 0.4f) * expf(-t * 8.5f) * 28500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_HAMMER_CLANG] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.90f);
        std::vector<int16_t> pcm(count, 0);
        float beats[6] = { 0.02f, 0.11f, 0.20f, 0.38f, 0.47f, 0.56f };
        for (float bt : beats) {
            int s0 = (int)(bt * SAMPLE_RATE), len = (int)(0.07f * SAMPLE_RATE);
            for (int j = 0; j < len && (s0 + j) < count; j++) {
                float u = (float)j / len;
                float clop = sinf(2.0f * PI_F * (420.0f - 180.0f * u) * j / SAMPLE_RATE) * expf(-u * 12.0f);
                pcm[s0 + j] = (int16_t)std::clamp((float)pcm[s0 + j] + clop * 24000.0f, -32000.0f, 32000.0f);
            }
        }
        m_sounds[SFX_HORSE_GALLOP] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.30f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float drop = sinf(2.0f * PI_F * (320.0f * expf(-t * 9.0f) + 55.0f) * i / SAMPLE_RATE);
            pcm[i] = (int16_t)std::clamp((drop * 0.75f + RandFloat() * expf(-t * 14.0f) * 0.35f) * expf(-t * 6.5f) * 27000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_TRAPDOOR] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.80f);
        std::vector<int16_t> st(count * 2);
        Resonator rc1L, rc2L, rc1R, rc2R;
        rc1L.Set(750.0f, 350.0f, (float)SAMPLE_RATE);
        rc2L.Set(1650.0f, 520.0f, (float)SAMPLE_RATE);
        rc1R.Set(790.0f, 360.0f, (float)SAMPLE_RATE);
        rc2R.Set(1720.0f, 540.0f, (float)SAMPLE_RATE);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = sinf(powf(t, 0.62f) * PI_F);
            float nL = RandFloat(), nR = RandFloat();
            st[i * 2 + 0] = (int16_t)std::clamp((rc1L.Process(nL) * 0.65f + rc2L.Process(nL) * 0.45f) * env * 25500.0f, -32000.0f, 32000.0f);
            st[i * 2 + 1] = (int16_t)std::clamp((rc1R.Process(nR) * 0.65f + rc2R.Process(nR) * 0.45f) * env * 25500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_CROWD_CHEER] = BuildStereoSound(count, st.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.58f);
        std::vector<int16_t> pcm(count, 0);
        const float starts[3] = { 0.00f, 0.19f, 0.38f };
        const float pitches[3] = { 154.0f, 147.0f, 139.0f };
        for (int b = 0; b < 3; b++) {
            int s0 = (int)(starts[b] * SAMPLE_RATE);
            int len = (int)(0.145f * SAMPLE_RATE);
            Resonator f1, f2;
            f1.Set(740.0f, 130.0f, (float)SAMPLE_RATE);
            f2.Set(1180.0f, 170.0f, (float)SAMPLE_RATE);
            float ph = 0.0f;
            for (int j = 0; j < len && (s0 + j) < count; j++) {
                float u = (float)j / len;
                float env = (u < 0.10f) ? (u / 0.10f) : sinf((1.0f - (u - 0.10f) / 0.90f) * PI_F * 0.5f);
                float breath = (u < 0.18f) ? RandFloat() * 0.35f : RandFloat() * 0.06f;
                float glot = OscSaw(ph, pitches[b] * (1.0f - 0.12f * u)) + breath;
                float v = f1.Process(glot) * 1.45f + f2.Process(glot) * 0.95f + glot * 0.18f;
                pcm[s0 + j] = (int16_t)std::clamp(tanhf(v * 1.5f) * env * 27000.0f, -32000.0f, 32000.0f);
            }
        }
        m_sounds[SFX_DICK_LAUGH] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }

    {
        int count = (int)(SAMPLE_RATE * 0.56f);
        std::vector<int16_t> pcm(count);
        float lp = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float n = RandFloat();
            lp += (n - lp) * (0.45f * expf(-t * 6.0f) + 0.06f);
            float boom = (lp * 0.85f + sinf(2.0f * PI_F * (120.0f * expf(-t * 14.0f) + 38.0f) * i / SAMPLE_RATE) * 0.75f) * expf(-t * 9.5f);
            float rack = 0.0f;
            if ((t > 0.58f && t < 0.68f) || (t > 0.74f && t < 0.84f)) {
                float rt = (t < 0.70f) ? (t - 0.58f) / 0.10f : (t - 0.74f) / 0.10f;
                rack = (n * 0.55f + sinf(2.0f * PI_F * 1650.0f * i / SAMPLE_RATE) * 0.45f) * expf(-rt * 7.0f) * 0.55f;
            }
            pcm[i] = (int16_t)std::clamp((boom + rack) * 31000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SHOTGUN_BLAST] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.24f);
        std::vector<int16_t> pcm(count);
        float hp = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float n = RandFloat();
            hp = n - hp * 0.65f;
            float whistle = sinf(2.0f * PI_F * (680.0f + 260.0f * sinf(t * PI_F)) * i / SAMPLE_RATE) * 0.35f;
            pcm[i] = (int16_t)std::clamp((hp * 0.45f + whistle) * sinf(t * PI_F) * 22000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_PAPER_PLANE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.72f);
        std::vector<int16_t> pcm(count, 0);
        float hitTimes[10] = { 0.00f, 0.05f, 0.09f, 0.14f, 0.19f, 0.25f, 0.31f, 0.38f, 0.46f, 0.55f };
        float woodFreqs[10] = { 620.0f, 745.0f, 540.0f, 810.0f, 680.0f, 590.0f, 770.0f, 510.0f, 650.0f, 480.0f };
        for (int k = 0; k < 10; k++) {
            int s0 = (int)(hitTimes[k] * SAMPLE_RATE);
            int len = (int)(0.11f * SAMPLE_RATE);
            float f0 = woodFreqs[k];
            for (int j = 0; j < len && (s0 + j) < count; j++) {
                float u = (float)j / len;
                float clack = (sinf(2.0f * PI_F * f0 * j / SAMPLE_RATE) * 0.65f +
                               sinf(2.0f * PI_F * (f0 * 1.58f) * j / SAMPLE_RATE) * 0.40f +
                               RandFloat() * expf(-u * 24.0f) * 0.35f) * expf(-u * 12.0f);
                pcm[s0 + j] = (int16_t)std::clamp((float)pcm[s0 + j] + clack * 21000.0f, -32000.0f, 32000.0f);
            }
        }
        m_sounds[SFX_JENGA_CRASH] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.52f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float ping = (sinf(2.0f * PI_F * 2180.0f * i / SAMPLE_RATE) * 0.60f +
                          sinf(2.0f * PI_F * 3410.0f * i / SAMPLE_RATE) * 0.35f) * expf(-t * 9.5f);
            float zoom = sinf(2.0f * PI_F * (520.0f + 950.0f * t) * i / SAMPLE_RATE) * sinf(t * PI_F) * 0.28f;
            pcm[i] = (int16_t)std::clamp((ping + zoom + RandFloat() * expf(-t * 32.0f) * 0.35f) * 28500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_GOLF_DRIVE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.50f);
        std::vector<int16_t> pcm(count);
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            bool beepOn = (t < 0.09f) || (t > 0.14f && t < 0.23f) || (t > 0.28f && t < 0.46f);
            float morse = beepOn ? sinf(2.0f * PI_F * 1080.0f * i / SAMPLE_RATE) * 0.55f : 0.0f;
            float zap = sinf(2.0f * PI_F * (180.0f + 340.0f * sinf(t * 75.0f)) * i / SAMPLE_RATE) * 0.35f + RandFloat() * 0.22f;
            pcm[i] = (int16_t)std::clamp((morse + zap * expf(-t * 2.5f)) * 26500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_SPY_RADIO] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.62f);
        std::vector<int16_t> pcm(count);
        float lp = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float horn = (sinf(2.0f * PI_F * 116.5f * i / SAMPLE_RATE) * 0.55f +
                          sinf(2.0f * PI_F * 174.6f * i / SAMPLE_RATE) * 0.35f +
                          sinf(2.0f * PI_F * 233.1f * i / SAMPLE_RATE) * 0.22f) * sinf(powf(t, 0.7f) * PI_F);
            lp += (RandFloat() - lp) * 0.12f;
            pcm[i] = (int16_t)std::clamp((horn + lp * 0.45f) * 28500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_HORMUZ_SIREN] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.26f);
        std::vector<int16_t> pcm(count);
        float lp = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float freq = 310.0f * expf(-t * 4.5f) + 65.0f;
            float thwop = sinf(2.0f * PI_F * freq * i / SAMPLE_RATE) * expf(-t * 5.0f) * 0.65f;
            lp += (RandFloat() - lp) * 0.28f;
            float squelch = lp * sinf(t * 55.0f) * expf(-t * 3.5f) * 0.55f;
            pcm[i] = (int16_t)std::clamp((thwop + squelch) * 28500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_TAR_SPLAT] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.34f);
        std::vector<int16_t> pcm(count);
        float bp1 = 0.0f, bp2 = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float n = RandFloat();
            bp1 += (n - bp1) * (0.35f - 0.20f * t);
            bp2 += (bp1 - bp2) * 0.14f;
            float slosh = (bp1 - bp2) * 2.2f + sinf(2.0f * PI_F * (420.0f - 260.0f * t) * i / SAMPLE_RATE) * 0.22f * expf(-t * 6.0f);
            float env = sinf(powf(t, 0.55f) * PI_F);
            pcm[i] = (int16_t)std::clamp(slosh * env * 29000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_WATER_SPLASH] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.38f);
        std::vector<int16_t> pcm(count);
        float prev = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float n = RandFloat();
            float hp = n - prev * 0.72f;
            prev = n;
            float env = expf(-t * 4.2f);
            pcm[i] = (int16_t)std::clamp(hp * env * 21000.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_STEAM_HISS] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.44f);
        std::vector<int16_t> pcm(count);
        float lp = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            lp += (RandFloat() - lp) * 0.16f;
            float boom = sinf(2.0f * PI_F * (105.0f * expf(-t * 3.8f) + 38.0f) * i / SAMPLE_RATE) * expf(-t * 4.5f) * 0.70f;
            float clatter = lp * (0.65f + 0.35f * sinf(t * 95.0f)) * expf(-t * 2.8f);
            pcm[i] = (int16_t)std::clamp((boom + clatter) * 29500.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_WALL_CRUMBLE] = BuildWaveformSound(SAMPLE_RATE, count, pcm.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 1.15f);
        std::vector<int16_t> st(count * 2, 0);
        const float notes[7] = { 293.66f, 311.13f, 369.99f, 392.00f, 440.00f, 369.99f, 392.00f };
        float ph = 0.0f, f1Ph = 0.0f, f2Ph = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float seg = t * 6.99f;
            int idx = std::min(6, (int)seg);
            int nxt = std::min(6, idx + 1);
            float frac = seg - (float)idx;
            float glide = (frac > 0.72f) ? ((frac - 0.72f) / 0.28f) : 0.0f;
            float vib = sinf(2.0f * PI_F * 5.8f * t * 1.15f) * 3.8f;
            float baseF = notes[idx] * (1.0f - glide) + notes[nxt] * glide + vib;
            ph += 2.0f * PI_F * baseF / SAMPLE_RATE;
            f1Ph += 2.0f * PI_F * (baseF * 2.0f) / SAMPLE_RATE;
            f2Ph += 2.0f * PI_F * (baseF * 3.0f) / SAMPLE_RATE;
            float voice = sinf(ph) * 0.52f + sinf(f1Ph) * 0.34f + sinf(f2Ph) * 0.22f + sinf(ph * 4.0f) * 0.10f;
            float drone = sinf(2.0f * PI_F * 146.83f * i / SAMPLE_RATE) * 0.20f;
            float env = (t < 0.06f) ? (t / 0.06f) : ((t > 0.82f) ? ((1.0f - t) / 0.18f) : 1.0f);
            float dry = (voice + drone) * env;
            int echo1 = i - (int)(SAMPLE_RATE * 0.095f);
            int echo2 = i - (int)(SAMPLE_RATE * 0.190f);
            float lVal = dry * 22500.0f + (echo1 >= 0 ? st[echo1 * 2 + 1] * 0.36f : 0.0f);
            float rVal = dry * 22500.0f + (echo2 >= 0 ? st[echo2 * 2 + 0] * 0.34f : 0.0f);
            st[i * 2 + 0] = (int16_t)std::clamp(lVal, -32000.0f, 32000.0f);
            st[i * 2 + 1] = (int16_t)std::clamp(rVal, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_CALL_TO_PRAYER] = BuildStereoSound(count, st.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.74f);
        std::vector<int16_t> st(count * 2, 0);
        float pG1 = 0.0f, pG2 = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float env = (t < 0.015f) ? (t / 0.015f) : expf(-t * 2.9f);
            float gong = sinf(2.0f * PI_F * 65.4f * i / SAMPLE_RATE) * 0.58f +
                         OscSaw(pG1, 65.4f) * 0.25f +
                         OscSaw(pG2, 98.0f) * 0.25f +
                         sinf(2.0f * PI_F * 523.25f * i / SAMPLE_RATE) * expf(-t * 8.0f) * 0.25f;
            float riser = sinf(2.0f * PI_F * (140.0f + 760.0f * t * t) * i / SAMPLE_RATE) * t * 0.36f;
            st[i * 2 + 0] = (int16_t)std::clamp(tanhf((gong + riser) * 1.4f) * env * 30800.0f, -32000.0f, 32000.0f);
            st[i * 2 + 1] = (int16_t)std::clamp(tanhf((gong - riser * 0.6f) * 1.4f) * env * 30800.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_FINISHER_DRAMATIC] = BuildStereoSound(count, st.data());
    }
    {
        int count = (int)(SAMPLE_RATE * 0.88f);
        std::vector<int16_t> st(count * 2, 0);
        SvfFilter lpL{}, lpR{};
        float gL = 0.0f, gR = 0.0f;
        for (int i = 0; i < count; i++) {
            float t = (float)i / count;
            float sub = sinf(2.0f * PI_F * (135.0f * expf(-t * 5.2f) + 34.0f) * i / SAMPLE_RATE) * expf(-t * 3.0f) * 0.78f;
            float pwrL = tanhf(OscSaw(gL, 55.0f) * 3.0f) * expf(-t * 4.2f) * 0.35f;
            float pwrR = tanhf(OscSaw(gR, 82.41f) * 3.0f) * expf(-t * 4.2f) * 0.35f;
            lpL.Step(RandFloat(), 3200.0f * (1.0f - t * 0.75f) + 280.0f, 1.8f);
            lpR.Step(RandFloat(), 3400.0f * (1.0f - t * 0.75f) + 300.0f, 1.8f);
            float roarEnv = expf(-t * 2.5f);
            st[i * 2 + 0] = (int16_t)std::clamp(tanhf((sub + pwrL + lpL.low * roarEnv * 0.70f) * 1.35f) * 31200.0f, -32000.0f, 32000.0f);
            st[i * 2 + 1] = (int16_t)std::clamp(tanhf((sub + pwrR + lpR.low * roarEnv * 0.70f) * 1.35f) * 31200.0f, -32000.0f, 32000.0f);
        }
        m_sounds[SFX_FINISHER_OBLITERATE] = BuildStereoSound(count, st.data());
    }
}

namespace {

enum LeadPatch {
    PATCH_CYBER_BRASS = 0,
    PATCH_OVERDRIVE_GUITAR,
    PATCH_SHRED_HARPSICHORD,
    PATCH_STADIUM_ORGAN,
    PATCH_KLEZMER_ELECTRIC,
    PATCH_INDUSTRIAL_SOVIET,
    PATCH_HIJAZ_ELECTRIC,
    PATCH_TRIPHOP_RHODES,
    PATCH_ACID_CYBER_LEAD
};

enum DrumStyle {
    DRUM_CYBER_BREAK = 0,
    DRUM_TRIP_HOP,
    DRUM_METAL_GROOVE,
    DRUM_MARCH_CYBER,
    DRUM_INDUSTRIAL_ANVIL,
    DRUM_DOUMBEK_TRIP,
    DRUM_HORA_ROCK
};

enum RiffStyle {
    RIFF_POWER_CHUG = 0,
    RIFF_CYBER_ACID,
    RIFF_TRIPHOP_WAH,
    RIFF_FUNK_GUITAR,
    RIFF_ORCH_INDUSTRIAL,
    RIFF_OUTLAW_TWANG
};

struct TrackSpec {
    float bpm;
    float rootHz;
    int scaleSemis[8];
    int melodyA[16];
    int melodyB[16];
    int bassPattern[8];
    LeadPatch patch;
    DrumStyle drums;
    RiffStyle riff;
};

static float SemiToRatio(int semi) {
    return powf(2.0f, (float)semi / 12.0f);
}

static const TrackSpec kTrackSpecs[BGM_TRACK_COUNT] = {

    { 134.0f, 110.0f, {0,2,3,5,7,8,10,12},
      {1,0,5,5, 6,5,4,3, 5,0,8,7, 6,5,4,5},
      {5,6,8,0, 7,6,5,4, 6,0,8,9, 8,7,6,5},
      {1,1,6,6, 4,4,5,7}, PATCH_CYBER_BRASS, DRUM_CYBER_BREAK, RIFF_POWER_CHUG },

    { 126.0f, 82.41f, {0,3,5,6,7,10,12,15},
      {1,0,3,4, 5,0,4,3, 1,3,5,6, 5,3,2,0},
      {5,0,6,5, 7,6,5,3, 5,6,7,8, 7,5,3,1},
      {1,1,5,1, 3,1,4,5}, PATCH_OVERDRIVE_GUITAR, DRUM_CYBER_BREAK, RIFF_POWER_CHUG },

    { 136.0f, 110.0f, {0,2,3,5,7,8,10,12},
      {1,2,3,0, 3,4,5,0, 6,5,4,3, 5,0,4,2},
      {5,0,8,7, 6,5,6,7, 8,0,7,6, 5,4,3,2},
      {1,1,6,6, 4,4,5,7}, PATCH_OVERDRIVE_GUITAR, DRUM_METAL_GROOVE, RIFF_POWER_CHUG },

    { 140.0f, 98.0f, {0,2,3,5,7,8,11,12},
      {1,3,5,8, 7,5,4,2, 3,5,6,4, 5,0,7,0},
      {8,7,6,5, 4,6,5,4, 3,5,7,8, 7,6,7,8},
      {1,5,6,3, 4,1,2,5}, PATCH_SHRED_HARPSICHORD, DRUM_CYBER_BREAK, RIFF_POWER_CHUG },

    { 144.0f, 73.42f, {0,2,3,5,7,10,12,15},
      {1,1,0,3, 4,0,3,1, 6,0,5,4, 3,4,3,1},
      {5,5,7,5, 8,7,6,5, 7,0,8,7, 6,5,4,3},
      {1,1,1,3, 4,4,6,5}, PATCH_OVERDRIVE_GUITAR, DRUM_METAL_GROOVE, RIFF_POWER_CHUG },

    { 118.0f, 116.54f, {0,2,3,5,7,9,10,12},
      {1,3,5,6, 0,5,3,2, 1,4,5,7, 6,5,3,0},
      {5,6,7,8, 7,5,6,0, 8,7,5,4, 5,3,2,1},
      {1,5,4,6, 2,6,5,7}, PATCH_TRIPHOP_RHODES, DRUM_TRIP_HOP, RIFF_TRIPHOP_WAH },

    { 132.0f, 82.41f, {0,1,4,5,7,8,10,12},
      {1,2,3,0, 4,3,2,1, 5,0,6,5, 4,3,2,0},
      {5,6,5,4, 3,4,3,2, 8,7,6,5, 4,3,2,1},
      {1,1,2,1, 6,5,2,1}, PATCH_STADIUM_ORGAN, DRUM_CYBER_BREAK, RIFF_ORCH_INDUSTRIAL },

    { 104.0f, 103.83f, {0,2,3,5,7,9,10,12},
      {5,0,6,5, 3,2,1,0, 6,0,7,6, 5,3,5,0},
      {8,0,7,6, 5,6,7,0, 8,7,6,5, 4,3,2,5},
      {1,1,6,6, 4,4,5,5}, PATCH_KLEZMER_ELECTRIC, DRUM_TRIP_HOP, RIFF_TRIPHOP_WAH },

    { 142.0f, 130.81f, {0,2,3,5,7,10,12,15},
      {1,3,5,6, 5,3,4,2, 1,3,5,8, 7,6,5,3},
      {6,5,3,1, 4,5,6,7, 8,6,5,3, 4,3,2,1},
      {1,5,6,5, 4,5,7,5}, PATCH_ACID_CYBER_LEAD, DRUM_CYBER_BREAK, RIFF_CYBER_ACID },

    { 114.0f, 98.0f, {0,2,3,5,7,9,10,12},
      {1,0,3,5, 6,0,5,3, 4,0,6,5, 4,3,2,0},
      {5,7,8,0, 7,6,5,3, 6,7,8,0, 7,5,4,3},
      {1,1,6,4, 1,1,5,7}, PATCH_OVERDRIVE_GUITAR, DRUM_TRIP_HOP, RIFF_FUNK_GUITAR },

    { 138.0f, 110.0f, {0,2,4,5,7,9,11,12},
      {1,0,3,5, 8,0,7,5, 6,5,4,3, 2,3,5,0},
      {8,0,6,8, 7,5,6,7, 8,0,5,6, 4,3,2,1},
      {1,5,6,3, 4,1,5,5}, PATCH_CYBER_BRASS, DRUM_METAL_GROOVE, RIFF_POWER_CHUG },

    { 145.0f, 123.47f, {0,2,4,5,7,9,11,12},
      {1,0,5,0, 5,4,3,2, 3,4,3,0, 2,3,5,0},
      {5,0,8,0, 7,6,5,4, 6,0,7,8, 7,6,5,0},
      {1,1,4,4, 6,6,5,5}, PATCH_OVERDRIVE_GUITAR, DRUM_METAL_GROOVE, RIFF_POWER_CHUG },

    { 102.0f, 98.0f, {0,2,3,5,7,8,10,12},
      {1,0,3,0, 5,0,8,0, 5,0,3,0, 5,4,3,0},
      {5,0,6,5, 4,0,3,5, 8,0,7,6, 5,0,5,0},
      {1,1,6,6, 4,4,5,1}, PATCH_CYBER_BRASS, DRUM_MARCH_CYBER, RIFF_TRIPHOP_WAH },

    { 146.0f, 110.0f, {0,2,3,5,7,8,10,12},
      {1,1,5,1, 6,5,3,0, 1,1,7,6, 5,3,4,5},
      {8,8,5,8, 7,6,5,3, 6,6,7,8, 7,5,3,1},
      {1,1,1,5, 6,6,4,5}, PATCH_ACID_CYBER_LEAD, DRUM_CYBER_BREAK, RIFF_CYBER_ACID },

    { 124.0f, 130.81f, {0,2,4,5,7,9,10,12},
      {1,0,8,0, 6,5,3,0, 5,0,6,0, 5,4,3,2},
      {3,4,5,0, 6,7,8,0, 8,7,6,5, 6,7,8,0},
      {1,5,3,5, 4,6,5,7}, PATCH_STADIUM_ORGAN, DRUM_CYBER_BREAK, RIFF_FUNK_GUITAR },

    { 126.0f, 98.0f, {0,2,3,5,7,8,11,12},
      {1,0,1,3, 5,0,5,6, 5,4,3,2, 5,0,1,0},
      {5,0,8,7, 6,5,4,3, 7,0,8,7, 6,5,7,8},
      {1,5,1,5, 4,1,5,5}, PATCH_CYBER_BRASS, DRUM_MARCH_CYBER, RIFF_ORCH_INDUSTRIAL },

    { 136.0f, 110.0f, {0,1,4,5,7,8,10,12},
      {1,2,3,4, 5,6,5,3, 4,3,2,1, 3,0,5,0},
      {5,6,7,8, 7,6,5,4, 6,5,4,3, 2,3,2,1},
      {1,5,1,5, 4,1,7,1}, PATCH_KLEZMER_ELECTRIC, DRUM_HORA_ROCK, RIFF_POWER_CHUG },

    { 140.0f, 110.0f, {0,2,3,5,7,8,11,12},
      {5,0,6,5, 4,3,2,1, 7,0,8,7, 6,5,7,8},
      {8,7,6,5, 6,5,4,3, 5,4,3,2, 7,6,7,8},
      {1,5,1,5, 4,6,5,7}, PATCH_INDUSTRIAL_SOVIET, DRUM_INDUSTRIAL_ANVIL, RIFF_ORCH_INDUSTRIAL },

    { 118.0f, 98.0f, {0,1,4,5,7,8,11,12},
      {1,2,3,4, 5,4,3,2, 3,4,5,6, 5,4,3,0},
      {5,6,7,8, 6,5,4,3, 5,6,5,4, 3,2,1,0},
      {1,1,5,4, 1,1,6,5}, PATCH_HIJAZ_ELECTRIC, DRUM_DOUMBEK_TRIP, RIFF_TRIPHOP_WAH },

    { 126.0f, 110.0f, {0,2,3,5,7,8,11,12},
      {1,3,5,8, 7,6,5,4, 3,5,4,2, 1,0,5,0},
      {5,6,7,8, 9,8,7,6, 5,7,6,4, 5,0,8,0},
      {1,5,6,3, 4,1,2,5}, PATCH_SHRED_HARPSICHORD, DRUM_INDUSTRIAL_ANVIL, RIFF_POWER_CHUG },

    { 94.0f, 73.42f, {0,3,5,6,7,10,12,15},
      {1,0,3,0, 5,4,3,1, 0,3,5,6, 5,3,2,0},
      {6,0,5,4, 3,0,5,6, 7,6,5,4, 3,2,1,0},
      {1,1,7,3, 4,4,5,3}, PATCH_TRIPHOP_RHODES, DRUM_TRIP_HOP, RIFF_TRIPHOP_WAH },

    { 126.0f, 130.81f, {0,2,4,5,7,9,10,12},
      {1,3,5,6, 7,6,5,3, 4,6,8,7, 6,5,3,2},
      {8,0,7,6, 5,6,7,8, 9,8,7,6, 5,6,7,8},
      {1,8,6,8, 4,8,5,7}, PATCH_ACID_CYBER_LEAD, DRUM_CYBER_BREAK, RIFF_FUNK_GUITAR },

    { 96.0f, 130.81f, {0,2,4,5,7,9,11,12},
      {5,8,8,5, 6,6,7,3, 3,6,6,5, 4,5,1,2},
      {10,10,9,8, 7,8,9,5, 8,8,7,6, 5,6,7,8},
      {1,5,6,3, 4,5,1,5}, PATCH_CYBER_BRASS, DRUM_MARCH_CYBER, RIFF_ORCH_INDUSTRIAL },

    { 134.0f, 110.0f, {0,3,5,6,7,10,12,15},
      {1,2,3,5, 6,5,3,2, 1,3,5,6, 5,3,2,1},
      {6,6,5,3, 5,6,7,6, 5,3,2,3, 5,3,2,1},
      {1,5,1,5, 4,5,1,7}, PATCH_OVERDRIVE_GUITAR, DRUM_METAL_GROOVE, RIFF_OUTLAW_TWANG },

    { 130.0f, 92.50f, {0,2,3,5,7,8,10,12},
      {5,5,4,3, 0,3,4,5, 6,5,4,3, 2,3,1,0},
      {8,8,7,5, 6,7,8,0, 7,6,5,4, 3,4,5,0},
      {1,8,4,8, 6,8,7,8}, PATCH_OVERDRIVE_GUITAR, DRUM_DOUMBEK_TRIP, RIFF_FUNK_GUITAR },

    { 128.0f, 110.0f, {0,3,5,7,10,12,15,17},
      {1,3,4,5, 0,4,3,1, 3,4,5,6, 5,4,3,0},
      {6,5,4,3, 4,5,6,0, 7,6,5,4, 5,4,3,1},
      {1,1,4,4, 5,5,3,4}, PATCH_TRIPHOP_RHODES, DRUM_CYBER_BREAK, RIFF_CYBER_ACID },

    { 136.0f, 110.0f, {0,2,4,7,9,12,14,16},
      {1,2,3,5, 6,5,3,2, 1,3,5,6, 5,0,0,0},
      {5,6,7,8, 7,6,5,3, 5,6,5,3, 2,0,0,0},
      {1,5,1,5, 4,5,1,5}, PATCH_OVERDRIVE_GUITAR, DRUM_CYBER_BREAK, RIFF_POWER_CHUG },

    { 138.0f, 82.41f, {0,2,3,6,7,8,11,12},
      {1,0,5,4, 5,0,6,5, 1,3,5,6, 7,6,5,4},
      {8,0,7,6, 5,4,5,6, 8,7,6,5, 4,3,2,1},
      {1,1,5,1, 6,6,5,7}, PATCH_ACID_CYBER_LEAD, DRUM_CYBER_BREAK, RIFF_ORCH_INDUSTRIAL },

    { 154.0f, 82.41f, {0,3,5,6,7,10,12,15},
      {1,3,4,5, 4,3,1,0, 5,6,7,6, 5,4,3,1},
      {6,6,7,8, 7,6,5,3, 8,7,6,5, 4,3,4,5},
      {1,1,1,1, 3,3,5,4}, PATCH_OVERDRIVE_GUITAR, DRUM_METAL_GROOVE, RIFF_POWER_CHUG },

    { 108.0f, 130.81f, {0,2,4,5,7,9,10,12},
      {1,3,5,6, 5,0,3,0, 4,5,6,8, 7,6,5,0},
      {8,7,6,5, 6,5,4,3, 2,3,4,6, 5,0,8,0},
      {1,5,6,3, 4,1,2,5}, PATCH_TRIPHOP_RHODES, DRUM_TRIP_HOP, RIFF_FUNK_GUITAR },

    { 148.0f, 110.0f, {0,3,5,7,10,12,15,17},
      {1,3,5,6, 5,3,4,2, 1,3,5,8, 7,6,5,3},
      {8,6,5,3, 5,6,8,9, 8,7,6,5, 4,3,2,1},
      {1,5,3,5, 4,5,2,5}, PATCH_ACID_CYBER_LEAD, DRUM_CYBER_BREAK, RIFF_FUNK_GUITAR },

    { 158.0f, 146.83f, {0,2,3,5,7,8,11,12},
      {1,5,3,5, 4,5,2,5, 1,3,5,8, 7,6,5,7},
      {8,7,8,5, 6,5,6,3, 4,3,4,2, 5,7,8,0},
      {1,1,4,4, 5,5,1,7}, PATCH_SHRED_HARPSICHORD, DRUM_METAL_GROOVE, RIFF_POWER_CHUG },

    { 136.0f, 164.81f, {0,2,4,5,7,9,11,12},
      {1,1,3,5, 5,6,5,3, 4,4,6,8, 7,6,5,0},
      {8,8,7,6, 5,5,6,8, 9,8,7,6, 5,4,3,5},
      {1,1,5,5, 6,6,4,5}, PATCH_OVERDRIVE_GUITAR, DRUM_METAL_GROOVE, RIFF_POWER_CHUG }
};

}

void AudioEngine::SynthesizeSingleBgmTrack(int t) {
    if (t < 0 || t >= BGM_TRACK_COUNT || !m_bgmTracks[t].empty()) return;

    constexpr int STEPS = 128;
    uint32_t rng = 0xC0FFEE42u ^ ((uint32_t)t * 0x9E3779B9u);

    const TrackSpec& spec = kTrackSpecs[t];
    int samplesPerStep = std::max(2400, (int)roundf((60.0f / spec.bpm) * 0.25f * SAMPLE_RATE));
    int totalFrames = samplesPerStep * STEPS;
    m_bgmTracks[t].assign((size_t)totalFrames * 2, 0);

    std::vector<float> dryL(totalFrames, 0.0f), dryR(totalFrames, 0.0f);
    std::vector<float> sendL(totalFrames, 0.0f), sendR(totalFrames, 0.0f);

    float ratios[11];
    ratios[0] = 0.0f;
    for (int k = 0; k < 8; k++) ratios[k + 1] = SemiToRatio(spec.scaleSemis[k]);
    ratios[9]  = SemiToRatio(spec.scaleSemis[1] + 12);
    ratios[10] = SemiToRatio(spec.scaleSemis[2] + 12);

    float subPh = 0.0f, reesePh1 = 0.0f, reesePh2 = 0.0f;
    float riffPhL1 = 0.0f, riffPhL2 = 0.0f, riffPhR1 = 0.0f, riffPhR2 = 0.0f;
    float padPh1 = 0.0f, padPh2 = 0.0f, padPh3 = 0.0f;
    float leadPh1 = 0.0f, leadPh2 = 0.0f, leadPh3 = 0.0f;
    float cymPh1 = 0.0f, cymPh2 = 0.0f, cymPh3 = 0.0f, cymPh4 = 0.0f;
    float earCandyPh = 0.0f;

    SvfFilter bassFlt{};
    SvfFilter riffCabL{}, riffCabR{};
    SvfFilter padFltL{}, padFltR{};
    SvfFilter leadFltL{}, leadFltR{};
    SvfFilter snrFlt{}, hatFlt{};

    int swingOffset = (spec.drums == DRUM_TRIP_HOP || spec.drums == DRUM_DOUMBEK_TRIP)
        ? (samplesPerStep * 14) / 100
        : 0;

    for (int step = 0; step < STEPS; step++) {
        int section = step / 32;
        bool isBridge = (section == 2);
        bool isClimax = (section == 3);
        bool isRiser = (step >= 88 && step < 96);
        bool isTurnaround = (step >= 30 && step < 32) || (step >= 62 && step < 64) || (step >= 122 && step < 128);
        bool isSectionCrash = (step == 0 || step == 32 || step == 96);

        int deg = (section == 3) ? spec.melodyB[step % 16]
                : (section == 2) ? ((step % 4 == 0) ? spec.melodyA[(step / 2) % 16] : 0)
                                 : spec.melodyA[step % 16];

        float leadFreq = (deg > 0 && deg <= 10) ? (spec.rootHz * 2.0f * ratios[deg]) : 0.0f;

        int bDeg = spec.bassPattern[(step / 2) % 8];
        int bDegClamped = std::clamp(bDeg, 1, 8);
        float rootNoteHz = spec.rootHz * ratios[bDegClamped];
        float bassFreq = rootNoteHz * 0.5f;
        if (bassFreq < 41.0f) bassFreq *= 2.0f;
        if ((step % 4) == 3 && spec.riff == RIFF_CYBER_ACID) bassFreq *= 2.0f;

        float chord3Hz = spec.rootHz * ratios[((bDegClamped + 1) % 7) + 1];
        float chord5Hz = spec.rootHz * ratios[((bDegClamped + 3) % 7) + 1];
        float chord7Hz = spec.rootHz * ratios[((bDegClamped + 5) % 7) + 1];

        static const uint8_t kChugGate[16] = { 2, 1, 1, 2,  0, 1, 2, 1,  2, 1, 0, 2,  1, 1, 2, 0 };
        static const uint8_t kFunkGate[16] = { 1, 0, 2, 1,  0, 2, 1, 0,  1, 2, 0, 1,  2, 0, 2, 1 };
        int m16 = step % 16;
        int riffArt = (spec.riff == RIFF_FUNK_GUITAR) ? kFunkGate[m16] : kChugGate[m16];

        int stepStart = step * samplesPerStep + (((step & 1) == 1) ? swingOffset : 0);
        int stepLen = samplesPerStep + (((step & 1) == 0) ? swingOffset : -swingOffset);
        if (stepStart + stepLen > totalFrames) stepLen = totalFrames - stepStart;
        float invStep = 1.0f / (float)std::max(1, stepLen);

        bool trigKick = false, trigSnare = false, trigGhostSnare = false, trigOpenHat = false, trigClosedHat = false, trigAnvil = false;
        if (spec.drums == DRUM_TRIP_HOP) {

            trigKick = (m16 == 0 || m16 == 8 || m16 == 10 || (m16 == 3 && (step % 32 >= 16)));
            trigSnare = (m16 == 4 || m16 == 12);
            trigGhostSnare = (m16 == 7 || m16 == 9 || m16 == 15 || isTurnaround);
            trigOpenHat = (m16 == 14);
            trigClosedHat = ((m16 % 2) == 0) && !trigOpenHat;
        } else if (spec.drums == DRUM_CYBER_BREAK) {

            trigKick = (m16 == 0 || m16 == 10 || (m16 == 6 && !isBridge));
            trigSnare = (m16 == 4 || m16 == 12 || (isTurnaround && (m16 % 2 == 1)));
            trigGhostSnare = (m16 == 7 || m16 == 9 || m16 == 15);
            trigOpenHat = (m16 == 2 || m16 == 14);
            trigClosedHat = !trigOpenHat;
        } else if (spec.drums == DRUM_METAL_GROOVE) {

            trigKick = (m16 == 0 || m16 == 2 || m16 == 6 || m16 == 8 || m16 == 10 || (isClimax && (m16 == 14 || m16 == 15)));
            trigSnare = (m16 == 4 || m16 == 12 || isTurnaround);
            trigGhostSnare = (m16 == 15);
            trigOpenHat = (m16 % 4 == 2);
            trigClosedHat = (m16 % 2 == 0) && !trigOpenHat;
        } else if (spec.drums == DRUM_INDUSTRIAL_ANVIL) {
            trigKick = ((m16 % 4) == 0 || m16 == 10);
            trigSnare = (m16 == 4 || m16 == 12 || isTurnaround);
            trigAnvil = (m16 == 4 || m16 == 12 || m16 == 14);
            trigClosedHat = (m16 % 2 == 1);
        } else if (spec.drums == DRUM_DOUMBEK_TRIP) {

            trigKick = (m16 == 0 || m16 == 6 || m16 == 10);
            trigSnare = (m16 == 4 || m16 == 12);
            trigGhostSnare = (m16 == 3 || m16 == 7 || m16 == 14 || isTurnaround);
            trigClosedHat = (m16 % 2 == 0);
        } else if (spec.drums == DRUM_HORA_ROCK) {
            trigKick = (m16 == 0 || m16 == 3 || m16 == 8 || m16 == 11);
            trigSnare = (m16 == 2 || m16 == 6 || m16 == 10 || m16 == 14 || isTurnaround);
            trigOpenHat = (m16 % 4 == 2);
            trigClosedHat = !trigOpenHat;
        } else {

            trigKick = ((m16 % 4) == 0);
            trigSnare = (m16 == 4 || m16 == 12 || isTurnaround);
            trigGhostSnare = ((m16 % 2) == 1 || m16 == 6 || m16 == 14);
            trigClosedHat = ((m16 % 4) == 2);
        }

        if (isBridge && step < 80) {

            trigClosedHat = ((m16 % 4) == 2);
            trigOpenHat = (m16 == 14);
            if (m16 != 0 && m16 != 10) trigKick = false;
        }
        if (isRiser) {

            trigGhostSnare = true;
            if (step >= 92) trigSnare = true;
        }

        for (int s = 0; s < stepLen; s++) {
            float u = (float)s * invStep;
            float sec = (float)(stepStart + s) / (float)SAMPLE_RATE;
            float noise = FastRand(rng);

            float drumL = 0.0f, drumR = 0.0f;
            float sidechainDuck = 1.0f;

            if (trigKick) {
                float kPitch = 185.0f * FastDecay(u * 19.0f) + 45.0f;
                float kSub = sinf(2.0f * PI_F * kPitch * ((float)s / SAMPLE_RATE));
                float kClick = sinf(2.0f * PI_F * 1950.0f * ((float)s / SAMPLE_RATE)) * FastDecay(u * 42.0f) * 0.42f;
                float kEnv = FastDecay(u * 7.2f);
                float kick = FastTanh((kSub + kClick) * 1.55f) * kEnv * 0.78f;
                drumL += kick;
                drumR += kick;
                sidechainDuck = 1.0f - 0.62f * FastDecay(u * 6.5f);
            }

            if (trigSnare || trigGhostSnare) {
                float snGain = trigSnare ? (isRiser ? (0.42f + 0.35f * ((float)(step - 88) / 8.0f)) : 0.64f) : 0.24f;
                float sSec = (float)s / SAMPLE_RATE;
                float body = (sinf(2.0f * PI_F * (205.0f * FastDecay(u * 8.0f) + 145.0f) * sSec) * 0.60f +
                              sinf(2.0f * PI_F * 330.0f * sSec) * FastDecay(u * 14.0f) * 0.35f) * FastDecay(u * 11.0f);
                snrFlt.Step(noise, (spec.drums == DRUM_TRIP_HOP) ? 2350.0f : 3100.0f, 1.5f);
                float wires = (snrFlt.band * 0.75f + snrFlt.high * 0.40f) * FastDecay(u * (trigSnare ? 8.2f : 15.0f));
                float sn = FastTanh((body + wires * 0.90f) * 1.4f) * snGain;
                drumL += sn * 0.96f;
                drumR += sn * 1.04f;
                if (trigSnare && spec.drums == DRUM_TRIP_HOP) {

                    sendL[stepStart + s] += sn * 0.22f;
                }
            }

            if (trigClosedHat || trigOpenHat || isSectionCrash) {
                float m1 = OscPulse(cymPh1, 3140.0f, 0.5f);
                float m2 = OscPulse(cymPh2, 4580.0f, 0.5f);
                float m3 = OscPulse(cymPh3, 6290.0f, 0.5f);
                float m4 = OscPulse(cymPh4, 8120.0f, 0.5f);
                float metal = (m1 + m2 + m3 + m4) * 0.22f + noise * 0.58f;
                hatFlt.Step(metal, 6800.0f, 1.4f);
                if (isSectionCrash) {
                    float cr = hatFlt.high * FastDecay(u * 2.2f) * 0.30f;
                    drumL += cr * 1.1f;
                    drumR += cr * 0.9f;
                }
                if (trigOpenHat) {
                    float oh = hatFlt.high * FastDecay(u * 6.5f) * 0.17f;
                    drumL += oh * 0.82f;
                    drumR += oh * 1.18f;
                } else if (trigClosedHat) {
                    float ch = hatFlt.high * FastDecay(u * 26.0f) * 0.11f;
                    drumL += ch * 1.12f;
                    drumR += ch * 0.88f;
                }
            }

            if (trigAnvil) {
                float sSec = (float)s / SAMPLE_RATE;
                float anv = (sinf(2.0f * PI_F * 1260.0f * sSec) * 0.55f +
                             sinf(2.0f * PI_F * 1895.0f * sSec) * 0.40f +
                             sinf(2.0f * PI_F * 2740.0f * sSec) * 0.25f) * FastDecay(u * 13.0f) * 0.24f;
                drumL += anv * ((step & 4) ? 1.25f : 0.75f);
                drumR += anv * ((step & 4) ? 0.75f : 1.25f);
            }

            if (spec.drums == DRUM_TRIP_HOP) {
                float crackle = (noise > 0.994f) ? (noise * 0.055f) : (noise * 0.004f);
                drumL += crackle;
                drumR += crackle * 0.8f;
            }

            subPh += 2.0f * PI_F * bassFreq / (float)SAMPLE_RATE;
            if (subPh > 2.0f * PI_F) subPh -= 2.0f * PI_F;
            float subBass = sinf(subPh) * FastDecay(u * 2.4f) * 0.42f;

            float reeseRaw = OscSaw(reesePh1, bassFreq * 0.996f) * 0.5f +
                             OscPulse(reesePh2, bassFreq * 1.005f, 0.38f) * 0.5f;
            float acidCut = (spec.riff == RIFF_CYBER_ACID)
                ? (320.0f + 2400.0f * FastDecay(u * 7.5f) + 450.0f * sinf(sec * 2.2f))
                : (260.0f + 950.0f * FastDecay(u * 5.5f));
            float acidRes = (spec.riff == RIFF_CYBER_ACID) ? 4.2f : 2.1f;
            bassFlt.Step(reeseRaw, acidCut, acidRes);
            float midBass = FastTanh((bassFlt.low + bassFlt.band * 0.45f) * 1.85f) * FastDecay(u * 3.2f) * 0.30f;
            float bassMono = (subBass + midBass) * sidechainDuck;

            float riffL = 0.0f, riffR = 0.0f;

            if (spec.riff == RIFF_POWER_CHUG || spec.riff == RIFF_ORCH_INDUSTRIAL || spec.riff == RIFF_OUTLAW_TWANG) {
                if (riffArt > 0 && (!isBridge || step >= 80)) {
                    bool openChord = (riffArt == 2) || (isClimax && (m16 == 0 || m16 == 8));
                    float decayRate = openChord ? 2.6f : 11.5f;
                    float rEnv = ((u < 0.015f) ? (u / 0.015f) : 1.0f) * FastDecay(u * decayRate) * sidechainDuck;
                    float slide = (spec.riff == RIFF_OUTLAW_TWANG && openChord && u < 0.18f)
                        ? (0.94f + 0.06f * (u / 0.18f)) : 1.0f;
                    float gRoot = rootNoteHz * slide;
                    float gFifth = gRoot * 1.4983f;

                    float rawGL = OscSaw(riffPhL1, gRoot * 0.998f) * 0.55f + OscSaw(riffPhL2, gFifth) * 0.45f;
                    float rawGR = OscSaw(riffPhR1, gRoot * 1.003f) * 0.55f + OscPulse(riffPhR2, gFifth * 1.002f, 0.44f) * 0.45f;
                    float drive = openChord ? 3.2f : 2.5f;
                    float cabCut = openChord ? 3100.0f : 1650.0f;
                    riffCabL.Step(FastTanh(rawGL * drive), cabCut, 1.75f);
                    riffCabR.Step(FastTanh(rawGR * drive), cabCut * 1.04f, 1.75f);
                    riffL += (riffCabL.low * 0.65f + riffCabL.band * 0.45f) * rEnv * 0.28f;
                    riffR += (riffCabR.low * 0.65f + riffCabR.band * 0.45f) * rEnv * 0.28f;
                }
            } else if (spec.riff == RIFF_FUNK_GUITAR) {
                if (riffArt > 0) {
                    float fEnv = FastDecay(u * ((riffArt == 2) ? 6.5f : 14.0f)) * sidechainDuck;
                    float wahCut = 950.0f + 2200.0f * FastDecay(u * 9.0f) + 550.0f * sinf(sec * 5.0f);
                    float rawL = OscSaw(riffPhL1, chord3Hz * 2.0f) * 0.5f + OscPulse(riffPhL2, chord5Hz * 2.0f, 0.30f) * 0.5f;
                    float rawR = OscSaw(riffPhR1, chord5Hz * 2.004f) * 0.5f + OscPulse(riffPhR2, chord7Hz * 2.0f, 0.30f) * 0.5f;
                    riffCabL.Step(FastTanh(rawL * 1.8f), wahCut, 3.4f);
                    riffCabR.Step(FastTanh(rawR * 1.8f), wahCut * 1.06f, 3.4f);
                    riffL += riffCabL.band * fEnv * 0.26f;
                    riffR += riffCabR.band * fEnv * 0.26f;
                }
            }

            {
                bool chordStrum = ((step % 8) == 0 || (step % 8) == 6);
                float cEnv = chordStrum ? FastDecay(u * 2.2f) : (0.52f * FastDecay(u * 1.5f));
                float n1 = OscSaw(padPh1, rootNoteHz);
                float n2 = OscSaw(padPh2, chord3Hz * 1.003f);
                float n3 = OscPulse(padPh3, chord7Hz, 0.45f);
                float padSweep = isRiser
                    ? (650.0f + 4200.0f * ((float)(step - 88) / 8.0f + u * 0.125f))
                    : (isBridge ? (950.0f + 550.0f * sinf(sec * 1.8f)) : 1850.0f);
                padFltL.Step(n1 * 0.4f + n2 * 0.35f + n3 * 0.25f, padSweep, isBridge ? 2.6f : 1.4f);
                padFltR.Step(n1 * 0.35f + n2 * 0.25f + n3 * 0.40f, padSweep * 1.05f, isBridge ? 2.6f : 1.4f);
                float padGain = (spec.riff == RIFF_TRIPHOP_WAH || isBridge) ? 0.21f : 0.12f;
                riffL += padFltL.low * cEnv * sidechainDuck * padGain;
                riffR += padFltR.low * cEnv * sidechainDuck * padGain;
            }

            if (isBridge) {
                float candy = 0.0f;
                if (spec.drums == DRUM_TRIP_HOP) {

                    if ((step % 16) == 4 || (step % 16) == 11) {
                        candy = sinf(2.0f * PI_F * (chord7Hz * 2.0f) * ((float)s / SAMPLE_RATE)) * FastDecay(u * 7.5f) * 0.20f;
                        sendL[stepStart + s] += candy * 0.65f;
                    }
                } else if (spec.riff == RIFF_CYBER_ACID || t == BGM_MENU_SPLASH || t == BGM_WORLD_CONFLICT_MAP) {

                    float arpF = spec.rootHz * 4.0f * ratios[((step * 3) % 7) + 1];
                    candy = OscPulse(earCandyPh, arpF, 0.25f) * FastDecay(u * 8.5f) * 0.14f;
                    sendR[stepStart + s] += candy * 0.55f;
                } else {

                    if ((step % 4) == 0) {
                        candy = sinf(2.0f * PI_F * (chord5Hz * 2.0f) * ((float)s / SAMPLE_RATE)) * FastDecay(u * 5.0f) * 0.15f;
                        sendL[stepStart + s] += candy * 0.45f;
                    }
                }
                riffL += candy * 0.8f;
                riffR += candy * 1.1f;
            }

            float leadL = 0.0f, leadR = 0.0f;
            if (leadFreq > 30.0f) {
                float lEnv = (u < 0.035f) ? (u / 0.035f) : FastDecay((u - 0.035f) * 1.95f);
                float vibDepth = 0.0055f;
                float vibRate = 6.0f;
                if (spec.patch == PATCH_KLEZMER_ELECTRIC || spec.patch == PATCH_HIJAZ_ELECTRIC) {
                    vibDepth = 0.012f;
                    vibRate = 6.4f;
                } else if (spec.patch == PATCH_SHRED_HARPSICHORD) {
                    vibDepth = 0.0015f;
                    lEnv = FastDecay(u * 4.6f);
                } else if (spec.patch == PATCH_TRIPHOP_RHODES) {
                    vibDepth = 0.003f;
                    lEnv = FastDecay(u * 3.1f);
                }

                float scoop = ((spec.patch == PATCH_OVERDRIVE_GUITAR || spec.patch == PATCH_KLEZMER_ELECTRIC) && u < 0.08f)
                    ? (0.975f + 0.025f * (u / 0.08f)) : 1.0f;
                float fActual = leadFreq * scoop * (1.0f + vibDepth * sinf(2.0f * PI_F * vibRate * sec));

                if (spec.patch == PATCH_OVERDRIVE_GUITAR) {
                    float o1 = OscSaw(leadPh1, fActual);
                    float o2 = OscPulse(leadPh2, fActual * 1.004f, 0.36f);
                    float o3 = isClimax ? OscSaw(leadPh3, fActual * 1.4983f) * 0.40f : 0.0f;
                    leadFltL.Step(FastTanh((o1 * 0.7f + o3) * 2.9f), 3400.0f - 800.0f * u, 2.1f);
                    leadFltR.Step(FastTanh((o2 * 0.7f + o3) * 2.9f), 3600.0f - 800.0f * u, 2.1f);
                    leadL = (leadFltL.low * 0.65f + leadFltL.band * 0.45f) * lEnv * 0.30f;
                    leadR = (leadFltR.low * 0.65f + leadFltR.band * 0.45f) * lEnv * 0.30f;
                } else if (spec.patch == PATCH_TRIPHOP_RHODES) {

                    leadPh1 += 2.0f * PI_F * fActual / SAMPLE_RATE;
                    leadPh2 += 2.0f * PI_F * (fActual * 7.0f) / SAMPLE_RATE;
                    float tine = sinf(leadPh2) * FastDecay(u * 14.0f) * 0.28f;
                    float body = sinf(leadPh1 + tine * 1.8f) + 0.25f * sinf(leadPh1 * 2.0f);
                    float panTrem = 0.18f * sinf(2.0f * PI_F * 4.2f * sec);
                    leadL = body * (0.82f + panTrem) * lEnv * 0.26f;
                    leadR = body * (0.82f - panTrem) * lEnv * 0.26f;
                } else if (spec.patch == PATCH_ACID_CYBER_LEAD) {
                    float s1 = OscSaw(leadPh1, fActual);
                    float s2 = OscSaw(leadPh2, fActual * 1.008f);
                    float cut = 900.0f + 4200.0f * FastDecay(u * 4.5f);
                    leadFltL.Step(s1, cut, 3.4f);
                    leadFltR.Step(s2, cut * 1.05f, 3.4f);
                    leadL = FastTanh((leadFltL.low + leadFltL.band * 0.55f) * 1.7f) * lEnv * 0.27f;
                    leadR = FastTanh((leadFltR.low + leadFltR.band * 0.55f) * 1.7f) * lEnv * 0.27f;
                } else if (spec.patch == PATCH_STADIUM_ORGAN) {
                    leadPh1 += 2.0f * PI_F * fActual / SAMPLE_RATE;
                    float leslie = 0.16f * sinf(2.0f * PI_F * 6.8f * sec);
                    float org = sinf(leadPh1) * 0.50f + sinf(leadPh1 * 2.0f) * 0.34f +
                                sinf(leadPh1 * 3.0f) * 0.24f + sinf(leadPh1 * 4.0f) * 0.14f;
                    leadL = FastTanh(org * 1.4f) * (0.85f + leslie) * lEnv * 0.26f;
                    leadR = FastTanh(org * 1.4f) * (0.85f - leslie) * lEnv * 0.26f;
                } else if (spec.patch == PATCH_KLEZMER_ELECTRIC || spec.patch == PATCH_HIJAZ_ELECTRIC) {
                    float reed = OscPulse(leadPh1, fActual, 0.32f) * 0.65f + OscSaw(leadPh2, fActual * 1.003f) * 0.35f;
                    leadFltL.Step(reed, 2450.0f, 2.6f);
                    leadFltR.Step(reed, 2600.0f, 2.6f);
                    leadL = FastTanh((leadFltL.low * 0.6f + leadFltL.band * 0.65f) * 1.8f) * lEnv * 0.27f;
                    leadR = FastTanh((leadFltR.low * 0.6f + leadFltR.band * 0.65f) * 1.8f) * lEnv * 0.27f;
                } else if (spec.patch == PATCH_SHRED_HARPSICHORD || spec.patch == PATCH_INDUSTRIAL_SOVIET) {
                    float plkL = OscPulse(leadPh1, fActual, 0.22f) * 0.6f + OscSaw(leadPh2, fActual * 2.0f) * FastDecay(u * 7.0f) * 0.4f;
                    float plkR = OscPulse(leadPh3, fActual * 1.005f, 0.28f) * 0.6f + plkL * 0.4f;
                    leadFltL.Step(plkL, 4200.0f * FastDecay(u * 2.5f) + 950.0f, 1.9f);
                    leadFltR.Step(plkR, 4400.0f * FastDecay(u * 2.5f) + 980.0f, 1.9f);
                    leadL = leadFltL.low * lEnv * 0.28f;
                    leadR = leadFltR.low * lEnv * 0.28f;
                } else {

                    float bL = OscSaw(leadPh1, fActual * 0.997f) * 0.6f + (isClimax ? OscSaw(leadPh3, fActual * 2.0f) * 0.32f : 0.0f);
                    float bR = OscSaw(leadPh2, fActual * 1.004f) * 0.6f + (isClimax ? bL * 0.30f : 0.0f);
                    float brassCut = 800.0f + 3400.0f * (1.0f - FastDecay(u * 14.0f)) * FastDecay(u * 1.2f);
                    leadFltL.Step(bL, brassCut, 1.85f);
                    leadFltR.Step(bR, brassCut * 1.04f, 1.85f);
                    leadL = FastTanh(leadFltL.low * 1.5f) * lEnv * 0.29f;
                    leadR = FastTanh(leadFltR.low * 1.5f) * lEnv * 0.29f;
                }

                sendL[stepStart + s] += leadL * 0.34f;
                sendR[stepStart + s] += leadR * 0.34f;
            }

            int idx = stepStart + s;
            dryL[idx] += drumL + bassMono + riffL + leadL;
            dryR[idx] += drumR + bassMono + riffR + leadR;
        }
    }

    int delayFrames = samplesPerStep * 3;
    for (int pass = 0; pass < 2; pass++) {
        for (int i = 0; i < totalFrames; i++) {
            int src = i - delayFrames;
            if (src < 0) src += totalFrames;
            sendL[i] = std::clamp(sendL[i] + sendR[src] * 0.34f, -1.5f, 1.5f);
            sendR[i] = std::clamp(sendR[i] + sendL[src] * 0.32f, -1.5f, 1.5f);
        }
    }

    for (int i = 0; i < totalFrames; i++) {
        float mixL = dryL[i] + sendL[i] * 0.42f;
        float mixR = dryR[i] + sendR[i] * 0.42f;

        float outL = FastTanh(mixL * 0.88f) * 30200.0f;
        float outR = FastTanh(mixR * 0.88f) * 30200.0f;
        m_bgmTracks[t][(size_t)i * 2 + 0] = (int16_t)std::clamp(outL, -32000.0f, 32000.0f);
        m_bgmTracks[t][(size_t)i * 2 + 1] = (int16_t)std::clamp(outR, -32000.0f, 32000.0f);
    }
}

void AudioEngine::SynthesizeBgmTracks() {

    SynthesizeSingleBgmTrack(BGM_MENU_SPLASH);
    SynthesizeSingleBgmTrack(BGM_CHAR_SELECT);
}

void AudioEngine::BgmAudioCallback(void* bufferData, unsigned int frames) {
    int16_t* out = (int16_t*)bufferData;
    int track = s_currentTrack.load(std::memory_order_relaxed);

    if (s_trackDataPtr != nullptr && track >= 0 && track < BGM_TRACK_COUNT && !s_trackDataPtr[track].empty()) {
        const auto& pcm = s_trackDataPtr[track];
        size_t totalFrames = pcm.size() / 2;
        size_t pos = s_trackPlayhead.load(std::memory_order_relaxed);

        bool fast = s_ceCriticalTempo.load(std::memory_order_relaxed);
        static unsigned int fracAcc = 0;
        for (unsigned int i = 0; i < frames; i++) {
            if (pos >= totalFrames) pos = 0;
            out[i * 2 + 0] = pcm[pos * 2 + 0];
            out[i * 2 + 1] = pcm[pos * 2 + 1];
            if (fast) {
                fracAcc += 5;
                pos += (fracAcc >> 2);
                fracAcc &= 3;
            } else {
                pos++;
            }
        }
        s_trackPlayhead.store(pos, std::memory_order_relaxed);
    } else {
        memset(bufferData, 0, (size_t)frames * 2 * sizeof(int16_t));
    }
}

void AudioEngine::Init() {
    if (m_initialized) return;
    InitAudioDevice();

    SynthesizeAll();
    LoadAnnouncerVoices();
    SynthesizeBgmTracks();

    s_trackDataPtr = m_bgmTracks;
    s_currentTrack.store(BGM_MENU_SPLASH, std::memory_order_release);
    s_trackPlayhead.store(0, std::memory_order_relaxed);

    m_bgmStream = LoadAudioStream(SAMPLE_RATE, 16, 2);
    SetAudioStreamCallback(m_bgmStream, BgmAudioCallback);
    PlayAudioStream(m_bgmStream);
    m_streamActive = true;

    m_initialized = true;
}

void AudioEngine::Shutdown() {
    if (!m_initialized) return;
    s_currentTrack.store(-1, std::memory_order_release);
    if (m_streamActive) {
        StopAudioStream(m_bgmStream);
        UnloadAudioStream(m_bgmStream);
        m_streamActive = false;
    }
    s_trackDataPtr = nullptr;
    std::vector<void*> freedBuffers;
    freedBuffers.reserve(SFX_COUNT);
    for (int i = 0; i < SFX_COUNT; i++) {
        void* buf = m_sounds[i].stream.buffer;
        if (m_sounds[i].frameCount > 0 && buf != nullptr) {
            if (std::find(freedBuffers.begin(), freedBuffers.end(), buf) == freedBuffers.end()) {
                freedBuffers.push_back(buf);
                UnloadSound(m_sounds[i]);
            }
        }
        m_sounds[i] = Sound{};
    }
    CloseAudioDevice();
    m_initialized = false;
}

void AudioEngine::PlaySfx(SfxId id, float pitch, float volume) {
    if (!m_initialized || id < 0 || id >= SFX_COUNT) return;
    if (m_sounds[id].frameCount == 0) return;
    SetSoundPitch(m_sounds[id], pitch);
    SetSoundVolume(m_sounds[id], volume);
    SetSoundPan(m_sounds[id], 0.5f);
    ::PlaySound(m_sounds[id]);
}

void AudioEngine::PlaySfxPan(SfxId id, float pitch, float volume, float screenX) {
    if (!m_initialized || id < 0 || id >= SFX_COUNT) return;
    if (m_sounds[id].frameCount == 0) return;
    float normX = std::clamp(screenX / (float)STAGE_TOTAL_WIDTH, 0.0f, 1.0f);
    float pan = std::clamp(0.82f - normX * 0.64f, 0.18f, 0.82f);
    SetSoundPitch(m_sounds[id], pitch);
    SetSoundVolume(m_sounds[id], volume);
    SetSoundPan(m_sounds[id], pan);
    ::PlaySound(m_sounds[id]);
}

void AudioEngine::SetBgmTrack(int trackId) {
    SetCriticalTempo(false);
    if (trackId >= 0 && trackId < BGM_TRACK_COUNT) {
        if (m_bgmTracks[trackId].empty()) {
            SynthesizeSingleBgmTrack(trackId);
        }
        if (s_currentTrack.load(std::memory_order_relaxed) != trackId) {
            s_trackPlayhead.store(0, std::memory_order_relaxed);
            s_currentTrack.store(trackId, std::memory_order_release);
        }
    }
}

void AudioEngine::StopBgm() {
    SetCriticalTempo(false);
    s_currentTrack.store(-1, std::memory_order_release);
    s_trackPlayhead.store(0, std::memory_order_relaxed);
}

void AudioEngine::Update() {
}
