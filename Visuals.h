#pragma once
#include "Common.h"
#include "AssetManager.h"
#include "Fighter.h"
#include "Collision.h"
#include "Stages.h"
#include "InputManager.h"
#include "AudioEngine.h"
#include <vector>

struct BonusDebris {
    bool alive = false;
    int kind = 0;
    Vector2 pos{ 0.0f, 0.0f };
    Vector2 vel{ 0.0f, 0.0f };
    float angle = 0.0f;
    float angVel = 0.0f;
    float life = 0.0f;
    float maxLife = 1.0f;
    Color col{ 255, 255, 255, 255 };
    int targetPile = -1;
};

struct BonusPopup {
    bool alive = false;
    char text[48] = {};
    Vector2 pos{ 0.0f, 0.0f };
    float life = 0.0f;
    float maxLife = 0.95f;
    Color col{ 255, 230, 50, 255 };
};

struct PoolCrackSlot {
    bool active = false;
    int state = 0;
    float x = 100.0f;
    float y = 198.0f;
    float animTimer = 0.0f;
};

struct TarGlobShot {
    bool alive = false;
    Vector2 pos{ 0.0f, 0.0f };
    Vector2 vel{ 0.0f, 0.0f };
    float life = 0.0f;
};

struct RollingBarrel {
    bool alive = false;
    float x = 0.0f;
    float y = 132.0f;
    float vx = 60.0f;
    float angle = 0.0f;
};

struct BookFirePile {
    float hp = 100.0f;
    float maxHp = 100.0f;
    float x = 80.0f;
    float hitFlash = 0.0f;
    float steamTimer = 0.0f;
};

struct EastGermanRunner {
    bool active = false;
    int variant = 0;
    float x = 192.0f;
    float y = 198.0f;
    float vx = 0.0f;
    float animPhase = 0.0f;
    bool facingRight = true;
};

struct BonusStageState {
    BonusStageType type = BONUS_STAGE_TESLA;
    float timer = 35.0f;
    float maxTimer = 35.0f;
    float elapsed = 0.0f;
    float postTimer = 0.0f;
    bool cleared = false;
    bool endVoicePlayed = false;

    float hp = 420.0f;
    float maxHp = 420.0f;
    float targetShake = 0.0f;
    int lastDamageTier = 0;
    bool carAlarmPlayed = false;

    int cracksSealed = 0;
    int cracksGoal = 18;
    float crackSpawnTimer = 0.0f;
    float gunRecoil = 0.0f;
    float gunCooldown = 0.0f;
    float barrelSpawnTimer = 1.0f;
    std::vector<PoolCrackSlot> cracks;
    std::vector<TarGlobShot> tarShots;
    std::vector<RollingBarrel> barrels;

    BookFirePile bookPiles[4];
    float bucketSwingTimer = 0.0f;
    float bucketCooldown = 0.0f;
    bool bucketFacingRight = true;
    float bucketOriginX = 100.0f;
    float villainThrowTimer[3] = { 0.7f, 1.4f, 2.1f };
    float villainAnimTimer[3] = { 0.0f, 0.0f, 0.0f };
    float villainQuipTimer[3] = { 0.0f, 0.0f, 0.0f };

    float blockHp[9] = {};
    std::vector<EastGermanRunner> eastGermans;
    float runnerSpawnTimer = 0.0f;

    std::vector<BonusDebris> debris;
    std::vector<BonusPopup> popups;
};

class VisualsEngine {
public:
    static void Init();
    static void Shutdown();
    static void PrefetchMatchFighters(int p1Char, int p2Char);

    static void DrawDropShadow(const Fighter& f);
    static void DrawFighterSprite(const Fighter& f);
    static void DrawProjectiles(const Fighter& f);
    static void DrawSparks(const std::vector<SparkEffect>& sparks);
    static void DrawBattleHUD(const Fighter& p1, const Fighter& p2, int roundTimer, int roundNumber, int fightIntroTimer = 0, bool p1WasKo = false, bool p2WasKo = false);
    static void DrawCinematicIntro(float timer, bool logoDropped, const Shader& flagShader, const Texture2D& flagTex);
    static void DrawStageAndCharSelect(int selectedStage, int p1Char, int p2Char, float timer, const bool defeatedFighters[CHARACTER_COUNT], bool playerLocked = false, bool choosingOpponent = false);
    static void DrawWorldConflictMap(int worldActIndex, int p1Char, float timer, const bool defeatedFighters[CHARACTER_COUNT]);
    static void DrawSecretSelectScreen(int step, int p1Char, int p2Char, int stage, int difficulty, float timer);
    static void DrawVersusScreen(const Fighter& p1, const Fighter& p2, float timer);
    static void DrawVictorySplash(int winnerChar, int loserChar, float timer);
    static void DrawBonusStage(const BonusStageState& bonus, const Fighter& player, const std::vector<SparkEffect>& sparks);
    static void DrawMatchOver(const Fighter& winner, float timer);
    static void DrawEndingCelebration(float timer);
    static void DrawCthulhuEnding(float timer);
    static void DrawPauseOverlay(float timer);

    static void DrawPixelText(const char* text, int x, int y, int size, Color col);
    static Texture2D CreateProceduralFlagTexture();
    static const Texture2D& GetArabicTauntTexture(int idx);
    static const Texture2D& GetVfxSprite(int idx);
    static const Texture2D& GetWorldProp(int idx);
    static bool DrawCenteredSprite(const Texture2D& tex, float cx, float cy, float w, float h, float rotDeg = 0.0f, Color tint = WHITE, bool flipX = false);
    static void DrawFinisherBackdrop(const Fighter& p1, const Fighter& p2, float camX);
    static void DrawFinisherWorldFX(const Fighter& p1, const Fighter& p2);

    enum StripClipId {
        STRIP_IDLE = 0,
        STRIP_WALK,
        STRIP_JUMP,
        STRIP_CROUCH,
        STRIP_BLOCK,
        STRIP_LP,
        STRIP_HP,
        STRIP_LK,
        STRIP_HK,
        STRIP_SPEC,
        STRIP_HIT,
        STRIP_TAUNT,
        STRIP_CLIP_COUNT
    };

private:
    static bool s_loaded;
    static Texture2D s_titleSplash;
    static Texture2D s_portraits[CHARACTER_COUNT];
    static Texture2D s_portraitsLose[CHARACTER_COUNT];

    static Texture2D s_sprites[CHARACTER_COUNT][6];
    static Texture2D s_strips[CHARACTER_COUNT][STRIP_CLIP_COUNT];
    static Texture2D s_stripsHillary[STRIP_CLIP_COUNT];
    static Texture2D s_stripsRoberts[STRIP_CLIP_COUNT];
    static Texture2D s_stripsSotomayor[STRIP_CLIP_COUNT];
    static Texture2D s_stripAbdulSpec2;
    static Texture2D s_stripPaleMothVictory;
    static Texture2D s_stripWVictory;
    static Texture2D s_stripDickLaugh;
    static Texture2D s_stripAyatollahPrayer;
    static Texture2D s_projSprites[16];
    static Texture2D s_finisherProps[16];
    static Texture2D s_vfxSprites[16];
    static Texture2D s_worldProps[16];
    static Texture2D s_arabicTaunts[3];
    static Texture2D s_mapWorldConflict;
    static Texture2D s_propShawarma;
    static Texture2D s_propWheelchair;
    static Texture2D s_propAtmHack;
    static Texture2D s_propHammerSickle;
    static Texture2D s_teslaSprites[8];
    static Texture2D s_bonusStageBgs[BONUS_STAGE_COUNT];
    static Texture2D s_bonusTarGun;
    static Texture2D s_bonusPoolCrack;
    static Texture2D s_bonusTarSplat;
    static Texture2D s_bonusTarPatch;
    static Texture2D s_bonusVillainStrips[3];
    static Texture2D s_bonusWaterBucket;
    static Texture2D s_bonusBookPiles[4];
    static Texture2D s_bonusBerlinWall[4];
    static Texture2D s_bonusEastGermans[4];
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

#include <cstdlib>
#include <cstdio>

namespace ChampionshipEdition {

inline bool s_forceProceduralRig = false;
inline int  s_superFlashTimer = 0;
inline int  s_superFlashPlayer = 0;
inline std::string s_superFlashMove = "TOTAL VETO FLURRY!";
inline float s_p1TrailHp = 100.0f;
inline float s_p2TrailHp = 100.0f;
inline int   s_continuesRemaining = 2;
inline bool  s_isSecretMatch = false;
inline bool  s_opponentLocked = false;
inline TurboSpeed s_turboSpeed = TURBO_SPEED_2;
inline bool  s_finishHimActive = false;
inline bool  s_vetoedKoActive = false;
inline float s_crowdExciteTimer = 0.0f;
inline float s_camRenderX = 208.0f;

struct CEAfterimage {
    Vector2 pos;
    bool facingRight = true;
    FighterState state = FSTATE_IDLE;
    int frame = 0;
    int life = 0;
};

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
                nodes[i] = Vector2{ anchor.x, anchor.y + i * 4.2f };
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
            float flutter = sinf((float)GetTime() * 16.0f + i * 1.3f) * 0.35f;
            nodes[i].x += vx.x - charVel.x * 0.38f + (facingRight ? -0.22f : 0.22f) + flutter;
            nodes[i].y += vx.y + 0.45f;
        }
        for (int iter = 0; iter < 3; iter++) {
            nodes[0] = anchor;
            for (int i = 0; i < 4; i++) {
                float dx = nodes[i + 1].x - nodes[i].x;
                float dy = nodes[i + 1].y - nodes[i].y;
                float dist = sqrtf(dx * dx + dy * dy);
                float rest = 4.2f;
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
            float w = (i == 3) ? 4.8f : 3.4f;
            DrawLineEx(nodes[i], nodes[i + 1], w + 1.8f, Color{ 10, 10, 16, 235 });
            DrawLineEx(nodes[i], nodes[i + 1], w, tieCol);
        }
        DrawCircleV(nodes[4], 2.5f, trimCol);
    }
};

inline CEVerletTie s_ties[2];
inline std::vector<CEAfterimage> s_afterimages[2];
inline std::vector<CEStamp> s_stamps;
inline std::vector<CEParticle> s_particles;
inline std::vector<CECalloutBanner> s_banners;
inline bool s_firstStrikeAwarded = false;
inline int s_prevP1Hits = 0;
inline int s_prevP2Hits = 0;

inline void ResetRoundFX() {
    s_stamps.clear();
    s_particles.clear();
    s_banners.clear();
    s_afterimages[0].clear();
    s_afterimages[1].clear();
    s_superFlashTimer = 0;
    s_firstStrikeAwarded = false;
    s_prevP1Hits = 0;
    s_prevP2Hits = 0;
    s_p1TrailHp = 100.0f;
    s_p2TrailHp = 100.0f;
    s_finishHimActive = false;
    s_vetoedKoActive = false;
    s_crowdExciteTimer = 0.0f;
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
    int sweatCount = heavy ? 10 : 5;
    for (int i = 0; i < sweatCount; i++) {
        CEParticle p{};
        p.pos = pos;
        float ang = -PI_F * 0.85f + ((float)(rand() % 100) / 100.0f) * PI_F * 0.7f;
        float spd = 1.8f + ((float)(rand() % 100) / 100.0f) * 3.4f;
        p.vel = Vector2{ cosf(ang) * spd, sinf(ang) * spd - 1.4f };
        p.color = Color{ 130, 230, 255, 245 };
        p.size = 2.2f;
        p.maxFrames = 26 + (rand() % 12);
        p.isPaper = false;
        p.alive = true;
        s_particles.push_back(p);
    }
    if (heavy || isSpecial) {
        int docs = isSpecial ? 9 : 5;
        for (int i = 0; i < docs; i++) {
            CEParticle p{};
            p.pos = pos;
            p.vel = Vector2{ ((float)(rand() % 100) / 50.0f - 1.0f) * 3.6f, -2.4f - ((float)(rand() % 100) / 100.0f) * 2.8f };
            p.color = (i % 2 == 0) ? Color{ 255, 250, 230, 255 } : Color{ 255, 215, 50, 255 };
            p.size = 5.0f;
            p.rot = (float)(rand() % 360);
            p.rotVel = ((float)(rand() % 100) / 50.0f - 1.0f) * 10.0f;
            p.maxFrames = 45 + (rand() % 18);
            p.isPaper = true;
            p.alive = true;
            s_particles.push_back(p);
        }
    }
    if (stampText != nullptr && stampText[0] != '\0') {
        CEStamp st{};
        st.pos = pos;
        st.text = stampText;
        st.color = isSpecial ? Color{ 255, 55, 55, 255 } : Color{ 255, 220, 40, 255 };
        st.frame = 0;
        st.maxFrames = 24;
        st.alive = true;
        s_stamps.push_back(st);
    }
}

inline void DrawComicThoughtBubble(const char* text, int centerX, int bottomY, int tailTargetX, int tailTargetY, int fontSize, Color textCol, Color cloudCol = Color{ 255, 255, 248, 238 }) {
    if (!text || text[0] == '\0') return;
    bool isArabic = (text[0] == '@' && text[1] == 'A' && text[2] == 'R' && text[3] >= '0' && text[3] <= '2');
    const Texture2D* arTex = isArabic ? &VisualsEngine::GetArabicTauntTexture(text[3] - '0') : nullptr;
    int arDrawW = 0;
    int arDrawH = 0;
    int actualH = std::max(fontSize, 10);
    int tw = 0;
    if (isArabic && arTex != nullptr && arTex->id > 0) {
        arDrawW = std::max(40, (int)(arTex->width * 0.58f));
        arDrawH = std::max(16, (int)(arTex->height * 0.58f));
        tw = arDrawW;
        actualH = arDrawH;
    } else {
        tw = MeasureText(text, actualH);
    }
    int padX = 6;
    int padY = 4;
    int halfW = tw / 2 + padX;
    int halfH = actualH / 2 + padY;

    int minCamX = (int)s_camRenderX + halfW + 5;
    int maxCamX = (int)s_camRenderX + SCREEN_WIDTH - halfW - 5;
    int cx = centerX;
    if (minCamX <= maxCamX) {
        cx = std::clamp(cx, minCamX, maxCamX);
    } else {
        cx = (int)s_camRenderX + SCREEN_WIDTH / 2;
    }
    cx = std::clamp(cx, (int)STAGE_MIN_X + halfW, (int)STAGE_MAX_X - halfW);
    int cy = std::clamp(bottomY - halfH, 34 + halfH, (int)GROUND_Y - 28);

    int numTop = std::max(4, (tw + 10) / 8);
    Color outlineCol{ 18, 22, 34, 245 };
    Color shadowCol{ 0, 0, 0, 70 };

    auto drawCloudPass = [&](int ox, int oy, float rGrow, Color col) {
        DrawRectangle(cx - halfW + 3 + ox, cy - halfH + 2 + oy, (halfW - 3) * 2, (halfH - 2) * 2, col);
        DrawCircle(cx - halfW + 3 + ox, cy + oy, (float)halfH - 0.5f + rGrow, col);
        DrawCircle(cx + halfW - 3 + ox, cy + oy, (float)halfH - 0.5f + rGrow, col);
        for (int i = 0; i < numTop; i++) {
            float t = (numTop == 1) ? 0.5f : (float)i / (float)(numTop - 1);
            float lx = (cx - halfW + 4) + t * ((halfW - 4) * 2.0f);
            float arch = sinf(t * PI_F) * 1.6f;
            float rTop = ((i % 2 == 0) ? (halfH * 0.68f) : (halfH * 0.78f)) + rGrow;
            float rBot = ((i % 2 == 1) ? (halfH * 0.64f) : (halfH * 0.74f)) + rGrow;
            DrawCircle((int)lx + ox, (int)(cy - halfH + 2 - arch) + oy, rTop, col);
            DrawCircle((int)lx + ox, (int)(cy + halfH - 2 + arch * 0.6f) + oy, rBot, col);
        }
    };

    drawCloudPass(1, 1, 1.1f, shadowCol);
    drawCloudPass(0, 0, 1.2f, outlineCol);
    drawCloudPass(0, 0, 0.0f, cloudCol);

    for (int i = 0; i < numTop; i++) {
        float t = (numTop == 1) ? 0.5f : (float)i / (float)(numTop - 1);
        float lx = (cx - halfW + 4) + t * ((halfW - 4) * 2.0f);
        float arch = sinf(t * PI_F) * 1.6f;
        float rTop = (i % 2 == 0) ? (halfH * 0.48f) : (halfH * 0.56f);
        DrawCircle((int)lx, (int)(cy - halfH + 2 - arch), rTop, WHITE);
    }

    float startX = (float)std::clamp(tailTargetX, cx - halfW + 6, cx + halfW - 6);
    float startY = (float)(cy + halfH + 1);
    float endX = (float)tailTargetX;
    float endY = (float)std::max((int)startY + 4, tailTargetY);
    float radii[3] = { 2.3f, 1.6f, 1.0f };
    for (int p = 0; p < 3; p++) {
        float u = (float)(p + 1) / 3.3f;
        float px = startX + (endX - startX) * u + ((p == 1) ? 1.0f : 0.0f);
        float py = startY + (endY - startY) * u;
        DrawCircle((int)px + 1, (int)py + 1, radii[p] + 0.8f, shadowCol);
        DrawCircle((int)px, (int)py, radii[p] + 1.0f, outlineCol);
        DrawCircle((int)px, (int)py, radii[p], cloudCol);
    }

    if (isArabic && arTex != nullptr && arTex->id > 0) {
        DrawTexturePro(
            *arTex,
            Rectangle{ 0.0f, 0.0f, (float)arTex->width, (float)arTex->height },
            Rectangle{ (float)(cx - arDrawW / 2), (float)(cy - arDrawH / 2), (float)arDrawW, (float)arDrawH },
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    } else {
        VisualsEngine::DrawPixelText(text, cx - tw / 2, cy - actualH / 2, actualH, textCol);
    }
}

inline void DrawBatmanActionBurst(const char* text, int centerX, int centerY, int fontSize, Color burstOuter, Color burstInner, Color textCol, float scale = 1.0f) {
    if (!text || text[0] == '\0') return;
    int tw = MeasureText(text, fontSize);
    float rx = (tw * 0.50f + 7.5f) * scale;
    float ry = (fontSize * 0.52f + 5.5f) * scale;

    float cx = std::clamp((float)centerX, STAGE_MIN_X + rx, STAGE_MAX_X - rx);
    float cy = std::clamp((float)centerY, ry + 18.0f, GROUND_Y - ry - 8.0f);

    const int N = 16;
    auto buildBurstPts = [&](float mult, Vector2 outPts[N]) {
        for (int i = 0; i < N; i++) {
            float ang = (float)i * (2.0f * PI_F / (float)N) - 0.14f;
            bool isSpike = (i % 2 == 0);
            float spikeBoost = isSpike ? (((i % 4 == 0) ? 1.24f : 1.12f)) : 0.68f;
            outPts[i] = Vector2{
                cx + cosf(ang) * rx * spikeBoost * mult,
                cy + sinf(ang) * ry * spikeBoost * mult
            };
        }
    };

    auto fillStarburst = [&](float mult, Color col, float ox, float oy) {
        Vector2 pts[N];
        buildBurstPts(mult, pts);
        Vector2 c{ cx + ox, cy + oy };
        for (int i = 0; i < N; i++) {
            Vector2 p1{ pts[i].x + ox, pts[i].y + oy };
            Vector2 p2{ pts[(i + 1) % N].x + ox, pts[(i + 1) % N].y + oy };
            DrawTriangle(c, p1, p2, col);
            DrawTriangle(c, p2, p1, col);
        }
    };

    fillStarburst(1.12f, Color{ 0, 0, 0, 95 }, 1.5f, 1.5f);
    fillStarburst(1.13f, Color{ 12, 12, 18, 250 }, 0.0f, 0.0f);
    fillStarburst(1.00f, burstOuter, 0.0f, 0.0f);
    fillStarburst(0.74f, burstInner, 0.0f, 0.0f);

    Vector2 outerPts[N];
    buildBurstPts(1.01f, outerPts);
    for (int i = 0; i < N; i++) {
        DrawLineEx(outerPts[i], outerPts[(i + 1) % N], 1.2f, Color{ 12, 12, 18, 250 });
    }

    int tx = (int)(cx - tw * 0.5f);
    int ty = (int)(cy - fontSize * 0.5f);
    VisualsEngine::DrawPixelText(text, tx + 1, ty + 1, fontSize, Color{ 10, 10, 16, 255 });
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx != 0 || dy != 0) {
                VisualsEngine::DrawPixelText(text, tx + dx, ty + dy, fontSize, Color{ 12, 12, 18, 255 });
            }
        }
    }
    VisualsEngine::DrawPixelText(text, tx, ty, fontSize, textCol);
}

inline void DrawArcade3DText(const char* text, int x, int y, int fontSize, Color topCol, Color botCol) {
    if (!text || text[0] == '\0') return;
    VisualsEngine::DrawPixelText(text, x + 2, y + 2, fontSize, Color{ 8, 10, 20, 245 });
    VisualsEngine::DrawPixelText(text, x + 1, y + 2, fontSize, Color{ 8, 10, 20, 245 });
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx != 0 || dy != 0) {
                VisualsEngine::DrawPixelText(text, x + dx, y + dy, fontSize, Color{ 14, 16, 28, 255 });
            }
        }
    }
    VisualsEngine::DrawPixelText(text, x, y + 1, fontSize, botCol);
    VisualsEngine::DrawPixelText(text, x, y, fontSize, topCol);
}

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

    bool isNewHope = (f.GetCharacterId() == CHAR_A_NEW_HOPE);
    Color pantsCol = isNewHope ? trim : suit;
    Color forearmCol = isNewHope ? skin : suit;

    DrawLimb3Tone(hipL, kneeL, 4.5f, 3.8f, pantsCol);
    DrawLimb3Tone(kneeL, footL, 3.6f, 3.0f, pantsCol);
    DrawLimb3Tone(footL, Vector2{ footL.x + dir * 5.0f, footL.y }, 3.0f, 2.5f, shoes);
    DrawLimb3Tone(shL, elbL, 3.8f, 3.2f, suit);
    DrawLimb3Tone(elbL, handL, 3.0f, 2.6f, forearmCol);
    DrawCircleV(handL, 3.2f, skin);

    DrawLimb3Tone(pelvis, torso, 7.5f, 9.0f, suit);

    DrawLimb3Tone(hipR, kneeR, 4.8f, 4.0f, pantsCol);
    DrawLimb3Tone(kneeR, footR, 3.8f, 3.2f, pantsCol);
    DrawLimb3Tone(footR, Vector2{ footR.x + dir * 5.5f, footR.y }, 3.2f, 2.6f, shoes);

    DrawCircleV(head, 8.5f, Color{ 12, 12, 18, 255 });
    DrawCircleV(head, 7.2f, skin);
    DrawRectangle((int)(head.x - 7), (int)(head.y - 8), 14, 4, isNewHope ? Color{ 35, 38, 44, 255 } : trim);
    DrawCircle((int)(head.x + dir * 3.5f), (int)(head.y - 1.5f), (st == FSTATE_HITSTUN) ? 2.5f : 1.5f, WHITE);
    DrawCircle((int)(head.x + dir * 4.0f), (int)(head.y - 1.5f), 1.0f, BLACK);
    if (st == FSTATE_ATTACK || st == FSTATE_SPECIAL || st == FSTATE_HITSTUN) {
        DrawRectangle((int)(head.x + dir * 2.0f), (int)(head.y + 2.5f), 4, 3, Color{ 140, 25, 35, 255 });
    } else {
        DrawLine((int)(head.x + dir * 1.5f), (int)(head.y + 3.0f), (int)(head.x + dir * 5.0f), (int)(head.y + 2.5f), Color{ 40, 25, 20, 255 });
    }

    DrawLimb3Tone(shR, elbR, 4.0f, 3.4f, suit);
    DrawLimb3Tone(elbR, handR, 3.2f, 2.8f, forearmCol);
    DrawCircleV(handR, 3.6f, skin);
}

inline void DrawStageUpgrades(StageId stageId, float camX, float time) {

    for (int b = 0; b < 2; b++) {
        float pivotX = (b == 0) ? 75.0f : (SCREEN_WIDTH - 75.0f);
        float sway = sinf(time * (1.3f + b * 0.4f) + b * 2.2f) * 95.0f;
        Color beamCol = (stageId == STAGE_CONGRESSIONAL_FLOOR || stageId == STAGE_PENTAGON_ALTAR)
            ? Color{ 255, 215, 90, 22 }
            : Color{ 160, 225, 255, 22 };
        DrawTriangle(
            Vector2{ pivotX, (float)GROUND_Y },
            Vector2{ pivotX + sway - 28.0f, 0.0f },
            Vector2{ pivotX + sway + 28.0f, 0.0f },
            beamCol
        );
    }

    for (int c = 0; c < 10; c++) {
        float flashPhase = fmodf(time * 3.4f + c * 1.37f, 2.4f);
        if (flashPhase < 0.08f) {
            float fx = fmodf(c * 47.0f - camX * 0.45f + 800.0f, (float)SCREEN_WIDTH);
            float fy = 108.0f + (c % 3) * 12.0f;
            VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(0), fx, fy, 11.0f, 11.0f, (float)(c * 25));
        }
    }

    for (int p = 0; p < 16; p++) {
        float px = fmodf(p * 37.0f + time * (16.0f + (p % 4) * 4.0f) - camX * 0.6f + 1600.0f, (float)SCREEN_WIDTH);
        float py = fmodf(p * 23.0f + time * (18.0f + (p % 3) * 5.0f), GROUND_Y - 8.0f);
        if (stageId == STAGE_REFLECTING_POOL || stageId == STAGE_FLORIDA_KEYS || stageId == STAGE_LINCOLN_MEMORIAL) {
            DrawRectangle((int)px, (int)py, 2, 2, Color{ 255, 185, 215, 210 });
            DrawPixel((int)px + 1, (int)py, WHITE);
        } else if (stageId == STAGE_CONGRESSIONAL_FLOOR || stageId == STAGE_PRESS_BRIEFING || stageId == STAGE_WTC_PLAZA || stageId == STAGE_SMITHSONIAN_AERO || stageId == STAGE_ENN_NEWS_STUDIO) {
            float sway = sinf(time * 5.0f + p) * 3.0f;
            DrawRectangle((int)(px + sway), (int)py, 3, 2, Color{ 250, 245, 220, 195 });
        } else {
            float ey = GROUND_Y - py;
            DrawRectangle((int)px, (int)ey, 2, 2, (p % 2 == 0) ? Color{ 255, 190, 50, 200 } : Color{ 255, 90, 30, 190 });
        }
    }

    float minCam = STAGE_MIN_X;
    float maxCam = STAGE_MAX_X - SCREEN_WIDTH;
    float scrollT = std::clamp((camX - minCam) / (maxCam - minCam), 0.0f, 1.0f);
    float srcH = 640.0f;
    float srcW = srcH * ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
    float srcX = scrollT * (1140.0f - srcW);
    auto BgX = [&](float tx) -> float { return (tx - srcX) * ((float)SCREEN_WIDTH / srcW); };
    auto BgY = [&](float ty) -> float { return ty * ((float)SCREEN_HEIGHT / srcH); };

    if (stageId == STAGE_LINCOLN_MEMORIAL) {
        float fightProgress = std::clamp((200.0f - (s_p1TrailHp + s_p2TrailHp)) / 160.0f, 0.0f, 1.0f);
        float clapSpeed = 6.5f + fightProgress * 9.5f;
        float clapWave = fabsf(sinf(time * clapSpeed));
        int lincX = (int)BgX(564.0f);
        int lincHandsY = (int)BgY(186.0f);
        if (clapWave < 0.25f) {
            VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(0), (float)lincX, (float)lincHandsY, 10.0f, 10.0f, time * 40.0f);
        }
    } else if (stageId == STAGE_SMITHSONIAN_AERO) {
        if (fmodf(time * 2.6f, 1.0f) < 0.45f) {
            DrawPixel((int)BgX(255.0f), (int)BgY(62.0f), Color{ 255, 45, 45, 225 });
            DrawPixel((int)BgX(395.0f), (int)BgY(56.0f), Color{ 45, 255, 90, 225 });
        }
    } else if (stageId == STAGE_TOMB_UNKNOWN) {
        float flameX = BgX(992.0f);
        float flameY = BgY(188.0f);
        float flScale = 1.0f + sinf(time * 16.0f) * 0.10f;
        VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(11), flameX, flameY - 7.0f * flScale, 16.0f * flScale, 20.0f * flScale);

        static Texture2D s_sentinelTex = { 0 };
        static bool s_sentinelTexChecked = false;
        if (!s_sentinelTexChecked) {
            s_sentinelTexChecked = true;
            AssetManager::Init();
            const char* paths[3] = {
                "assets/spr_tomb_sentinel.png",
                "../assets/spr_tomb_sentinel.png",
                "spr_tomb_sentinel.png"
            };
            for (int p = 0; p < 3; p++) {
                if (AssetManager::FileExists(paths[p])) {
                    s_sentinelTex = AssetManager::LoadTexture(paths[p]);
                    if (s_sentinelTex.id != 0) {
                        SetTextureFilter(s_sentinelTex, TEXTURE_FILTER_POINT);
                        break;
                    }
                }
            }
        }

        auto drawMarchingSentinel = [&](float xA, float xB, float phaseOffset) {
            float cyc = fmodf(time * 0.12f + phaseOffset, 1.0f);
            float tPos = 0.0f;
            bool facingRight = true;
            bool isHalted = false;
            float haltProg = 0.0f;

            if (cyc < 0.40f) {
                tPos = cyc / 0.40f;
                facingRight = (xB >= xA);
            } else if (cyc < 0.50f) {
                tPos = 1.0f;
                facingRight = (xB < xA);
                isHalted = true;
                haltProg = (cyc - 0.40f) / 0.10f;
            } else if (cyc < 0.90f) {
                tPos = 1.0f - (cyc - 0.50f) / 0.40f;
                facingRight = (xA > xB);
            } else {
                tPos = 0.0f;
                facingRight = (xB >= xA);
                isHalted = true;
                haltProg = (cyc - 0.90f) / 0.10f;
            }

            float texX = xA + (xB - xA) * tPos;
            float sx = BgX(texX);
            float sy = BgY(285.0f);
            float dir = facingRight ? 1.0f : -1.0f;

            static const int stepCycle[4] = { 0, 1, 2, 1 };
            int frameIdx = isHalted ? 1 : stepCycle[((int)(time * 4.2f + phaseOffset * 8.0f)) & 3];
            int bobPx = (!isHalted && frameIdx != 1) ? 1 : 0;

            DrawEllipse((int)sx, (int)sy, 7.0f, 1.8f, Color{ 0, 0, 0, 135 });
            if (isHalted && haltProg < 0.45f) {
                VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(14), sx, sy, 14.0f + haltProg * 14.0f, 5.0f);
            }

            if (s_sentinelTex.id != 0) {
                float cellW = (float)s_sentinelTex.width / 3.0f;
                float cellH = (float)s_sentinelTex.height;
                Rectangle srcRec{ frameIdx * cellW, 0.0f, facingRight ? cellW : -cellW, cellH };
                Rectangle dstRec{ (float)((int)(sx - 17.0f)), (float)((int)(sy - 49.0f - bobPx)), 34.0f, 50.0f };
                DrawTexturePro(s_sentinelTex, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            }

            if (fmodf(time * 2.2f + phaseOffset, 1.0f) < 0.22f) {
                int bx = (int)(sx - dir * 13.0f);
                int by = (int)(sy - 46.0f - bobPx);
                VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(0), (float)bx, (float)by, 6.0f, 6.0f);
            }
        };

        drawMarchingSentinel(175.0f, 465.0f, 0.0f);
        drawMarchingSentinel(815.0f, 525.0f, 0.45f);
    } else if (stageId == STAGE_ENN_NEWS_STUDIO) {
        if (fmodf(time * 2.4f, 1.0f) < 0.65f) {
            DrawRectangle((int)BgX(118.0f), (int)BgY(276.0f), 2, 2, Color{ 255, 35, 35, 235 });
            DrawRectangle((int)BgX(298.0f), (int)BgY(320.0f), 2, 2, Color{ 255, 35, 35, 235 });
        }
    } else if (stageId == STAGE_CHICAGO_WRIGLEY) {
        for (int b = 0; b < 8; b++) {
            float bx = BgX(370.0f + b * 38.0f);
            float by = BgY(212.0f);
            bool lit = (((int)(time * 8.0f) + b) % 2) == 0;
            DrawRectangle((int)bx, (int)by, 2, 2, lit ? Color{ 255, 240, 110, 250 } : Color{ 180, 60, 60, 190 });
        }
        for (int s = 0; s < 3; s++) {
            float phase = fmodf(time * 1.4f + s * 0.55f, 1.8f);
            float sx = BgX(88.0f + s * 9.0f) + sinf(time * 5.0f + s) * 2.0f;
            float sy = BgY(355.0f) - phase * 16.0f;
            unsigned char sa = (unsigned char)std::clamp((int)((1.8f - phase) * 105.0f), 0, 165);
            float sz = 6.0f + phase * 4.0f;
            VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(10), sx, sy, sz, sz, 0.0f, Color{ 255, 255, 255, sa });
        }
    }

    float wSrcH = 700.0f;
    float wSrcW = wSrcH * ((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT);
    float wSrcX = scrollT * (1376.0f - wSrcW);
    auto WBgX = [&](float tx) -> float { return (tx - wSrcX) * ((float)SCREEN_WIDTH / wSrcW); };
    auto WBgY = [&](float ty) -> float { return (ty - 52.0f) * ((float)SCREEN_HEIGHT / wSrcH); };

    float excite = std::clamp(s_crowdExciteTimer, 0.0f, 1.5f);

    if (stageId == STAGE_RYUGYONG_HOTEL) {

        for (int m = 0; m < 14; m++) {
            if (fmodf(time * 3.5f + m * 0.29f, 1.0f) < 0.16f) {
                float mx = WBgX(615.0f + (m % 7) * 112.0f + (m * 11 % 24));
                float my = WBgY(435.0f + (m % 3) * 16.0f);
                DrawPixel((int)mx, (int)my, Color{ 255, 240, 110, 245 });
            }
        }
    } else if (stageId == STAGE_WESTERN_WALL) {
        for (int m = 0; m < 12; m++) {
            float mx = fmodf(m * 39.0f + time * 9.0f - camX * 0.35f + 900.0f, (float)SCREEN_WIDTH);
            float my = 24.0f + fmodf(m * 19.0f + sinf(time * 2.0f + m) * 8.0f + 200.0f, 128.0f);
            DrawPixel((int)mx, (int)my, Color{ 255, 245, 190, 200 });
        }
    } else if (stageId == STAGE_SOVIET_FOUNDRY) {
        for (int s = 0; s < 18; s++) {
            float life = fmodf(time * (1.4f + (s % 4) * 0.35f) + s * 0.19f, 1.0f);
            float originX = (s % 2 == 0) ? WBgX(480.0f) : WBgX(895.0f);
            float sx = originX + sinf(time * 6.0f + s * 1.7f) * (12.0f + life * 24.0f);
            float sy = WBgY(345.0f) - life * 95.0f;
            Color spCol = (s % 3 == 0) ? Color{ 255, 245, 130, 240 } : Color{ 255, 115, 25, 220 };
            DrawRectangle((int)sx, (int)sy, 2, 2, spCol);
        }
    }

    if (excite > 0.05f) {
        int flashCount = 3 + (int)(excite * 4.0f);
        for (int f = 0; f < flashCount; f++) {
            if (fmodf(time * 9.0f + f * 0.63f, 1.0f) < 0.12f) {
                float fx = fmodf(f * 67.0f - camX * 0.5f + 600.0f, (float)SCREEN_WIDTH);
                float fy = 96.0f + (f % 4) * 14.0f;
                VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(0), fx, fy, 10.0f, 10.0f, (float)(f * 35));
            }
        }
    }
}

inline void UpdateAndTrackFighters(Fighter& p1, Fighter& p2) {
    if (s_p1TrailHp > (float)p1.GetHealth()) {
        s_p1TrailHp = std::max((float)p1.GetHealth(), s_p1TrailHp - 0.35f);
    } else {
        s_p1TrailHp = (float)p1.GetHealth();
    }
    if (s_p2TrailHp > (float)p2.GetHealth()) {
        s_p2TrailHp = std::max((float)p2.GetHealth(), s_p2TrailHp - 0.35f);
    } else {
        s_p2TrailHp = (float)p2.GetHealth();
    }

    bool critical = (p1.GetHealth() > 0 && p1.GetHealth() < 30) || (p2.GetHealth() > 0 && p2.GetHealth() < 30);
    AudioEngine::SetCriticalTempo(critical);

    s_ties[0].Update(Vector2{ p1.GetPosition().x + (p1.IsFacingRight() ? 4.0f : -4.0f), p1.GetPosition().y - (p1.IsCrouching() ? 48.0f : 68.0f) }, p1.GetVelocity(), p1.IsFacingRight());
    s_ties[1].Update(Vector2{ p2.GetPosition().x + (p2.IsFacingRight() ? 4.0f : -4.0f), p2.GetPosition().y - (p2.IsCrouching() ? 48.0f : 68.0f) }, p2.GetVelocity(), p2.IsFacingRight());

    const Fighter* fighters[2] = { &p1, &p2 };
    for (int i = 0; i < 2; i++) {
        const Fighter& f = *fighters[i];
        FighterState st = f.GetState();
        if (f.GetHealth() <= 0 || st == FSTATE_KNOCKDOWN_AIR || st == FSTATE_KNOCKDOWN_GROUND || st == FSTATE_DEFEAT || st == FSTATE_WAKEUP || st == FSTATE_HITSTUN) {
            s_afterimages[i].clear();
            continue;
        }
        for (auto& ai : s_afterimages[i]) ai.life--;
        s_afterimages[i].erase(std::remove_if(s_afterimages[i].begin(), s_afterimages[i].end(), [](const CEAfterimage& a) { return a.life <= 0; }), s_afterimages[i].end());
        if ((st == FSTATE_SPECIAL || st == FSTATE_DASH_FWD || st == FSTATE_DASH_BACK || st == FSTATE_JUMP || (st == FSTATE_ATTACK && f.GetCurrentAttack().isHeavy)) && (f.GetStateFrame() % 3 == 0)) {
            CEAfterimage ghost{};
            ghost.pos = f.GetPosition();
            ghost.facingRight = f.IsFacingRight();
            ghost.state = st;
            ghost.frame = f.GetStateFrame();
            ghost.life = f.IsSuperMoveActive() ? 14 : 9;
            s_afterimages[i].push_back(ghost);
        }
        if ((st == FSTATE_DASH_FWD || st == FSTATE_DASH_BACK) && (f.GetStateFrame() % 2 == 0)) {
            CEParticle dust{};
            dust.pos = Vector2{ f.GetPosition().x, GROUND_Y - 2.0f };
            dust.vel = Vector2{ (st == FSTATE_DASH_FWD ? -1.4f : 1.4f) * (f.IsFacingRight() ? 1.0f : -1.0f), -0.6f - (rand() % 10) * 0.08f };
            dust.color = Color{ 235, 230, 215, 200 };
            dust.size = 3.0f;
            dust.maxFrames = 14;
            dust.isPaper = false;
            dust.alive = true;
            s_particles.push_back(dust);
        }
    }

    if (s_crowdExciteTimer > 0.0f) {
        s_crowdExciteTimer = std::max(0.0f, s_crowdExciteTimer - (1.0f / 60.0f));
    }

    if (p1.PopSuperFlashTrigger()) {
        s_superFlashTimer = 28;
        s_superFlashPlayer = 0;
        s_superFlashMove = (p1.GetCurrentAttack().moveType == MOVE_SUPER && !p1.GetCurrentAttack().name.empty())
            ? p1.GetCurrentAttack().name : "SUPER VETO COMBO!";
        s_crowdExciteTimer = 2.4f;
        PushBanner(0, "SUPER VETO!", p1.GetName().c_str(), Color{ 255, 215, 40, 255 });
    }
    if (p2.PopSuperFlashTrigger()) {
        s_superFlashTimer = 28;
        s_superFlashPlayer = 1;
        s_superFlashMove = (p2.GetCurrentAttack().moveType == MOVE_SUPER && !p2.GetCurrentAttack().name.empty())
            ? p2.GetCurrentAttack().name : "SUPER VETO COMBO!";
        s_crowdExciteTimer = 2.4f;
        PushBanner(1, "SUPER VETO!", p2.GetName().c_str(), Color{ 255, 215, 40, 255 });
    }

    if (p1.PopAlphaCounterCallout()) {
        PushBanner(0, "ALPHA COUNTER!", "GUARD REVERSAL", Color{ 80, 255, 160, 255 });
        s_crowdExciteTimer = 1.6f;
    }
    if (p2.PopAlphaCounterCallout()) {
        PushBanner(1, "ALPHA COUNTER!", "GUARD REVERSAL", Color{ 80, 255, 160, 255 });
        s_crowdExciteTimer = 1.6f;
    }
    if (p1.PopThrowTechCallout()) PushBanner(0, "THROW TECH!", "GRAB ESCAPED", Color{ 130, 230, 255, 255 });
    if (p2.PopThrowTechCallout()) PushBanner(1, "THROW TECH!", "GRAB ESCAPED", Color{ 130, 230, 255, 255 });

    if (p1.PopCancelCallout()) PushBanner(0, "2-IN-1 CANCEL!", "TURBO LINK", Color{ 90, 230, 255, 255 });
    if (p2.PopCancelCallout()) PushBanner(1, "2-IN-1 CANCEL!", "TURBO LINK", Color{ 90, 230, 255, 255 });
    if (p1.PopReversalCallout()) {
        PushBanner(0, "REVERSAL!", "TOTAL VETO", Color{ 255, 110, 50, 255 });
        SpawnImpactJuice(Vector2{ p1.GetPosition().x, p1.GetPosition().y - 44.0f }, true, true, "VETO!");
    }
    if (p2.PopReversalCallout()) {
        PushBanner(1, "REVERSAL!", "TOTAL VETO", Color{ 255, 110, 50, 255 });
        SpawnImpactJuice(Vector2{ p2.GetPosition().x, p2.GetPosition().y - 44.0f }, true, true, "VETO!");
    }

    static const char* s_batmanLightWords[6] = { "POW!", "BAM!", "BIFF!", "SOCK!", "WHAM!", "ZAP!" };
    static const char* s_batmanHeavyWords[4] = { "KAPOW!", "SMACK!", "WHAM!", "BAM!" };
    int c1 = p1.GetComboCount();
    int c2 = p2.GetComboCount();
    if (c1 > s_prevP1Hits && c1 >= 1) {
        if (!s_firstStrikeAwarded) {
            s_firstStrikeAwarded = true;
            PushBanner(0, "FIRST STRIKE!", p1.GetName().c_str(), Color{ 255, 230, 60, 255 });
        }
        if (c1 >= 2) {
            char buf[48];
            snprintf(buf, sizeof(buf), "%d HIT COMBO!", c1);
            PushBanner(0, buf, p1.GetCurrentAttack().name.c_str(), Color{ 255, 190, 40, 255 });
            s_crowdExciteTimer = std::max(s_crowdExciteTimer, 0.9f + c1 * 0.2f);
        }
        bool isSpec = (p1.GetState() == FSTATE_SPECIAL);
        const char* word1 = isSpec ? "VETO!" : (p1.GetCurrentAttack().isHeavy ? s_batmanHeavyWords[(c1 + rand()) % 4] : s_batmanLightWords[(c1 + rand()) % 6]);
        float hitX = p2.GetPosition().x + (p1.GetPosition().x < p2.GetPosition().x ? 12.0f : -12.0f);
        float hitY = p2.GetPosition().y - 44.0f;
        SpawnImpactJuice(Vector2{ hitX, hitY }, p1.GetCurrentAttack().isHeavy, isSpec, word1);
    }
    if (c2 > s_prevP2Hits && c2 >= 1) {
        if (!s_firstStrikeAwarded) {
            s_firstStrikeAwarded = true;
            PushBanner(1, "FIRST STRIKE!", p2.GetName().c_str(), Color{ 255, 230, 60, 255 });
        }
        if (c2 >= 2) {
            char buf[48];
            snprintf(buf, sizeof(buf), "%d HIT COMBO!", c2);
            PushBanner(1, buf, p2.GetCurrentAttack().name.c_str(), Color{ 255, 120, 60, 255 });
            s_crowdExciteTimer = std::max(s_crowdExciteTimer, 0.9f + c2 * 0.2f);
        }
        bool isSpec = (p2.GetState() == FSTATE_SPECIAL);
        const char* word2 = isSpec ? "REDACTED!" : (p2.GetCurrentAttack().isHeavy ? "OBJECTION!" : s_batmanLightWords[(c2 + rand()) % 6]);
        float hitX = p1.GetPosition().x + (p2.GetPosition().x < p1.GetPosition().x ? 12.0f : -12.0f);
        float hitY = p1.GetPosition().y - 44.0f;
        SpawnImpactJuice(Vector2{ hitX, hitY }, p2.GetCurrentAttack().isHeavy, isSpec, word2);
    }
    s_prevP1Hits = c1;
    s_prevP2Hits = c2;

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
        st.pos.y -= 0.14f;
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
    if (f.GetCharacterId() == CHAR_THE_PUNDIT && f.GetState() == FSTATE_SPECIAL && f.GetSpecialIndex() == 1) {
        int fr = f.GetStateFrame();
        float dir = f.IsFacingRight() ? 1.0f : -1.0f;
        float wbCenterX = f.GetPosition().x + dir * 42.0f;
        float wbCenterY = f.GetPosition().y - 54.0f;
        float popScale = std::min(1.0f, 0.55f + fr * 0.08f);
        VisualsEngine::DrawCenteredSprite(VisualsEngine::GetWorldProp(14), wbCenterX, wbCenterY, 64.0f * popScale, 52.0f * popScale);
        if (fr >= 6) {
            VisualsEngine::DrawPixelText("270 TO WIN!", (int)wbCenterX - 24, (int)(wbCenterY - 34.0f), 6, Color{ 255, 235, 80, 255 });
        }
    }

    if (f.GetCharacterId() == CHAR_SI) {
        FighterState st = f.GetState();
        float t = (float)GetTime();
        if (st == FSTATE_IDLE || st == FSTATE_WALK_FWD || st == FSTATE_WALK_BACK || st == FSTATE_CROUCH) {
            for (int d = 0; d < 7; d++) {
                float dx = f.GetPosition().x - 16.0f + ((d * 9 + (int)(t * 14.0f)) % 32);
                float dy = f.GetPosition().y - 68.0f + fmodf(d * 11.0f + t * 38.0f, 58.0f);
                const char* bit = (((int)(t * 12.0f) + d) % 2 == 0) ? "1" : "0";
                VisualsEngine::DrawPixelText(bit, (int)dx, (int)dy, 6, Color{ 45, 255, 95, 215 });
            }
        }
    }

    if (f.GetCharacterId() == CHAR_COUCHIE && f.GetState() == FSTATE_SPECIAL) {
        int fr = f.GetStateFrame();
        float pulse = fabsf(sinf(fr * 0.45f));
        VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(14), f.GetPosition().x, GROUND_Y - 3.0f, 68.0f + pulse * 8.0f, 16.0f, 0.0f, Color{ 165, 240, 255, 235 });
    }

    if (f.GetCharacterId() == CHAR_THE_DICTATOR && f.GetState() == FSTATE_SPECIAL) {
        float cx = f.GetPosition().x;
        VisualsEngine::DrawCenteredSprite(VisualsEngine::GetWorldProp(12), cx, GROUND_Y - 12.0f, 56.0f, 38.0f);
        if (f.IsSpiderHoleHidden()) {
            VisualsEngine::DrawPixelText("SPIDER HOLE!", (int)cx - 26, (int)GROUND_Y - 34, 6, Color{ 255, 225, 80, 235 });
        }
    }

    if (f.GetStarBurstTimer() > 0) {
        float prog = (float)f.GetStarBurstTimer() / 65.0f;
        float t = (float)GetTime() * 12.0f;
        for (int s = 0; s < 6; s++) {
            float ang = t + s * (2.0f * PI_F / 6.0f);
            float rad = 14.0f + (1.0f - prog) * 22.0f;
            float sx = f.GetPosition().x + cosf(ang) * rad;
            float sy = (f.GetPosition().y - 46.0f) + sinf(ang * 1.3f) * (rad * 0.65f);
            const Texture2D& stTex = (s % 2 == 0) ? VisualsEngine::GetVfxSprite(6) : VisualsEngine::GetVfxSprite(0);
            VisualsEngine::DrawCenteredSprite(stTex, sx, sy, 14.0f, 14.0f, (t * 45.0f) + s * 30.0f);
        }
    }

    if (f.GetElectroShockTimer() > 0) {
        int et = f.GetElectroShockTimer();
        float cx = f.GetPosition().x;
        float cy = f.GetPosition().y - 42.0f;
        for (int b = 0; b < 4; b++) {
            float ang = (float)b * (PI_F * 0.5f) + (float)et * 0.65f;
            float ax = cx + cosf(ang) * 15.0f;
            float ay = cy + sinf(ang) * 20.0f;
            const Texture2D& bolt = ((b + et) % 2 == 0) ? VisualsEngine::GetVfxSprite(8) : VisualsEngine::GetVfxSprite(9);
            VisualsEngine::DrawCenteredSprite(bolt, ax, ay, 34.0f, 34.0f, ang * 57.2958f, WHITE, (b % 2 == 1));
        }
        if ((et % 3) == 0) {
            VisualsEngine::DrawPixelText("BZZZT!", (int)cx - 18, (int)(f.GetPosition().y - 84.0f), 7, Color{ 110, 250, 255, 255 });
        }
    }

    if (f.GetCharacterId() == CHAR_W && f.GetState() == FSTATE_VICTORY) {
        int vfr = f.GetStateFrame();
        float dir = f.IsFacingRight() ? 1.0f : -1.0f;
        float teeX = f.GetPosition().x + dir * 22.0f;
        float teeY = GROUND_Y - 2.0f;
        if (vfr < 18) {
            VisualsEngine::DrawCenteredSprite(VisualsEngine::GetWorldProp(13), teeX, teeY - 6.0f, 12.0f, 16.0f);
        } else {
            float tb = (float)(vfr - 18);
            for (int k = 1; k <= 6; k++) {
                float u = std::max(0.0f, tb - k * 1.8f);
                float tx = teeX + dir * (u * 6.8f);
                float ty = (teeY - 6.0f) - u * 4.1f + u * u * 0.025f;
                unsigned char ta = (unsigned char)std::max(35, 210 - k * 28);
                VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(0), tx, ty, 7.0f, 7.0f, u * 15.0f, Color{ 255, 245, 175, ta });
            }
            float bx = teeX + dir * (tb * 6.8f);
            float by = (teeY - 6.0f) - tb * 4.1f + tb * tb * 0.025f;
            VisualsEngine::DrawCenteredSprite(VisualsEngine::GetWorldProp(13), bx, by, 10.0f, 12.0f, tb * 24.0f);
            if (vfr < 28) {
                VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(1), teeX, teeY - 5.0f, 18.0f, 18.0f);
            }
        }
    }

    if (f.GetCharacterId() == CHAR_AYATOLLAH && f.GetState() == FSTATE_CALL_TO_PRAYER) {
        int fr = f.GetStateFrame();
        float cx = f.GetPosition().x;
        float gy = GROUND_Y;
        float pulse = 0.5f + 0.5f * sinf(fr * 0.35f);

        VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(14), cx, gy - 3.0f, 72.0f + pulse * 10.0f, 16.0f);
        float mcX = cx + (f.IsFacingRight() ? 24.0f : -24.0f);
        float mcY = gy - 80.0f - sinf(fr * 0.18f) * 4.0f;
        VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(15), mcX, mcY, 26.0f, 26.0f);
        for (int k = 0; k < 5; k++) {
            float sx = cx - 22.0f + ((k * 13 + fr * 2) % 44);
            float sy = gy - 14.0f - fmodf((float)(fr * 2.4f + k * 18.0f), 92.0f);
            VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(0), sx, sy, 8.0f, 8.0f, (float)(fr * 6 + k * 30));
        }
        char prayTag[48];
        snprintf(prayTag, sizeof(prayTag), "CALL TO PRAYER! (%d/7)", f.GetRefillCount());
        int ptw = MeasureText(prayTag, 7);
        DrawRectangle((int)cx - ptw / 2 - 4, (int)gy - 150, ptw + 8, 10, Color{ 8, 28, 22, 235 });
        DrawRectangleLines((int)cx - ptw / 2 - 4, (int)gy - 150, ptw + 8, 10, Color{ 255, 220, 65, 255 });
        VisualsEngine::DrawPixelText(prayTag, (int)cx - ptw / 2, (int)gy - 148, 7, Color{ 255, 240, 95, 255 });
    }

    if ((f.GetState() == FSTATE_HITSTUN && f.GetHealth() > 0) ||
        f.GetState() == FSTATE_FINISH_HIM_DIZZY ||
        f.GetState() == FSTATE_FINISHER_VICTIM) {
        bool isDizzy = (f.GetState() == FSTATE_FINISH_HIM_DIZZY || f.GetState() == FSTATE_FINISHER_VICTIM);
        float t = (float)GetTime() * (isDizzy ? 8.5f : 7.5f);
        int numOrbit = isDizzy ? 6 : 4;
        float orbitRx = isDizzy ? 22.0f : 18.0f;
        float orbitRy = isDizzy ? 7.0f : 5.5f;
        for (int s = 0; s < numOrbit; s++) {
            float ang = t + s * (2.0f * PI_F / (float)numOrbit);
            float sx = f.GetPosition().x + cosf(ang) * orbitRx;
            float sy = f.GetPosition().y - 74.0f + sinf(ang) * orbitRy;
            bool useBird = isDizzy && (s % 2 == 1);
            const Texture2D& orbTex = useBird ? VisualsEngine::GetVfxSprite(7) : VisualsEngine::GetVfxSprite(6);
            float sz = useBird ? 14.0f : 12.0f;
            VisualsEngine::DrawCenteredSprite(orbTex, sx, sy, sz, sz, useBird ? 0.0f : (t * 25.0f), WHITE, cosf(ang) < 0.0f);
        }
    }
}

inline void DrawWorldParticlesAndDebug(const Fighter& p1, const Fighter& p2) {
    for (const auto& p : s_particles) {
        if (!p.alive) continue;
        if (p.isPaper) {
            float pw = std::max(8.0f, p.size * 2.2f);
            float ph = std::max(5.0f, p.size * 1.3f);
            VisualsEngine::DrawCenteredSprite(VisualsEngine::GetWorldProp(10), p.pos.x, p.pos.y, pw, ph, p.rot, Color{ 255, 255, 255, p.color.a });
        } else {
            float sw = std::max(7.0f, p.size * 2.8f);
            VisualsEngine::DrawCenteredSprite(VisualsEngine::GetVfxSprite(13), p.pos.x, p.pos.y, sw, sw, 0.0f, Color{ 255, 255, 255, p.color.a });
        }
    }

    for (const auto& st : s_stamps) {
        if (!st.alive) continue;
        float popScale = (st.frame < 4) ? (0.72f + st.frame * 0.05f) : 0.88f;
        bool isCrimson = (st.color.r > 220 && st.color.g < 100);
        Color outerCol = isCrimson ? Color{ 245, 38, 32, 255 } : Color{ 255, 115, 20, 255 };
        Color innerCol = isCrimson ? Color{ 255, 230, 45, 255 } : Color{ 255, 245, 70, 255 };
        Color txtCol   = isCrimson ? Color{ 255, 255, 255, 255 } : Color{ 220, 22, 28, 255 };
        DrawBatmanActionBurst(st.text.c_str(), (int)st.pos.x, (int)st.pos.y, 7, outerCol, innerCol, txtCol, popScale);
    }
    (void)p1;
    (void)p2;
}

inline void DrawScreenHUDAndSuperFlash(const Fighter& p1, const Fighter& p2, const InputManager& in1) {

    const char* turboBadge = (s_turboSpeed == TURBO_SPEED_1) ? "TURBO 1" : ((s_turboSpeed == TURBO_SPEED_3) ? "TURBO 3 MAX" : "TURBO 2");
    int ceTw = MeasureText(turboBadge, 6);
    VisualsEngine::DrawPixelText(turboBadge, (SCREEN_WIDTH - ceTw) / 2, 36, 6, Color{ 95, 235, 255, 235 });

    auto drawSuperGauge = [](int x, int y, int meter, bool flipRight) {
        constexpr int SEG_W = 36;
        constexpr int SEG_H = 6;
        constexpr int GAP = 2;
        int totalW = SEG_W * 3 + GAP * 2;
        int lvl = meter / 100;
        bool isMax = (meter >= 300);
        float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 14.0f);

        DrawRectangle(x - 2, y - 2, totalW + 4, SEG_H + 4, Color{ 8, 12, 22, 230 });
        DrawRectangleLines(x - 2, y - 2, totalW + 4, SEG_H + 4, isMax ? Color{ 255, 235, 65, 255 } : Color{ 70, 130, 195, 235 });

        for (int s = 0; s < 3; s++) {
            int segIdx = flipRight ? (2 - s) : s;
            int sx = x + s * (SEG_W + GAP);
            DrawRectangle(sx, y, SEG_W, SEG_H, Color{ 20, 28, 46, 240 });
            int segFill = std::clamp(meter - segIdx * 100, 0, 100);
            if (segFill > 0) {
                int fw = (segFill * SEG_W) / 100;
                int fx = flipRight ? (sx + SEG_W - fw) : sx;
                Color cTop = (segIdx == 0) ? Color{ 65, 210, 255, 255 }
                           : ((segIdx == 1) ? Color{ 255, 210, 45, 255 } : Color{ 255, 75, 55, 255 });
                if (isMax && ((int)(GetTime() * 18.0f) % 2 == 0)) cTop = Color{ 255, 255, 160, 255 };
                DrawRectangle(fx, y, fw, SEG_H, cTop);
                DrawLine(fx, y, fx + fw, y, Color{ 255, 255, 255, (unsigned char)(140 + pulse * 100) });
            }
            DrawRectangleLines(sx, y, SEG_W, SEG_H, Color{ 45, 75, 115, 220 });
        }

        const char* lbl = isMax ? "MAX SUPER!" : ((lvl == 2) ? "SUPER LV.2" : ((lvl == 1) ? "SUPER LV.1" : "SUPER"));
        Color lblCol = isMax ? Color{ 255, 240, 75, 255 } : ((lvl >= 1) ? Color{ 110, 235, 255, 255 } : Color{ 165, 185, 210, 220 });
        int lw = MeasureText(lbl, 6);
        int lx = flipRight ? (x + totalW - lw) : x;
        VisualsEngine::DrawPixelText(lbl, lx, y - 8, 6, lblCol);
    };

    drawSuperGauge(10, SCREEN_HEIGHT - 10, p1.GetSuperMeter(), false);
    drawSuperGauge(SCREEN_WIDTH - 10 - (36 * 3 + 4), SCREEN_HEIGHT - 10, p2.GetSuperMeter(), true);

    if (p1.IsPerformingFinisher() || p2.IsPerformingFinisher()) {
        const Fighter& atk = p1.IsPerformingFinisher() ? p1 : p2;
        float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 22.0f);
        const char* fName = Fighter::GetFinisherName(atk.GetCharacterId());
        int fnw = MeasureText(fName, 12);
        int bx = std::max(6, (SCREEN_WIDTH - fnw) / 2 - 8);
        int bw = std::min(SCREEN_WIDTH - 12, fnw + 16);
        DrawRectangle(bx, 42, bw, 24, Color{ 16, 6, 18, 235 });
        DrawRectangleLines(bx, 42, bw, 24, Color{ 255, 215, 55, 255 });
        DrawRectangleLines(bx + 1, 43, bw - 2, 22, Color{ 235, 35, 55, 240 });
        const char* subHdr = "SPECIAL FINISHING MOVE!";
        int shw = MeasureText(subHdr, 6);
        VisualsEngine::DrawPixelText(subHdr, (SCREEN_WIDTH - shw) / 2, 44, 6, Color{ 110, 240, 255, 255 });
        Color fTop = (pulse > 0.5f) ? Color{ 255, 245, 95, 255 } : Color{ 255, 145, 55, 255 };
        DrawArcade3DText(fName, (SCREEN_WIDTH - fnw) / 2, 52, 12, fTop, Color{ 205, 20, 35, 255 });
    } else if (s_finishHimActive) {
        float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 18.0f);
        const char* fh = "FINISH HIM!";
        int fhw = MeasureText(fh, 24);
        int fhx = (SCREEN_WIDTH - fhw) / 2;
        int fhy = 50;
        Color topC = (pulse > 0.5f) ? Color{ 255, 245, 90, 255 } : Color{ 255, 90, 50, 255 };
        DrawArcade3DText(fh, fhx, fhy, 24, topC, Color{ 195, 15, 25, 255 });
    } else if (s_vetoedKoActive) {
        float pulse = 0.5f + 0.5f * sinf((float)GetTime() * 20.0f);
        const char* vt = "VETOED!";
        int vtw = MeasureText(vt, 28);
        int vtx = (SCREEN_WIDTH - vtw) / 2;
        int vty = 56;
        Color topC = (pulse > 0.5f) ? Color{ 255, 248, 110, 255 } : Color{ 255, 215, 45, 255 };
        DrawArcade3DText(vt, vtx, vty, 28, topC, Color{ 215, 20, 30, 255 });
    }

    for (const auto& b : s_banners) {
        float slide = std::min(1.0f, (float)(b.maxTimer - b.timer) / 5.0f);
        int bw = 132;
        int bx = (b.playerIndex == 0) ? (int)(-bw + slide * (bw + 10)) : (int)(SCREEN_WIDTH - slide * (bw + 6));
        int by = 46 + b.playerIndex * 27;

        Color topCol = Color{ 255, 245, 95, 255 };
        Color botCol = b.color;
        Color slashCol = (b.playerIndex == 0) ? Color{ 255, 195, 35, 245 } : Color{ 255, 95, 45, 245 };

        int slashY = by + 12;
        int slashW = MeasureText(b.text.c_str(), 9) + 14;
        DrawLineEx(Vector2{ (float)bx - 2.0f, (float)slashY + 2.0f }, Vector2{ (float)(bx + slashW), (float)slashY - 1.0f }, 4.0f, Color{ 10, 12, 22, 230 });
        DrawLineEx(Vector2{ (float)bx - 2.0f, (float)slashY + 1.0f }, Vector2{ (float)(bx + slashW), (float)slashY - 2.0f }, 2.2f, slashCol);
        for (int k = 0; k < 3; k++) {
            float tx = (float)(bx + slashW - 14 + k * 5);
            DrawLineEx(Vector2{ tx, (float)slashY + 3.0f }, Vector2{ tx + 3.0f, (float)slashY - 3.0f }, 1.6f, WHITE);
        }

        if (b.text.size() >= 2 && b.text[0] >= '0' && b.text[0] <= '9') {
            size_t sp = b.text.find(' ');
            if (sp != std::string::npos) {
                std::string numStr = b.text.substr(0, sp);
                std::string restStr = b.text.substr(sp + 1);
                int nw = MeasureText(numStr.c_str(), 14);
                DrawArcade3DText(numStr.c_str(), bx, by - 4, 14, Color{ 255, 255, 140, 255 }, Color{ 255, 110, 25, 255 });
                DrawArcade3DText(restStr.c_str(), bx + nw + 4, by + 1, 9, topCol, botCol);
            } else {
                DrawArcade3DText(b.text.c_str(), bx, by, 9, topCol, botCol);
            }
        } else {
            DrawArcade3DText(b.text.c_str(), bx, by, 9, topCol, botCol);
        }

        DrawArcade3DText(b.subtext.c_str(), bx + 4, by + 14, 6, Color{ 165, 240, 255, 255 }, Color{ 55, 155, 235, 255 });
    }

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
    (void)in1;
}

}
