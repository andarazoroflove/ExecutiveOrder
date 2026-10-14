#pragma once
#include "Common.h"
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
