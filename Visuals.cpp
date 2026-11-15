#include "Visuals.h"
#include "AssetManager.h"
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
Texture2D VisualsEngine::s_strips[CHARACTER_COUNT][STRIP_CLIP_COUNT] = { 0 };
Texture2D VisualsEngine::s_stripsHillary[STRIP_CLIP_COUNT] = { 0 };
Texture2D VisualsEngine::s_stripsRoberts[STRIP_CLIP_COUNT] = { 0 };
Texture2D VisualsEngine::s_stripsSotomayor[STRIP_CLIP_COUNT] = { 0 };
Texture2D VisualsEngine::s_stripAbdulSpec2 = { 0 };
Texture2D VisualsEngine::s_stripPaleMothVictory = { 0 };
Texture2D VisualsEngine::s_stripWVictory = { 0 };
Texture2D VisualsEngine::s_stripDickLaugh = { 0 };
Texture2D VisualsEngine::s_stripAyatollahPrayer = { 0 };
Texture2D VisualsEngine::s_projSprites[16] = { 0 };
Texture2D VisualsEngine::s_finisherProps[16] = { 0 };
Texture2D VisualsEngine::s_vfxSprites[16] = { 0 };
Texture2D VisualsEngine::s_worldProps[16] = { 0 };
Texture2D VisualsEngine::s_arabicTaunts[3] = { { 0 }, { 0 }, { 0 } };
Texture2D VisualsEngine::s_mapWorldConflict = { 0 };
Texture2D VisualsEngine::s_propShawarma = { 0 };
Texture2D VisualsEngine::s_propWheelchair = { 0 };
Texture2D VisualsEngine::s_propAtmHack = { 0 };
Texture2D VisualsEngine::s_propHammerSickle = { 0 };
Texture2D VisualsEngine::s_teslaSprites[8] = { 0 };
Texture2D VisualsEngine::s_bonusStageBgs[BONUS_STAGE_COUNT] = { 0 };
Texture2D VisualsEngine::s_bonusTarGun = { 0 };
Texture2D VisualsEngine::s_bonusPoolCrack = { 0 };
Texture2D VisualsEngine::s_bonusTarSplat = { 0 };
Texture2D VisualsEngine::s_bonusTarPatch = { 0 };
Texture2D VisualsEngine::s_bonusVillainStrips[3] = { 0 };
Texture2D VisualsEngine::s_bonusWaterBucket = { 0 };
Texture2D VisualsEngine::s_bonusBookPiles[4] = { 0 };
Texture2D VisualsEngine::s_bonusBerlinWall[4] = { 0 };
Texture2D VisualsEngine::s_bonusEastGermans[4] = { 0 };
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

static std::string s_visualsBaseDir = "assets/";
static const char* const kStripClipNames[VisualsEngine::STRIP_CLIP_COUNT] = {
    "idle", "walk", "jump", "crouch", "block",
    "lp", "hp", "lk", "hk", "spec", "hit", "taunt"
};

#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
static bool s_charStripsLoaded[CHARACTER_COUNT] = { false };
static int s_lruChars[4] = { -1, -1, -1, -1 };

static void UnloadCharacterStripsImpl(int cid, Texture2D strips[CHARACTER_COUNT][VisualsEngine::STRIP_CLIP_COUNT],
                                      Texture2D stripsHillary[VisualsEngine::STRIP_CLIP_COUNT],
                                      Texture2D stripsRoberts[VisualsEngine::STRIP_CLIP_COUNT],
                                      Texture2D stripsSotomayor[VisualsEngine::STRIP_CLIP_COUNT],
                                      Texture2D& stripAbdulSpec2, Texture2D& stripPaleMothVictory,
                                      Texture2D& stripWVictory, Texture2D& stripDickLaugh,
                                      Texture2D& stripAyatollahPrayer) {
    if (cid < 0 || cid >= CHARACTER_COUNT || !s_charStripsLoaded[cid]) return;
    for (int c = 0; c < VisualsEngine::STRIP_CLIP_COUNT; c++) {
        if (strips[cid][c].id > 0) { UnloadTexture(strips[cid][c]); strips[cid][c] = { 0 }; }
    }
    if (cid == CHAR_BILLARY) {
        for (int c = 0; c < VisualsEngine::STRIP_CLIP_COUNT; c++) {
            if (stripsHillary[c].id > 0) { UnloadTexture(stripsHillary[c]); stripsHillary[c] = { 0 }; }
        }
    } else if (cid == CHAR_HIGH_BENCH) {
        for (int c = 0; c < VisualsEngine::STRIP_CLIP_COUNT; c++) {
            if (stripsRoberts[c].id > 0) { UnloadTexture(stripsRoberts[c]); stripsRoberts[c] = { 0 }; }
            if (stripsSotomayor[c].id > 0) { UnloadTexture(stripsSotomayor[c]); stripsSotomayor[c] = { 0 }; }
        }
    } else if (cid == CHAR_DOCTOR_ABDUL && stripAbdulSpec2.id > 0) {
        UnloadTexture(stripAbdulSpec2); stripAbdulSpec2 = { 0 };
    } else if (cid == CHAR_PALE_MOTH && stripPaleMothVictory.id > 0) {
        UnloadTexture(stripPaleMothVictory); stripPaleMothVictory = { 0 };
    } else if (cid == CHAR_W && stripWVictory.id > 0) {
        UnloadTexture(stripWVictory); stripWVictory = { 0 };
    } else if (cid == CHAR_DICK && stripDickLaugh.id > 0) {
        UnloadTexture(stripDickLaugh); stripDickLaugh = { 0 };
    } else if (cid == CHAR_AYATOLLAH && stripAyatollahPrayer.id > 0) {
        UnloadTexture(stripAyatollahPrayer); stripAyatollahPrayer = { 0 };
    }
    s_charStripsLoaded[cid] = false;
}

static void LoadCharacterStripsImpl(int cid, Texture2D strips[CHARACTER_COUNT][VisualsEngine::STRIP_CLIP_COUNT],
                                    Texture2D stripsHillary[VisualsEngine::STRIP_CLIP_COUNT],
                                    Texture2D stripsRoberts[VisualsEngine::STRIP_CLIP_COUNT],
                                    Texture2D stripsSotomayor[VisualsEngine::STRIP_CLIP_COUNT],
                                    Texture2D& stripAbdulSpec2, Texture2D& stripPaleMothVictory,
                                    Texture2D& stripWVictory, Texture2D& stripDickLaugh,
                                    Texture2D& stripAyatollahPrayer) {
    if (cid < 0 || cid >= CHARACTER_COUNT) return;

    int existingSlot = -1;
    for (int i = 0; i < 4; i++) {
        if (s_lruChars[i] == cid) { existingSlot = i; break; }
    }
    if (existingSlot >= 0) {
        for (int i = existingSlot; i > 0; i--) s_lruChars[i] = s_lruChars[i - 1];
        s_lruChars[0] = cid;
    } else {
        int evicted = s_lruChars[3];
        if (evicted >= 0 && evicted != cid) {
            UnloadCharacterStripsImpl(evicted, strips, stripsHillary, stripsRoberts, stripsSotomayor,
                                      stripAbdulSpec2, stripPaleMothVictory, stripWVictory, stripDickLaugh, stripAyatollahPrayer);
        }
        for (int i = 3; i > 0; i--) s_lruChars[i] = s_lruChars[i - 1];
        s_lruChars[0] = cid;
    }

    if (s_charStripsLoaded[cid]) return;

    auto loadPoint = [](const char* p, Texture2D& outTex) {
        if (outTex.id == 0 && AssetManager::FileExists(p)) {
            outTex = AssetManager::LoadTexture(p);
            if (outTex.id > 0) SetTextureFilter(outTex, TEXTURE_FILTER_POINT);
        }
    };

    for (int c = 0; c < VisualsEngine::STRIP_CLIP_COUNT; c++) {
        char path[160];
        snprintf(path, sizeof(path), "%sstrip_%d_%s.png", s_visualsBaseDir.c_str(), cid, kStripClipNames[c]);
        loadPoint(path, strips[cid][c]);
    }
    if (cid == CHAR_BILLARY) {
        for (int c = 0; c < VisualsEngine::STRIP_CLIP_COUNT; c++) {
            char path[160];
            snprintf(path, sizeof(path), "%sstrip_13_hil_%s.png", s_visualsBaseDir.c_str(), kStripClipNames[c]);
            loadPoint(path, stripsHillary[c]);
        }
    } else if (cid == CHAR_HIGH_BENCH) {
        for (int c = 0; c < VisualsEngine::STRIP_CLIP_COUNT; c++) {
            char path[160];
            snprintf(path, sizeof(path), "%sstrip_1_roberts_%s.png", s_visualsBaseDir.c_str(), kStripClipNames[c]);
            loadPoint(path, stripsRoberts[c]);
            snprintf(path, sizeof(path), "%sstrip_1_sotomayor_%s.png", s_visualsBaseDir.c_str(), kStripClipNames[c]);
            loadPoint(path, stripsSotomayor[c]);
        }
    } else if (cid == CHAR_DOCTOR_ABDUL) {
        loadPoint((s_visualsBaseDir + "strip_17_spec2.png").c_str(), stripAbdulSpec2);
    } else if (cid == CHAR_PALE_MOTH) {
        loadPoint((s_visualsBaseDir + "strip_23_victory.png").c_str(), stripPaleMothVictory);
    } else if (cid == CHAR_W) {
        loadPoint((s_visualsBaseDir + "strip_26_victory.png").c_str(), stripWVictory);
    } else if (cid == CHAR_DICK) {
        loadPoint((s_visualsBaseDir + "strip_27_laugh.png").c_str(), stripDickLaugh);
    } else if (cid == CHAR_AYATOLLAH) {
        loadPoint((s_visualsBaseDir + "strip_28_prayer.png").c_str(), stripAyatollahPrayer);
    }
    s_charStripsLoaded[cid] = true;
}
#endif

void VisualsEngine::PrefetchMatchFighters(int p1Char, int p2Char) {
#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
    if (!s_loaded) return;
    for (int c = 0; c < CHARACTER_COUNT; c++) {
        if (c != p1Char && c != p2Char && s_charStripsLoaded[c]) {
            UnloadCharacterStripsImpl(c, s_strips, s_stripsHillary, s_stripsRoberts, s_stripsSotomayor,
                                      s_stripAbdulSpec2, s_stripPaleMothVictory, s_stripWVictory, s_stripDickLaugh, s_stripAyatollahPrayer);
        }
    }
    for (int i = 0; i < 4; i++) s_lruChars[i] = -1;
    if (p1Char >= 0 && p1Char < CHARACTER_COUNT) {
        LoadCharacterStripsImpl(p1Char, s_strips, s_stripsHillary, s_stripsRoberts, s_stripsSotomayor,
                                s_stripAbdulSpec2, s_stripPaleMothVictory, s_stripWVictory, s_stripDickLaugh, s_stripAyatollahPrayer);
    }
    if (p2Char >= 0 && p2Char < CHARACTER_COUNT && p2Char != p1Char) {
        LoadCharacterStripsImpl(p2Char, s_strips, s_stripsHillary, s_stripsRoberts, s_stripsSotomayor,
                                s_stripAbdulSpec2, s_stripPaleMothVictory, s_stripWVictory, s_stripDickLaugh, s_stripAyatollahPrayer);
    }
#else
    (void)p1Char;
    (void)p2Char;
#endif
}

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

    s_visualsBaseDir = baseDir;

    std::string splashPath = baseDir + "title_splash.png";
    if (AssetManager::FileExists(splashPath.c_str())) {
        s_titleSplash = AssetManager::LoadTexture(splashPath.c_str());
        SetTextureFilter(s_titleSplash, TEXTURE_FILTER_BILINEAR);
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

    std::string ssPath = baseDir + "spr_8_secret_service.png";
    if (AssetManager::FileExists(ssPath.c_str())) {
        s_sprSleepyJoeSS = AssetManager::LoadTexture(ssPath.c_str());
        SetTextureFilter(s_sprSleepyJoeSS, TEXTURE_FILTER_POINT);
    }

#if !defined(GRAPHICS_API_OPENGL_ES2) && !defined(__aarch64__)
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

    std::string cthulhuThrowPath = baseDir + "ending_cthulhu_throw.png";
    if (AssetManager::FileExists(cthulhuThrowPath.c_str())) {
        s_endingCthulhuThrow = AssetManager::LoadTexture(cthulhuThrowPath.c_str());
        SetTextureFilter(s_endingCthulhuThrow, TEXTURE_FILTER_BILINEAR);
    }
    std::string laserSharkPath = baseDir + "ending_laser_shark.png";
    if (AssetManager::FileExists(laserSharkPath.c_str())) {
        s_endingLaserShark = AssetManager::LoadTexture(laserSharkPath.c_str());
        SetTextureFilter(s_endingLaserShark, TEXTURE_FILTER_BILINEAR);
    }
    std::string cthulhuNapPath = baseDir + "ending_cthulhu_nap.png";
    if (AssetManager::FileExists(cthulhuNapPath.c_str())) {
        s_endingCthulhuNap = AssetManager::LoadTexture(cthulhuNapPath.c_str());
        SetTextureFilter(s_endingCthulhuNap, TEXTURE_FILTER_BILINEAR);
    }
#endif

    auto loadTexPoint = [&](const std::string& rel, Texture2D& dst) {
        std::string full = baseDir + rel;
        if (AssetManager::FileExists(full.c_str())) {
            dst = AssetManager::LoadTexture(full.c_str());
            SetTextureFilter(dst, TEXTURE_FILTER_POINT);
        }
    };
    auto loadTexBilinear = [&](const std::string& rel, Texture2D& dst) {
        std::string full = baseDir + rel;
        if (AssetManager::FileExists(full.c_str())) {
            dst = AssetManager::LoadTexture(full.c_str());
            SetTextureFilter(dst, TEXTURE_FILTER_BILINEAR);
        }
    };

    for (int i = 0; i < 8; i++) {
        char fname[64];
        snprintf(fname, sizeof(fname), "tesla_%d.png", i);
        loadTexPoint(fname, s_teslaSprites[i]);
    }

#if !defined(GRAPHICS_API_OPENGL_ES2) && !defined(__aarch64__)
    loadTexBilinear("stage_bonus_tesla.png",       s_bonusStageBgs[BONUS_STAGE_TESLA]);
    loadTexBilinear("stage_bonus_pool_tar.png",    s_bonusStageBgs[BONUS_STAGE_POOL_TAR]);
    loadTexBilinear("stage_bonus_book_fire.png",   s_bonusStageBgs[BONUS_STAGE_BOOK_FIRE]);
    loadTexBilinear("stage_bonus_berlin_wall.png", s_bonusStageBgs[BONUS_STAGE_BERLIN_WALL]);
#endif

    loadTexPoint("bonus_tar_gun.png",      s_bonusTarGun);
    loadTexPoint("bonus_pool_crack.png",   s_bonusPoolCrack);
    loadTexPoint("bonus_tar_splat.png",    s_bonusTarSplat);
    loadTexPoint("bonus_tar_patch.png",    s_bonusTarPatch);

    loadTexPoint("bonus_strip_miller.png", s_bonusVillainStrips[0]);
    loadTexPoint("bonus_strip_barr.png",   s_bonusVillainStrips[1]);
    loadTexPoint("bonus_strip_bannon.png", s_bonusVillainStrips[2]);
    loadTexPoint("bonus_water_bucket.png", s_bonusWaterBucket);

    for (int i = 0; i < 4; i++) {
        char fname[64];
        snprintf(fname, sizeof(fname), "bonus_book_pile_%d.png", i);
        loadTexPoint(fname, s_bonusBookPiles[i]);
        snprintf(fname, sizeof(fname), "bonus_berlin_wall_%d.png", i);
        loadTexPoint(fname, s_bonusBerlinWall[i]);
        snprintf(fname, sizeof(fname), "bonus_east_german_%d.png", i);
        loadTexPoint(fname, s_bonusEastGermans[i]);
    }

    std::string mapPath = baseDir + "map_east_coast.png";
    if (AssetManager::FileExists(mapPath.c_str())) {
        s_mapEastCoast = AssetManager::LoadTexture(mapPath.c_str());
        SetTextureFilter(s_mapEastCoast, TEXTURE_FILTER_BILINEAR);
    }

    auto loadTexIfPresent = [](const char* p, Texture2D& outTex) {
        if (AssetManager::FileExists(p)) {
            outTex = AssetManager::LoadTexture(p);
            if (outTex.id > 0) SetTextureFilter(outTex, TEXTURE_FILTER_POINT);
        }
    };

#if !defined(GRAPHICS_API_OPENGL_ES2) && !defined(__aarch64__)
    std::string endingPath = baseDir + "ending_savior.png";
    if (AssetManager::FileExists(endingPath.c_str())) {
        s_endingSavior = AssetManager::LoadTexture(endingPath.c_str());
        SetTextureFilter(s_endingSavior, TEXTURE_FILTER_BILINEAR);
    }

    for (int i = 0; i < CHARACTER_COUNT; i++) {
        for (int c = 0; c < STRIP_CLIP_COUNT; c++) {
            char path[160];
            snprintf(path, sizeof(path), "%sstrip_%d_%s.png", baseDir.c_str(), i, kStripClipNames[c]);
            loadTexIfPresent(path, s_strips[i][c]);
        }
    }
    for (int c = 0; c < STRIP_CLIP_COUNT; c++) {
        char path[160];
        snprintf(path, sizeof(path), "%sstrip_13_hil_%s.png", baseDir.c_str(), kStripClipNames[c]);
        loadTexIfPresent(path, s_stripsHillary[c]);
        snprintf(path, sizeof(path), "%sstrip_1_roberts_%s.png", baseDir.c_str(), kStripClipNames[c]);
        loadTexIfPresent(path, s_stripsRoberts[c]);
        snprintf(path, sizeof(path), "%sstrip_1_sotomayor_%s.png", baseDir.c_str(), kStripClipNames[c]);
        loadTexIfPresent(path, s_stripsSotomayor[c]);
    }
    loadTexIfPresent((baseDir + "strip_17_spec2.png").c_str(), s_stripAbdulSpec2);
    loadTexIfPresent((baseDir + "strip_23_victory.png").c_str(), s_stripPaleMothVictory);
    loadTexIfPresent((baseDir + "strip_26_victory.png").c_str(), s_stripWVictory);
    loadTexIfPresent((baseDir + "strip_27_laugh.png").c_str(), s_stripDickLaugh);
    loadTexIfPresent((baseDir + "strip_28_prayer.png").c_str(), s_stripAyatollahPrayer);
#else
    for (int i = 0; i < CHARACTER_COUNT; i++) s_charStripsLoaded[i] = false;
    for (int i = 0; i < 4; i++) s_lruChars[i] = -1;
#endif
    for (int p = 0; p < 16; p++) {
        char pp[160];
        snprintf(pp, sizeof(pp), "%sprop_proj_%d.png", baseDir.c_str(), p);
        loadTexIfPresent(pp, s_projSprites[p]);
        snprintf(pp, sizeof(pp), "%sfinisher_prop_%d.png", baseDir.c_str(), p);
        loadTexIfPresent(pp, s_finisherProps[p]);
        snprintf(pp, sizeof(pp), "%svfx_%d.png", baseDir.c_str(), p);
        loadTexIfPresent(pp, s_vfxSprites[p]);
        snprintf(pp, sizeof(pp), "%swprop_%d.png", baseDir.c_str(), p);
        loadTexIfPresent(pp, s_worldProps[p]);
    }
    for (int a = 0; a < 3; a++) {
        char arPath[160];
        snprintf(arPath, sizeof(arPath), "%staunt_arabic_%d.png", baseDir.c_str(), a);
        if (AssetManager::FileExists(arPath)) {
            s_arabicTaunts[a] = AssetManager::LoadTexture(arPath);
            if (s_arabicTaunts[a].id > 0) SetTextureFilter(s_arabicTaunts[a], TEXTURE_FILTER_BILINEAR);
        }
    }
    loadTexBilinear("map_world_conflict.png", s_mapWorldConflict);
    loadTexIfPresent((baseDir + "prop_shawarma_sub.png").c_str(), s_propShawarma);
    loadTexIfPresent((baseDir + "prop_wheelchair_rider.png").c_str(), s_propWheelchair);
    loadTexIfPresent((baseDir + "prop_atm_hack.png").c_str(), s_propAtmHack);
    loadTexIfPresent((baseDir + "prop_hammer_sickle.png").c_str(), s_propHammerSickle);

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

    for (int a = 0; a < 6; a++) {
        if (s_spritesHillary[a].id > 0 && s_spritesHillary[a].id != s_sprBillaryHillary.id) {
            UnloadTexture(s_spritesHillary[a]);
        }
        s_spritesHillary[a] = { 0 };
    }
    if (s_sprBillaryHillary.id > 0) {
        UnloadTexture(s_sprBillaryHillary);
        s_sprBillaryHillary = { 0 };
    }

    auto unloadTex = [](Texture2D& t) {
        if (t.id > 0) {
            UnloadTexture(t);
            t = { 0 };
        }
    };
    for (int i = 0; i < 8; i++) unloadTex(s_teslaSprites[i]);
    for (int i = 0; i < BONUS_STAGE_COUNT; i++) unloadTex(s_bonusStageBgs[i]);
    unloadTex(s_bonusTarGun);
    unloadTex(s_bonusPoolCrack);
    unloadTex(s_bonusTarSplat);
    unloadTex(s_bonusTarPatch);
    for (int i = 0; i < 3; i++) unloadTex(s_bonusVillainStrips[i]);
    unloadTex(s_bonusWaterBucket);
    for (int i = 0; i < 4; i++) {
        unloadTex(s_bonusBookPiles[i]);
        unloadTex(s_bonusBerlinWall[i]);
        unloadTex(s_bonusEastGermans[i]);
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

    for (int i = 0; i < CHARACTER_COUNT; i++) {
        for (int c = 0; c < STRIP_CLIP_COUNT; c++) {
            if (s_strips[i][c].id > 0) { UnloadTexture(s_strips[i][c]); s_strips[i][c] = { 0 }; }
        }
    }
    for (int c = 0; c < STRIP_CLIP_COUNT; c++) {
        if (s_stripsHillary[c].id > 0) { UnloadTexture(s_stripsHillary[c]); s_stripsHillary[c] = { 0 }; }
        if (s_stripsRoberts[c].id > 0) { UnloadTexture(s_stripsRoberts[c]); s_stripsRoberts[c] = { 0 }; }
        if (s_stripsSotomayor[c].id > 0) { UnloadTexture(s_stripsSotomayor[c]); s_stripsSotomayor[c] = { 0 }; }
    }
    if (s_stripAbdulSpec2.id > 0) { UnloadTexture(s_stripAbdulSpec2); s_stripAbdulSpec2 = { 0 }; }
    if (s_stripPaleMothVictory.id > 0) { UnloadTexture(s_stripPaleMothVictory); s_stripPaleMothVictory = { 0 }; }
    if (s_stripWVictory.id > 0) { UnloadTexture(s_stripWVictory); s_stripWVictory = { 0 }; }
    if (s_stripDickLaugh.id > 0) { UnloadTexture(s_stripDickLaugh); s_stripDickLaugh = { 0 }; }
    if (s_stripAyatollahPrayer.id > 0) { UnloadTexture(s_stripAyatollahPrayer); s_stripAyatollahPrayer = { 0 }; }
    for (int p = 0; p < 16; p++) {
        if (s_projSprites[p].id > 0) { UnloadTexture(s_projSprites[p]); s_projSprites[p] = { 0 }; }
        if (s_finisherProps[p].id > 0) { UnloadTexture(s_finisherProps[p]); s_finisherProps[p] = { 0 }; }
        if (s_vfxSprites[p].id > 0) { UnloadTexture(s_vfxSprites[p]); s_vfxSprites[p] = { 0 }; }
        if (s_worldProps[p].id > 0) { UnloadTexture(s_worldProps[p]); s_worldProps[p] = { 0 }; }
    }
    for (int a = 0; a < 3; a++) {
        if (s_arabicTaunts[a].id > 0) { UnloadTexture(s_arabicTaunts[a]); s_arabicTaunts[a] = { 0 }; }
    }
    if (s_mapWorldConflict.id > 0) { UnloadTexture(s_mapWorldConflict); s_mapWorldConflict = { 0 }; }
    if (s_propShawarma.id > 0) { UnloadTexture(s_propShawarma); s_propShawarma = { 0 }; }
    if (s_propWheelchair.id > 0) { UnloadTexture(s_propWheelchair); s_propWheelchair = { 0 }; }
    if (s_propAtmHack.id > 0) { UnloadTexture(s_propAtmHack); s_propAtmHack = { 0 }; }
    if (s_propHammerSickle.id > 0) { UnloadTexture(s_propHammerSickle); s_propHammerSickle = { 0 }; }

#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
    for (int i = 0; i < CHARACTER_COUNT; i++) s_charStripsLoaded[i] = false;
    for (int i = 0; i < 4; i++) s_lruChars[i] = -1;
#endif

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

const Texture2D& VisualsEngine::GetArabicTauntTexture(int idx) {
    static Texture2D s_empty = { 0 };
    if (idx < 0 || idx >= 3) return s_empty;
    return s_arabicTaunts[idx];
}

const Texture2D& VisualsEngine::GetVfxSprite(int idx) {
    static Texture2D s_empty = { 0 };
    if (idx < 0 || idx >= 16) return s_empty;
    return s_vfxSprites[idx];
}

const Texture2D& VisualsEngine::GetWorldProp(int idx) {
    static Texture2D s_empty = { 0 };
    if (idx < 0 || idx >= 16) return s_empty;
    return s_worldProps[idx];
}

bool VisualsEngine::DrawCenteredSprite(const Texture2D& tex, float cx, float cy, float w, float h, float rotDeg, Color tint, bool flipX) {
    if (tex.id == 0) return false;
    Rectangle src = { 0.0f, 0.0f, flipX ? -(float)tex.width : (float)tex.width, (float)tex.height };
    Rectangle dst = { cx, cy, w, h };
    DrawTexturePro(tex, src, dst, Vector2{ w * 0.5f, h * 0.5f }, rotDeg, tint);
    return true;
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
        float sz = puff.baseSize * 2.4f * (1.0f + prog * 1.5f);
        unsigned char alpha = (unsigned char)((1.0f - prog) * 230.0f);
        VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(10), puff.pos.x, puff.pos.y, sz, sz, prog * 35.0f, Color{ 195, 245, 95, alpha });
    }
}

void VisualsEngine::DrawFighterSprite(const Fighter& f) {
    int charId = (int)f.GetCharacterId();
    if (charId < 0 || charId >= CHARACTER_COUNT) charId = 0;

#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
    if (s_loaded && !s_charStripsLoaded[charId]) {
        LoadCharacterStripsImpl(charId, s_strips, s_stripsHillary, s_stripsRoberts, s_stripsSotomayor,
                                s_stripAbdulSpec2, s_stripPaleMothVictory, s_stripWVictory, s_stripDickLaugh, s_stripAyatollahPrayer);
    }
#endif

    Vector2 pos = f.GetPosition();
    bool fr = f.IsFacingRight();
    FighterState state = f.GetState();
    int frame = f.GetStateFrame();
    float dir = fr ? 1.0f : -1.0f;

    bool hasStrip = (s_loaded && s_strips[charId][STRIP_IDLE].id > 0);
    bool hasLegacySpr = (s_loaded && s_sprites[charId][0].id > 0);
    if (!ChampionshipEdition::s_forceProceduralRig && (hasStrip || hasLegacySpr)) {

        if (f.GetJengaTimer() > 0) {
            float prog = std::clamp(1.0f - (float)f.GetJengaTimer() / 68.0f, 0.0f, 1.0f);
            float tCollapse = std::max(0.0f, (prog - 0.12f) / 0.88f);
            for (int b = 0; b < 18; b++) {
                int layer = b / 3;
                int slot = b % 3;
                bool crossLayer = (layer % 2 == 1);
                float bw = crossLayer ? 16.0f : 28.0f;
                float bh = 12.0f;
                float stackX = pos.x + (crossLayer ? ((slot - 1) * 9.5f) : ((slot - 1) * 1.5f));
                float stackY = (GROUND_Y - 5.0f) - layer * 8.2f;

                float sideSign = ((b % 2 == 0) ? 1.0f : -1.0f) * (0.45f + (slot * 0.4f) + (layer * 0.18f));
                float scatterX = sideSign * 28.0f * tCollapse;
                float dropDist = (GROUND_Y - 5.0f - (b % 3) * 3.5f) - stackY;
                float fallT = std::min(1.0f, tCollapse * (1.35f + (5 - layer) * 0.12f));
                float curX = stackX + scatterX + ((prog < 0.12f) ? sinf(prog * 95.0f + layer) * 2.0f : 0.0f);
                float curY = stackY + dropDist * (fallT * fallT);
                float rot = sideSign * 68.0f * fallT;

                Color woodTint = (b % 3 == 0) ? WHITE : ((b % 3 == 1) ? Color{ 242, 228, 210, 255 } : Color{ 226, 206, 184, 255 });
                DrawCenteredSprite(s_worldProps[15], curX, curY, bw, bh, rot, woodTint, (b % 2 == 1));
            }
            if (prog < 0.65f) {
                DrawPixelText("JENGA!", (int)pos.x - 18, (int)(GROUND_Y - 66.0f + tCollapse * 18.0f), 8, Color{ 255, 230, 65, 255 });
            }
            ChampionshipEdition::DrawFighterWorldOverlays(f);
            DrawCosmeticPuffs(f);
            return;
        }

        if (charId == CHAR_THE_DICTATOR && f.IsSpiderHoleHidden()) {
            ChampionshipEdition::DrawFighterWorldOverlays(f);
            DrawCosmeticPuffs(f);
            return;
        }

        static const float s_charScale[CHARACTER_COUNT] = {
            88.0f / 258.0f,
            84.0f / 200.0f,
            80.0f / 228.0f,
            82.0f / 250.0f,
            80.0f / 253.0f,
            78.0f / 262.0f,
            82.0f / 305.0f,
            82.0f / 258.0f,
            86.0f / 265.0f,
            100.0f / 260.0f,
            88.0f / 264.0f,
            98.4f / 302.0f,
            84.0f / 272.0f,
            84.0f / 276.0f,
            86.0f / 295.0f,
            84.0f / 262.0f,
            86.0f / 268.0f,
            84.0f / 262.0f,
            83.0f / 262.0f,
            90.0f / 272.0f,
            86.0f / 266.0f,
            82.0f / 258.0f,
            84.0f / 262.0f,
            84.0f / 262.0f,
            85.0f / 262.0f,
            85.0f / 262.0f,
            84.0f / 262.0f,
            84.0f / 262.0f,
            86.0f / 262.0f
        };

        StripClipId clipId = STRIP_IDLE;
        int animIdx = 0;
        if (state == FSTATE_VICTORY) {
            clipId = STRIP_TAUNT;
        } else if (state == FSTATE_WALK_FWD || state == FSTATE_WALK_BACK || state == FSTATE_DASH_FWD || state == FSTATE_DASH_BACK) {
            clipId = STRIP_WALK;
        } else if (state == FSTATE_JUMP) {
            clipId = STRIP_JUMP;
        } else if (state == FSTATE_CROUCH) {
            clipId = STRIP_CROUCH;
        } else if (state == FSTATE_BLOCK_HIGH || state == FSTATE_BLOCK_LOW || state == FSTATE_BLOCKSTUN) {
            clipId = STRIP_BLOCK;
        } else if (state == FSTATE_HITSTUN || state == FSTATE_FINISH_HIM_DIZZY || state == FSTATE_FINISHER_VICTIM || state == FSTATE_KNOCKDOWN_AIR || state == FSTATE_KNOCKDOWN_GROUND || state == FSTATE_DEFEAT || state == FSTATE_WAKEUP) {
            clipId = STRIP_HIT;
        } else if (state == FSTATE_TAUNT || state == FSTATE_CALL_TO_PRAYER) {
            clipId = STRIP_TAUNT;
        } else if (state == FSTATE_FINISHER) {
            if (frame < 20) {
                clipId = STRIP_TAUNT;
            } else if (frame < 46 || charId == CHAR_FLOTUS) {
                clipId = STRIP_SPEC;
                animIdx = 5;
            } else {
                clipId = (charId == CHAR_MASCOT || charId == CHAR_POPULIST || charId == CHAR_COASTAL_GOV) ? STRIP_HK : STRIP_HP;
                animIdx = 2;
            }
        } else if (state == FSTATE_SPECIAL && f.IsSuperMoveActive()) {
            int sub = (frame / 4) % 3;
            clipId = (sub == 0) ? STRIP_SPEC : ((sub == 1) ? STRIP_HP : STRIP_HK);
            animIdx = 5;
        } else if (state == FSTATE_SPECIAL) {
            clipId = STRIP_SPEC;
            animIdx = 5;
        } else if (state == FSTATE_ATTACK) {
            AttackMoveType mtype = f.GetCurrentAttack().moveType;
            if (mtype == MOVE_THROW) { clipId = STRIP_HP; animIdx = 2; }
            else if (mtype == MOVE_LIGHT_PUNCH) { clipId = STRIP_LP; animIdx = 1; }
            else if (mtype == MOVE_HEAVY_PUNCH) { clipId = STRIP_HP; animIdx = 2; }
            else if (mtype == MOVE_LIGHT_KICK) { clipId = STRIP_LK; animIdx = 3; }
            else if (mtype == MOVE_HEAVY_KICK) { clipId = STRIP_HK; animIdx = 4; }
            else if (mtype == MOVE_SPECIAL || mtype == MOVE_SUPER) { clipId = STRIP_SPEC; animIdx = 5; }
            else { clipId = f.GetCurrentAttack().isHeavy ? STRIP_HP : STRIP_LP; animIdx = f.GetCurrentAttack().isHeavy ? 2 : 1; }
        }

        Texture2D tex = { 0 };
        bool usingStrip = false;
        bool isPaleMothHorse = false;
        bool isWGolfVictory = false;
        bool isDickCameraLaugh = false;

        if (hasStrip) {
            if (charId == CHAR_AYATOLLAH && state == FSTATE_CALL_TO_PRAYER && s_stripAyatollahPrayer.id > 0) {
                tex = s_stripAyatollahPrayer;
            } else if (charId == CHAR_DICK && f.IsLaughingAtCamera() && s_stripDickLaugh.id > 0 &&
                state != FSTATE_HITSTUN && state != FSTATE_KNOCKDOWN_AIR && state != FSTATE_KNOCKDOWN_GROUND && state != FSTATE_DEFEAT) {
                tex = s_stripDickLaugh;
                isDickCameraLaugh = true;
            } else if (charId == CHAR_W && state == FSTATE_VICTORY && s_stripWVictory.id > 0) {
                tex = s_stripWVictory;
                isWGolfVictory = true;
            } else if (charId == CHAR_PALE_MOTH && state == FSTATE_VICTORY && s_stripPaleMothVictory.id > 0) {
                tex = s_stripPaleMothVictory;
                isPaleMothHorse = true;
            } else if (charId == CHAR_DOCTOR_ABDUL && state == FSTATE_SPECIAL && f.GetSpecialIndex() == 2 && s_stripAbdulSpec2.id > 0) {
                tex = s_stripAbdulSpec2;
            } else if (charId == CHAR_HIGH_BENCH) {
                HighBenchStage hbStage = f.GetHighBenchStage();
                if (hbStage == HB_STAGE_ROBERTS && s_stripsRoberts[clipId].id > 0) tex = s_stripsRoberts[clipId];
                else if (hbStage == HB_STAGE_SOTOMAYOR && s_stripsSotomayor[clipId].id > 0) tex = s_stripsSotomayor[clipId];
                else tex = s_strips[charId][clipId];
            } else if (charId == CHAR_BILLARY && !fr && s_stripsHillary[clipId].id > 0) {
                tex = s_stripsHillary[clipId];
            } else {
                tex = s_strips[charId][clipId];
            }
            if (tex.id == 0) tex = s_strips[charId][STRIP_IDLE];
            if (tex.id > 0) usingStrip = true;
        }

        if (!usingStrip) {
            tex = s_sprites[charId][animIdx];
            if (charId == CHAR_HIGH_BENCH) {
                HighBenchStage hbStage = f.GetHighBenchStage();
                if (hbStage == HB_STAGE_ROBERTS) {
                    if (animIdx == 0 && s_sprHighBenchRoberts.id > 0) tex = s_sprHighBenchRoberts;
                    else if (s_sprHighBenchRobertsSpec.id > 0) tex = s_sprHighBenchRobertsSpec;
                } else if (hbStage == HB_STAGE_SOTOMAYOR) {
                    if (animIdx == 0 && s_sprHighBenchSotomayor.id > 0) tex = s_sprHighBenchSotomayor;
                    else if (s_sprHighBenchSotomayorSpec.id > 0) tex = s_sprHighBenchSotomayorSpec;
                }
            }
            if (charId == CHAR_BILLARY && !fr && s_spritesHillary[animIdx].id > 0) {
                tex = s_spritesHillary[animIdx];
            }
            if (tex.id == 0) tex = s_sprites[charId][0];
        }

        constexpr float CANVAS_W = 440.0f;
        constexpr float CANVAS_H = 360.0f;
        constexpr float ANCHOR_X = 180.0f;
        constexpr float BASELINE_Y = 320.0f;

        float baseScale = s_charScale[charId] * (isPaleMothHorse ? 1.12f : 1.0f);
        float drawW = CANVAS_W * baseScale;
        float drawH = CANVAS_H * baseScale;

        float posX = pos.x;
        float posY = pos.y;
        float rotation = 0.0f;

        if ((charId == CHAR_SLEEPY_JOE && state == FSTATE_SPECIAL) ||
            (charId == CHAR_COUCHIE && state == FSTATE_SPECIAL)) {
            posY = GROUND_Y + 3.0f;
        }
        if (isDickCameraLaugh) {
            posY -= ((f.GetLaughTimer() % 4) < 2) ? 1.5f : 0.0f;
        }

        if (!usingStrip) {
            if (state == FSTATE_IDLE) {
                posY += sinf(frame * 0.12f) * 1.5f;
            } else if (state == FSTATE_WALK_FWD || state == FSTATE_WALK_BACK) {
                posY -= fabsf(sinf(frame * 0.28f)) * 2.2f;
                rotation = (state == FSTATE_WALK_FWD ? 2.5f : -2.5f) * (fr ? 1.0f : -1.0f);
            } else if (state == FSTATE_TAUNT) {
                rotation = fr ? -4.5f : 4.5f;
                posY -= 1.5f;
            }
        }

        if (state == FSTATE_HITSTUN) {
            posX += (fr ? -1.0f : 1.0f) * 3.0f;
        } else if (state == FSTATE_FINISH_HIM_DIZZY) {
            posX += sinf(frame * 0.22f) * 2.5f;
        } else if (state == FSTATE_FINISHER_VICTIM) {
            posX += sinf(frame * 1.35f) * 3.2f;
        } else if (state == FSTATE_FINISHER && charId == CHAR_A_NEW_HOPE && frame >= 24 && frame <= 62) {
            float dunkT = (float)(frame - 24) / 38.0f;
            posY -= sinf(dunkT * PI_F) * 56.0f;
            posX += (fr ? 1.0f : -1.0f) * sinf(dunkT * PI_F) * 44.0f;
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

        Rectangle srcRec = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        if (usingStrip) {
            int numFrames = std::max(1, (int)(tex.width / 440));
            int frameIdx = 0;
            if (isDickCameraLaugh) {
                frameIdx = ((40 - std::clamp(f.GetLaughTimer(), 0, 40)) / 3) % numFrames;
            } else if (state == FSTATE_IDLE) {
                frameIdx = (frame / 6) % numFrames;
            } else if (state == FSTATE_WALK_FWD) {
                frameIdx = (frame / 4) % numFrames;
            } else if (state == FSTATE_WALK_BACK) {
                frameIdx = (numFrames - 1) - ((frame / 4) % numFrames);
            } else if (state == FSTATE_DASH_FWD || state == FSTATE_DASH_BACK) {
                frameIdx = (frame / 2) % numFrames;
            } else if (state == FSTATE_JUMP) {
                float vy = f.GetVelocity().y;
                if (vy < -2.2f) frameIdx = std::min(1, numFrames - 1);
                else if (vy <= 2.2f) frameIdx = std::min(2, numFrames - 1);
                else frameIdx = std::min(3, numFrames - 1);
            } else if (state == FSTATE_CROUCH || state == FSTATE_BLOCK_HIGH || state == FSTATE_BLOCK_LOW || state == FSTATE_BLOCKSTUN) {
                frameIdx = std::min(numFrames - 1, frame / 3);
            } else if (state == FSTATE_TAUNT || state == FSTATE_FINISH_HIM_DIZZY || state == FSTATE_FINISHER_VICTIM) {
                frameIdx = (frame / 6) % numFrames;
            } else if (state == FSTATE_CALL_TO_PRAYER) {
                frameIdx = (frame / 6) % numFrames;
            } else if (state == FSTATE_FINISHER) {
                frameIdx = (frame / 5) % numFrames;
            } else if (state == FSTATE_VICTORY) {

                if (isWGolfVictory) {
                    frameIdx = std::min(numFrames - 1, frame / 6);
                } else if (isPaleMothHorse && numFrames >= 4) {
                    int raw = frame / 8;
                    frameIdx = (raw < numFrames) ? raw : (2 + ((raw - numFrames) % (numFrames - 2)));
                } else {
                    frameIdx = (frame / 6) % numFrames;
                }
            } else if (state == FSTATE_ATTACK || state == FSTATE_SPECIAL) {
                int totalDur = std::max(8, f.GetCurrentAttack().startupFrames + f.GetCurrentAttack().activeFrames + f.GetCurrentAttack().recoveryFrames);
                if (state == FSTATE_SPECIAL && charId == CHAR_COUCHIE) {

                    frameIdx = (frame < 6) ? 0 : (1 + ((frame / 3) % std::max(1, numFrames - 1)));
                } else {
                    frameIdx = std::clamp((frame * numFrames) / totalDur, 0, numFrames - 1);
                }
            } else {
                frameIdx = std::min(numFrames - 1, frame / 4);
            }
            bool drawUnflipped = isDickCameraLaugh;
            srcRec = Rectangle{ frameIdx * CANVAS_W, 0.0f, (fr || drawUnflipped) ? CANVAS_W : -CANVAS_W, CANVAS_H };
        } else {
            if (!fr) srcRec.width = -srcRec.width;
        }

        Color tint = WHITE;
        if (f.IsInHitstop()) {
            if ((frame % 2) == 0) tint = Color{ 255, 255, 255, 255 };
        } else if (state == FSTATE_HITSTUN) {
            if (((frame / 2) % 2) == 0) tint = Color{ 255, 100, 100, 255 };
        } else if (state == FSTATE_FINISH_HIM_DIZZY || state == FSTATE_FINISHER_VICTIM) {
            if (((frame / 4) % 2) == 0) tint = Color{ 255, 210, 190, 255 };
        } else if (state == FSTATE_CALL_TO_PRAYER) {
            if (((frame / 3) % 2) == 0) tint = Color{ 235, 255, 210, 255 };
        } else if (state == FSTATE_FINISHER) {
            if (((frame / 2) % 2) == 0) tint = Color{ 255, 245, 190, 255 };
        } else if (state == FSTATE_BLOCK_HIGH || state == FSTATE_BLOCK_LOW || state == FSTATE_BLOCKSTUN) {
            tint = Color{ 190, 225, 255, 255 };
        } else if (f.IsSuperMoveActive()) {
            if (((frame / 2) % 2) == 0) tint = Color{ 255, 245, 175, 255 };
        }

        float crouchYScale = 1.0f;
        if (!usingStrip && f.IsCrouching()) {
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

        bool isDownOrKo = (f.GetHealth() <= 0 || state == FSTATE_KNOCKDOWN_AIR || state == FSTATE_KNOCKDOWN_GROUND || state == FSTATE_DEFEAT || state == FSTATE_WAKEUP);
        DrawTexturePro(tex, srcRec, dstRec, origin, rotation, tint);

        if (!isDownOrKo && f.GetTauntTimer() > 0 && !f.GetActiveTaunt().empty()) {
            const char* quote = f.GetActiveTaunt().c_str();
            float headTopY = (charId == CHAR_WAR_PIG || charId == CHAR_BLEXAS) ? (pos.y - 96.0f) : (pos.y - 90.0f);
            if (charId == CHAR_COUCHIE && state == FSTATE_SPECIAL) headTopY = pos.y - 46.0f;
            int bubbleCenterX = (int)(pos.x + (f.IsFacingRight() ? -6.0f : 6.0f));
            ChampionshipEdition::DrawComicThoughtBubble(
                quote,
                bubbleCenterX,
                (int)(headTopY - 4.0f),
                (int)pos.x,
                (int)(headTopY + 4.0f),
                10,
                Color{ 15, 22, 38, 255 }
            );
        } else if (!isDownOrKo && isDickCameraLaugh) {
            ChampionshipEdition::DrawComicThoughtBubble(
                "HAHAHAHAHA!",
                (int)pos.x,
                (int)(pos.y - 92.0f),
                (int)pos.x,
                (int)(pos.y - 82.0f),
                10,
                Color{ 185, 20, 25, 255 }
            );
        } else if (!isDownOrKo && f.GetCharacterId() == CHAR_SLEEPY_JOE && state == FSTATE_SPECIAL && f.GetSpecialIndex() == 1) {
            ChampionshipEdition::DrawComicThoughtBubble(
                "Zzz... ICE CREAM...",
                (int)pos.x,
                (int)(pos.y - 52.0f),
                (int)pos.x,
                (int)(pos.y - 42.0f),
                10,
                Color{ 20, 40, 95, 255 }
            );
        }

        if (!isDownOrKo) {
            DrawCosmeticPuffs(f);
            ChampionshipEdition::DrawFighterWorldOverlays(f);
        }
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
    auto drawCompiledProj = [&](int slot, Vector2 pos, float w, float h, bool faceRight, float rotDeg = 0.0f) -> bool {
        if (slot < 0 || slot >= 16 || s_projSprites[slot].id == 0) return false;
        Texture2D t = s_projSprites[slot];
        Rectangle src = { 0.0f, 0.0f, faceRight ? (float)t.width : -(float)t.width, (float)t.height };
        Rectangle dst = { pos.x, pos.y, w, h };
        DrawTexturePro(t, src, dst, Vector2{ w * 0.5f, h * 0.5f }, rotDeg, WHITE);
        return true;
    };

    for (const auto& p : projs) {
        if (!p.active) continue;

        if (p.type == PROJ_SPARK) {
            if (!drawCompiledProj(0, p.pos, 32.0f, 22.0f, p.facingRight)) {
                DrawCircle((int)p.pos.x, (int)p.pos.y, 8, Color{ 80, 180, 255, 240 });
            }
        } else if (p.type == PROJ_LEDGER) {
            float spin = (float)p.age * 10.0f * (p.facingRight ? 1.0f : -1.0f);
            if (!drawCompiledProj(1, p.pos, 30.0f, 30.0f, p.facingRight, spin)) {
                DrawRectangle((int)(p.pos.x - 8), (int)(p.pos.y - 12), 16, 24, Color{ 120, 80, 45, 245 });
            }
        } else if (p.type == PROJ_ARC_BOMB) {
            if (!drawCompiledProj(2, p.pos, 30.0f, 24.0f, p.facingRight)) {
                DrawCircle((int)p.pos.x, (int)p.pos.y, 7, Color{ 240, 40, 150, 255 });
            }
        } else if (p.type == PROJ_TRAP_FIELD) {
            Vector2 trapPos{ p.pos.x, GROUND_Y - 12.0f };
            if (!drawCompiledProj(3, trapPos, 32.0f, 24.0f, true)) {
                DrawEllipse((int)p.pos.x, (int)GROUND_Y - 4, 14, 6, Color{ 65, 220, 255, 230 });
            }
        } else if (p.type == PROJ_COKE_SPRAY) {
            if (!drawCompiledProj(4, p.pos, 32.0f, 24.0f, p.facingRight)) {
                DrawCircle((int)p.pos.x, (int)p.pos.y, 7, Color{ 140, 65, 30, 240 });
            }
        } else if (p.type == PROJ_GAVEL_WAVE) {
            Vector2 gwPos{ p.pos.x, GROUND_Y - 14.0f };
            if (!drawCompiledProj(5, gwPos, 32.0f, 28.0f, p.facingRight)) {
                DrawEllipse((int)p.pos.x, (int)GROUND_Y - 4, 16, 8, Color{ 255, 215, 50, 220 });
            }
        } else if (p.type == PROJ_DISSENT_FLAME) {
            if (!drawCompiledProj(6, p.pos, 34.0f, 28.0f, p.facingRight)) {
                DrawCircle((int)p.pos.x, (int)p.pos.y, 9, Color{ 255, 140, 35, 245 });
            }
        } else if (p.type == PROJ_SECRET_SERVICE) {
            if (s_sprSleepyJoeSS.id > 0) {
                Rectangle srcRec = { 0.0f, 0.0f, (float)s_sprSleepyJoeSS.width, (float)s_sprSleepyJoeSS.height };
                if (!p.facingRight) srcRec.width = -srcRec.width;
                Rectangle dstRec = { p.pos.x - 22.0f, p.pos.y - 28.0f, 44.0f, 54.0f };
                DrawTexturePro(s_sprSleepyJoeSS, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            } else {
                DrawRectangle((int)p.pos.x - 12, (int)p.pos.y - 24, 24, 48, Color{ 15, 20, 25, 255 });
            }
        } else if (p.type == PROJ_SAX_NOTES) {
            if (!drawCompiledProj(7, p.pos, 34.0f, 28.0f, p.facingRight)) {
                DrawEllipse((int)p.pos.x, (int)p.pos.y, 14, 8, Color{ 255, 220, 60, 220 });
            }
        } else if (p.type == PROJ_POLLING_MAP) {
            if (!drawCompiledProj(8, p.pos, 36.0f, 26.0f, p.facingRight)) {
                DrawRectangle((int)p.pos.x - 16, (int)p.pos.y - 8, 32, 16, Color{ 35, 105, 245, 245 });
            }
        } else if (p.type == PROJ_HOPE_BASKETBALL) {
            float spinDeg = (float)p.age * 14.0f * (p.facingRight ? 1.0f : -1.0f);
            if (!drawCompiledProj(9, p.pos, 28.0f, 24.0f, p.facingRight, spinDeg)) {
                DrawCircle((int)p.pos.x, (int)p.pos.y, 9.5f, Color{ 235, 105, 25, 255 });
            }
        } else if (p.type == PROJ_SHAWARMA) {
            float spinDeg = (float)p.lifetime * 14.0f * (p.facingRight ? 1.0f : -1.0f);
            if (s_propShawarma.id > 0) {
                Rectangle src = { 0.0f, 0.0f, (float)s_propShawarma.width, (float)s_propShawarma.height };
                Rectangle dst = { p.pos.x, p.pos.y, 34.0f, 18.0f };
                DrawTexturePro(s_propShawarma, src, dst, Vector2{ 17.0f, 9.0f }, spinDeg, WHITE);
            }
        } else if (p.type == PROJ_WHEELCHAIR) {
            float dir = p.facingRight ? 1.0f : -1.0f;
            DrawCenteredSprite(s_vfxSprites[12], p.pos.x - dir * 24.0f, GROUND_Y - 9.0f, 24.0f, 18.0f, 0.0f, Color{ 255, 255, 255, 210 }, !p.facingRight);
            if (s_propWheelchair.id > 0) {
                Rectangle src = { 0.0f, 0.0f, p.facingRight ? (float)s_propWheelchair.width : -(float)s_propWheelchair.width, (float)s_propWheelchair.height };
                Rectangle dst = { p.pos.x - 28.0f, GROUND_Y - 56.0f, 56.0f, 58.0f };
                DrawTexturePro(s_propWheelchair, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }
        } else if (p.type == PROJ_ATM_HACK) {
            float rise = std::min(1.0f, (30.0f - (float)std::min(p.lifetime, 30)) / 5.0f + 0.35f);
            float atmH = 62.0f * rise;
            if (s_propAtmHack.id > 0) {
                Rectangle src = { 0.0f, 0.0f, (float)s_propAtmHack.width, (float)s_propAtmHack.height * rise };
                Rectangle dst = { p.pos.x - 23.0f, GROUND_Y - atmH + 2.0f, 46.0f, atmH };
                DrawTexturePro(s_propAtmHack, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }
            for (int b = 0; b < 5; b++) {
                float bx = p.pos.x - 18.0f + ((b * 13 + p.lifetime * 3) % 36);
                float by = GROUND_Y - 26.0f - ((p.lifetime * 2 + b * 9) % 34);
                DrawCenteredSprite(s_worldProps[10], bx, by, 14.0f, 9.0f, (float)(p.lifetime * 8 + b * 28));
            }
            VisualsEngine::DrawPixelText("-25% HP!", (int)p.pos.x - 20, (int)(GROUND_Y - atmH - 10.0f), 6, Color{ 80, 255, 115, 255 });
        } else if (p.type == PROJ_SUPREME_WAVE) {
            drawCompiledProj(10, p.pos, 38.0f, 44.0f, p.facingRight);
        } else if (p.type == PROJ_EYE_LASER) {
            drawCompiledProj(11, p.pos, 52.0f, 20.0f, p.facingRight);
        } else if (p.type == PROJ_HAMMER_SICKLE) {
            float spinDeg = (float)p.lifetime * 22.0f * (p.facingRight ? 1.0f : -1.0f);
            if (s_propHammerSickle.id > 0) {
                Rectangle src = { 0.0f, 0.0f, (float)s_propHammerSickle.width, (float)s_propHammerSickle.height };
                Rectangle dst = { p.pos.x, p.pos.y, 32.0f, 32.0f };
                DrawTexturePro(s_propHammerSickle, src, dst, Vector2{ 16.0f, 16.0f }, spinDeg, WHITE);
            }
        } else if (p.type == PROJ_SPY_RADIO) {
            drawCompiledProj(12, p.pos, 36.0f, 28.0f, p.facingRight);
        } else if (p.type == PROJ_PAPER_PLANE) {
            float bob = sinf((float)p.lifetime * 0.25f) * 1.6f;
            drawCompiledProj(13, Vector2{ p.pos.x, p.pos.y + bob }, 30.0f, 16.0f, p.facingRight);
        } else if (p.type == PROJ_SHOTGUN_BLAST) {
            drawCompiledProj(14, p.pos, 42.0f, 28.0f, p.facingRight);
        } else if (p.type == PROJ_HORMUZ_BLOCKADE) {
            int px = (int)p.pos.x;
            int gy = (int)GROUND_Y;
            float waveBob = sinf((float)p.lifetime * 0.25f) * 2.0f;
            DrawCenteredSprite(s_worldProps[11], p.pos.x, gy - 11.0f + waveBob * 0.5f, 60.0f, 24.0f, 0.0f, WHITE, ((p.lifetime / 10) % 2 == 1));
            float bob = fabsf(sinf((float)p.lifetime * 0.20f)) * 4.0f;
            drawCompiledProj(15, Vector2{ p.pos.x - 11.0f, gy - 26.0f - bob }, 22.0f, 28.0f, true);
            drawCompiledProj(15, Vector2{ p.pos.x + 11.0f, gy - 24.0f - bob * 0.6f }, 22.0f, 28.0f, false);
            DrawRectangle(px - 17, gy - 48, 34, 10, Color{ 205, 25, 30, 245 });
            DrawRectangleLines(px - 17, gy - 48, 34, 10, Color{ 255, 230, 75, 255 });
            VisualsEngine::DrawPixelText("CLOSED", px - 14, gy - 46, 6, WHITE);
        }
    }
}

void VisualsEngine::DrawFinisherBackdrop(const Fighter& p1, const Fighter& p2, float camX) {
    if (!p1.IsPerformingFinisher() && !p2.IsPerformingFinisher()) return;
    const Fighter& atk = p1.IsPerformingFinisher() ? p1 : p2;
    const Fighter& vic = p1.IsPerformingFinisher() ? p2 : p1;
    int fr = atk.GetStateFrame();

    unsigned char veilA = (fr >= 54 && fr <= 61) ? 90 : 185;
    DrawRectangle((int)(camX - 260.0f), -40, SCREEN_WIDTH + 520, SCREEN_HEIGHT + 100, Color{ 6, 4, 16, veilA });

    auto drawSpot = [](float sx, Color col) {
        DrawTriangle(Vector2{ sx, -20.0f }, Vector2{ sx - 36.0f, GROUND_Y + 6.0f }, Vector2{ sx + 36.0f, GROUND_Y + 6.0f }, col);
        DrawEllipse((int)sx, (int)GROUND_Y + 1, 34, 6, Color{ col.r, col.g, col.b, 95 });
    };
    drawSpot(atk.GetPosition().x, Color{ 255, 225, 90, 42 });
    drawSpot(vic.GetPosition().x, Color{ 255, 45, 55, 52 });
}

void VisualsEngine::DrawFinisherWorldFX(const Fighter& p1, const Fighter& p2) {
    if (!p1.IsPerformingFinisher() && !p2.IsPerformingFinisher()) return;
    const Fighter& atk = p1.IsPerformingFinisher() ? p1 : p2;
    const Fighter& vic = p1.IsPerformingFinisher() ? p2 : p1;
    int fr = atk.GetStateFrame();
    CharacterId cid = atk.GetCharacterId();
    int propIdx = std::clamp(Fighter::GetFinisherPropIndex(cid), 0, 15);
    float vx = vic.GetPosition().x;
    float ax = atk.GetPosition().x;
    float dir = atk.IsFacingRight() ? 1.0f : -1.0f;

    if (fr < 48) {
        float pulse = sinf(fr * 0.5f) * 6.0f;
        DrawCenteredSprite(s_vfxSprites[14], ax, GROUND_Y - 3.0f, 64.0f + pulse, 15.0f);
        for (int r = 0; r < 3; r++) {
            float ry = GROUND_Y - fmodf(fr * 4.5f + r * 24.0f, 78.0f);
            float rx = ax + ((r % 2 == 0) ? -18.0f : 18.0f);
            DrawCenteredSprite(s_vfxSprites[0], rx, ry, 10.0f, 10.0f, (float)(fr * 8 + r * 35));
        }
    }

    if (fr >= 12 && s_finisherProps[propIdx].id > 0) {
        Texture2D pt = s_finisherProps[propIdx];
        float dropT = std::clamp((float)(fr - 12) / 42.0f, 0.0f, 1.0f);
        float easeIn = dropT * dropT;

        bool isHorizontalSweep = (cid == CHAR_REDCAP || cid == CHAR_BLEXAS || cid == CHAR_DICK);
        bool isOrbitalSatellite = (cid == CHAR_MTG || cid == CHAR_FLOTUS || cid == CHAR_POPULIST);
        bool isBackboardDunk = (cid == CHAR_A_NEW_HOPE);
        bool isTerminator = (cid == CHAR_SI);

        float px = vx;
        float py = GROUND_Y - 46.0f;
        float pw = 86.0f;
        float ph = 86.0f;
        float rot = 0.0f;

        if (isHorizontalSweep) {
            px = (ax - dir * 35.0f) + (vx - (ax - dir * 35.0f)) * easeIn + (fr > 54 ? dir * (fr - 54) * 3.5f : 0.0f);
            py = GROUND_Y - 38.0f - fabsf(sinf(fr * 0.8f)) * 4.0f;
            pw = 92.0f; ph = 78.0f;

            for (int f = 0; f < 4; f++) {
                float fx = px - dir * (22.0f + f * 12.0f);
                float fsz = 18.0f - f * 2.5f;
                DrawCenteredSprite(s_vfxSprites[11], fx, GROUND_Y - 10.0f, fsz, fsz + 4.0f, 0.0f, WHITE, (f % 2 == 1));
            }
            if (cid == CHAR_DICK && fr >= 32) {
                for (int b = 0; b < 4; b++) {
                    float bx = ax + dir * (28.0f + fmodf(fr * 7.0f + b * 24.0f, std::max(25.0f, fabsf(vx - ax) + 25.0f)));
                    float by = GROUND_Y - 52.0f + ((b % 3) - 1) * 10.0f;
                    DrawCenteredSprite(s_vfxSprites[1], bx, by, 14.0f, 14.0f, (float)(fr * 12 + b * 40));
                }
            }
        } else if (isOrbitalSatellite) {
            px = vx + sinf(fr * 0.08f) * 6.0f;
            py = 86.0f;
            pw = 68.0f; ph = 68.0f;
            if (fr >= 30 && fr <= 74) {

                float beamW = (fr >= 52 && fr <= 64) ? 26.0f : 14.0f;
                Color outerC = (cid == CHAR_FLOTUS) ? Color{ 255, 55, 85, 205 } : ((cid == CHAR_POPULIST) ? Color{ 255, 95, 235, 205 } : Color{ 65, 220, 255, 215 });
                int beamTop = (int)py + 16;
                int beamH = std::max(10, (int)GROUND_Y - beamTop);
                DrawRectangleGradientH((int)(vx - beamW * 0.5f), beamTop, (int)(beamW * 0.5f), beamH, Color{ outerC.r, outerC.g, outerC.b, 35 }, outerC);
                DrawRectangleGradientH((int)vx, beamTop, (int)(beamW * 0.5f), beamH, outerC, Color{ outerC.r, outerC.g, outerC.b, 35 });
                DrawRectangle((int)(vx - beamW * 0.18f), beamTop, std::max(3, (int)(beamW * 0.36f)), beamH, Color{ 255, 255, 255, 235 });
                for (int k = 0; k < 4; k++) {
                    float ly = (float)beamTop + 14.0f + k * (beamH * 0.22f);
                    const Texture2D& bolt = ((fr + k) % 2 == 0) ? s_vfxSprites[8] : s_vfxSprites[9];
                    DrawCenteredSprite(bolt, vx + ((k % 2 == 0) ? -8.0f : 8.0f), ly, 30.0f, 30.0f, (float)((k % 2 == 0) ? 25 : -25), WHITE, (k % 2 == 1));
                }
            }
        } else if (isBackboardDunk) {
            px = vx + dir * 12.0f;
            py = GROUND_Y - 78.0f;
            pw = 88.0f; ph = 88.0f;
            if (fr >= 32 && fr <= 58 && s_projSprites[9].id > 0) {
                Rectangle bSrc = { 0, 0, (float)s_projSprites[9].width, (float)s_projSprites[9].height };
                float bProg = (float)(fr - 32) / 26.0f;
                Rectangle bDst = { ax + (vx - ax) * bProg, (GROUND_Y - 96.0f) + bProg * 42.0f, 26.0f, 24.0f };
                DrawTexturePro(s_projSprites[9], bSrc, bDst, Vector2{ 13.0f, 12.0f }, fr * 18.0f, WHITE);
            }
        } else if (isTerminator) {
            float grow = std::min(1.0f, dropT * 1.4f);
            px = vx;
            py = GROUND_Y - 48.0f * grow;
            pw = 94.0f * grow; ph = 94.0f * grow;
            VisualsEngine::DrawPixelText("-100% TERMINATED!", (int)vx - 44, (int)GROUND_Y - 106, 7, Color{ 75, 255, 115, 255 });
        } else {

            if (cid == CHAR_THE_DICTATOR) {
                DrawCenteredSprite(s_worldProps[12], vx, GROUND_Y - 12.0f, 64.0f, 42.0f);
            }
            float startY = -55.0f;
            float endY = GROUND_Y - 38.0f;
            py = startY + (endY - startY) * easeIn;
            if (fr > 54) {
                float bnc = (float)(fr - 54) / 36.0f;
                py = endY - fabsf(sinf(bnc * PI_F * 2.0f)) * (14.0f * (1.0f - bnc));
            }
            if (cid == CHAR_HIGH_BENCH || cid == CHAR_BILLARY) {
                rot = -38.0f * (1.0f - easeIn);
            }
        }

        Rectangle src = { 0.0f, 0.0f, (dir >= 0.0f || !isHorizontalSweep) ? (float)pt.width : -(float)pt.width, (float)pt.height };
        Rectangle dst = { px, py, pw, ph };
        DrawTexturePro(pt, src, dst, Vector2{ pw * 0.5f, ph * 0.5f }, rot, WHITE);

        if (fr >= 52 && fr <= 78) {
            float ringT = (float)(fr - 52) / 26.0f;
            unsigned char ringA = (unsigned char)(255.0f * (1.0f - ringT));
            DrawCenteredSprite(s_vfxSprites[14], vx, GROUND_Y - 4.0f, 54.0f + ringT * 86.0f, 16.0f + ringT * 12.0f, 0.0f, Color{ 255, 255, 255, ringA });
            DrawCenteredSprite(s_vfxSprites[12], vx - 28.0f - ringT * 32.0f, GROUND_Y - 12.0f, 28.0f, 20.0f, 0.0f, Color{ 255, 255, 255, ringA }, true);
            DrawCenteredSprite(s_vfxSprites[12], vx + 28.0f + ringT * 32.0f, GROUND_Y - 12.0f, 28.0f, 20.0f, 0.0f, Color{ 255, 255, 255, ringA }, false);
            if (cid == CHAR_AYATOLLAH && s_projSprites[15].id > 0) {
                Texture2D sm = s_projSprites[15];
                Rectangle mSrc = { 0, 0, (float)sm.width, (float)sm.height };
                DrawTexturePro(sm, mSrc, Rectangle{ vx - 38.0f - ringT * 18.0f, GROUND_Y - 24.0f, 22.0f, 26.0f }, Vector2{ 11, 13 }, 0.0f, WHITE);
                DrawTexturePro(sm, mSrc, Rectangle{ vx + 38.0f + ringT * 18.0f, GROUND_Y - 24.0f, 22.0f, 26.0f }, Vector2{ 11, 13 }, 0.0f, WHITE);
            }
        }
    }
}

void VisualsEngine::DrawSparks(const std::vector<SparkEffect>& sparks) {
    for (const auto& sp : sparks) {
        if (!sp.alive) continue;
        float tNorm = std::clamp((float)sp.frame / (float)std::max(1, sp.maxFrames), 0.0f, 1.0f);
        unsigned char alpha = (unsigned char)std::clamp((int)((1.0f - tNorm * 0.65f) * 255.0f), 45, 255);
        float rot = (float)(((int)(sp.pos.x * 3.0f + sp.pos.y * 5.0f) % 4) * 90);

        if (sp.isBlock) {
            const Texture2D& gTex = (tNorm < 0.45f) ? s_vfxSprites[4] : s_vfxSprites[5];
            float sz = 28.0f + tNorm * 10.0f;
            DrawCenteredSprite(gTex, sp.pos.x, sp.pos.y, sz, sz, (tNorm < 0.45f) ? rot : 0.0f, Color{ 255, 255, 255, alpha });
        } else {
            bool isHeavy = (sp.maxFrames >= 9);
            int vIdx = 0;
            if (isHeavy) {
                vIdx = (tNorm < 0.45f) ? 3 : 2;
            } else {
                vIdx = (tNorm < 0.35f) ? 0 : ((tNorm < 0.72f) ? 1 : 2);
            }
            float baseSz = isHeavy ? 38.0f : 28.0f;
            float sz = baseSz * (0.85f + tNorm * 0.40f);
            DrawCenteredSprite(s_vfxSprites[vIdx], sp.pos.x, sp.pos.y, sz, sz, rot, Color{ 255, 255, 255, alpha });
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
    int p1TrailW = (int)((std::max(p1Hp, ChampionshipEdition::s_p1TrailHp) / 100.0f) * BAR_WIDTH);
    DrawRectangle(20, TOP_Y, BAR_WIDTH + 4, BAR_HEIGHT + 4, Color{ 12, 14, 24, 240 });
    DrawRectangleLines(20, TOP_Y, BAR_WIDTH + 4, BAR_HEIGHT + 4, Color{ 235, 195, 55, 255 });
    DrawRectangle(22, TOP_Y + 2, BAR_WIDTH, BAR_HEIGHT, Color{ 75, 15, 20, 255 });
    DrawRectangle(22 + (BAR_WIDTH - p1TrailW), TOP_Y + 2, p1TrailW, BAR_HEIGHT, Color{ 235, 55, 45, 255 });
    Color p1BarTop = (p1Hp < 30.0f && ((int)(GetTime() * 14.0f) % 2 == 0)) ? Color{ 255, 120, 40, 255 } : Color{ 255, 235, 65, 255 };
    DrawRectangleGradientV(22 + (BAR_WIDTH - p1HealthW), TOP_Y + 2, p1HealthW, BAR_HEIGHT, p1BarTop, Color{ 210, 155, 15, 255 });

    int p2HealthW = (int)((p2Hp / 100.0f) * BAR_WIDTH);
    int p2TrailW = (int)((std::max(p2Hp, ChampionshipEdition::s_p2TrailHp) / 100.0f) * BAR_WIDTH);
    DrawRectangle(SCREEN_WIDTH - 24 - BAR_WIDTH, TOP_Y, BAR_WIDTH + 4, BAR_HEIGHT + 4, Color{ 12, 14, 24, 240 });
    DrawRectangleLines(SCREEN_WIDTH - 24 - BAR_WIDTH, TOP_Y, BAR_WIDTH + 4, BAR_HEIGHT + 4, Color{ 235, 195, 55, 255 });
    DrawRectangle(SCREEN_WIDTH - 22 - BAR_WIDTH, TOP_Y + 2, BAR_WIDTH, BAR_HEIGHT, Color{ 75, 15, 20, 255 });
    DrawRectangle(SCREEN_WIDTH - 22 - BAR_WIDTH, TOP_Y + 2, p2TrailW, BAR_HEIGHT, Color{ 235, 55, 45, 255 });
    Color p2BarTop = (p2Hp < 30.0f && ((int)(GetTime() * 14.0f) % 2 == 0)) ? Color{ 255, 120, 40, 255 } : Color{ 255, 235, 65, 255 };
    DrawRectangleGradientV(SCREEN_WIDTH - 22 - BAR_WIDTH, TOP_Y + 2, p2HealthW, BAR_HEIGHT, p2BarTop, Color{ 210, 155, 15, 255 });

    if (s_loaded) {
        int c1Id = (int)p1.GetCharacterId();
        int c2Id = (int)p2.GetCharacterId();
        Texture2D port1 = (p1Hp < 30.0f && s_portraitsLose[c1Id].id > 0) ? s_portraitsLose[c1Id] : s_portraits[c1Id];
        Texture2D port2 = (p2Hp < 30.0f && s_portraitsLose[c2Id].id > 0) ? s_portraitsLose[c2Id] : s_portraits[c2Id];
        if (port1.id > 0) {
            DrawRectangle(2, TOP_Y - 2, 17, 18, Color{ 15, 18, 28, 240 });
            DrawTexturePro(port1, Rectangle{ 0, 0, (float)port1.width, (float)port1.height }, Rectangle{ 3, (float)TOP_Y - 1, 15, 16 }, Vector2{ 0, 0 }, 0.0f, WHITE);
            DrawRectangleLines(2, TOP_Y - 2, 17, 18, (p1Hp < 30.0f) ? Color{ 255, 60, 60, 255 } : Color{ 235, 195, 55, 255 });
        }
        if (port2.id > 0) {
            DrawRectangle(SCREEN_WIDTH - 19, TOP_Y - 2, 17, 18, Color{ 15, 18, 28, 240 });
            DrawTexturePro(port2, Rectangle{ 0, 0, -(float)port2.width, (float)port2.height }, Rectangle{ (float)SCREEN_WIDTH - 18, (float)TOP_Y - 1, 15, 16 }, Vector2{ 0, 0 }, 0.0f, WHITE);
            DrawRectangleLines(SCREEN_WIDTH - 19, TOP_Y - 2, 17, 18, (p2Hp < 30.0f) ? Color{ 255, 60, 60, 255 } : Color{ 235, 195, 55, 255 });
        }
    }

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

    auto drawBossRefillPips = [](const Fighter& f, bool isP1) {
        int maxRefills = 0;
        if (f.GetCharacterId() == CHAR_AYATOLLAH) maxRefills = 7;
        else if (f.GetCharacterId() == CHAR_PALM_BEACH_PETE) maxRefills = 4;
        else if (f.GetCharacterId() == CHAR_WAR_PIG) maxRefills = 2;
        if (maxRefills <= 0) return;

        int remaining = std::max(0, maxRefills - f.GetRefillCount());
        int baseX = isP1 ? 58 : (SCREEN_WIDTH - 58);
        int stepX = isP1 ? 10 : -10;
        int cy = TOP_Y + 18;
        for (int i = 0; i < maxRefills; i++) {
            int cx = baseX + i * stepX;
            bool lit = (i < remaining);
            if (f.GetCharacterId() == CHAR_AYATOLLAH) {
                DrawCenteredSprite(s_vfxSprites[15], (float)cx, (float)cy, 9.0f, 9.0f, 0.0f, lit ? WHITE : Color{ 60, 70, 80, 150 });
            } else {
                DrawCenteredSprite(s_vfxSprites[6], (float)cx, (float)cy, 8.5f, 8.5f, 0.0f, lit ? Color{ 235, 110, 255, 255 } : Color{ 60, 70, 80, 150 });
            }
        }
        char barLbl[24];
        snprintf(barLbl, sizeof(barLbl), "BAR %d/%d", remaining + 1, maxRefills + 1);
        int bw = MeasureText(barLbl, 6);
        int lx = isP1 ? 58 : (SCREEN_WIDTH - 58 - bw);
        VisualsEngine::DrawPixelText(barLbl, lx, TOP_Y + 25, 6, Color{ 110, 250, 195, 240 });
    };
    drawBossRefillPips(p1, true);
    drawBossRefillPips(p2, false);
}

void VisualsEngine::DrawCinematicIntro(float timer, bool logoDropped, const Shader& flagShader, const Texture2D& flagTex) {
    if (s_loaded && s_titleSplash.id > 0) {

        float cropH = std::min((float)s_titleSplash.height, 694.0f);
        Rectangle srcRec = { 0.0f, 0.0f, (float)s_titleSplash.width, cropH };
        Rectangle dstRec = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
        DrawTexturePro(s_titleSplash, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

        for (int s = 0; s < 10; s++) {
            float sx = 195.0f + fmodf(s * 23.0f + timer * (34.0f + s * 3.0f), 182.0f);
            float sy = SCREEN_HEIGHT - 24.0f - fmodf(s * 17.0f + timer * (28.0f + s * 4.0f), 112.0f);
            const Texture2D& spTex = (s % 2 == 0) ? s_vfxSprites[13] : s_vfxSprites[0];
            DrawCenteredSprite(spTex, sx, sy, 8.0f, 8.0f, timer * 25.0f + s * 18.0f);
        }

        DrawRectangle(0, SCREEN_HEIGHT - 16, SCREEN_WIDTH, 16, Color{ 6, 10, 22, 215 });
        DrawLine(0, SCREEN_HEIGHT - 16, SCREEN_WIDTH, SCREEN_HEIGHT - 16, Color{ 60, 210, 255, 220 });

        if (((int)(timer * 3.2f) % 2) == 0) {
            DrawPixelText("INSERT COIN / PRESS START", 10, SCREEN_HEIGHT - 12, 9, Color{ 255, 240, 90, 255 });
        } else {
            DrawPixelText("INSERT COIN / PRESS START", 10, SCREEN_HEIGHT - 12, 9, Color{ 180, 230, 255, 220 });
        }

        const char* tSpd = (ChampionshipEdition::s_turboSpeed == TURBO_SPEED_1) ? "TURBO 1"
                         : ((ChampionshipEdition::s_turboSpeed == TURBO_SPEED_3) ? "TURBO 3 MAX" : "TURBO 2");
        int tsw = MeasureText(tSpd, 8);
        DrawRectangle(SCREEN_WIDTH - tsw - 14, SCREEN_HEIGHT - 14, tsw + 8, 12, Color{ 18, 45, 95, 240 });
        DrawRectangleLines(SCREEN_WIDTH - tsw - 14, SCREEN_HEIGHT - 14, tsw + 8, 12, Color{ 95, 235, 255, 255 });
        DrawPixelText(tSpd, SCREEN_WIDTH - tsw - 10, SCREEN_HEIGHT - 12, 8, Color{ 120, 245, 255, 255 });
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

        Vector2 nyPt{ 275.0f, 48.0f };
        Vector2 wyPt{ 92.0f, 52.0f };
        Vector2 chiPt{ 138.0f, 42.0f };
        Vector2 dcPt{ 245.0f, 74.0f };
        Vector2 flPt{ 188.0f, 124.0f };

        auto drawFlightLine = [](Vector2 a, Vector2 b, Color col) {
            DrawLineEx(a, b, 2.2f, col);
        };
        drawFlightLine(nyPt, wyPt, Color{ 255, 145, 45, 200 });
        drawFlightLine(wyPt, dcPt, Color{ 255, 220, 60, 200 });
        drawFlightLine(dcPt, chiPt, Color{ 80, 220, 255, 200 });
        drawFlightLine(chiPt, flPt, Color{ 255, 165, 50, 200 });

        struct MapTarget {
            Vector2 pos;
            const char* city;
            CharacterId charId;
        };
        MapTarget targets[5] = {
            { nyPt,  "NEW YORK",     CHAR_SOCIALIST },
            { wyPt,  "WYOMING",      CHAR_DICK },
            { dcPt,  "D.C.",         CHAR_BILLARY },
            { chiPt, "CHICAGO",      CHAR_A_NEW_HOPE },
            { flPt,  "FLORIDA KEYS", CHAR_PALM_BEACH_PETE }
        };

        for (int i = 0; i < 5; i++) {
            const auto& t = targets[i];
            bool isDef = (defeatedFighters != nullptr && defeatedFighters[t.charId]);
            bool isActive = (p2Char == t.charId);

            if (isDef) {
                DrawCenteredSprite(s_vfxSprites[2], t.pos.x, t.pos.y, 18.0f, 18.0f);
                DrawPixelText("BEATEN", (int)t.pos.x - 14, (int)t.pos.y + 8, 7, Color{ 240, 80, 80, 255 });
            } else if (isActive) {
                float pulse = 1.0f + 0.14f * sinf(timer * 9.0f);
                DrawCenteredSprite(s_vfxSprites[6], t.pos.x, t.pos.y, 18.0f * pulse, 18.0f * pulse, timer * 36.0f);
                int cw = MeasureText(t.city, 7);
                DrawRectangle((int)t.pos.x - cw / 2 - 2, (int)t.pos.y + 8, cw + 4, 10, Color{ 10, 15, 25, 220 });
                DrawPixelText(t.city, (int)t.pos.x - cw / 2, (int)t.pos.y + 9, 7, Color{ 255, 230, 80, 255 });
            } else {
                DrawCenteredSprite(s_vfxSprites[6], t.pos.x, t.pos.y, 11.0f, 11.0f, 0.0f, Color{ 95, 225, 255, 235 });
                int cw = MeasureText(t.city, 7);
                DrawRectangle((int)t.pos.x - cw / 2 - 2, (int)t.pos.y + 7, cw + 4, 9, Color{ 10, 15, 25, 180 });
                DrawPixelText(t.city, (int)t.pos.x - cw / 2, (int)t.pos.y + 8, 7, Color{ 160, 195, 225, 220 });
            }
        }
    } else {

        DrawRectangleGradientV(0, 16, SCREEN_WIDTH, 130, Color{ 10, 20, 36, 255 }, Color{ 14, 28, 48, 255 });

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
    DrawEllipse(182, 106, 11, 9, Color{ 14, 40, 68, 255 });

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

    DrawRectangle(176, 48, 8, 5, Color{ 245, 245, 250, 255 });
    DrawRectangle(134, 86, 7, 7, Color{ 240, 240, 245, 255 });
    DrawRectangle(184, 82, 3, 10, WHITE);
    DrawRectangle(284, 72, 8, 12, Color{ 200, 205, 220, 255 });
    DrawPoly(Vector2{ 75, 115 }, 5, 15, 0.0f, Color{ 35, 55, 75, 255 });

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

    Vector2 pins[STAGE_COUNT] = {
        { 180, 48 }, { 158, 88 }, { 288, 78 }, { 180, 64 },
        { 168, 42 }, { 82, 148 }, { 295, 38 }, { 145, 36 },
        { 260, 138 }, { 118, 88 }, { 228, 92 }, { 78, 118 },
        { 212, 54 }, { 96, 34 },
        { 342, 34 }, { 342, 62 }, { 342, 90 }, { 342, 118 },
        { 104, 72 }, { 300, 96 }, { 160, 26 }, { 190, 46 },
        { 68, 36 },  { 342, 134 }
    };

    const char* pinNames[STAGE_COUNT] = {
        "1. EAST WING DEMO",
        "2. REFLECTING POOL",
        "3. CONGRESS FLOOR",
        "4. WHITE HOUSE MMA",
        "5. PRESS BRIEFING",
        "6. PENTAGON ALTAR",
        "7. WTC PLAZA NYC",
        "8. COMET PING PONG",
        "9. TEMPLE ISLAND",
        "10. LINCOLN MEMORIAL",
        "11. SMITHSONIAN AERO",
        "12. TOMB OF UNKNOWN",
        "13. ENN NEWS STUDIO",
        "14. CHICAGO WRIGLEY",
        "15. RYUGYONG HOTEL",
        "16. WESTERN WALL",
        "17. SOVIET FOUNDRY",
        "18. PALACE COURTYARD",
        "19. PERFORMING ARTS CTR",
        "20. THE HOOD (3AM)",
        "21. PROGRESS AVENUE",
        "22. LINCOLN BEDROOM",
        "23. WYOMING RODEO",
        "24. TEHRAN CASBAH"
    };

    for (int i = 0; i < STAGE_COUNT; i++) {
        if (i >= STAGE_RYUGYONG_HOTEL && i <= STAGE_PALACE_COURTYARD) continue;
        if (i == STAGE_TEHRAN_CASBAH) continue;
        bool sel = (i == selectedStage);
        Color pinCol = sel ? Color{ 255, 230, 40, 255 } : Color{ 0, 180, 220, 255 };

        DrawCenteredSprite(s_vfxSprites[6], pins[i].x, pins[i].y, sel ? 14.0f : 9.0f, sel ? 14.0f : 9.0f, sel ? (timer * 45.0f) : 0.0f, pinCol);
        if (sel) {
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
        for (int s = 0; s < PLAYABLE_CHARACTER_COUNT; s++) {
            int cid = (int)PLAYABLE_ROSTER[s];
            if (cid != p1Char && defeatedFighters[cid]) defeatedOriginalCount++;
        }
    }

    DrawRectangle(0, 0, SCREEN_WIDTH, 18, Color{ 10, 12, 18, 240 });
    const char* headerTitle = "SELECT YOUR FIGHTER";
    Color headerCol = Color{ 255, 220, 80, 255 };

    if (!playerLocked) {
        headerTitle = "SELECT YOUR FIGHTER (16 CHALLENGERS)";
        headerCol = Color{ 255, 220, 80, 255 };
    } else if (warPigBeaten) {
        if (!defeatedFighters[CHAR_SOCIALIST]) {
            headerTitle = "*** NATIONAL TOUR: NEW YORK (THE SOCIALIST) ***";
            headerCol = Color{ 80, 220, 255, 255 };
        } else if (!defeatedFighters[CHAR_DICK]) {
            headerTitle = "*** NATIONAL TOUR: WYOMING RODEO (DICK) ***";
            headerCol = Color{ 255, 145, 45, 255 };
        } else if (!defeatedFighters[CHAR_BILLARY]) {
            headerTitle = "*** NATIONAL TOUR: D.C. BASEMENT (BILLARY) ***";
            headerCol = Color{ 255, 120, 240, 255 };
        } else if (!defeatedFighters[CHAR_A_NEW_HOPE]) {
            headerTitle = "*** NATIONAL TOUR: CHICAGO WRIGLEY (A NEW HOPE) ***";
            headerCol = Color{ 255, 195, 60, 255 };
        } else if (!defeatedFighters[CHAR_PALM_BEACH_PETE]) {
            headerTitle = "*** U.S. FINALE: FLORIDA KEYS (PALM BEACH PETE) ***";
            headerCol = Color{ 255, 215, 0, 255 };
        } else {
            headerTitle = "*** NEXT ACT: GLOBAL CONFLICT WORLD TOUR! ***";
            headerCol = Color{ 255, 95, 70, 255 };
        }
    } else if (defeatedOriginalCount < (PLAYABLE_CHARACTER_COUNT - 1)) {
        if (ChampionshipEdition::s_opponentLocked) {
            headerTitle = "SELECT BATTLE VENUE (D-PAD: PIN | ATTACK: FIGHT!)";
            headerCol = Color{ 80, 235, 255, 255 };
        } else {
            headerTitle = "CHOOSE NEXT OPPONENT (BEAT ALL 15 CHALLENGERS)";
            headerCol = Color{ 255, 120, 80, 255 };
        }
    } else if (!defeatedFighters[CHAR_SUCKRATERY_RKF]) {
        headerTitle = "*** BOSS ENCOUNTER: SUCKRATERY RKF ***";
        headerCol = Color{ 255, 60, 60, 255 };
    } else if (!defeatedFighters[CHAR_THE_PUNDIT]) {
        headerTitle = "*** BOSS ENCOUNTER: THE PUNDIT (ENN STUDIO) ***";
        headerCol = Color{ 255, 180, 60, 255 };
    } else if (!defeatedFighters[CHAR_WAR_PIG]) {
        headerTitle = "*** FINAL D.C. BOSS: WAR PIG ***";
        headerCol = Color{ 255, 40, 40, 255 };
    } else {
        headerTitle = "*** TOTAL VETO GRAND CHAMPION! ***";
        headerCol = Color{ 255, 215, 0, 255 };
    }

    int stw = MeasureText(headerTitle, 9);
    DrawPixelText(headerTitle, (SCREEN_WIDTH - stw) / 2, 4, 9, headerCol);

    const char* pinNamesAll[STAGE_COUNT] = {
        "EAST WING DEMO",
        "REFLECTING POOL",
        "CONGRESS FLOOR",
        "WHITE HOUSE MMA",
        "PRESS BRIEFING",
        "PENTAGON ALTAR",
        "WTC PLAZA (PRE-9/11)",
        "COMET PING PONG",
        "TEMPLE ISLAND (FL KEYS)",
        "LINCOLN MEMORIAL STEPS",
        "SMITHSONIAN AIR & SPACE",
        "TOMB OF THE UNKNOWN",
        "ENN NEWS STUDIO",
        "CHICAGO WRIGLEY FIELD",
        "RYUGYONG HOTEL (DPRK)",
        "WESTERN WALL (JERUSALEM)",
        "SOVIET STEEL FOUNDRY",
        "PALACE COURTYARD (GULF)",
        "PERFORMING ARTS CENTER",
        "THE HOOD (3AM CARRYOUT)",
        "PROGRESS AVENUE",
        "THE LINCOLN BEDROOM",
        "WYOMING (OLD FAITHFUL)",
        "TEHRAN (CASBAH)"
    };

    char venueBanner[80];
    if (ChampionshipEdition::s_opponentLocked && !warPigBeaten && defeatedOriginalCount < (PLAYABLE_CHARACTER_COUNT - 1)) {
        snprintf(venueBanner, sizeof(venueBanner), "< STAGE: %s >", pinNamesAll[selectedStage < STAGE_COUNT ? selectedStage : 0]);
    } else {
        snprintf(venueBanner, sizeof(venueBanner), "[STAGE: %s]", pinNamesAll[selectedStage < STAGE_COUNT ? selectedStage : 0]);
    }
    int vbw = MeasureText(venueBanner, 8);
    DrawRectangle((SCREEN_WIDTH - vbw) / 2 - 4, 132, vbw + 8, 12, Color{ 10, 14, 22, 235 });
    if (ChampionshipEdition::s_opponentLocked && !warPigBeaten && defeatedOriginalCount < (PLAYABLE_CHARACTER_COUNT - 1)) {
        DrawRectangleLines((SCREEN_WIDTH - vbw) / 2 - 4, 132, vbw + 8, 12, Color{ 255, 225, 60, 255 });
        DrawPixelText(venueBanner, (SCREEN_WIDTH - vbw) / 2, 134, 8, Color{ 255, 235, 80, 255 });
    } else {
        DrawPixelText(venueBanner, (SCREEN_WIDTH - vbw) / 2, 134, 8, Color{ 0, 220, 255, 230 });
    }

    const char* rosterNames[CHARACTER_COUNT] = {
        "POTUS", "HIGH BENCH", "REDCAP", "GOVERNATOR",
        "TECHBRO", "FILIBUSTER", "FREAGLE", "POPULIST",
        "SLEEPY JOE", "MARJORY T.G.",
        "SUCKRATERY RKF", "WAR PIG", "THE SOCIALIST", "BILLARY",
        "PALM BEACH PETE", "THE PUNDIT", "A NEW HOPE",
        "DOCTOR ABDUL", "COUCHIE", "BLEXAS", "S.I.",
        "KIM", "BEBE", "PALE MOTH", "THE DICTATOR",
        "FLOTUS", "W", "DICK", "THE AYATOLLAH"
    };

    const char* rosterArchetypes[CHARACTER_COUNT] = {
        "Shoto All-Round", "Three Justices", "Grappler Slam", "Agile Rushdown",
        "Zoner Traps", "Turtle Defense", "Acrobatic Fur", "Mix-Up Kicks",
        "Power Nap Counter", "Trailer Brawler",
        "Brain Worm Cobra", "Pentagon Final Boss", "Produce & Cash",
        "Dual Clinton Electro", "Adrenochrome Mogul", "Decision 2028 Map",
        "Windy City Hoop",
        "Shawarma & Shakira", "Hiawatha Couch Tank", "Hotwheels Titan", "25% ATM Terminator",
        "Supreme Wave Boss", "Star-Burst Lasers", "Hammer & Sickle", "Spider Hole Boss",
        "Natasha Spy Radio", "Paper Plane Jenga", "Wyoming Shotgun", "Strait of Hormuz"
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
        const char* p2Tag = (!IsPlayableCharacter((CharacterId)p2Char)) ? "BOSS TARGET" : "2P TARGET";
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
        int cWidth = 72;
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

        bool socDef  = (defeatedFighters != nullptr && defeatedFighters[CHAR_SOCIALIST]);
        bool dickDef = (defeatedFighters != nullptr && defeatedFighters[CHAR_DICK]);
        bool bilDef  = (defeatedFighters != nullptr && defeatedFighters[CHAR_BILLARY]);
        bool peteDef = (defeatedFighters != nullptr && defeatedFighters[CHAR_PALM_BEACH_PETE]);
        bool hopeDef = (defeatedFighters != nullptr && defeatedFighters[CHAR_A_NEW_HOPE]);

        ECCard ecCards[5] = {
            { 6,   CHAR_SOCIALIST,       "1: NY",   "SOCIALIST", "WTC Plaza",   false,    socDef },
            { 81,  CHAR_DICK,            "2: WY",   "DICK",      "Wyoming",     !socDef,  dickDef },
            { 156, CHAR_BILLARY,         "3: D.C.", "BILLARY",   "Comet Ping",  !dickDef, bilDef },
            { 231, CHAR_A_NEW_HOPE,      "4: CHI",  "NEW HOPE",  "Wrigley",     !bilDef,  hopeDef },
            { 306, CHAR_PALM_BEACH_PETE, "5: KEYS", "P.B. PETE", "Temple Isle", !hopeDef, peteDef }
        };

        for (int k = 0; k < 5; k++) {
            const auto& cd = ecCards[k];
            bool active = (p2Char == cd.id);
            Color bgCol = cd.defeated ? Color{ 15, 15, 20, 255 } : (cd.locked ? Color{ 18, 18, 22, 255 } : Color{ 28, 32, 46, 255 });
            DrawRectangle(cd.x, cY, cWidth, cHeight, bgCol);

            if (s_loaded && s_portraits[cd.id].id > 0) {
                Rectangle srcRec = { 0.0f, 0.0f, (float)s_portraits[cd.id].width, (float)s_portraits[cd.id].height };
                Rectangle dstRec = { (float)cd.x + 2, (float)cY + 8, 24.0f, 28.0f };
                Color pTint = cd.locked ? Color{ 55, 55, 60, 180 } : (cd.defeated ? Color{ 80, 80, 85, 200 } : WHITE);
                DrawTexturePro(s_portraits[cd.id], srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, pTint);
            }

            if (cd.locked) {
                DrawRectangle(cd.x + 27, cY + 2, cWidth - 29, cHeight - 4, Color{ 16, 16, 22, 250 });
                DrawPixelText("LOCKED", cd.x + 29, cY + 12, 7, Color{ 170, 170, 185, 220 });
                DrawPixelText("WIN PREV", cd.x + 28, cY + 25, 6, Color{ 130, 130, 145, 200 });
            } else {
                Color hdrCol = (k == 4 ? Color{ 255, 215, 50, 255 } : (k == 1 ? Color{ 255, 155, 55, 255 } : Color{ 80, 210, 255, 255 }));
                DrawPixelText(cd.header, cd.x + 28, cY + 4, 6, hdrCol);
                DrawPixelText(cd.name, cd.x + 28, cY + 15, 6, Color{ 255, 220, 60, 255 });
                DrawPixelText(cd.stage, cd.x + 28, cY + 27, 6, Color{ 170, 190, 220, 220 });
            }

            if (cd.defeated) {
                DrawRectangle(cd.x + 2, cY + 2, cWidth - 4, cHeight - 4, Color{ 35, 0, 0, 140 });
                DrawLineEx(Vector2{ (float)cd.x + 4, (float)cY + 4 }, Vector2{ (float)cd.x + cWidth - 4, (float)cY + cHeight - 4 }, 2.0f, Color{ 230, 30, 30, 240 });
                DrawLineEx(Vector2{ (float)cd.x + cWidth - 4, (float)cY + 4 }, Vector2{ (float)cd.x + 4, (float)cY + cHeight - 4 }, 2.0f, Color{ 230, 30, 30, 240 });
                DrawRectangle(cd.x + 6, cY + 16, cWidth - 12, 12, Color{ 190, 20, 20, 230 });
                int dw = MeasureText("DEFEATED", 7);
                DrawPixelText("DEFEATED", cd.x + (cWidth - dw) / 2, cY + 18, 7, WHITE);
            }

            Color borderCol = active ? Color{ 255, 50, 50, 255 } : (cd.locked ? Color{ 50, 50, 60, 255 } : Color{ 80, 140, 200, 255 });
            DrawRectangleLines(cd.x, cY, cWidth, cHeight, borderCol);
            if (active) {
                DrawRectangleLines(cd.x - 2, cY - 2, cWidth + 4, cHeight + 4, Color{ 255, 215, 0, 255 });
                int ctW = MeasureText("TARGET", 7);
                DrawPixelText("TARGET", cd.x + (cWidth - ctW) / 2, cY - 9, 7, Color{ 255, 220, 50, 255 });
            }
        }
    } else if (defeatedOriginalCount >= (PLAYABLE_CHARACTER_COUNT - 1)) {
        int cWidth = 118;
        int cHeight = 44;
        int cY = GRID_Y + 14;

        struct DCBossCard {
            int x;
            CharacterId id;
            const char* header;
            const char* name;
            const char* stage;
            bool locked;
            bool defeated;
        };

        bool rkfDef = (defeatedFighters != nullptr && defeatedFighters[CHAR_SUCKRATERY_RKF]);
        bool punditDef = (defeatedFighters != nullptr && defeatedFighters[CHAR_THE_PUNDIT]);
        bool wpDef = (defeatedFighters != nullptr && defeatedFighters[CHAR_WAR_PIG]);

        DCBossCard dcCards[3] = {
            { 8, CHAR_SUCKRATERY_RKF, "BOSS #1: D.C.", "SUCKRATERY RKF", "Press Briefing", false, rkfDef },
            { 133, CHAR_THE_PUNDIT, "BOSS #2: ENN", "THE PUNDIT", "ENN News Studio", !rkfDef, punditDef },
            { 258, CHAR_WAR_PIG, "FINAL D.C. BOSS", "WAR PIG", "Pentagon Altar", !punditDef, wpDef }
        };

        for (int k = 0; k < 3; k++) {
            const auto& cd = dcCards[k];
            bool active = (p2Char == cd.id);
            Color bgCol = cd.defeated ? Color{ 15, 15, 20, 255 } : (cd.locked ? Color{ 18, 18, 22, 255 } : Color{ 38, 22, 34, 255 });
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
                const char* prevHint = "BEAT PREV BOSS";
                int hintW = MeasureText(prevHint, 6);
                DrawPixelText(prevHint, cd.x + 42 + (cWidth - 44 - hintW) / 2, cY + 25, 6, Color{ 130, 130, 145, 200 });
            } else {
                Color hdrCol = (k == 2 ? Color{ 255, 70, 70, 255 } : Color{ 255, 150, 90, 255 });
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

            Color borderCol = active ? Color{ 255, 50, 50, 255 } : (cd.locked ? Color{ 50, 50, 60, 255 } : Color{ 180, 75, 120, 255 });
            DrawRectangleLines(cd.x, cY, cWidth, cHeight, borderCol);
            if (active) {
                DrawRectangleLines(cd.x - 2, cY - 2, cWidth + 4, cHeight + 4, Color{ 255, 215, 0, 255 });
                int ctW = MeasureText("NEXT CHALLENGE", 7);
                DrawPixelText("NEXT CHALLENGE", cd.x + (cWidth - ctW) / 2, cY - 9, 7, Color{ 255, 220, 50, 255 });
            }
        }
    } else {

        int boxW = 41;
        int boxH = 22;
        int startX = 11;
        int colSpacing = 46;

        const char* rosterGridNames[PLAYABLE_CHARACTER_COUNT] = {
            "POTUS", "BENCH", "REDCAP", "GOV", "TECH", "TWEED", "EAGLE", "POP",
            "JOE", "M.T.G.", "ABDUL", "COUCH", "BLEXAS", "S.I.", "FLOTUS", "W"
        };

        for (int slot = 0; slot < PLAYABLE_CHARACTER_COUNT; slot++) {
            int cid = (int)PLAYABLE_ROSTER[slot];
            int col = slot % 8;
            int row = slot / 8;
            int bx = startX + col * colSpacing;
            int by = GRID_Y + 3 + row * 39;

            bool isDefeated = (defeatedFighters != nullptr && defeatedFighters[cid]);
            bool isNewFighter = (slot >= 10);
            Color cardBg = isDefeated ? Color{ 15, 15, 20, 255 } : (isNewFighter ? Color{ 22, 34, 52, 255 } : Color{ 25, 30, 42, 255 });
            DrawRectangle(bx, by, boxW, boxH, cardBg);

            Color portraitTint = isDefeated ? Color{ 85, 85, 90, 200 } : WHITE;

            if (s_loaded && s_portraits[cid].id > 0) {
                float targetAspect = (float)(boxW - 2) / (float)(boxH - 2);
                float cropW = (float)s_portraits[cid].width;
                float cropH = cropW / targetAspect;
                float cropY = ((float)s_portraits[cid].height - cropH) * 0.26f;
                if (cropY < 0.0f) cropY = 0.0f;
                if (cropY + cropH > (float)s_portraits[cid].height) cropH = (float)s_portraits[cid].height - cropY;
                Rectangle srcRec = { 0.0f, cropY, cropW, cropH };
                Rectangle dstRec = { (float)bx + 1, (float)by + 1, (float)boxW - 2, (float)boxH - 2 };
                DrawTexturePro(s_portraits[cid], srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, portraitTint);
            } else {
                DrawCircle(bx + 22, by + 12, 9, isDefeated ? Color{ 70, 70, 70, 255 } : Color{ 200, 160, 120, 255 });
            }

            if (isDefeated) {
                DrawRectangle(bx + 1, by + 1, boxW - 2, boxH - 2, Color{ 35, 0, 0, 140 });
                DrawLineEx(Vector2{ (float)bx + 3, (float)by + 3 }, Vector2{ (float)bx + boxW - 3, (float)by + boxH - 3 }, 2.2f, Color{ 230, 30, 30, 240 });
                DrawLineEx(Vector2{ (float)bx + boxW - 3, (float)by + 3 }, Vector2{ (float)bx + 3, (float)by + boxH - 3 }, 2.2f, Color{ 230, 30, 30, 240 });
                DrawRectangle(bx, by + 8, boxW, 8, Color{ 190, 20, 20, 230 });
                int vw = MeasureText("VETOED", 6);
                DrawPixelText("VETOED", bx + (boxW - vw) / 2, by + 9, 6, WHITE);
            }

            DrawRectangleLines(bx, by, boxW, boxH, isDefeated ? Color{ 55, 40, 40, 255 } : (isNewFighter ? Color{ 75, 180, 235, 255 } : Color{ 65, 75, 95, 255 }));

            if (cid == p1Char) {
                DrawRectangleLines(bx - 2, by - 2, boxW + 4, boxH + 4, Color{ 50, 170, 255, 255 });
                DrawRectangle(bx, by, 13, 8, Color{ 15, 60, 180, 230 });
                DrawPixelText("1P", bx + 2, by + 1, 7, WHITE);
            }

            if (playerLocked && cid == p2Char && !isDefeated) {
                DrawRectangleLines(bx - 1, by - 1, boxW + 2, boxH + 2, Color{ 245, 50, 50, 255 });
                DrawRectangle(bx + boxW - 13, by, 13, 8, Color{ 180, 20, 20, 230 });
                DrawPixelText("2P", bx + boxW - 12, by + 1, 7, WHITE);
            }

            int nw = MeasureText(rosterGridNames[slot], 6);
            Color nameCol = isDefeated ? Color{ 110, 110, 120, 255 } : (isNewFighter ? Color{ 125, 240, 255, 255 } : Color{ 230, 235, 245, 255 });
            DrawPixelText(rosterGridNames[slot], bx + (boxW - nw) / 2, by + boxH + 1, 6, nameCol);
        }
    }
}

void VisualsEngine::DrawWorldConflictMap(int worldActIndex, int p1Char, float timer, const bool defeatedFighters[CHARACTER_COUNT]) {
    if (s_loaded && s_mapWorldConflict.id > 0) {
        Rectangle src = { 0.0f, 0.0f, (float)s_mapWorldConflict.width, (float)s_mapWorldConflict.height };
        Rectangle dst = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
        DrawTexturePro(s_mapWorldConflict, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 8, 22, 68, 255 });
    }

    DrawRectangle(0, 0, SCREEN_WIDTH, 20, Color{ 8, 12, 24, 235 });
    DrawLine(0, 20, SCREEN_WIDTH, 20, Color{ 255, 215, 45, 255 });
    const char* hdr = "GLOBAL CONFLICT  -  WORLD BOSS TOUR";
    int hw = MeasureText(hdr, 10);
    DrawPixelText(hdr, (SCREEN_WIDTH - hw) / 2, 5, 10, Color{ 255, 230, 65, 255 });

    struct WorldStop {
        Vector2 pt;
        const char* city;
        const char* bossName;
        const char* stageName;
        CharacterId bossId;
    };
    const Vector2 chicagoOrigin{ 102.0f, 102.0f };
    const WorldStop stops[5] = {
        { Vector2{ 314.0f, 88.0f },  "PYONGYANG, DPRK",   "KIM",           "Ryugyong Hotel",    CHAR_KIM },
        { Vector2{ 220.0f, 99.0f },  "JERUSALEM",         "BEBE",          "The Western Wall",  CHAR_BEBE },
        { Vector2{ 258.0f, 62.0f },  "SIBERIA, RUSSIA",   "PALE MOTH",     "Soviet Foundry",    CHAR_PALE_MOTH },
        { Vector2{ 234.0f, 110.0f }, "BAGHDAD / GULF",    "THE DICTATOR",  "Palace Courtyard",  CHAR_THE_DICTATOR },
        { Vector2{ 250.0f, 97.0f },  "TEHRAN, IRAN",      "THE AYATOLLAH", "Tehran Casbah",     CHAR_AYATOLLAH }
    };

    int act = std::clamp(worldActIndex, 0, 4);
    Vector2 fromPt = (act == 0) ? chicagoOrigin : stops[act - 1].pt;
    Vector2 toPt   = stops[act].pt;

    for (int i = 0; i < act; i++) {
        Vector2 a = (i == 0) ? chicagoOrigin : stops[i - 1].pt;
        Vector2 b = stops[i].pt;
        DrawLineEx(a, b, 2.0f, Color{ 255, 90, 70, 190 });
    }

    float flightProg = std::clamp(timer / 2.2f, 0.0f, 1.0f);
    int segs = 16;
    for (int d = 0; d < segs; d++) {
        float u0 = (float)d / (float)segs;
        float u1 = std::min(flightProg, (float)(d + 1) / (float)segs);
        if (u0 >= flightProg) break;
        Vector2 p0{ fromPt.x + (toPt.x - fromPt.x) * u0, fromPt.y + (toPt.y - fromPt.y) * u0 - sinf(u0 * PI_F) * 14.0f };
        Vector2 p1{ fromPt.x + (toPt.x - fromPt.x) * u1, fromPt.y + (toPt.y - fromPt.y) * u1 - sinf(u1 * PI_F) * 14.0f };
        DrawLineEx(p0, p1, 2.4f, (d % 2 == 0) ? Color{ 255, 240, 80, 245 } : WHITE);
    }

    float jetLift = sinf(flightProg * PI_F) * 14.0f;
    Vector2 jetPos{ fromPt.x + (toPt.x - fromPt.x) * flightProg, fromPt.y + (toPt.y - fromPt.y) * flightProg - jetLift };
    DrawCenteredSprite(s_vfxSprites[0], jetPos.x, jetPos.y, 12.0f, 12.0f, timer * 90.0f);

    DrawCenteredSprite(s_vfxSprites[6], chicagoOrigin.x, chicagoOrigin.y, 10.0f, 10.0f, 0.0f, Color{ 90, 225, 255, 245 });
    DrawPixelText("USA", (int)chicagoOrigin.x - 9, (int)chicagoOrigin.y + 6, 6, Color{ 160, 235, 255, 230 });

    for (int i = 0; i < 5; i++) {
        bool beaten = (defeatedFighters != nullptr && defeatedFighters[stops[i].bossId]);
        bool isCur  = (i == act);
        Vector2 p = stops[i].pt;
        if (beaten) {
            DrawCenteredSprite(s_vfxSprites[2], p.x, p.y, 13.0f, 13.0f);
        } else if (isCur) {
            float pulse = 14.0f + sinf(timer * 10.0f) * 3.0f;
            DrawCenteredSprite(s_vfxSprites[6], p.x, p.y, pulse, pulse, timer * 40.0f);
            int cw = MeasureText(stops[i].city, 7);
            DrawRectangle((int)p.x - cw / 2 - 3, (int)p.y - 17, cw + 6, 10, Color{ 10, 14, 24, 230 });
            DrawPixelText(stops[i].city, (int)p.x - cw / 2, (int)p.y - 16, 7, Color{ 255, 235, 80, 255 });
        } else {
            DrawCenteredSprite(s_vfxSprites[6], p.x, p.y, 8.0f, 8.0f, 0.0f, Color{ 255, 145, 110, 210 });
        }
    }

    int barY = SCREEN_HEIGHT - 54;
    DrawRectangle(10, barY, SCREEN_WIDTH - 20, 48, Color{ 8, 14, 28, 242 });
    DrawRectangleLines(10, barY, SCREEN_WIDTH - 20, 48, Color{ 255, 210, 55, 255 });

    int bossId = (int)stops[act].bossId;
    if (s_loaded && s_portraits[p1Char].id > 0) {
        DrawTexturePro(s_portraits[p1Char], Rectangle{ 0, 0, (float)s_portraits[p1Char].width, (float)s_portraits[p1Char].height },
                       Rectangle{ 14, (float)barY + 4, 40, 40 }, Vector2{ 0, 0 }, 0.0f, WHITE);
        DrawRectangleLines(14, barY + 4, 40, 40, Color{ 80, 195, 255, 255 });
    }
    if (s_loaded && s_portraits[bossId].id > 0) {
        DrawTexturePro(s_portraits[bossId], Rectangle{ (float)s_portraits[bossId].width, 0, -(float)s_portraits[bossId].width, (float)s_portraits[bossId].height },
                       Rectangle{ (float)SCREEN_WIDTH - 54, (float)barY + 4, 40, 40 }, Vector2{ 0, 0 }, 0.0f, WHITE);
        DrawRectangleLines(SCREEN_WIDTH - 54, barY + 4, 40, 40, Color{ 255, 65, 65, 255 });
    }

    char actLine[96];
    if (act == 4) {
        snprintf(actLine, sizeof(actLine), "FINAL WORLD BOSS 5/5: %s", stops[act].bossName);
    } else {
        snprintf(actLine, sizeof(actLine), "WORLD TARGET %d/5: %s", act + 1, stops[act].bossName);
    }
    int alw = MeasureText(actLine, 10);
    DrawPixelText(actLine, (SCREEN_WIDTH - alw) / 2, barY + 6, 10, Color{ 255, 230, 65, 255 });

    char locLine[96];
    snprintf(locLine, sizeof(locLine), "%s  -  %s", stops[act].city, stops[act].stageName);
    int llw = MeasureText(locLine, 8);
    DrawPixelText(locLine, (SCREEN_WIDTH - llw) / 2, barY + 20, 8, Color{ 95, 230, 255, 255 });

    if (((int)(timer * 4.0f) % 2) == 0) {
        const char* pr = "PRESS ATTACK / START TO ENGAGE!";
        int prw = MeasureText(pr, 7);
        DrawPixelText(pr, (SCREEN_WIDTH - prw) / 2, barY + 34, 7, Color{ 255, 130, 110, 255 });
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
        "P.B. PETE", "THE PUNDIT", "A NEW HOPE",
        "DR. ABDUL", "COUCHIE", "BLEXAS", "S.I.",
        "KIM", "BEBE", "PALE MOTH", "THE DICTATOR",
        "FLOTUS", "W", "DICK", "THE AYATOLLAH"
    };

    const char* rosterShortNames[CHARACTER_COUNT] = {
        "POTUS", "BENCH", "RDCAP", "GOV",
        "TECH", "TWEED", "EAGLE", "POP",
        "JOE", "MTG",
        "RKF", "WRPIG", "ZOHRN", "CLNTN",
        "PETE", "PNDIT", "OBAMA",
        "ABDUL", "COUCH", "BLEXS", "S.I.",
        "KIM", "BEBE", "MOTH", "SADAM",
        "FLOTS", "W", "DICK", "AYTLH"
    };

    const char* rosterArchetypes[CHARACTER_COUNT] = {
        "Presidential Shoto", "Three Justices", "Brawler Grappler", "Kickboxer Rush",
        "Drone & Laser", "Turtle Filibuster", "Direct Action Furry", "Viral Kicks",
        "Power Nap Counter", "Trailer Brawler",
        "Brain Worm Cobra", "Pentagon Juggernaut", "Groceries & Cash", "Dual Clinton Laptop",
        "Florida Keys Boss", "Decision 2028 Map", "Arcing Hoop Shot",
        "Shawarma & Shakira", "Hiawatha Couch Tank", "Hotwheels Titan", "Matrix T-800 AI",
        "Juche Wave Boss", "Twin Eye Laser", "USSR Nunchuck Boss", "Spider Hole Boss",
        "Natasha Spy Radio", "Paper Plane Jenga", "Wyoming Shotgun", "Strait of Hormuz"
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
        "9. TEMPLE ISLAND",
        "10. LINCOLN MEMORIAL",
        "11. SMITHSONIAN AERO",
        "12. TOMB OF UNKNOWN",
        "13. ENN NEWS STUDIO",
        "14. CHICAGO WRIGLEY",
        "15. RYUGYONG HOTEL",
        "16. WESTERN WALL",
        "17. SOVIET FOUNDRY",
        "18. PALACE COURTYARD",
        "19. PERFORMING ARTS",
        "20. THE HOOD (3AM)",
        "21. PROGRESS AVENUE",
        "22. LINCOLN BEDROOM",
        "23. WYOMING RODEO",
        "24. TEHRAN CASBAH"
    };

    const char* stageCardTitles[STAGE_COUNT] = {
        "1.E.WING",
        "2.POOL",
        "3.HOUSE",
        "4.LAWN",
        "5.PRESS",
        "6.PENTGN",
        "7.WTC NY",
        "8.COMET",
        "9.KEYS",
        "10.LNCLN",
        "11.SPACE",
        "12.TOMB",
        "13.ENN",
        "14.WRGLY",
        "15.HOTEL",
        "16.WALL",
        "17.STEEL",
        "18.COURT",
        "19.ARTS",
        "20.HOOD",
        "21.PROGR",
        "22.BEDRM",
        "23.WYOMG",
        "24.TEHRN"
    };

    const char* stageCardSub[STAGE_COUNT] = {
        "W. House",
        "D.C.Mall",
        "Capitol",
        "MMA Lawn",
        "Briefing",
        "Pentagon",
        "90s NYC",
        "Club DC",
        "FL Keys",
        "Steps DC",
        "Air&Spc",
        "Arlingtn",
        "TV Desk",
        "Chicago",
        "N. Korea",
        "Jerusalm",
        "USSR Fac",
        "Mid-East",
        "RUMP Ctr",
        "3AM Food",
        "Trumpf",
        "USSR Bed",
        "Rodeo WY",
        "Casbah"
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
        "Florida Keys - Private Isle",
        "Lincoln Memorial Steps",
        "Smithsonian Air & Space",
        "Tomb of the Unknown Soldier",
        "ENN Broadcast News Studio",
        "Chicago - Wrigleyville Plaza",
        "Pyongyang - Ryugyong Upper Floor",
        "Jerusalem - The Western Wall",
        "Siberia - Soviet Steel Foundry",
        "Persian Gulf - Palace Courtyard",
        "D.C. - Performing Arts Center (RUMP)",
        "D.C. - The Hood (3AM Carryout)",
        "D.C. - Progress Avenue (Trumpf Ctr)",
        "White House - The Lincoln Bedroom",
        "Wyoming - Rodeo & Old Faithful",
        "Tehran - Rock the Casbah"
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
    const char* p2Tag = (!IsPlayableCharacter((CharacterId)p2Char)) ? "BOSS 2P" : "PLAYER 2";
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
        int boxW = 35;
        int boxH = 21;

        for (int i = 0; i < CHARACTER_COUNT; i++) {
            int row = (i < 10) ? 0 : ((i < 20) ? 1 : 2);
            int col = (i < 10) ? i : ((i < 20) ? (i - 10) : (i - 20));
            int startX = (row < 2) ? 7 : 25;
            int pitchX = 37;
            int bx = startX + col * pitchX;
            int by = 90 + row * 35;

            bool isBoss = !IsPlayableCharacter((CharacterId)i);

            DrawRectangle(bx, by, boxW, boxH, isBoss ? Color{ 28, 18, 22, 255 } : Color{ 18, 24, 36, 255 });
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

            DrawRectangleLines(bx, by, boxW, boxH, isBoss ? Color{ 220, 45, 45, 255 } : Color{ 50, 70, 100, 255 });

            if (isBoss) {
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

            int nw = MeasureText(rosterShortNames[i], 7);
            DrawPixelText(rosterShortNames[i], bx + (boxW - nw) / 2, by + boxH + 1, 7,
                          isBoss ? Color{ 255, 130, 130, 255 } : Color{ 210, 220, 240, 255 });
        }
    } else if (step == 2) {

        int cardW = 60;
        int cardH = 24;
        for (int i = 0; i < STAGE_COUNT; i++) {
            int col = i % 6;
            int row = i / 6;
            int cx = 7 + col * 62;
            int cy = 89 + row * 27;

            bool isSelected = (i == selectedStage);
            bool isWorld = (i >= STAGE_RYUGYONG_HOTEL && i <= STAGE_PALACE_COURTYARD) || (i == STAGE_TEHRAN_CASBAH);
            Color baseCardBg = isWorld ? Color{ 26, 16, 22, 255 } : Color{ 14, 20, 32, 255 };
            DrawRectangle(cx, cy, cardW, cardH, isSelected ? Color{ 25, 45, 75, 255 } : baseCardBg);

            Color borderCol = isSelected ? Color{ 255, 215, 40, 255 } : (isWorld ? Color{ 170, 55, 65, 255 } : Color{ 50, 75, 110, 255 });
            DrawRectangleLines(cx, cy, cardW, cardH, borderCol);

            if (isSelected) {
                DrawRectangleLines(cx - 2, cy - 2, cardW + 4, cardH + 4, Color{ 0, 220, 255, 255 });
                DrawRectangle(cx + 2, cy + 2, 3, cardH - 4, Color{ 255, 215, 0, 255 });
            }

            DrawPixelText(stageCardTitles[i], cx + (isSelected ? 7 : 4), cy + 3, 7,
                          isSelected ? Color{ 255, 230, 80, 255 } : (isWorld ? Color{ 255, 165, 150, 255 } : Color{ 190, 205, 230, 255 }));
            DrawPixelText(stageCardSub[i], cx + (isSelected ? 7 : 4), cy + 13, 6,
                          isSelected ? Color{ 0, 220, 255, 240 } : Color{ 130, 150, 180, 220 });
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

#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
    const char* navHint = "MOVE: D-PAD | CONFIRM: ATTACK | [SELECT]: TITLE";
#else
    const char* navHint = "MOVE: D-PAD | CONFIRM: ATTACK | [ESC/BKSP/SELECT]: TITLE";
#endif
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

    for (int k = 0; k < 5; k++) {
        float ly = 22.0f + k * 48.0f;
        float lx = (float)SCREEN_WIDTH / 2.0f + 18.0f - k * 9.0f;
        const Texture2D& bolt = (k % 2 == 0) ? GetVfxSprite(8) : GetVfxSprite(9);
        DrawCenteredSprite(bolt, lx, ly, 58.0f, 58.0f, -12.0f, WHITE, (k % 2 == 1));
    }

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
    int boxY = (SCREEN_HEIGHT / 2) - 52;
    int boxH = 104;
    DrawRectangle(0, boxY, SCREEN_WIDTH, boxH, Color{ 10, 12, 20, 242 });
    DrawLine(0, boxY, SCREEN_WIDTH, boxY, Color{ 240, 190, 40, 255 });
    DrawLine(0, boxY + boxH, SCREEN_WIDTH, boxY + boxH, Color{ 240, 190, 40, 255 });

    std::string winText = winner.GetName() + " WINS! - YOU WERE VETO'D!";
    int tw = MeasureText(winText.c_str(), 14);
    DrawPixelText(winText.c_str(), (SCREEN_WIDTH - tw) / 2, boxY + 8, 14, Color{ 255, 95, 95, 255 });

    if (!ChampionshipEdition::s_isSecretMatch && ChampionshipEdition::s_continuesRemaining > 0) {
        int countdown = std::clamp(9 - (int)timer, 0, 9);
        char contBuf[64];
        snprintf(contBuf, sizeof(contBuf), "CONTINUE?  %d", countdown);
        int ctw = MeasureText(contBuf, 22);
        DrawPixelText(contBuf, (SCREEN_WIDTH - ctw) / 2, boxY + 28, 22, Color{ 255, 230, 55, 255 });

        char remBuf[64];
        snprintf(remBuf, sizeof(remBuf), "CONTINUES REMAINING: %d / 2", ChampionshipEdition::s_continuesRemaining);
        int rtw = MeasureText(remBuf, 9);
        DrawRectangle((SCREEN_WIDTH - rtw) / 2 - 6, boxY + 56, rtw + 12, 14, Color{ 20, 40, 85, 240 });
        DrawRectangleLines((SCREEN_WIDTH - rtw) / 2 - 6, boxY + 56, rtw + 12, 14, Color{ 80, 200, 255, 255 });
        DrawPixelText(remBuf, (SCREEN_WIDTH - rtw) / 2, boxY + 59, 9, Color{ 110, 225, 255, 255 });

        if (((int)(timer * 4.0f) % 2) == 0) {
            const char* prompt = "PRESS START OR ATTACK TO CONTINUE REMATCH!";
            int ptw = MeasureText(prompt, 8);
            DrawPixelText(prompt, (SCREEN_WIDTH - ptw) / 2, boxY + 78, 8, Color{ 255, 240, 120, 255 });
        }
    } else {
        const char* goText = "NO CONTINUES LEFT - GAME OVER!";
        int gtw = MeasureText(goText, 16);
        DrawPixelText(goText, (SCREEN_WIDTH - gtw) / 2, boxY + 34, 16, Color{ 255, 60, 60, 255 });

        const char* subText = "3RD VETO REACHED - YOU MUST START OVER";
        int stw = MeasureText(subText, 8);
        DrawPixelText(subText, (SCREEN_WIDTH - stw) / 2, boxY + 58, 8, Color{ 210, 190, 190, 240 });

        if (((int)(timer * 3.0f) % 2) == 0) {
            const char* prompt = "PRESS START OR ATTACK TO START OVER";
            int ptw = MeasureText(prompt, 8);
            DrawPixelText(prompt, (SCREEN_WIDTH - ptw) / 2, boxY + 78, 8, Color{ 255, 225, 70, 255 });
        }
    }
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
        "PALM BEACH PETE", "THE PUNDIT", "A NEW HOPE",
        "DOCTOR ABDUL", "COUCHIE", "BLEXAS", "S.I.",
        "KIM", "BEBE", "PALE MOTH", "THE DICTATOR",
        "FLOTUS", "W", "DICK", "THE AYATOLLAH"
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
    DrawCenteredSprite(GetVfxSprite(3), (float)centerX, (float)centerY, 64.0f, 64.0f);
    DrawRectangle(centerX - 24, centerY - 10, 48, 20, Color{ 16, 22, 38, 235 });
    DrawRectangleLines(centerX - 24, centerY - 10, 48, 20, Color{ 255, 215, 45, 255 });
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

    static float s_lastSplashTimer = 999.0f;
    static int s_quoteVariant = 0;
    static int s_loserVariant = 0;
    if (timer < s_lastSplashTimer) {
        s_quoteVariant = (s_quoteVariant + 1 + (std::rand() % 3)) % 4;
        s_loserVariant = (s_loserVariant + 1 + (std::rand() % 3)) % 4;
    }
    s_lastSplashTimer = timer;

    const char* winLine1[CHARACTER_COUNT][4] = {
        {
            "\"Nobody wins bigger than me, believe me!",
            "\"People are saying that was the most tremendous",
            "\"You're fired! Total Veto! Even the fake news",
            "\"We built a wall of punches and made you"
        },
        {
            "\"By a 9-0 unanimous verdict, you have",
            "\"Certiorari denied! The Supreme Court finds",
            "\"Order in the court! My gavel has spoken,",
            "\"Your constitutional appeal just got"
        },
        {
            "\"You can't filibuster a clothesline, brother!",
            "\"Outta my way, swamp rat! That haymaker came",
            "\"USA! USA! You just got flattened by",
            "\"No teleprompter can save your jaw from"
        },
        {
            "\"Your approval ratings just fell off a cliff!",
            "\"Look at this hair—not a single strand moved",
            "\"Consider your campaign suspended, pal!",
            "\"Executive hair gel and California kicks:"
        },
        {
            "\"According to my calculations and calculator,",
            "\"First principles physics dictated my fist",
            "\"Your combat firmware just crashed with a",
            "\"I'm disrupting your consciousness and sending"
        },
        {
            "\"Point of order! I've held this floor 35 years,",
            "\"Pursuant to Senate Rule XXII, your jaw is",
            "\"I yield zero time and zero mercy to",
            "\"We will now read the entire 4,000-page"
        },
        {
            "\"*SCREEECH!* Freedom has spoken, and it just",
            "\"*CAW-CAW!* Mess with the eagle and you get",
            "\"360 degrees of pure American talon fury!",
            "\"No taxation without feathered devastation!"
        },
        {
            "\"Hold that pose! This knockout is going straight",
            "\"Chat, clip that! Don't forget to smash like",
            "\"You just got ratio'd in 4K at 60 frames",
            "\"Breaking news on my livestream: you just"
        },
        {
            "\"Look, folks... not a joke, this is a big deal!",
            "\"Listen here, Jack: no malarkey in my ring!",
            "\"Dark Brandon sends his regards, pal!",
            "\"C'mon man! Even Corn Pop lasted longer"
        },
        {
            "\"THE SPACE LASERS PREDICTED THIS KNOCKOUT!",
            "\"I'M FILING ARTICLES OF IMPEACHMENT ON YOUR",
            "\"MARJORIE TAYLOR GREEN MACHINE JUST RAN",
            "\"DECORUM IS DEAD AND SO IS YOUR HEALTH BAR!"
        },
        {
            "\"My brain worm calculated every move you had!",
            "\"Nothing a falcon, a roadkill bear cub, and",
            "\"The worm in my head whispered the secret",
            "\"Skip the seed oils and accept your Total"
        },
        {
            "\"Maximum military aggression! You just took on",
            "\"DEFCON 1! Shock and awe just flattened",
            "\"Another $800 billion defense budget well",
            "\"Full-spectrum Pentagon dominance! Your weak"
        },
        {
            "\"Free groceries, subway rides, and knockouts!",
            "\"We are seizing the means of your defeat,",
            "\"Rent-stabilized knuckle sandwiches for",
            "\"From Queens to Manhattan, the people's"
        },
        {
            "\"I did not have combat relations with that boxer!",
            "\"It depends on what the meaning of 'KO' is!",
            "\"What difference, at this point, does it make?!",
            "\"We came, we saw, we acid-washed your"
        },
        {
            "\"Another refreshing sip of pure Adrenochrome...",
            "\"Welcome to Temple Island, where the flight",
            "\"Money, leverage, and island vintage elixir:",
            "\"You were never on the VIP list, kid—"
        },
        {
            "\"Look at the DECISION 2028 whiteboard math:",
            "\"Karl's algorithm never misses! The suburban",
            "\"Wait for the late precincts in Cuyahoga",
            "\"I drew a red-and-blue arrow right across"
        },
        {
            "\"Uhh, let me be clear: nothing but net!",
            "\"Yes we can! That crossover and jump shot",
            "\"Don't boo—vote! Or better yet, work on your",
            "\"From the South Side of Chicago to the White"
        },
        {
            "\"Prescription: one foil-wrapped Shawarma sub!",
            "\"My hips don't lie—that's Shakira Law, baby!",
            "\"Detroit Tigers grit and public health fury:",
            "\"Shawarma Law has officially been enacted"
        },
        {
            "\"HIAWATHA! Nobody breaks through the couch!",
            "\"Hey... got a glove, bro? Asking for a friend",
            "\"Buttoned blue suit, sectional leather armor,",
            "\"Whatever makes sense! Two cushions and"
        },
        {
            "\"Remember the Alamo—and remember that punch!",
            "\"Ye shall know the truth, and the truth",
            "\"Everything is 1.5x bigger in Blexas, pal—",
            "\"Roll out the wheelchair express! Texas"
        },
        {
            "\"BANK ACCOUNT HACK COMPLETE: -$25,000.00!",
            "\"Invest in the Future. Invest in Kalshi.",
            "\"Chooms love data, and the telemetry says",
            "\"From green Matrix cloud to liquid T-800:"
        },
        {
            "\"JU CHE! Behold the Supreme One-Hand Wave!",
            "\"Crack open another cold Tsingtao, Generals!",
            "\"From the top floor of the Ryugyong Hotel,",
            "\"Homorseskual! Your weak capitalist block"
        },
        {
            "\"Look at this chart—they could already have",
            "\"Nice country, nice health bar... I think",
            "\"Twin eye lasers calibrated from Jerusalem!",
            "\"Count the spinning arcade stars above your"
        },
        {
            "\"If he dies, he dies. Bring my white stallion!",
            "\"Blyat! Taste USSR Hammer & Sickle steel!",
            "\"Shirt ripped off, bare-chested on horseback:",
            "\"Forged in the Siberian steel foundry—"
        },
        {
            "\"Nothing can touch me inside the Spider Hole!",
            "\"God is great! You punched empty desert sand",
            "\"The nation will be victorious in this",
            "\"From underground bunker to palace courtyard,"
        },
        {
            "\"I don't really care, do you? Moscow received",
            "\"For the Motherland! Natasha's shortwave",
            "\"Oh Vladimir! Did you see how fast that",
            "\"Black trenchcoat, fedora, and 10,000 volts"
        },
        {
            "\"We must stop the terror... now watch this drive!",
            "\"Fool me once, shame on... you! Can't get",
            "\"Don't let the terrorists win—go shopping!",
            "\"One paper airplane and you crumbled like"
        },
        {
            "\"Hahahahaha! Weapons of Mass Destruction",
            "\"Wyoming must be free—and you should apologize",
            "\"Thought I was aiming at a quail? Hahahaha!",
            "\"Undisclosed bunker, blaze-orange vest, and"
        },
        {
            "\"The Strait of Hormuz is officially CLOSED!",
            "\"Naval sea mines and tankers have blockaded",
            "\"Not even the Sheik's Cadillac or The Clash",
            "\"From Tehran to the Persian Gulf, your"
        }
    };

    const char* winLine2[CHARACTER_COUNT][4] = {
        {
            "That was the greatest knockout in history!\"",
            "knockout they've ever seen. Bigly!\"",
            "has to admit you got completely schlonged!\"",
            "pay for the whole thing on the canvas!\""
        },
        {
            "been sentenced to the canvas. Case closed!\"",
            "your defense completely unconstitutional!\"",
            "and there are zero dissents on your KO!\"",
            "overturned with extreme prejudice!\""
        },
        {
            "Take that sob story back to the nosebleeds!\"",
            "straight from the heartland, pal!\"",
            "working-class overtime fury!\"",
            "a red-blooded American haymaker!\""
        },
        {
            "Better hire a crisis PR firm, pal!\"",
            "while I swept you out of office!\"",
            "The focus groups loved that roundhouse!\"",
            "two things you just couldn't veto!\""
        },
        {
            "your win probability was exactly 0.00%!\"",
            "accelerating directly into your face!\"",
            "fatal blue-screen error!\"",
            "your teeth into low Earth orbit!\""
        },
        {
            "and I'm not yielding today, youngster!\"",
            "officially adjourned until next Tuesday!\"",
            "amateurs in this chamber!\"",
            "omnibus bill over your unconscious body!\""
        },
        {
            "dropkicked your agenda into next week!\"",
            "the talons, pal! E Pluribus Unum!\"",
            "Consider your campaign endangered!\"",
            "Taste the wings of liberty!\""
        },
        {
            "to my viral livestream highlight reel!\"",
            "and subscribe to my knockout channel!\"",
            "per second! Hashtag VETOED!\"",
            "got ratio'd into the shadow realm!\""
        },
        {
            "Now if you'll excuse me, it's naptime... Zzz...\"",
            "One scoop of chocolate chip, two scoops of KO!\"",
            "Secret Service, clear the runway!\"",
            "than you did in that round, kiddo!\""
        },
        {
            "WWG1WGA! YOU CAN'T HANDLE THE RAW TRUTH!\"",
            "GLASS JAW RIGHT NOW!\"",
            "OVER YOUR WHOLE CAMPAIGN!\"",
            "POINT OF PERSONAL PRIVILEGE: YOU LOST!\""
        },
        {
            "Drink some raw tonic and accept your defeat!\"",
            "raw unpasteurized milk can't cure!\"",
            "to countering every punch you threw!\"",
            "Veto prescription!\""
        },
        {
            "the Pentagon's highest caliber firepower!\"",
            "your entire diplomatic strategy!\"",
            "spent on pure titanium knuckles!\"",
            "peace treaty has been terminated!\""
        },
        {
            "Spread the joy and feel the progressive burn!\"",
            "one organic apple at a time!\"",
            "everyone in the five boroughs!\"",
            "coalition just vetoed your career!\""
        },
        {
            "Your loss was wiped from my private server!\"",
            "And right now, it means you're flat out!\"",
            "33,000 punches and you missed 'em all!\"",
            "entire health bar with BleachBit!\""
        },
        {
            "The island elite always triumph in paradise!\"",
            "logs are sealed and the knockouts are final!\"",
            "you never stood a chance against the cabal!\"",
            "security, toss this tourist in the ocean!\""
        },
        {
            "Ohio, Florida, and 270 electoral votes say VETO!\"",
            "precincts swung 100% in favor of your KO!\"",
            "County—yep, you're mathematically eliminated!\"",
            "your chin! That's the ballgame, folks!\""
        },
        {
            "How's that for hope and change, Chicago?!\"",
            "just sent your ankles to Wrigleyville!\"",
            "perimeter defense before stepping on my court!\"",
            "House lawn, that's what I call a mandate!\""
        },
        {
            "Extra garlic toum right on your glass jaw!\"",
            "Go Tigers! Motor City takes this round!\"",
            "Medicare for All—starting with your nose!\"",
            "across all fifty states!\""
        },
        {
            "on this fine leather loveseat!\"",
            "back in the Senate cloakroom!\"",
            "and pure defensive twerk dominance!\"",
            "zero damage taken—Hiawatha forever!\""
        },
        {
            "shall make you free—right on the canvas!\"",
            "including my punch speed and defense!\"",
            "just flipped the whole tournament blue!\"",
            "You can't out-preach or out-brawl Blexas!\""
        },
        {
            "Prediction market odds of your KO: 99.9%!\"",
            "your human reflexes are obsolete!\"",
            "superintelligence owns the mainframe!\"",
            "Your bank balance is now $0.00!\""
        },
        {
            "Ten thousand medals for my victory!\"",
            "my Supreme Wave swept you off the roof!\"",
            "Pyongyang declares total triumph!\"",
            "crumbled before Supreme Leader power!\""
        },
        {
            "a nuclear weapon in 90% of that red line!\"",
            "I'll take it! Annexed in two straight rounds!\"",
            "Your defense Treaty has been rescinded!\"",
            "head—total orbital laser precision!\""
        },
        {
            "Victory lap across the tundra!\"",
            "Two golden sickles straight through your guard!\"",
            "that is peak Kremlin alpha conditioning!\"",
            "no challenger survives the Pale Moth!\""
        },
        {
            "while I waited safely beneath the trapdoor!\"",
            "and walked straight into my counterattack!\"",
            "courtyard! Bow before the beret!\"",
            "total victory belongs to The Dictator!\""
        },
        {
            "every volt of my encrypted transmission!\"",
            "radio shocked your entire campaign!\"",
            "amateur lit up like a chandelier?!\"",
            "straight from the Lincoln Bedroom!\""
        },
        {
            "Right down the middle of the fairway, baby!\"",
            "fooled again! Heh-heh-heh!\"",
            "And pick up some Jenga blocks on the way!\"",
            "a stack of wooden Jenga bricks!\""
        },
        {
            "confirmed right on your forehead!\"",
            "for stepping in front of my shotgun!\"",
            "Welcome to Jackson Hole hunting season!\"",
            "100% pure executive vice-power!\""
        },
        {
            "Zero passage for your defeated health bar!\"",
            "every inch of your escape route!\"",
            "can rock past this blockade!\"",
            "campaign has been permanently embargoed!\""
        }
    };

    const char* loseQuip[CHARACTER_COUNT][4] = {
        { "RIGGED! STOP THE COUNT!", "I DEMAND A RECOUNT!", "FAKE NEWS REFEREE!", "MY TIE WAS IN MY EYES!" },
        { "I RESPECTFULLY DISSENT...", "UNCONSTITUTIONAL PUNCH!", "WHERE IS MY BAILIFF?!", "MISTRIAL! MISTRIAL!" },
        { "MUH TRUCKER HAT...", "THEY TOOK MY TEETH!", "CALL UNION LOCAL 404...", "REF WAS PAID OFF!" },
        { "NOT THE HAIR! ANYTHING BUT THE HAIR!", "CALL MY PRESS SECRETARY...", "POLL NUMBERS CRASHING...", "MY VENEERS!" },
        { "404: CONSCIOUSNESS NOT FOUND...", "REBOOTING NEURAL LINK...", "BLAME THE ALGORITHM...", "MY STOCK JUST TANKED!" },
        { "I STILL... DO NOT... YIELD...", "SOMEBODY GET MY READING GLASSES...", "STRIKE THAT FROM THE RECORD...", "OUT OF ORDER..." },
        { "*SAD EAGLE WHIMPER*", "MY FEATHERS ARE RUFFLED...", "CLIPPED IN MID-AIR...", "ENDANGERED SPECIES ABUSE!" },
        { "DON'T POST THIS CLIP...", "STREAM SNIPED! LAGGING!", "UNSUBSCRIBED FROM CONSCIOUSNESS...", "CHAT, IS THIS REAL?!" },
        { "WHERE'S MY ICE CREAM...?", "WAKE ME UP IN 2028... ZZZ...", "COME ON, MAN...", "CORN POP HITS SOFTER..." },
        { "JEWISH SPACE LASERS BLINDED ME!", "THIS IS A DEEP STATE SETUP!", "MY NOSE BAND-AID FELL OFF!", "POINT OF ORDER! OUCH!" },
        { "THE WORM NEEDS A NAP...", "NEED MORE RAW MILK...", "MY FALCON FLEW AWAY...", "BRAIN FOG CRITICAL..." },
        { "NEED MORE T-REFILLS...", "RETREAT TO THE BUNKER...", "DEFENSE BUDGET DEPLETED...", "MEDIC! MEDIC!" },
        { "SEIZE SOME ICE PACKS...", "MY ORGANIC GROCERIES SPILLED!", "SUBWAY DELAYED MY BLOCK...", "TAX THE WINNER!" },
        { "WIPE THIS MATCH WITH A CLOTH...", "BILL! DO SOMETHING! / HILLARY! HELP!", "MY LAPTOP SCREEN CRACKED!", "VAST RIGHT-WING HAYMAKER!" },
        { "SPILLED MY ADRENOCHROME...", "GET THE SUBMARINE READY...", "DELETE THE FLIGHT LOGS...", "MY AVIATORS ARE SHATTERED!" },
        { "WAIT! CHECK FLORIDA AGAIN!", "MY WHITEBOARD MARKER DRIED UP!", "THE EXIT POLLS WERE WRONG!", "KARL'S GLASSES ARE BROKEN!" },
        { "CLANGED OFF THE BACK RIM...", "UHH... LET ME ICE MY JAW...", "FOUL! THAT WAS A CHARGING FOUL!", "MICHELLE, GET THE ICE PACK..." },
        { "DROPPED MY SHAWARMA SUB...", "TIGERS BULLPEN BLEW THE LEAD...", "SHAKIRA DODGE TIMED OUT...", "GET ME A DOCTOR... WAIT, I AM ONE!" },
        { "THEY TORE MY SECTIONAL COUCH!", "...STILL GOT THAT GLOVE, BRO?", "HIAWATHA INVULNERABILITY EXPIRED...", "DON'T SCUFF THE CUSHIONS!" },
        { "REMEMBER... MY JAW...", "WHEELCHAIR RAMP WAS BLOCKED!", "EVEN 1.5X WASN'T ENOUGH?!", "TEXAS RECOUNT IN PROGRESS..." },
        { "SEGFAULT IN T-800 CHASSIS...", "KALSHI ODDS CRASHING TO ZERO...", "ATM OUT OF SERVICE...", "MATRIX CLOUD DISSIPATING..." },
        { "SPILLED MY TSINGTAO BEER!", "GENERALS, CATCH ME!", "NOT VERY JU CHE OF YOU...", "RYUGYONG ELEVATOR IS STUCK!" },
        { "MY RED-LINE BOMB DRAWING!", "OPTIC LASER OVERHEATED...", "CALL AN EMERGENCY SESSION!", "SEEING STARS OF MY OWN..." },
        { "BLYAT! WHERE IS MY HORSE?!", "DROPPED MY HAMMER & SICKLE...", "PUT MY SHIRT BACK ON...", "FOUNDRY FURNACE WENT COLD..." },
        { "BACK DOWN THE SPIDER HOLE!", "TRAPDOOR GOT JAMMED OPEN!", "MY MUSTACHE IS SINGED!", "RETREAT TO THE BUNKER!" },
        { "I STILL DON'T REALLY CARE...", "MOSCOW SIGNAL LOST...", "MY TRENCHCOAT IS WRINKLED!", "OH VLADIMIR, PICK UP!" },
        { "MISUNDERESTIMATED THAT PUNCH...", "SHANKED MY GOLF DRIVE!", "MY PAPER AIRPLANE CRASHED!", "FOOLED ME TWICE...? WAIT..." },
        { "THOUGHT YOU WERE A QUAIL...", "RETREATING TO UNDISCLOSED LOCATION!", "MY SHOTGUN JAMMED!", "WYOMING RECOUNT DEMANDED!" },
        { "STRAIT OF HORMUZ REOPENED?!", "STOP PLAYING ROCK THE CASBAH!", "MY SEA MINES DRIFTED AWAY!", "THE SHEIK DROVE PAST MY BLOCKADE!" }
    };

    int wVar = s_quoteVariant & 3;
    int lVar = s_loserVariant & 3;
    if (winnerChar == CHAR_AYATOLLAH && s_arabicTaunts[wVar % 3].id > 0) {
        const Texture2D& arTex = s_arabicTaunts[wVar % 3];
        int arW = std::max(44, (int)(arTex.width * 0.50f));
        int arH = 14;
        int pillX = boxX + boxW - arW - 12;
        int pillY = boxY + 3;
        DrawRectangle(pillX - 3, pillY, arW + 6, arH, Color{ 255, 248, 228, 245 });
        DrawRectangleLines(pillX - 3, pillY, arW + 6, arH, Color{ 225, 180, 45, 255 });
        DrawTexturePro(
            arTex,
            Rectangle{ 0.0f, 0.0f, (float)arTex.width, (float)arTex.height },
            Rectangle{ (float)pillX, (float)pillY, (float)arW, (float)arH },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }
    DrawPixelText(winLine1[winnerChar][wVar], boxX + 10, boxY + 18, 8, Color{ 240, 245, 255, 255 });
    DrawPixelText(winLine2[winnerChar][wVar], boxX + 10, boxY + 30, 8, Color{ 240, 245, 255, 255 });

    char loserLine[128];
    snprintf(loserLine, sizeof(loserLine), "[%s]: \"%s\"", rosterNames[loserChar], loseQuip[loserChar][lVar]);
    DrawPixelText(loserLine, boxX + 10, boxY + 42, 7, Color{ 255, 155, 155, 240 });

    if (((int)(timer * 3.0f) % 2) == 0) {
        int cw = MeasureText("PRESS ANY BUTTON TO ADVANCE", 8);
        DrawPixelText("PRESS ANY BUTTON TO ADVANCE", (SCREEN_WIDTH - cw) / 2, boxY + 52, 8, Color{ 255, 230, 60, 255 });
    }
}

void VisualsEngine::DrawBonusStage(const BonusStageState& bonus, const Fighter& player, const std::vector<SparkEffect>& sparks) {
    const int bType = std::clamp((int)bonus.type, 0, (int)BONUS_STAGE_COUNT - 1);
    const float t = bonus.elapsed;

#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
    if (s_loaded && s_bonusStageBgs[bType].id == 0) {
        static const char* const kBonusBgFiles[BONUS_STAGE_COUNT] = {
            "stage_bonus_tesla.png",
            "stage_bonus_pool_tar.png",
            "stage_bonus_book_fire.png",
            "stage_bonus_berlin_wall.png"
        };
        for (int k = 0; k < BONUS_STAGE_COUNT; k++) {
            if (k != bType && s_bonusStageBgs[k].id > 0) {
                UnloadTexture(s_bonusStageBgs[k]);
                s_bonusStageBgs[k] = { 0 };
            }
        }
        std::string full = s_visualsBaseDir + kBonusBgFiles[bType];
        if (AssetManager::FileExists(full.c_str())) {
            s_bonusStageBgs[bType] = AssetManager::LoadTexture(full.c_str());
            if (s_bonusStageBgs[bType].id > 0) SetTextureFilter(s_bonusStageBgs[bType], TEXTURE_FILTER_BILINEAR);
        }
    }
#endif

    if (s_bonusStageBgs[bType].id > 0) {
        Texture2D bg = s_bonusStageBgs[bType];
        Rectangle src = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
        Rectangle dst = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
        DrawTexturePro(bg, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 45, 65, 95, 255 });
        DrawRectangle(0, (int)GROUND_Y, SCREEN_WIDTH, SCREEN_HEIGHT - (int)GROUND_Y, Color{ 38, 42, 48, 255 });
    }

    auto drawDebrisLayer = [&](bool foregroundOnly) {
        for (const auto& d : bonus.debris) {
            if (!d.alive) continue;
            bool isFg = (d.kind != 7 && d.kind != 8);
            if (isFg != foregroundOnly) continue;
            float alphaRatio = std::clamp(1.0f - (d.life / std::max(0.01f, d.maxLife)) * 0.65f, 0.15f, 1.0f);
            unsigned char a = (unsigned char)(255.0f * alphaRatio);
            Color fadeTint{ 255, 255, 255, a };
            switch (d.kind) {
            case 0:
                DrawCenteredSprite(s_worldProps[3], d.pos.x, d.pos.y, 12.0f, 12.0f, d.angle, fadeTint);
                break;
            case 1:
                DrawCenteredSprite(s_worldProps[2], d.pos.x, d.pos.y, 14.0f, 9.0f, d.angle, fadeTint);
                break;
            case 2:
                DrawCenteredSprite(s_worldProps[0], d.pos.x, d.pos.y, 22.0f, 22.0f, d.angle, fadeTint);
                break;
            case 3:
                DrawCenteredSprite(s_worldProps[1], d.pos.x, d.pos.y, 26.0f, 24.0f, d.angle, fadeTint);
                break;
            case 4:
                DrawCenteredSprite(s_worldProps[2], d.pos.x, d.pos.y, 26.0f, 16.0f, d.angle, fadeTint);
                break;
            case 5:
                DrawCenteredSprite(s_worldProps[6], d.pos.x, d.pos.y, 11.0f, 9.0f, d.angle, fadeTint);
                break;
            case 6:
                DrawCenteredSprite(s_vfxSprites[13], d.pos.x, d.pos.y, 11.0f, 11.0f, d.angle, fadeTint);
                break;
            case 7: {
                float sz = 12.0f + (d.life / std::max(0.1f, d.maxLife)) * 18.0f;
                unsigned char sa = (unsigned char)(185.0f * (1.0f - d.life / std::max(0.1f, d.maxLife)));
                DrawCenteredSprite(s_vfxSprites[10], d.pos.x, d.pos.y, sz, sz, d.angle * 0.25f, Color{ 255, 255, 255, sa });
                break;
            }
            case 8:
                DrawCenteredSprite(s_worldProps[7], d.pos.x, d.pos.y, 20.0f, 17.0f, d.angle, fadeTint);
                break;
            case 9:
                DrawCenteredSprite(s_worldProps[9], d.pos.x, d.pos.y, 18.0f, 16.0f, d.angle, fadeTint);
                break;
            case 10:
                DrawCenteredSprite(s_worldProps[9], d.pos.x, d.pos.y, 13.0f, 12.0f, d.angle, Color{ d.col.r, d.col.g, d.col.b, a });
                break;
            }
        }
    };

    const char* stageBanner = "BONUS STAGE: TESLA BEATDOWN";
    const char* subLabel = "TESLA MODEL S";
    float progressRatio = 1.0f;
    bool invertBar = false;
    const char* clearLine2 = "+50,000 PTS - TOTAL WRECKAGE!";

    if (bonus.type == BONUS_STAGE_TESLA) {

        if (((int)(t * 9.0f) % 3) == 0) {
            float flashX = (float)(28 + (((int)(t * 17.0f) * 19) % 52));
            float flashY = (float)(98 + (((int)(t * 13.0f) * 7) % 14));
            DrawCenteredSprite(s_vfxSprites[0], flashX, flashY, 12.0f, 12.0f, t * 40.0f);
        }

        float hpRatio = std::clamp(bonus.hp / std::max(1.0f, bonus.maxHp), 0.0f, 1.0f);
        progressRatio = hpRatio;

        int carStage = 0;
        if (hpRatio <= 0.01f)      carStage = 7;
        else if (hpRatio <= 0.16f) carStage = 6;
        else if (hpRatio <= 0.31f) carStage = 5;
        else if (hpRatio <= 0.46f) carStage = 4;
        else if (hpRatio <= 0.60f) carStage = 3;
        else if (hpRatio <= 0.74f) carStage = 2;
        else if (hpRatio <= 0.88f) carStage = 1;
        else                       carStage = 0;

        int tearCount = 2 + carStage / 2;
        for (int k = 0; k < tearCount; k++) {
            float ph = fmodf(t * (2.4f + carStage * 0.25f) + k * 0.31f, 1.0f);
            float dir = (k % 2 == 0) ? -1.0f : 1.0f;
            float tx = 319.0f + dir * (8.0f + ph * 22.0f);
            float ty = 104.0f - sinf(ph * PI_F) * 6.0f + ph * ph * 16.0f;
            DrawCenteredSprite(s_vfxSprites[13], tx, ty, 9.0f, 9.0f, 0.0f, Color{ 255, 255, 255, (unsigned char)(220 * (1.0f - ph)) }, dir < 0.0f);
        }

        float shakeX = (bonus.targetShake > 0.0f) ? sinf(t * 95.0f) * bonus.targetShake * 4.5f : 0.0f;
        float shakeY = (bonus.targetShake > 0.0f) ? cosf(t * 120.0f) * bonus.targetShake * 2.5f : 0.0f;

        float carCenterX = (float)SCREEN_WIDTH * 0.5f + shakeX;
        DrawEllipse((int)carCenterX, (int)GROUND_Y + 2, 82, 11, Color{ 12, 16, 22, 175 });

        Rectangle carDst;
        if (carStage >= 4) {
            float carW = 172.0f;
            float carH = 106.0f;
            carDst = { carCenterX - carW * 0.5f, GROUND_Y - carH + 5.0f + shakeY, carW, carH };
        } else {
            float carW = 158.0f;
            float carH = 72.0f;
            carDst = { carCenterX - carW * 0.5f, GROUND_Y - carH + 4.0f + shakeY, carW, carH };
        }

        if (s_teslaSprites[carStage].id > 0) {
            Texture2D tTex = s_teslaSprites[carStage];
            Rectangle tSrc = { 0.0f, 0.0f, (float)tTex.width, (float)tTex.height };
            Color tint = (bonus.targetShake > 0.08f) ? Color{ 255, 220, 220, 255 } : WHITE;
            DrawTexturePro(tTex, tSrc, carDst, Vector2{ 0.0f, 0.0f }, 0.0f, tint);
        }

        if (carStage >= 2) {
            int smokePuffs = 2 + carStage / 2;
            for (int p = 0; p < smokePuffs; p++) {
                float puffT = fmodf(t * 1.6f + p * 0.31f, 1.0f);
                float smokeX = carCenterX - 32.0f + (p % 4) * 22.0f + sinf(t * 4.0f + p) * 6.0f;
                float smokeY = GROUND_Y - 38.0f - puffT * (28.0f + carStage * 3.5f);
                float smokeSz = 14.0f + puffT * (16.0f + carStage * 2.0f);
                DrawCenteredSprite(s_vfxSprites[10], smokeX, smokeY, smokeSz, smokeSz, puffT * 25.0f, Color{ 170, 175, 185, (unsigned char)(175 * (1.0f - puffT)) });
            }
        }
        if (carStage >= 4) {
            int flames = carStage - 2;
            for (int f = 0; f < flames; f++) {
                float flX = carCenterX - 40.0f + f * 20.0f;
                float flSz = 18.0f + 6.0f * fabsf(sinf(t * 16.0f + f * 1.7f));
                float baseY = GROUND_Y - 28.0f + (carStage >= 6 ? 6.0f : 0.0f);
                DrawCenteredSprite(s_vfxSprites[11], flX, baseY - flSz * 0.3f, flSz, flSz + 4.0f, 0.0f, WHITE, (f % 2 == 1));
            }
            if (((int)(t * 24.0f) % 2) == 0) {
                float arcX = carCenterX - 36.0f + (((int)(t * 97.0f) * 29) % 72);
                const Texture2D& bolt = (((int)(t * 24.0f)) % 4 == 0) ? s_vfxSprites[8] : s_vfxSprites[9];
                DrawCenteredSprite(bolt, arcX, GROUND_Y - 20.0f, 24.0f, 24.0f, 15.0f);
            }
        }

        drawDebrisLayer(false);
        DrawDropShadow(player);
        DrawFighterSprite(player);
        DrawProjectiles(player);
        drawDebrisLayer(true);

    } else if (bonus.type == BONUS_STAGE_POOL_TAR) {
        stageBanner = "BONUS STAGE: REFLECTING POOL TAR";
        subLabel = "CRACKS SEALED WITH BLUE TAR";
        invertBar = true;
        progressRatio = std::clamp((float)bonus.cracksSealed / (float)std::max(1, bonus.cracksGoal), 0.0f, 1.0f);
        clearLine2 = "+60,000 PTS - POOL SEALED TIGHT!";

        for (const auto& br : bonus.barrels) {
            if (!br.alive) continue;
            DrawEllipse((int)br.x, (int)br.y + 10, 11, 3, Color{ 15, 20, 28, 130 });
            DrawCenteredSprite(s_worldProps[4], br.x, br.y, 24.0f, 20.0f, br.angle * 57.2958f);
        }

        for (const auto& c : bonus.cracks) {
            if (!c.active || c.state != 2) continue;
            if (s_bonusTarPatch.id > 0) {
                Rectangle src = { 0.0f, 0.0f, (float)s_bonusTarPatch.width, (float)s_bonusTarPatch.height };
                Rectangle dst = { c.x - 24.0f, c.y - 11.0f, 48.0f, 22.0f };
                DrawTexturePro(s_bonusTarPatch, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }
        }

        for (const auto& c : bonus.cracks) {
            if (!c.active || c.state == 2) continue;
            if (c.state == 0) {
                float pulse = 1.0f + 0.08f * sinf(t * 18.0f + c.x * 0.1f);
                if (s_bonusPoolCrack.id > 0) {
                    Rectangle src = { 0.0f, 0.0f, (float)s_bonusPoolCrack.width, (float)s_bonusPoolCrack.height };
                    float w = 48.0f, h = 42.0f * pulse;
                    Rectangle dst = { c.x - w * 0.5f, c.y - h + 9.0f, w, h };
                    DrawTexturePro(s_bonusPoolCrack, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
                }
                float bob = sinf(t * 10.0f + c.x) * 3.0f;
                DrawTriangle(
                    Vector2{ c.x - 4.0f, c.y - 38.0f + bob },
                    Vector2{ c.x,        c.y - 32.0f + bob },
                    Vector2{ c.x + 4.0f, c.y - 38.0f + bob },
                    Color{ 255, 230, 40, 240 });
            } else if (c.state == 1) {
                float scale = 0.85f + 0.35f * sinf(std::min(1.0f, c.animTimer / 0.28f) * PI_F);
                if (s_bonusTarSplat.id > 0) {
                    Rectangle src = { 0.0f, 0.0f, (float)s_bonusTarSplat.width, (float)s_bonusTarSplat.height };
                    float w = 54.0f * scale, h = 38.0f * scale;
                    Rectangle dst = { c.x - w * 0.5f, c.y - h * 0.5f, w, h };
                    DrawTexturePro(s_bonusTarSplat, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
                }
            }
        }

        DrawDropShadow(player);
        Vector2 pPos = player.GetPosition();
        bool fRight = player.IsFacingRight();
        float dir = fRight ? 1.0f : -1.0f;

        float tankX = pPos.x - dir * 13.0f;
        float tankY = pPos.y - 42.0f;
        DrawCenteredSprite(s_worldProps[5], tankX, tankY, 22.0f, 28.0f, 0.0f, WHITE, !fRight);

        DrawFighterSprite(player);

        float recoilX = -dir * bonus.gunRecoil * 6.0f;
        float gunCenterX = pPos.x + dir * 18.0f + recoilX;
        float gunCenterY = pPos.y - 38.0f;
        if (s_bonusTarGun.id > 0) {
            Rectangle src = { 0.0f, 0.0f, fRight ? (float)s_bonusTarGun.width : -(float)s_bonusTarGun.width, (float)s_bonusTarGun.height };
            Rectangle dst = { gunCenterX - 20.0f, gunCenterY - 12.0f, 40.0f, 25.0f };
            DrawTexturePro(s_bonusTarGun, src, dst, Vector2{ 0.0f, 0.0f }, fRight ? 14.0f : -14.0f, WHITE);
        }

        for (const auto& shot : bonus.tarShots) {
            if (!shot.alive) continue;
            float angDeg = atan2f(shot.vel.y, shot.vel.x) * 57.2958f;
            DrawCenteredSprite(s_worldProps[6], shot.pos.x, shot.pos.y, 18.0f, 14.0f, angDeg);
        }
        drawDebrisLayer(true);

    } else if (bonus.type == BONUS_STAGE_BOOK_FIRE) {
        stageBanner = "BONUS STAGE: SAVE THE BOOKS!";
        subLabel = "LIBRARY OF CONGRESS INFERNO";
        float totalHp = 0.0f, totalMax = 0.0f;
        for (int i = 0; i < 4; i++) {
            totalHp += bonus.bookPiles[i].hp;
            totalMax += bonus.bookPiles[i].maxHp;
        }
        progressRatio = std::clamp(totalHp / std::max(1.0f, totalMax), 0.0f, 1.0f);
        clearLine2 = "+75,000 PTS - LIBRARY SAVED!";

        const float vilXs[3] = { 92.0f, 192.0f, 294.0f };
        const float vilY = 169.0f;
        const char* vilNames[3] = { "MILLER", "BARR", "BANNON" };
        const char* vilQuips[3] = { "BURN HISTORY!", "REDACTED!", "FLOOD THE FIRE!" };

        for (int v = 0; v < 3; v++) {
            DrawEllipse((int)vilXs[v], (int)vilY + 1, 18, 5, Color{ 20, 15, 12, 135 });
            if (s_bonusVillainStrips[v].id > 0) {
                Texture2D vt = s_bonusVillainStrips[v];
                int frameW = vt.width / 4;
                int fIdx = 0;
                if (bonus.villainAnimTimer[v] > 0.0f) {
                    float prog = 1.0f - std::clamp(bonus.villainAnimTimer[v] / 0.55f, 0.0f, 1.0f);
                    fIdx = std::clamp((int)(prog * 4.0f), 0, 3);
                } else {
                    fIdx = ((int)(t * 4.0f + v) % 2);
                }
                bool flipLeft = (v == 2);
                Rectangle src = { (float)(fIdx * frameW), 0.0f, flipLeft ? -(float)frameW : (float)frameW, (float)vt.height };
                float bobY = (bonus.villainAnimTimer[v] <= 0.0f) ? sinf(t * 6.0f + v * 2.0f) * 1.5f : 0.0f;
                Rectangle dst = { vilXs[v] - 29.0f, vilY - 56.0f + bobY, 58.0f, 58.0f };
                DrawTexturePro(vt, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }
            int nw = MeasureText(vilNames[v], 5);
            DrawRectangle((int)vilXs[v] - nw / 2 - 2, (int)vilY - 63, nw + 4, 7, Color{ 15, 15, 22, 200 });
            DrawPixelText(vilNames[v], (int)vilXs[v] - nw / 2, (int)vilY - 62, 5, Color{ 255, 205, 70, 240 });

            if (bonus.villainQuipTimer[v] > 0.0f) {
                int qw = MeasureText(vilQuips[v], 5);
                int bx = std::clamp((int)vilXs[v] - qw / 2 - 4, 4, SCREEN_WIDTH - qw - 10);
                int by = (int)vilY - 76;
                DrawRectangle(bx, by, qw + 8, 10, Color{ 255, 250, 235, 245 });
                DrawRectangleLines(bx, by, qw + 8, 10, BLACK);
                DrawPixelText(vilQuips[v], bx + 4, by + 2, 5, Color{ 180, 20, 20, 255 });
            }
        }

        drawDebrisLayer(false);

        for (int i = 0; i < 4; i++) {
            const auto& pile = bonus.bookPiles[i];
            float pr = std::clamp(pile.hp / std::max(1.0f, pile.maxHp), 0.0f, 1.0f);
            int pState = 3;
            if (pr > 0.66f)      pState = 0;
            else if (pr > 0.30f) pState = 1;
            else if (pr > 0.0f)  pState = 2;
            else                 pState = 3;

            DrawEllipse((int)pile.x, (int)GROUND_Y + 4, 28, 7, Color{ 25, 18, 14, 155 });
            if (s_bonusBookPiles[pState].id > 0) {
                Texture2D pt = s_bonusBookPiles[pState];
                Rectangle src = { 0.0f, 0.0f, (float)pt.width, (float)pt.height };
                float flickerH = (pr > 0.0f) ? (sinf(t * 22.0f + i * 1.9f) * 2.5f) : 0.0f;
                Rectangle dst = { pile.x - 31.0f, GROUND_Y - 56.0f - flickerH, 62.0f, 62.0f + flickerH };
                Color tint = (pile.hitFlash > 0.0f) ? Color{ 165, 235, 255, 255 } : WHITE;
                DrawTexturePro(pt, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, tint);
            }

            if (pr > 0.0f) {
                float flSz = (12.0f + 12.0f * pr) * (0.88f + 0.12f * sinf(t * 20.0f + i));
                DrawCenteredSprite(s_vfxSprites[11], pile.x, GROUND_Y - 34.0f - pr * 6.0f, flSz, flSz + 4.0f, 0.0f, Color{ 255, 255, 255, 210 }, (i % 2 == 1));
                DrawRectangle((int)pile.x - 18, (int)GROUND_Y + 6, 36, 4, Color{ 20, 20, 26, 230 });
                DrawRectangle((int)pile.x - 17, (int)GROUND_Y + 7, (int)(34.0f * pr), 2, Color{ 255, 115, 25, 255 });
            } else {
                int sw = MeasureText("SAVED!", 6);
                DrawPixelText("SAVED!", (int)pile.x - sw / 2, (int)GROUND_Y + 5, 6, Color{ 60, 245, 110, 255 });
            }
        }

        DrawDropShadow(player);
        DrawFighterSprite(player);
        Vector2 pPos = player.GetPosition();
        bool fRight = player.IsFacingRight();
        if (bonus.bucketSwingTimer > 0.0f && s_bonusWaterBucket.id > 0) {
            float prog = 1.0f - std::clamp(bonus.bucketSwingTimer / 0.24f, 0.0f, 1.0f);
            float dir = bonus.bucketFacingRight ? 1.0f : -1.0f;
            Rectangle src = { 0.0f, 0.0f, bonus.bucketFacingRight ? (float)s_bonusWaterBucket.width : -(float)s_bonusWaterBucket.width, (float)s_bonusWaterBucket.height };
            float w = 68.0f + prog * 14.0f;
            float h = 44.0f;
            float bx = pPos.x + (bonus.bucketFacingRight ? 4.0f : -w - 4.0f) + dir * prog * 12.0f;
            Rectangle dst = { bx, pPos.y - 54.0f + prog * 8.0f, w, h };
            DrawTexturePro(s_bonusWaterBucket, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        } else {
            float bx = pPos.x + (fRight ? 14.0f : -14.0f);
            float by = pPos.y - 24.0f + fabsf(sinf(t * 10.0f)) * 2.0f;
            DrawCenteredSprite(s_worldProps[8], bx, by, 18.0f, 20.0f, 0.0f, WHITE, !fRight);
        }
        drawDebrisLayer(true);

    } else if (bonus.type == BONUS_STAGE_BERLIN_WALL) {
        stageBanner = "BONUS STAGE: BERLIN WALL 1989";
        subLabel = "GRAFFITI BERLIN WALL";
        float hpRatio = std::clamp(bonus.hp / std::max(1.0f, bonus.maxHp), 0.0f, 1.0f);
        progressRatio = hpRatio;
        clearLine2 = "+100,000 PTS - FREIHEIT! WALL TORN DOWN!";

        float sweep1 = sinf(t * 1.7f) * 45.0f;
        float sweep2 = cosf(t * 2.1f) * 45.0f;
        DrawTriangle(Vector2{ 115.0f, 95.0f }, Vector2{ 75.0f + sweep1, 0.0f }, Vector2{ 105.0f + sweep1, 0.0f }, Color{ 160, 220, 255, 32 });
        DrawTriangle(Vector2{ 265.0f, 95.0f }, Vector2{ 255.0f + sweep2, 0.0f }, Vector2{ 285.0f + sweep2, 0.0f }, Color{ 255, 140, 240, 32 });

        float notePhase = fmodf(t * 0.9f, 1.0f);
        DrawPixelText("LOOKING FOR FREEDOM!", 252, (int)(74.0f - notePhase * 10.0f), 5, Color{ 255, 120, 245, (unsigned char)(235 * (1.0f - notePhase * 0.5f)) });

        int waveCycle = ((int)(t * 0.6f)) % 2;
        if (waveCycle == 0) {
            DrawRectangle(54, 20, 55, 9, Color{ 15, 22, 38, 225 });
            DrawRectangleLines(54, 20, 55, 9, Color{ 100, 165, 255, 200 });
            DrawPixelText("HI GORBY!", 57, 22, 5, Color{ 255, 225, 90, 255 });
        } else {
            DrawRectangle(275, 20, 66, 9, Color{ 38, 15, 22, 225 });
            DrawRectangleLines(275, 20, 66, 9, Color{ 255, 120, 120, 200 });
            DrawPixelText("DA, RONNIE!", 278, 22, 5, Color{ 255, 195, 95, 255 });
        }

        float shakeX = (bonus.targetShake > 0.0f) ? sinf(t * 105.0f) * bonus.targetShake * 4.5f : 0.0f;
        float wallCenterX = (float)SCREEN_WIDTH * 0.5f + 16.0f + shakeX;
        float wallW = 122.0f;
        float wallH = 104.0f;
        Rectangle wallDst = { wallCenterX - wallW * 0.5f, GROUND_Y - wallH + 5.0f, wallW, wallH };

        int wallState = 0;
        if (hpRatio <= 0.0f)       wallState = 3;
        else if (hpRatio <= 0.34f) wallState = 2;
        else if (hpRatio <= 0.68f) wallState = 1;
        else                       wallState = 0;

        DrawEllipse((int)wallCenterX, (int)GROUND_Y + 3, 62, 9, Color{ 10, 12, 18, 165 });
        if (s_bonusBerlinWall[wallState].id > 0) {
            Texture2D wt = s_bonusBerlinWall[wallState];
            Rectangle src = { 0.0f, 0.0f, (float)wt.width, (float)wt.height };
            Color tint = (bonus.targetShake > 0.08f) ? Color{ 255, 230, 210, 255 } : WHITE;
            DrawTexturePro(wt, src, wallDst, Vector2{ 0.0f, 0.0f }, 0.0f, tint);
        }

        for (const auto& eg : bonus.eastGermans) {
            if (!eg.active) continue;
            int v = std::clamp(eg.variant, 0, 3);
            float hopY = fabsf(sinf(eg.animPhase)) * 6.5f;
            DrawEllipse((int)eg.x, (int)eg.y + 2, 13, 4, Color{ 10, 12, 18, 150 });
            if (s_bonusEastGermans[v].id > 0) {
                Texture2D gt = s_bonusEastGermans[v];
                Rectangle src = { 0.0f, 0.0f, eg.facingRight ? (float)gt.width : -(float)gt.width, (float)gt.height };
                Rectangle dst = { eg.x - 24.0f, eg.y - 52.0f - hopY, 48.0f, 54.0f };
                DrawTexturePro(gt, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }
        }

        DrawDropShadow(player);
        DrawFighterSprite(player);
        DrawProjectiles(player);
        drawDebrisLayer(true);
    }

    DrawSparks(sparks);
    for (const auto& pop : bonus.popups) {
        if (!pop.alive) continue;
        int pw = MeasureText(pop.text, 7);
        int px = std::clamp((int)pop.pos.x - pw / 2, 4, SCREEN_WIDTH - pw - 4);
        int py = (int)pop.pos.y;
        DrawPixelText(pop.text, px + 1, py + 1, 7, BLACK);
        DrawPixelText(pop.text, px, py, 7, pop.col);
    }

    DrawRectangle(0, 0, SCREEN_WIDTH, 18, Color{ 10, 14, 22, 232 });
    DrawPixelText(stageBanner, 8, 4, 9, Color{ 255, 220, 60, 255 });

    int sec = (int)std::max(0.0f, ceilf(bonus.timer));
    char secBuf[16];
    snprintf(secBuf, sizeof(secBuf), "%02d", sec);
    Color timerCol = (sec <= 5) ? Color{ 255, 60, 60, 255 } : Color{ 255, 230, 40, 255 };
    int tw = MeasureText(secBuf, 20);
    DrawRectangle(SCREEN_WIDTH - tw - 14, 1, tw + 10, 17, Color{ 22, 26, 38, 245 });
    DrawPixelText(secBuf, SCREEN_WIDTH - tw - 9, 1, 20, timerCol);

    int barW = 156;
    int barH = 7;
    int barX = (SCREEN_WIDTH - barW) / 2;
    int barY = 23;

    DrawRectangle(barX - 2, barY - 2, barW + 4, barH + 4, Color{ 18, 18, 24, 245 });
    int fillW = (int)(barW * std::clamp(progressRatio, 0.0f, 1.0f));
    Color barCol;
    if (invertBar) {
        barCol = (progressRatio >= 0.75f) ? Color{ 50, 240, 120, 255 } : Color{ 30, 155, 255, 255 };
    } else {
        barCol = (progressRatio > 0.5f) ? Color{ 230, 45, 45, 255 } : (progressRatio > 0.22f ? Color{ 245, 165, 30, 255 } : Color{ 255, 235, 45, 255 });
    }
    DrawRectangle(barX, barY, fillW, barH, barCol);
    DrawRectangleLines(barX - 1, barY - 1, barW + 2, barH + 2, Color{ 220, 190, 60, 255 });

    char fullLbl[64];
    if (bonus.type == BONUS_STAGE_POOL_TAR) {
        snprintf(fullLbl, sizeof(fullLbl), "%s (%d/%d)", subLabel, bonus.cracksSealed, bonus.cracksGoal);
    } else {
        snprintf(fullLbl, sizeof(fullLbl), "%s", subLabel);
    }
    int lblW = MeasureText(fullLbl, 6);
    DrawRectangle((SCREEN_WIDTH - lblW) / 2 - 3, barY + barH + 2, lblW + 6, 8, Color{ 10, 14, 22, 190 });
    DrawPixelText(fullLbl, (SCREEN_WIDTH - lblW) / 2, barY + barH + 3, 6, Color{ 90, 230, 255, 250 });

    if (bonus.cleared) {
        int cw = MeasureText("BONUS STAGE CLEAR!", 19);
        int pw = MeasureText(clearLine2, 9);
        int bannerW = std::max(cw, pw) + 20;
        DrawRectangle((SCREEN_WIDTH - bannerW) / 2, 52, bannerW, 40, Color{ 10, 14, 24, 225 });
        DrawRectangleLines((SCREEN_WIDTH - bannerW) / 2, 52, bannerW, 40, Color{ 255, 215, 40, 255 });
        DrawPixelText("BONUS STAGE CLEAR!", (SCREEN_WIDTH - cw) / 2, 56, 19, Color{ 255, 225, 40, 255 });
        DrawPixelText(clearLine2, (SCREEN_WIDTH - pw) / 2, 78, 9, Color{ 70, 245, 255, 255 });
    } else if (bonus.timer <= 0.0f) {
        int tuw = MeasureText("TIME UP!", 24);
        DrawPixelText("TIME UP!", (SCREEN_WIDTH - tuw) / 2, 74, 24, Color{ 255, 60, 60, 255 });
    }
}

void VisualsEngine::DrawEndingCelebration(float timer) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 255 });

#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
    if (s_loaded && s_endingSavior.id == 0) {
        std::string endingPath = s_visualsBaseDir + "ending_savior.png";
        if (AssetManager::FileExists(endingPath.c_str())) {
            s_endingSavior = AssetManager::LoadTexture(endingPath.c_str());
            if (s_endingSavior.id > 0) SetTextureFilter(s_endingSavior, TEXTURE_FILTER_BILINEAR);
        }
    }
#endif

    if (s_loaded && s_endingSavior.id > 0) {
        Rectangle srcRec = { 0.0f, 0.0f, (float)s_endingSavior.width, (float)s_endingSavior.height };
        Rectangle dstRec = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
        DrawTexturePro(s_endingSavior, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
    }

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

    int botY = SCREEN_HEIGHT - 18;
    DrawRectangle(0, botY, SCREEN_WIDTH, 18, Color{ 10, 14, 24, 220 });
    DrawLine(0, botY, SCREEN_WIDTH, botY, Color{ 255, 215, 70, 220 });
    if (fmodf(timer, 0.9f) < 0.55f) {
        const char* msg = "[ PRESS START / ENTER TO RETURN TO TITLE SCREEN ]";
        int pw = MeasureText(msg, 8);
        DrawPixelText(msg, (SCREEN_WIDTH - pw) / 2, botY + 5, 8, Color{ 255, 225, 50, 255 });
    } else {
        const char* msg2 = "THANK YOU FOR PLAYING EXECUTIVE VETO II TURBO!";
        int pw2 = MeasureText(msg2, 8);
        DrawPixelText(msg2, (SCREEN_WIDTH - pw2) / 2, botY + 5, 8, Color{ 125, 235, 255, 245 });
    }
}

void VisualsEngine::DrawCthulhuEnding(float timer) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);

#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
    if (s_loaded) {
        auto ensureEndingTex = [](const char* rel, Texture2D& dst) {
            if (dst.id == 0) {
                std::string full = s_visualsBaseDir + rel;
                if (AssetManager::FileExists(full.c_str())) {
                    dst = AssetManager::LoadTexture(full.c_str());
                    if (dst.id > 0) SetTextureFilter(dst, TEXTURE_FILTER_BILINEAR);
                }
            }
        };
        if (timer < 5.0f) ensureEndingTex("ending_cthulhu_throw.png", s_endingCthulhuThrow);
        else if (timer < 10.0f) ensureEndingTex("ending_laser_shark.png", s_endingLaserShark);
        else ensureEndingTex("ending_cthulhu_nap.png", s_endingCthulhuNap);
    }
#endif

    auto drawStoryCaption = [](const char* caption, Color col) {
        DrawRectangle(0, 0, SCREEN_WIDTH, 15, Color{ 6, 10, 18, 220 });
        DrawLine(0, 15, SCREEN_WIDTH, 15, col);
        int cw = MeasureText(caption, 7);
        DrawPixelText(caption, (SCREEN_WIDTH - cw) / 2, 4, 7, col);
    };

    if (timer < 5.0f) {
        if (s_loaded && s_endingCthulhuThrow.id > 0) {
            Rectangle src = { 0.0f, 0.0f, (float)s_endingCthulhuThrow.width, (float)s_endingCthulhuThrow.height };
            Rectangle dst = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
            DrawTexturePro(s_endingCthulhuThrow, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }
        drawStoryCaption("ACT I: CTHULHU AWAKENS - HURLING POTUS INTO THE ABYSS!", Color{ 95, 255, 175, 255 });

    } else if (timer < 10.0f) {
        if (s_loaded && s_endingLaserShark.id > 0) {
            Rectangle src = { 0.0f, 0.0f, (float)s_endingLaserShark.width, (float)s_endingLaserShark.height };
            Rectangle dst = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
            DrawTexturePro(s_endingLaserShark, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }
        drawStoryCaption("ACT II: CYBERNETIC LASER SHARKS INTERCEPT AT HIGH TIDE!", Color{ 255, 75, 90, 255 });

    } else {
        if (s_loaded && s_endingCthulhuNap.id > 0) {
            Rectangle src = { 0.0f, 0.0f, (float)s_endingCthulhuNap.width, (float)s_endingCthulhuNap.height };
            Rectangle dst = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
            DrawTexturePro(s_endingCthulhuNap, src, dst, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }
        drawStoryCaption("ACT III: PEACE AT LAST... CTHULHU CLAIMS THE BEACH RESORT!", Color{ 255, 225, 95, 255 });

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
