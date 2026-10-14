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
            86.0f / 295.0f,
            84.0f / 262.0f,
            86.0f / 268.0f
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

        bool isDownOrKo = (f.GetHealth() <= 0 || state == FSTATE_KNOCKDOWN_AIR || state == FSTATE_KNOCKDOWN_GROUND || state == FSTATE_DEFEAT || state == FSTATE_WAKEUP);
        if (!isDownOrKo) {
            int pIdx = (f.GetPlayerIndex() == 0) ? 0 : 1;
            for (const auto& gh : ChampionshipEdition::s_afterimages[pIdx]) {
                float ghDx = gh.pos.x - posX;
                float ghDy = gh.pos.y - posY;
                Rectangle ghDst = dstRec;
                ghDst.x += ghDx;
                ghDst.y += ghDy;
                unsigned char ga = (unsigned char)std::clamp(gh.life * 18, 20, 145);
                Color ghCol = (state == FSTATE_SPECIAL) ? Color{ 70, 210, 255, ga } : Color{ 255, 210, 70, ga };
                DrawTexturePro(tex, srcRec, ghDst, origin, rotation, ghCol);
            }

            Rectangle rimDark = dstRec;
            rimDark.x += (fr ? -1.5f : 1.5f);
            rimDark.y += 1.5f;
            DrawTexturePro(tex, srcRec, rimDark, origin, rotation, Color{ 10, 12, 22, 195 });

            Rectangle rimLight = dstRec;
            rimLight.x += (fr ? 1.0f : -1.0f);
            rimLight.y -= 1.0f;
            Color bevelTint = (state == FSTATE_SPECIAL) ? Color{ 255, 230, 90, 160 } : Color{ 220, 240, 255, 110 };
            DrawTexturePro(tex, srcRec, rimLight, origin, rotation, bevelTint);
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

        bool isFaintedOrDown = (f.GetHealth() <= 0 || state == FSTATE_KNOCKDOWN_AIR || state == FSTATE_KNOCKDOWN_GROUND || state == FSTATE_DEFEAT || state == FSTATE_WAKEUP);
        if (!isFaintedOrDown && f.GetTauntTimer() > 0 && f.HasSpeechTaunt() && !f.GetActiveTaunt().empty()) {
            const char* quote = f.GetActiveTaunt().c_str();
            int qw = MeasureText(quote, 10);
            int bubbleW = qw + 10;
            int bubbleH = 13;

            float headTopY = (f.GetCharacterId() == CHAR_WAR_PIG) ? (pos.y - 88.0f) : (pos.y - 70.0f);
            int bubbleY = (int)(headTopY - bubbleH - 4.0f);
            if (bubbleY < 12) bubbleY = 12;

            int bubbleX = (int)pos.x - bubbleW / 2;
            bubbleX = std::clamp(bubbleX, (int)STAGE_MIN_X + 6, (int)STAGE_MAX_X - 6 - bubbleW);

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
        } else if (!isFaintedOrDown && (state == FSTATE_TAUNT || f.GetTauntTimer() > 0) && !f.HasSpeechTaunt()) {
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

        if (!isDownOrKo) { DrawCosmeticPuffs(f); ChampionshipEdition::DrawFighterWorldOverlays(f); }
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
        } else if (p.type == 9) {

            float dir = p.facingRight ? 1.0f : -1.0f;
            int mode = ((p.lifetime / 5) % 3);
            bool isRedState = (mode != 1);
            Color stateCol = isRedState ? Color{ 230, 35, 45, 245 } : Color{ 35, 105, 245, 245 };
            int px = (int)p.pos.x;
            int py = (int)p.pos.y;

            DrawRectangle(px - 17, py - 8, 34, 16, WHITE);
            DrawRectangle(px - 15, py - 6, 30, 12, stateCol);
            DrawRectangleLines(px - 17, py - 8, 34, 16, Color{ 255, 230, 70, 255 });

            const char* stateLabel = (mode == 0) ? "270!" : ((mode == 1) ? "PA 19" : "TX 40");
            VisualsEngine::DrawPixelText(stateLabel, px - 13, py - 5, 10, (mode == 0) ? Color{ 255, 245, 90, 255 } : WHITE);

            int tipX = px + (int)(dir * 22.0f);
            DrawLineEx(Vector2{ (float)(px + dir * 16.0f), (float)py }, Vector2{ (float)tipX, (float)py }, 2.5f, Color{ 255, 235, 80, 255 });
            DrawTriangle(
                Vector2{ (float)(tipX + dir * 4.0f), (float)py },
                Vector2{ (float)tipX, (float)(py - 4) },
                Vector2{ (float)tipX, (float)(py + 4) },
                Color{ 255, 60, 60, 255 }
            );
            DrawCircle((int)(px - dir * 20.0f), py - 4, 2.0f, Color{ 230, 45, 55, 220 });
            DrawCircle((int)(px - dir * 25.0f), py + 3, 2.0f, Color{ 45, 130, 255, 220 });
        } else if (p.type == 10) {

            int px = (int)p.pos.x;
            int py = (int)p.pos.y;
            float spin = (float)p.lifetime * 0.35f;

            DrawCircle((int)(p.pos.x - p.vel.x * 2.2f), (int)(p.pos.y - p.vel.y * 2.2f), 6.0f, Color{ 255, 215, 60, 120 });
            DrawCircle((int)(p.pos.x - p.vel.x * 4.0f), (int)(p.pos.y - p.vel.y * 4.0f), 3.5f, Color{ 100, 220, 255, 100 });

            DrawCircle(px, py, 10.5f, Color{ 20, 12, 8, 255 });
            DrawCircle(px, py, 9.0f, Color{ 235, 105, 25, 255 });
            DrawCircle(px - 3, py - 3, 4.0f, Color{ 255, 155, 65, 230 });

            int sx = (int)(cosf(spin) * 7.5f);
            int sy = (int)(sinf(spin) * 7.5f);
            DrawLine(px - 8, py, px + 8, py, Color{ 25, 18, 14, 240 });
            DrawLine(px, py - 8, px, py + 8, Color{ 25, 18, 14, 240 });
            DrawLine(px - sx, py - sy, px + sx, py + sy, Color{ 25, 18, 14, 220 });
            DrawCircleLines(px, py, 6.5f, Color{ 25, 18, 14, 200 });
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
}

void VisualsEngine::DrawCinematicIntro(float timer, bool logoDropped, const Shader& flagShader, const Texture2D& flagTex) {
    if (s_loaded && s_titleSplash.id > 0) {

        Rectangle srcRec = { 0.0f, 0.0f, (float)s_titleSplash.width, (float)s_titleSplash.height };
        Rectangle dstRec = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
        DrawTexturePro(s_titleSplash, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

        float pulse = 0.5f + 0.5f * sinf(timer * 6.0f);
        int banW = 248;
        int banH = 22;
        int banX = (SCREEN_WIDTH - banW) / 2;
        int banY = SCREEN_HEIGHT - 48;
        DrawRectangle(banX - 2, banY - 2, banW + 4, banH + 4, Color{ 15, 10, 25, 240 });
        DrawRectangleGradientH(banX, banY, banW, banH, Color{ 180, 20, 35, 245 }, Color{ 40, 60, 170, 245 });
        DrawRectangleLines(banX, banY, banW, banH, Color{ 255, 215, 50, 255 });
        DrawRectangleLines(banX - 1, banY - 1, banW + 2, banH + 2, Color{ 255, 245, 150, (unsigned char)(140 + pulse * 115) });
        int ctw = MeasureText("CHAMPIONSHIP EDITION", 14);
        DrawPixelText("CHAMPIONSHIP EDITION", (SCREEN_WIDTH - ctw) / 2, banY + 4, 14, Color{ 255, 235, 70, 255 });

        if (((int)(timer * 3.0f) % 2) == 0) {
            int ptw = MeasureText("INSERT COIN / PRESS START", 10);
            DrawRectangle((SCREEN_WIDTH - ptw) / 2 - 6, SCREEN_HEIGHT - 20, ptw + 12, 14, Color{ 10, 12, 20, 210 });
            DrawPixelText("INSERT COIN / PRESS START", (SCREEN_WIDTH - ptw) / 2, SCREEN_HEIGHT - 18, 10, WHITE);
        }
        for (int s = 0; s < 12; s++) {
            float sx = fmodf(s * 33.0f + timer * (25.0f + s * 3.0f), (float)SCREEN_WIDTH);
            float sy = SCREEN_HEIGHT - fmodf(s * 19.0f + timer * (32.0f + s * 4.0f), (float)SCREEN_HEIGHT);
            DrawCircle((int)sx, (int)sy, 1.5f, (s % 2 == 0) ? Color{ 255, 225, 80, 210 } : Color{ 100, 220, 255, 200 });
        }
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

        Vector2 nyPt{ 275.0f, 48.0f };
        Vector2 chiPt{ 126.0f, 44.0f };
        Vector2 dcPt{ 245.0f, 74.0f };
        Vector2 flPt{ 188.0f, 124.0f };

        for (int step = 0; step <= 10; step++) {
            float f = (float)step / 10.0f;
            Vector2 p1 = { nyPt.x + (chiPt.x - nyPt.x) * f, nyPt.y + (chiPt.y - nyPt.y) * f };
            DrawCircle((int)p1.x, (int)p1.y, 1.5f, Color{ 255, 220, 60, 200 });
            Vector2 p2 = { chiPt.x + (dcPt.x - chiPt.x) * f, chiPt.y + (dcPt.y - chiPt.y) * f };
            DrawCircle((int)p2.x, (int)p2.y, 1.5f, Color{ 255, 165, 50, 200 });
            Vector2 p3 = { dcPt.x + (flPt.x - dcPt.x) * f, dcPt.y + (flPt.y - dcPt.y) * f };
            DrawCircle((int)p3.x, (int)p3.y, 1.5f, Color{ 80, 220, 255, 200 });
        }

        struct MapTarget {
            Vector2 pos;
            const char* city;
            CharacterId charId;
        };
        MapTarget targets[4] = {
            { nyPt, "NEW YORK", CHAR_SOCIALIST },
            { chiPt, "CHICAGO", CHAR_A_NEW_HOPE },
            { dcPt, "D.C.", CHAR_BILLARY },
            { flPt, "FLORIDA KEYS", CHAR_PALM_BEACH_PETE }
        };

        for (int i = 0; i < 4; i++) {
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

    Vector2 pins[STAGE_COUNT] = {
        { 180, 48 },
        { 158, 88 },
        { 288, 78 },
        { 180, 64 },
        { 168, 42 },
        { 82, 148 },
        { 295, 38 },
        { 145, 36 },
        { 260, 138 },
        { 118, 88 },
        { 228, 92 },
        { 78, 118 },
        { 212, 54 },
        { 96, 34 }
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
        "14. CHICAGO WRIGLEY"
    };

    for (int i = 0; i < STAGE_COUNT; i++) {
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
            headerTitle = "*** NATIONAL TOUR: NEW YORK (THE SOCIALIST) ***";
            headerCol = Color{ 80, 220, 255, 255 };
        } else if (!defeatedFighters[CHAR_A_NEW_HOPE]) {
            headerTitle = "*** NATIONAL TOUR: CHICAGO WRIGLEY (A NEW HOPE) ***";
            headerCol = Color{ 255, 195, 60, 255 };
        } else if (!defeatedFighters[CHAR_BILLARY]) {
            headerTitle = "*** NATIONAL TOUR: D.C. BASEMENT (BILLARY) ***";
            headerCol = Color{ 255, 120, 240, 255 };
        } else if (!defeatedFighters[CHAR_PALM_BEACH_PETE]) {
            headerTitle = "*** GRAND FINALE: FLORIDA KEYS (PALM BEACH PETE) ***";
            headerCol = Color{ 255, 215, 0, 255 };
        } else {
            headerTitle = "*** TOTAL VETO GRAND CHAMPION! ***";
            headerCol = Color{ 255, 215, 0, 255 };
        }
    } else if (defeatedOriginalCount < 9) {
        if (ChampionshipEdition::s_opponentLocked) {
            headerTitle = "SELECT BATTLE VENUE (D-PAD: PIN | ATTACK: FIGHT!)";
            headerCol = Color{ 80, 235, 255, 255 };
        } else {
            headerTitle = "CHOOSE NEXT OPPONENT";
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
        "TEMPLE ISLAND (FL KEYS)",
        "LINCOLN MEMORIAL STEPS",
        "SMITHSONIAN AIR & SPACE",
        "TOMB OF THE UNKNOWN",
        "ENN NEWS STUDIO",
        "CHICAGO WRIGLEY FIELD"
    };

    char venueBanner[80];
    if (ChampionshipEdition::s_opponentLocked && !warPigBeaten && defeatedOriginalCount < 9) {
        snprintf(venueBanner, sizeof(venueBanner), "< STAGE: %s >", pinNamesAll[selectedStage < STAGE_COUNT ? selectedStage : 0]);
    } else {
        snprintf(venueBanner, sizeof(venueBanner), "[STAGE: %s]", pinNamesAll[selectedStage < STAGE_COUNT ? selectedStage : 0]);
    }
    int vbw = MeasureText(venueBanner, 8);
    DrawRectangle((SCREEN_WIDTH - vbw) / 2 - 4, 132, vbw + 8, 12, Color{ 10, 14, 22, 235 });
    if (ChampionshipEdition::s_opponentLocked && !warPigBeaten && defeatedOriginalCount < 9) {
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
        "PALM BEACH PETE", "THE PUNDIT", "A NEW HOPE"
    };

    const char* rosterArchetypes[CHARACTER_COUNT] = {
        "Shoto All-Round", "Three Justices", "Grappler Slam", "Agile Rushdown",
        "Zoner Traps", "Turtle Defense", "Acrobatic Fur", "Mix-Up Kicks",
        "Power Nap Counter", "Trailer Brawler",
        "Brain Worm Cobra", "Pentagon Final Boss", "Produce & Cash",
        "Dual Clinton Electro", "Adrenochrome Mogul", "Decision 2028 Map",
        "Windy City Hoop"
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
        int cWidth = 90;
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
        bool hopeDef = (defeatedFighters != nullptr && defeatedFighters[CHAR_A_NEW_HOPE]);
        bool bilDef = (defeatedFighters != nullptr && defeatedFighters[CHAR_BILLARY]);
        bool peteDef = (defeatedFighters != nullptr && defeatedFighters[CHAR_PALM_BEACH_PETE]);

        ECCard ecCards[4] = {
            { 6,   CHAR_SOCIALIST,       "1: NEW YORK", "SOCIALIST",  "WTC Plaza",    false,   socDef },
            { 100, CHAR_A_NEW_HOPE,      "2: CHICAGO",  "A NEW HOPE", "Wrigleyville", !socDef, hopeDef },
            { 194, CHAR_BILLARY,         "3: D.C.",     "BILLARY",    "Comet Ping",   !hopeDef, bilDef },
            { 288, CHAR_PALM_BEACH_PETE, "4: FL KEYS",  "P.B. PETE",  "Temple Isle",  !bilDef, peteDef }
        };

        for (int k = 0; k < 4; k++) {
            const auto& cd = ecCards[k];
            bool active = (p2Char == cd.id);
            Color bgCol = cd.defeated ? Color{ 15, 15, 20, 255 } : (cd.locked ? Color{ 18, 18, 22, 255 } : Color{ 28, 32, 46, 255 });
            DrawRectangle(cd.x, cY, cWidth, cHeight, bgCol);

            if (s_loaded && s_portraits[cd.id].id > 0) {
                Rectangle srcRec = { 0.0f, 0.0f, (float)s_portraits[cd.id].width, (float)s_portraits[cd.id].height };
                Rectangle dstRec = { (float)cd.x + 2, (float)cY + 6, 32.0f, 32.0f };
                Color pTint = cd.locked ? Color{ 55, 55, 60, 180 } : (cd.defeated ? Color{ 80, 80, 85, 200 } : WHITE);
                DrawTexturePro(s_portraits[cd.id], srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, pTint);
            }

            if (cd.locked) {
                DrawRectangle(cd.x + 35, cY + 2, cWidth - 37, cHeight - 4, Color{ 16, 16, 22, 250 });
                DrawPixelText("LOCKED", cd.x + 38, cY + 12, 8, Color{ 170, 170, 185, 220 });
                DrawPixelText("BEAT PREV", cd.x + 37, cY + 25, 6, Color{ 130, 130, 145, 200 });
            } else {
                Color hdrCol = (k == 3 ? Color{ 255, 215, 50, 255 } : Color{ 80, 210, 255, 255 });
                DrawPixelText(cd.header, cd.x + 36, cY + 4, 6, hdrCol);
                DrawPixelText(cd.name, cd.x + 36, cY + 15, 7, Color{ 255, 220, 60, 255 });
                DrawPixelText(cd.stage, cd.x + 36, cY + 27, 6, Color{ 170, 190, 220, 220 });
            }

            if (cd.defeated) {
                DrawRectangle(cd.x + 2, cY + 2, cWidth - 4, cHeight - 4, Color{ 35, 0, 0, 140 });
                DrawLineEx(Vector2{ (float)cd.x + 4, (float)cY + 4 }, Vector2{ (float)cd.x + cWidth - 4, (float)cY + cHeight - 4 }, 2.0f, Color{ 230, 30, 30, 240 });
                DrawLineEx(Vector2{ (float)cd.x + cWidth - 4, (float)cY + 4 }, Vector2{ (float)cd.x + 4, (float)cY + cHeight - 4 }, 2.0f, Color{ 230, 30, 30, 240 });
                DrawRectangle(cd.x + 10, cY + 16, cWidth - 20, 12, Color{ 190, 20, 20, 230 });
                int dw = MeasureText("DEFEATED", 8);
                DrawPixelText("DEFEATED", cd.x + (cWidth - dw) / 2, cY + 18, 8, WHITE);
            }

            Color borderCol = active ? Color{ 255, 50, 50, 255 } : (cd.locked ? Color{ 50, 50, 60, 255 } : Color{ 80, 140, 200, 255 });
            DrawRectangleLines(cd.x, cY, cWidth, cHeight, borderCol);
            if (active) {
                DrawRectangleLines(cd.x - 2, cY - 2, cWidth + 4, cHeight + 4, Color{ 255, 215, 0, 255 });
                int ctW = MeasureText("NEXT TARGET", 7);
                DrawPixelText("NEXT TARGET", cd.x + (cWidth - ctW) / 2, cY - 9, 7, Color{ 255, 220, 50, 255 });
            }
        }
    } else if (defeatedOriginalCount >= 9) {
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
        "P.B. PETE", "THE PUNDIT", "A NEW HOPE"
    };

    const char* rosterShortNames[CHARACTER_COUNT] = {
        "POTUS", "BENCH", "REDCAP", "GOV",
        "TECHBRO", "TWEED", "FREAGLE", "POP",
        "JOE", "MTG",
        "RKF", "WAR PIG", "ZOHRAN", "BILLARY",
        "PETE", "PUNDIT", "OBAMA"
    };

    const char* rosterArchetypes[CHARACTER_COUNT] = {
        "Presidential Shoto", "Three Justices", "Brawler Grappler", "Kickboxer Rush",
        "Drone & Laser", "Turtle Filibuster", "Direct Action Furry", "Viral Kicks",
        "Power Nap Counter", "Trailer Brawler",
        "Brain Worm Cobra", "Pentagon Juggernaut", "Groceries & Cash", "Dual Clinton Laptop",
        "Florida Keys Boss", "Decision 2028 Map", "Arcing Hoop Shot"
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
        "14. CHICAGO WRIGLEY"
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
        "9. TEMPLE ISLE",
        "10. LINCOLN MEM",
        "11. AIR & SPACE",
        "12. UNKNOWN TOMB",
        "13. ENN STUDIO",
        "14. WRIGLEY FLD"
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
        "Florida Keys",
        "Memorial Steps",
        "Smithsonian",
        "Arlington, VA",
        "Cable News Desk",
        "Chicago, IL"
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
        "Chicago - Wrigleyville Plaza"
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

        boxW = 38;
        int startX1 = 5;
        int startY1 = 138;
        for (int i = 8; i < CHARACTER_COUNT; i++) {
            int col = i - 8;
            int bx = startX1 + col * 42;
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

        int cardW = 88;
        int cardH = 20;
        for (int i = 0; i < STAGE_COUNT; i++) {
            int col = i % 4;
            int row = i / 4;
            int cx = 10 + col * 92;
            int cy = 89 + row * 23;

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
        "PALM BEACH PETE", "THE PUNDIT", "A NEW HOPE"
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
        { "CLANGED OFF THE BACK RIM...", "UHH... LET ME ICE MY JAW...", "FOUL! THAT WAS A CHARGING FOUL!", "MICHELLE, GET THE ICE PACK..." }
    };

    int wVar = s_quoteVariant & 3;
    int lVar = s_loserVariant & 3;
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
