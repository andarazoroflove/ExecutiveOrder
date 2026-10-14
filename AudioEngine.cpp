#include "AudioEngine.h"
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <algorithm>

static constexpr int SAMPLE_RATE = 22050;

std::atomic<int> AudioEngine::s_currentTrack{ -1 };
std::atomic<size_t> AudioEngine::s_trackPlayhead{ 0 };
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

        for (unsigned int i = 0; i < frames; i++) {
            if (pos >= sz) pos = 0;
            out[i] = pcm[pos];
            pos++;
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
