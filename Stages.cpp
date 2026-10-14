#include "Stages.h"
#include "AssetManager.h"
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
