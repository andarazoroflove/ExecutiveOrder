#include "Stages.h"
#include "AssetManager.h"
#include "Visuals.h"
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

    m_venues[STAGE_LINCOLN_MEMORIAL] = VenueDetails{
        "LINCOLN MEMORIAL", "NATIONAL MALL - MEMORIAL STEPS",
        Color{ 25, 45, 85, 255 }, Color{ 70, 110, 165, 255 },
        Color{ 225, 220, 210, 255 }, Color{ 175, 172, 165, 255 },
        Color{ 255, 215, 60, 255 }
    };

    m_venues[STAGE_SMITHSONIAN_AERO] = VenueDetails{
        "SMITHSONIAN AEROSPACE", "NATIONAL AIR & SPACE MUSEUM",
        Color{ 18, 28, 48, 255 }, Color{ 45, 70, 110, 255 },
        Color{ 130, 145, 165, 255 }, Color{ 70, 75, 88, 255 },
        Color{ 80, 220, 255, 255 }
    };

    m_venues[STAGE_TOMB_UNKNOWN] = VenueDetails{
        "TOMB OF THE UNKNOWN", "ARLINGTON NATIONAL CEMETERY",
        Color{ 22, 28, 52, 255 }, Color{ 85, 55, 70, 255 },
        Color{ 215, 210, 205, 255 }, Color{ 145, 142, 140, 255 },
        Color{ 255, 185, 50, 255 }
    };

    m_venues[STAGE_ENN_NEWS_STUDIO] = VenueDetails{
        "ENN NEWS STUDIO", "EXECUTIVE NEWS NETWORK - LIVE",
        Color{ 14, 22, 48, 255 }, Color{ 30, 55, 110, 255 },
        Color{ 195, 25, 35, 255 }, Color{ 25, 35, 60, 255 },
        Color{ 255, 60, 60, 255 }
    };

    m_venues[STAGE_CHICAGO_WRIGLEY] = VenueDetails{
        "WRIGLEYVILLE", "CHICAGO, IL - WRIGLEY FIELD",
        Color{ 20, 32, 68, 255 }, Color{ 125, 65, 85, 255 },
        Color{ 195, 25, 35, 255 }, Color{ 145, 78, 55, 255 },
        Color{ 255, 215, 50, 255 }
    };

    m_venues[STAGE_RYUGYONG_HOTEL] = VenueDetails{
        "RYUGYONG UPPER FLOOR", "PYONGYANG, DPRK - 105TH FLOOR",
        Color{ 26, 22, 58, 255 }, Color{ 68, 54, 112, 255 },
        Color{ 92, 92, 98, 255 }, Color{ 118, 118, 122, 255 },
        Color{ 80, 225, 95, 255 }
    };

    m_venues[STAGE_WESTERN_WALL] = VenueDetails{
        "THE WESTERN WALL", "JERUSALEM - OLD CITY PLAZA",
        Color{ 120, 155, 210, 255 }, Color{ 245, 205, 140, 255 },
        Color{ 225, 190, 130, 255 }, Color{ 215, 185, 135, 255 },
        Color{ 255, 230, 120, 255 }
    };

    m_venues[STAGE_SOVIET_FOUNDRY] = VenueDetails{
        "SOVIET STEEL FOUNDRY", "SIBERIA, RUSSIA - BLAST PLANT",
        Color{ 35, 18, 16, 255 }, Color{ 120, 48, 20, 255 },
        Color{ 175, 40, 30, 255 }, Color{ 115, 112, 108, 255 },
        Color{ 255, 185, 25, 255 }
    };

    m_venues[STAGE_PALACE_COURTYARD] = VenueDetails{
        "PALACE COURTYARD", "PERSIAN GULF - ROYAL ARCHES",
        Color{ 35, 135, 235, 255 }, Color{ 135, 205, 255, 255 },
        Color{ 210, 165, 110, 255 }, Color{ 195, 155, 105, 255 },
        Color{ 55, 210, 245, 255 }
    };

    m_venues[STAGE_PERFORMING_ARTS] = VenueDetails{
        "PERFORMING ARTS CENTER", "WASHINGTON, D.C. - KENNEDY PLAZA",
        Color{ 24, 38, 76, 255 }, Color{ 185, 120, 75, 255 },
        Color{ 225, 220, 212, 255 }, Color{ 155, 150, 145, 255 },
        Color{ 245, 195, 55, 255 }
    };

    m_venues[STAGE_THE_HOOD] = VenueDetails{
        "THE HOOD", "WASHINGTON, D.C. - 3AM CARRYOUT",
        Color{ 12, 16, 28, 255 }, Color{ 28, 34, 52, 255 },
        Color{ 85, 42, 34, 255 }, Color{ 26, 30, 38, 255 },
        Color{ 65, 255, 130, 255 }
    };

    m_venues[STAGE_PROGRESS_AVENUE] = VenueDetails{
        "PROGRESS AVENUE", "DONGALD J. TRUMPF PROGRESSIVE CTR",
        Color{ 55, 165, 245, 255 }, Color{ 135, 215, 255, 255 },
        Color{ 205, 125, 90, 255 }, Color{ 185, 165, 145, 255 },
        Color{ 255, 95, 210, 255 }
    };

    m_venues[STAGE_LINCOLN_BEDROOM] = VenueDetails{
        "THE LINCOLN BEDROOM", "WHITE HOUSE - RESIDENCE SUITE",
        Color{ 115, 22, 28, 255 }, Color{ 165, 42, 45, 255 },
        Color{ 185, 138, 52, 255 }, Color{ 150, 24, 32, 255 },
        Color{ 255, 210, 55, 255 }
    };

    m_venues[STAGE_WYOMING_RODEO] = VenueDetails{
        "WYOMING", "JACKSON HOLE & OLD FAITHFUL RODEO",
        Color{ 48, 165, 248, 255 }, Color{ 145, 215, 255, 255 },
        Color{ 138, 92, 52, 255 }, Color{ 176, 108, 52, 255 },
        Color{ 255, 165, 40, 255 }
    };

    m_venues[STAGE_TEHRAN_CASBAH] = VenueDetails{
        "TEHRAN", "AZADI SQUARE - ROCK THE CASBAH",
        Color{ 92, 48, 98, 255 }, Color{ 245, 135, 55, 255 },
        Color{ 45, 175, 165, 255 }, Color{ 185, 152, 122, 255 },
        Color{ 255, 85, 45, 255 }
    };
}

StageManager::~StageManager() {
    Shutdown();
}

void StageManager::EnsureStageTextureLoaded(StageId id) {
    if (id < 0 || id >= STAGE_COUNT) return;
    if (m_stageTextures[id].id > 0) return;

#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)

    for (int k = 0; k < STAGE_COUNT; k++) {
        if (k != (int)id && m_stageTextures[k].id > 0) {
            UnloadTexture(m_stageTextures[k]);
            m_stageTextures[k] = { 0 };
        }
    }
#endif

    static const char* namedStages[STAGE_COUNT] = {
        "stage_0.png", "stage_1.png", "stage_2.png", "stage_3.png", "stage_4.png", "stage_5.png",
        "stage_wtc_plaza.png", "stage_comet_ping_pong.png", "stage_florida_keys.png",
        "stage_lincoln_memorial.png", "stage_smithsonian_aero.png", "stage_tomb_unknown.png",
        "stage_enn_news_studio.png", "stage_chicago_wrigley.png",
        "stage_ryugyong_hotel.png", "stage_western_wall.png",
        "stage_soviet_foundry.png", "stage_palace_courtyard.png",
        "stage_performing_arts.png", "stage_the_hood.png",
        "stage_progress_avenue.png", "stage_lincoln_bedroom.png",
        "stage_wyoming_rodeo.png", "stage_tehran_casbah.png"
    };

    char path[160];
    snprintf(path, sizeof(path), "%sstage_%d.png", m_baseDir.c_str(), (int)id);
    if (AssetManager::FileExists(path)) {
        m_stageTextures[id] = AssetManager::LoadTexture(path);
        if (m_stageTextures[id].id > 0) SetTextureFilter(m_stageTextures[id], TEXTURE_FILTER_BILINEAR);
    } else if (namedStages[id] != nullptr) {
        snprintf(path, sizeof(path), "%s%s", m_baseDir.c_str(), namedStages[id]);
        if (AssetManager::FileExists(path)) {
            m_stageTextures[id] = AssetManager::LoadTexture(path);
            if (m_stageTextures[id].id > 0) SetTextureFilter(m_stageTextures[id], TEXTURE_FILTER_BILINEAR);
        }
    }
}

void StageManager::Init() {
    if (m_texturesLoaded) return;

    AssetManager::Init();

    m_baseDir = "assets/";
    if (!AssetManager::HasEmbeddedAssets() && !DirectoryExists(m_baseDir.c_str())) {
        std::string appDirAssets = std::string(GetApplicationDirectory()) + "assets/";
        std::string parentAssets = "../assets/";
        std::string appParentAssets = std::string(GetApplicationDirectory()) + "../assets/";
        if (DirectoryExists(appDirAssets.c_str())) {
            m_baseDir = appDirAssets;
        } else if (DirectoryExists(parentAssets.c_str())) {
            m_baseDir = parentAssets;
        } else if (DirectoryExists(appParentAssets.c_str())) {
            m_baseDir = appParentAssets;
        }
    }

    m_texturesLoaded = true;
#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
    EnsureStageTextureLoaded(m_stageId);
#else
    for (int i = 0; i < STAGE_COUNT; i++) {
        EnsureStageTextureLoaded((StageId)i);
    }
#endif
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
#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
        if (m_texturesLoaded) {
            EnsureStageTextureLoaded(m_stageId);
        }
#endif
    }
}

void StageManager::DrawFarLayer(float camX, float time) {
#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
    if (m_texturesLoaded && m_stageTextures[m_stageId].id == 0) {
        EnsureStageTextureLoaded(m_stageId);
    }
#endif
    if (m_texturesLoaded && m_stageTextures[m_stageId].id > 0) {

        float minCam = STAGE_MIN_X;
        float maxCam = STAGE_MAX_X - SCREEN_WIDTH;
        float t = std::clamp((camX - minCam) / (maxCam - minCam), 0.0f, 1.0f);

        float texWf = (float)m_stageTextures[m_stageId].width;
        float texHf = (float)m_stageTextures[m_stageId].height;
        float srcH = std::min(640.0f, texHf);
        float srcY = std::max(0.0f, texHf - srcH);
        if (m_stageId >= STAGE_PERFORMING_ARTS && texHf >= 740.0f) {
            srcH = 752.0f;
            srcY = 8.0f;
        } else if (m_stageId >= STAGE_RYUGYONG_HOTEL && texHf >= 740.0f) {
            srcH = 700.0f;
            srcY = 52.0f;
        }
        float srcW = std::min(texWf, srcH * ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT));
        float maxScrollX = std::max(0.0f, texWf - srcW);
        float srcX = t * maxScrollX;

        Rectangle srcRec = { srcX, srcY, srcW, srcH };
        Rectangle dstRec = { 0.0f, 0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT };
        DrawTexturePro(m_stageTextures[m_stageId], srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);

        auto MapBgX = [&](float tx) -> float { return (tx - srcX) * ((float)SCREEN_WIDTH / srcW); };
        auto MapBgY = [&](float ty) -> float { return (ty - srcY) * ((float)SCREEN_HEIGHT / srcH); };

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
                DrawRectangle((int)monScreenX - 1, 9, 2, 2, Color{ 255, 45, 45, 240 });
            }
            break;
        }
        case STAGE_CONGRESSIONAL_FLOOR:
        case STAGE_WHITE_HOUSE_MMA:
            break;
        case STAGE_PRESS_BRIEFING: {
            float flashCycle = fmodf(time * 3.8f, 1.0f);
            if (flashCycle < 0.08f) {
                float fx = 60.0f + fmodf(time * 123.4f, 260.0f);
                float fy = 90.0f + fmodf(time * 56.7f, 50.0f);
                VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(0), fx, fy, 12.0f, 12.0f);
            }
            float flashCycle2 = fmodf(time * 2.3f + 0.5f, 1.0f);
            if (flashCycle2 < 0.06f) {
                float fx2 = 120.0f + fmodf(time * 78.9f, 220.0f);
                float fy2 = 110.0f + fmodf(time * 34.5f, 40.0f);
                VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(0), fx2, fy2, 10.0f, 10.0f, 22.0f);
            }
            break;
        }
        case STAGE_PENTAGON_ALTAR: {
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
                VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(0), (float)(SCREEN_WIDTH / 2) + 20.0f, 15.0f, 8.0f, 8.0f);
            }
            break;
        }
        case STAGE_COMET_PING_PONG: {
            if (fmodf(time * 7.5f, 1.0f) < 0.05f) {
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 20, 20, 30, 40 });
            }
            break;
        }
        case STAGE_FLORIDA_KEYS: {
            float waveTick = fmodf(time * 1.5f, 1.0f);
            float sparkX = 40.0f + waveTick * 180.0f;
            DrawRectangle((int)sparkX, (int)GROUND_Y - 4, 8, 1, Color{ 140, 240, 255, 180 });
            break;
        }
        case STAGE_RYUGYONG_HOTEL: {
            float jucheX = MapBgX(338.0f);
            float jucheY = MapBgY(298.0f);
            float flScale = 0.85f + 0.15f * sinf(time * 10.0f);
            VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(11), jucheX, jucheY - 2.0f, 10.0f * flScale, 12.0f * flScale);
            break;
        }
        case STAGE_WESTERN_WALL:
            break;
        case STAGE_SOVIET_FOUNDRY:
            break;
        case STAGE_PALACE_COURTYARD: {

            float ftnX = MapBgX(684.0f);
            float ftnTopY = MapBgY(358.0f);
            float ftnPoolY = MapBgY(522.0f);
            for (int j = 0; j < 9; j++) {
                float phase = fmodf(time * 2.6f + j * 0.13f, 1.0f);
                float spread = (j - 4) * 3.8f;
                float dropX = ftnX + spread * phase;
                float dropY = ftnTopY + phase * phase * (ftnPoolY - ftnTopY);
                DrawRectangle((int)dropX, (int)dropY, 2, 2, Color{ 195, 245, 255, 225 });
            }
            break;
        }
        case STAGE_PERFORMING_ARTS: {
            for (int sp = 0; sp < 8; sp++) {
                float ph = fmodf(time * 2.4f + sp * 0.17f, 1.0f);
                float sx = MapBgX(520.0f + (sp % 4) * 115.0f + sinf(sp * 3.1f + time * 5.0f) * 14.0f);
                float sy = MapBgY(78.0f + ph * ph * 165.0f);
                DrawPixel((int)sx, (int)sy, (sp % 2 == 0) ? Color{ 255, 240, 130, 240 } : Color{ 255, 155, 45, 220 });
            }
            if (fmodf(time * 3.6f, 1.0f) < 0.08f) {
                float fx = MapBgX((int(time * 5.0f) % 2 == 0) ? 512.0f : 935.0f);
                float fy = MapBgY(485.0f);
                VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(0), fx, fy, 11.0f, 11.0f);
            }
            break;
        }
        case STAGE_THE_HOOD: {
            float neonFlick = (fmodf(time * 9.0f, 1.0f) > 0.05f) ? 1.0f : 0.35f;
            DrawRectangle((int)MapBgX(190.0f), (int)MapBgY(32.0f), (int)(600.0f * SCREEN_WIDTH / srcW), (int)(135.0f * SCREEN_HEIGHT / srcH),
                          Color{ 255, 90, 35, (unsigned char)(18 * neonFlick) });
            DrawRectangle((int)MapBgX(735.0f) + (int)(sinf(time * 7.0f) * 2.0f), (int)MapBgY(705.0f), 14, 2, Color{ 80, 255, 140, 150 });
            for (int st = 0; st < 4; st++) {
                float up = fmodf(time * 1.4f + st * 0.25f, 1.0f);
                float bx = MapBgX((st < 2) ? 115.0f : 990.0f) + sinf(time * 4.0f + st) * 2.0f;
                float by = MapBgY(430.0f - up * 38.0f);
                DrawPixel((int)bx, (int)by, Color{ 230, 235, 245, (unsigned char)((1.0f - up) * 175) });
            }
            break;
        }
        case STAGE_PROGRESS_AVENUE: {
            static const Color kPrideCols[6] = {
                {235, 45, 55, 220}, {250, 145, 35, 220}, {250, 225, 45, 220},
                {45, 195, 85, 220}, {55, 135, 250, 220}, {175, 65, 235, 220}
            };
            for (int c = 0; c < 14; c++) {
                float cx = fmodf(c * 31.0f + time * (12.0f + (c % 3) * 4.0f), (float)SCREEN_WIDTH);
                float cy = fmodf(c * 19.0f + time * 16.0f, (float)GROUND_Y - 18.0f);
                DrawRectangle((int)cx + (int)(sinf(time * 5.0f + c) * 3.0f), (int)cy, 2, 2, kPrideCols[c % 6]);
            }
            break;
        }
        case STAGE_LINCOLN_BEDROOM: {
            float tvX = MapBgX(1215.0f), tvY = MapBgY(385.0f);
            float carShift = fmodf(time * 18.0f, 14.0f);
            DrawRectangle((int)(tvX - 6.0f + carShift * 0.3f), (int)(tvY + 4.0f - carShift * 0.4f), 3, 2, Color{ 255, 240, 120, 210 });
            float chX = MapBgX(790.0f), chY = MapBgY(82.0f);
            if (fmodf(time * 3.2f, 1.0f) < 0.35f) {
                DrawPixel((int)chX + ((int)(time * 9.0f) % 9 - 4) * 3, (int)chY, Color{ 255, 252, 210, 240 });
            }
            break;
        }
        case STAGE_WYOMING_RODEO: {
            float geyX = MapBgX(1065.0f);
            for (int m = 0; m < 10; m++) {
                float ph = fmodf(time * 1.9f + m * 0.11f, 1.0f);
                float mx = geyX + sinf(m * 2.3f + time * 3.0f) * (5.0f + ph * 14.0f);
                float my = MapBgY(410.0f - ph * 340.0f);
                DrawRectangle((int)mx, (int)my, 2, 2, Color{ 240, 252, 255, (unsigned char)((1.0f - ph) * 210) });
            }
            for (int h = 0; h < 2; h++) {
                float hp = fmodf(time * 0.85f + h * 0.5f, 1.0f);
                float hx = MapBgX(426.0f) + sinf(time * 4.0f + h * 3.0f) * 3.0f;
                float hy = MapBgY(428.0f - hp * 55.0f);
                Color hc{ 255, 75, 125, (unsigned char)((1.0f - hp) * 235) };
                DrawRectangle((int)hx - 2, (int)hy, 2, 2, hc);
                DrawRectangle((int)hx + 1, (int)hy, 2, 2, hc);
                DrawRectangle((int)hx - 1, (int)hy + 1, 3, 2, hc);
            }
            break;
        }
        case STAGE_TEHRAN_CASBAH: {
            float rigX = MapBgX(260.0f), rigY = MapBgY(215.0f);
            Color spotCol = ((int)(time * 5.0f) % 2 == 0) ? Color{ 255, 220, 75, 36 } : Color{ 80, 235, 215, 36 };
            DrawTriangle(Vector2{ rigX - 30.0f, rigY - 25.0f }, Vector2{ rigX - 55.0f, rigY + 85.0f }, Vector2{ rigX + 35.0f, rigY + 85.0f }, spotCol);

            for (int bf = 0; bf < 2; bf++) {
                float fx = MapBgX(bf == 0 ? 645.0f : 1190.0f);
                float fy = MapBgY(342.0f);
                float flScale = 0.9f + 0.15f * sinf(time * 18.0f + bf * 2.0f);
                VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(11), fx + 3.0f, fy - 3.0f, 9.0f * flScale, 11.0f * flScale);
            }
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
        case STAGE_CONGRESSIONAL_FLOOR:
            break;
        case STAGE_WHITE_HOUSE_MMA: {
            float midOffset = -camX * 0.50f;
            for (int i = 0; i < 10; i++) {
                float crowdX = i * 42.0f + fmodf(midOffset, 42.0f);
                if (fmodf(time * 9.0f + i * 1.9f, 2.2f) < 0.05f) {
                    VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(0), crowdX, 118.0f, 10.0f, 10.0f, (float)(i * 20));
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
    ChampionshipEdition::DrawStageUpgrades(m_stageId, camX, time);
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
    (void)camX;
    (void)time;
}
