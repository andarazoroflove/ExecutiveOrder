#pragma once
#include "Common.h"
#include <vector>
#include <atomic>

enum BgmTrackId {
    BGM_MENU_SPLASH = 0,
    BGM_STAGE_BASE = 1,
    BGM_CHAR_SELECT = 25,
    BGM_US_MAP = 26,
    BGM_WORLD_CONFLICT_MAP = 27,
    BGM_BONUS_CAR = 28,
    BGM_VICTORY_SCREEN = 29,
    BGM_BONUS_POOL = 30,
    BGM_BONUS_BOOKS = 31,
    BGM_BONUS_BERLIN = 32,
    BGM_TRACK_COUNT = 33
};

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
    SFX_VOICE_ROUND3,
    SFX_VOICE_FINAL_ROUND,
    SFX_VOICE_FIGHT,
    SFX_VOICE_FINISH_HIM,
    SFX_VOICE_VETOED,
    SFX_VOICE_YOU_WIN,
    SFX_VOICE_PERFECT,
    SFX_VOICE_TOTAL_VETO,
    SFX_VOICE_SUPER_VETO,
    SFX_VOICE_TITLE,
    SFX_VOICE_GLOBAL_CONFLICT,

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

    SFX_SUPER_FREEZE,
    SFX_DASH,
    SFX_THROW_TECH,
    SFX_SHAWARMA_TOSS,
    SFX_SHAKIRA_SHAKE,
    SFX_COUCH_SPRING,
    SFX_WHEELCHAIR_ROLL,
    SFX_ATM_JACKPOT,
    SFX_LASER_BEAM,
    SFX_SUPREME_WAVE,
    SFX_HAMMER_CLANG,
    SFX_HORSE_GALLOP,
    SFX_TRAPDOOR,
    SFX_CROWD_CHEER,

    SFX_DICK_LAUGH,
    SFX_SHOTGUN_BLAST,
    SFX_PAPER_PLANE,
    SFX_JENGA_CRASH,
    SFX_GOLF_DRIVE,
    SFX_SPY_RADIO,
    SFX_HORMUZ_SIREN,

    SFX_TAR_SPLAT,
    SFX_WATER_SPLASH,
    SFX_STEAM_HISS,
    SFX_WALL_CRUMBLE,

    SFX_CALL_TO_PRAYER,
    SFX_FINISHER_DRAMATIC,
    SFX_FINISHER_OBLITERATE,
    SFX_COUNT
};

class AudioEngine {
public:
    static AudioEngine& Instance();

    void Init();
    void Shutdown();

    void PlaySfx(SfxId id, float pitch = 1.0f, float volume = 1.0f);
    void PlaySfxPan(SfxId id, float pitch, float volume, float screenX);
    void SetBgmTrack(int trackId);
    static int GetStageBgmTrack(StageId stageId) { return BGM_STAGE_BASE + (int)stageId; }
    void StopBgm();
    void Update();
    static void SetCriticalTempo(bool critical);

private:
    AudioEngine() = default;
    ~AudioEngine() = default;

    void LoadAnnouncerVoices();
    void SynthesizeAll();
    void SynthesizeSingleBgmTrack(int trackId);
    void SynthesizeBgmTracks();
    Sound BuildWaveformSound(int sampleRate, int frameCount, const int16_t* pcmData, int channels = 1);
    Sound BuildStereoSound(int frameCount, const int16_t* interleavedStereo);

    static void BgmAudioCallback(void* bufferData, unsigned int frames);

    bool m_initialized = false;
    Sound m_sounds[SFX_COUNT]{};

    std::vector<int16_t> m_bgmTracks[BGM_TRACK_COUNT];
    AudioStream m_bgmStream{};
    bool m_streamActive = false;

    static std::atomic<int> s_currentTrack;
    static std::atomic<size_t> s_trackPlayhead;
    static std::vector<int16_t>* s_trackDataPtr;
};
