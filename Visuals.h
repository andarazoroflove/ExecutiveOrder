#pragma once
#include "Common.h"
#include "Fighter.h"
#include "Collision.h"
#include "Stages.h"
#include "InputManager.h"
#include "AudioEngine.h"
#include <vector>

class VisualsEngine {
public:
    static void Init();
    static void Shutdown();

    static void DrawDropShadow(const Fighter& f);
    static void DrawFighterSprite(const Fighter& f);
    static void DrawProjectiles(const Fighter& f);
    static void DrawSparks(const std::vector<SparkEffect>& sparks);
    static void DrawBattleHUD(const Fighter& p1, const Fighter& p2, int roundTimer, int roundNumber, int fightIntroTimer = 0, bool p1WasKo = false, bool p2WasKo = false);
    static void DrawCinematicIntro(float timer, bool logoDropped, const Shader& flagShader, const Texture2D& flagTex);
    static void DrawStageAndCharSelect(int selectedStage, int p1Char, int p2Char, float timer, const bool defeatedFighters[CHARACTER_COUNT], bool playerLocked = false, bool choosingOpponent = false);
    static void DrawSecretSelectScreen(int step, int p1Char, int p2Char, int stage, int difficulty, float timer);
    static void DrawVersusScreen(const Fighter& p1, const Fighter& p2, float timer);
    static void DrawVictorySplash(int winnerChar, int loserChar, float timer);
    static void DrawBonusStage(float carHp, float maxCarHp, float timer, const Fighter& player, const std::vector<SparkEffect>& sparks);
    static void DrawMatchOver(const Fighter& winner, float timer);
    static void DrawEndingCelebration(float timer);
    static void DrawCthulhuEnding(float timer);
    static void DrawPauseOverlay(float timer);

    static void DrawPixelText(const char* text, int x, int y, int size, Color col);
    static Texture2D CreateProceduralFlagTexture();

private:
    static bool s_loaded;
    static Texture2D s_titleSplash;
    static Texture2D s_portraits[CHARACTER_COUNT];
    static Texture2D s_portraitsLose[CHARACTER_COUNT];

    static Texture2D s_sprites[CHARACTER_COUNT][6];
    static Texture2D s_teslaSprites[4];
    static Texture2D s_bonusStageBg;
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

inline bool s_debugHitboxes = false;
inline bool s_forceProceduralRig = false;
inline int  s_superFlashTimer = 0;
inline int  s_superFlashPlayer = 0;
inline std::string s_superFlashMove = "TOTAL VETO FLURRY!";
inline float s_p1TrailHp = 100.0f;
inline float s_p2TrailHp = 100.0f;
inline int   s_continuesRemaining = 2;
inline bool  s_isSecretMatch = false;
inline bool  s_opponentLocked = false;

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
        st.pos = Vector2{ pos.x, pos.y - 20.0f };
        st.text = stampText;
        st.color = isSpecial ? Color{ 255, 55, 55, 255 } : Color{ 255, 220, 40, 255 };
        st.frame = 0;
        st.maxFrames = 34;
        st.alive = true;
        s_stamps.push_back(st);
    }
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
            DrawCircle((int)fx, (int)fy, 9, Color{ 255, 255, 240, 170 });
            DrawCircle((int)fx, (int)fy, 4, WHITE);
            DrawLine((int)fx - 12, (int)fy, (int)fx + 12, (int)fy, WHITE);
            DrawLine((int)fx, (int)fy - 12, (int)fx, (int)fy + 12, WHITE);
        }
    }

    int gy = (int)GROUND_Y;
    DrawLine(0, gy, SCREEN_WIDTH, gy, Color{ 255, 225, 110, 95 });
    for (int r = 1; r <= 4; r++) {
        int ry = gy + r * 6;
        if (ry < SCREEN_HEIGHT) {
            DrawLine(0, ry, SCREEN_WIDTH, ry, Color{ 255, 255, 255, (unsigned char)(18 + r * 6) });
        }
    }
    float floorScroll = fmodf(-camX, 32.0f);
    for (int i = -4; i < 18; i++) {
        float topX = i * 32.0f + floorScroll;
        float distFromCenter = (topX - SCREEN_WIDTH * 0.5f);
        float botX = SCREEN_WIDTH * 0.5f + distFromCenter * 1.48f;
        DrawLine((int)topX, gy, (int)botX, SCREEN_HEIGHT, Color{ 255, 255, 255, 32 });
    }

    for (int p = 0; p < 16; p++) {
        float px = fmodf(p * 37.0f + time * (16.0f + (p % 4) * 4.0f) - camX * 0.6f + 1600.0f, (float)SCREEN_WIDTH);
        float py = fmodf(p * 23.0f + time * (18.0f + (p % 3) * 5.0f), GROUND_Y - 8.0f);
        if (stageId == STAGE_REFLECTING_POOL || stageId == STAGE_FLORIDA_KEYS || stageId == STAGE_LINCOLN_MEMORIAL) {

            DrawCircle((int)px, (int)py, 2.0f, Color{ 255, 185, 215, 210 });
            DrawPixel((int)px + 1, (int)py, WHITE);
        } else if (stageId == STAGE_CONGRESSIONAL_FLOOR || stageId == STAGE_PRESS_BRIEFING || stageId == STAGE_WTC_PLAZA || stageId == STAGE_SMITHSONIAN_AERO || stageId == STAGE_ENN_NEWS_STUDIO) {

            float sway = sinf(time * 5.0f + p) * 3.0f;
            DrawRectangle((int)(px + sway), (int)py, 4, 3, Color{ 250, 245, 220, 195 });
        } else {

            float ey = GROUND_Y - py;
            DrawCircle((int)px, (int)ey, 1.5f, (p % 2 == 0) ? Color{ 255, 190, 50, 200 } : Color{ 255, 90, 30, 190 });
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
        float clapGap = clapWave * 4.5f;
        int lincX = (int)BgX(564.0f);
        int lincHandsY = (int)BgY(186.0f);
        int lincHeadY = (int)BgY(115.0f);

        Color marbleBase{ 242, 240, 234, 255 };
        Color marbleShadow{ 165, 162, 156, 255 };
        DrawCircle((int)(lincX - 2.0f - clapGap), lincHandsY, 3.2f, marbleShadow);
        DrawCircle((int)(lincX - 2.0f - clapGap), lincHandsY, 2.4f, marbleBase);
        DrawCircle((int)(lincX + 2.0f + clapGap), lincHandsY, 3.2f, marbleShadow);
        DrawCircle((int)(lincX + 2.0f + clapGap), lincHandsY, 2.4f, marbleBase);

        if (clapWave < 0.35f) {
            DrawCircleLines(lincX, lincHandsY, 6.0f + (0.35f - clapWave) * 22.0f, Color{ 255, 235, 110, 225 });
            DrawLine(lincX - 8, lincHandsY - 5, lincX - 13, lincHandsY - 9, Color{ 255, 245, 160, 235 });
            DrawLine(lincX + 8, lincHandsY - 5, lincX + 13, lincHandsY - 9, Color{ 255, 245, 160, 235 });
            const char* cheers[4] = { "CLAP! CLAP!", "BRAVO!", "FOUR SCORE!", "ENCORE!" };
            const char* ch = cheers[((int)(time * 2.0f)) % 4];
            int ctw = MeasureText(ch, 6);
            DrawRectangle(lincX - ctw / 2 - 3, lincHeadY, ctw + 6, 9, Color{ 15, 20, 35, 225 });
            DrawRectangleLines(lincX - ctw / 2 - 3, lincHeadY, ctw + 6, 9, Color{ 255, 220, 70, 240 });
            VisualsEngine::DrawPixelText(ch, lincX - ctw / 2, lincHeadY + 2, 6, Color{ 255, 235, 90, 255 });
        }
    } else if (stageId == STAGE_SMITHSONIAN_AERO) {

        int droneNoseX = (int)BgX(382.0f);
        int droneNoseY = (int)BgY(148.0f);
        int droneTailX = (int)BgX(198.0f);
        int droneTailY = (int)BgY(122.0f);
        float propSpan = sinf(time * 48.0f) * 8.0f;
        DrawLine(droneTailX, droneTailY - (int)propSpan, droneTailX, droneTailY + (int)propSpan, Color{ 220, 235, 255, 210 });
        if (fmodf(time * 2.6f, 1.0f) < 0.45f) {
            DrawCircle((int)BgX(255.0f), (int)BgY(138.0f), 2.2f, Color{ 255, 45, 45, 245 });
            DrawCircle((int)BgX(395.0f), (int)BgY(125.0f), 2.2f, Color{ 45, 255, 90, 245 });
        }
        float scanX = droneNoseX + 25.0f + sinf(time * 2.2f) * 40.0f;
        DrawLine(droneNoseX, droneNoseY, (int)scanX, (int)GROUND_Y - 8, Color{ 255, 45, 45, 70 });
        DrawCircle((int)scanX, (int)GROUND_Y - 8, 2.5f, Color{ 255, 60, 60, 175 });

        float p51Span = cosf(time * 44.0f) * 12.0f;
        DrawLine((int)BgX(492.0f), (int)BgY(192.0f) - (int)p51Span, (int)BgX(492.0f), (int)BgY(192.0f) + (int)p51Span, Color{ 225, 235, 250, 195 });

        int sallyX = (int)BgX(292.0f);
        int sallyY = (int)BgY(320.0f);
        int wrightX = (int)BgX(955.0f);
        int wrightY = (int)BgY(295.0f);
        if (fmodf(time, 3.6f) < 1.6f) {
            int stw = MeasureText("GO SALLY!", 5);
            DrawRectangle(sallyX - stw / 2 - 2, sallyY - 12, stw + 4, 8, Color{ 15, 35, 90, 225 });
            DrawRectangleLines(sallyX - stw / 2 - 2, sallyY - 12, stw + 4, 8, Color{ 100, 210, 255, 230 });
            VisualsEngine::DrawPixelText("GO SALLY!", sallyX - stw / 2, sallyY - 10, 5, Color{ 140, 235, 255, 255 });
        } else if (fmodf(time, 3.6f) > 1.8f && fmodf(time, 3.6f) < 3.4f) {
            int wtw = MeasureText("FIRST FLIGHT!", 5);
            DrawRectangle(wrightX - wtw / 2 - 2, wrightY - 12, wtw + 4, 8, Color{ 50, 35, 18, 225 });
            DrawRectangleLines(wrightX - wtw / 2 - 2, wrightY - 12, wtw + 4, 8, Color{ 255, 215, 90, 230 });
            VisualsEngine::DrawPixelText("FIRST FLIGHT!", wrightX - wtw / 2, wrightY - 10, 5, Color{ 255, 225, 120, 255 });
        }
    } else if (stageId == STAGE_TOMB_UNKNOWN) {

        float flameX = BgX(918.0f);
        float flameY = BgY(298.0f);
        float flicker1 = sinf(time * 17.0f) * 3.0f;
        float flicker2 = cosf(time * 23.0f + 1.1f) * 2.5f;
        DrawCircle((int)flameX, (int)flameY - 8, 22.0f + sinf(time * 9.0f) * 3.0f, Color{ 255, 145, 25, 55 });
        DrawTriangle(
            Vector2{ flameX + flicker1, flameY - 22.0f - fabsf(flicker2) },
            Vector2{ flameX - 9.0f, flameY + 2.0f },
            Vector2{ flameX + 9.0f, flameY + 2.0f },
            Color{ 255, 85, 15, 195 }
        );
        DrawTriangle(
            Vector2{ flameX - flicker2 * 0.6f, flameY - 16.0f - fabsf(flicker1) * 0.5f },
            Vector2{ flameX - 5.5f, flameY + 2.0f },
            Vector2{ flameX + 5.5f, flameY + 2.0f },
            Color{ 255, 205, 45, 220 }
        );
        DrawCircle((int)flameX, (int)flameY - 3, 4.0f, Color{ 255, 250, 215, 240 });
        for (int e = 0; e < 6; e++) {
            float eLife = fmodf(time * 1.8f + e * 0.17f, 1.0f);
            float ex = flameX + sinf(time * 8.0f + e * 2.0f) * 6.0f;
            float ey = flameY - 8.0f - eLife * 26.0f;
            DrawCircle((int)ex, (int)ey, 1.2f, (e % 2 == 0) ? Color{ 255, 235, 95, 235 } : Color{ 255, 120, 30, 215 });
        }

        int bayonetX = (int)BgX(398.0f);
        int bayonetY = (int)BgY(260.0f);
        if (fmodf(time * 2.2f, 1.0f) < 0.35f) {
            DrawCircle(bayonetX, bayonetY, 3.5f, WHITE);
            DrawLine(bayonetX - 6, bayonetY, bayonetX + 6, bayonetY, Color{ 220, 240, 255, 230 });
            DrawLine(bayonetX, bayonetY - 6, bayonetX, bayonetY + 6, Color{ 220, 240, 255, 230 });
        }
        int heelX = (int)BgX(442.0f);
        int heelY = (int)BgY(555.0f);
        float stepWave = fmodf(time * 1.5f, 1.0f);
        if (stepWave < 0.28f) {
            DrawCircleLines(heelX, heelY, 4.0f + stepWave * 22.0f, Color{ 255, 225, 120, 190 });
        }
    } else if (stageId == STAGE_ENN_NEWS_STUDIO) {

        if (fmodf(time * 2.4f, 1.0f) < 0.65f) {
            int cam1X = (int)BgX(118.0f);
            int cam1Y = (int)BgY(276.0f);
            int cam2X = (int)BgX(298.0f);
            int cam2Y = (int)BgY(320.0f);
            DrawCircle(cam1X, cam1Y, 4.0f, Color{ 255, 30, 30, 180 });
            DrawCircle(cam1X, cam1Y, 2.0f, WHITE);
            DrawCircle(cam2X, cam2Y, 3.5f, Color{ 255, 30, 30, 180 });
            DrawCircle(cam2X, cam2Y, 1.8f, WHITE);
        }

        int deskX = (int)BgX(565.0f);
        int deskY = (int)BgY(418.0f);
        float deskPulse = 0.5f + 0.5f * sinf(time * 6.0f);
        DrawCircle(deskX, deskY, 16.0f + deskPulse * 6.0f, Color{ 255, 40, 50, (unsigned char)(28 + deskPulse * 35) });

        int cycle = ((int)(time * 0.9f)) % 3;
        if (cycle == 0) {

            int hx = (int)BgX(268.0f);
            int hy = (int)BgY(218.0f);
            int tw = MeasureText("HANNITY: BREAKING!", 5);
            DrawRectangle(hx - tw / 2 - 2, hy, tw + 4, 8, Color{ 15, 25, 65, 230 });
            DrawRectangleLines(hx - tw / 2 - 2, hy, tw + 4, 8, Color{ 255, 70, 70, 240 });
            VisualsEngine::DrawPixelText("HANNITY: BREAKING!", hx - tw / 2, hy + 2, 5, Color{ 255, 225, 90, 255 });
        } else if (cycle == 1) {

            int cx = (int)BgX(352.0f);
            int cy = (int)BgY(216.0f);
            int tw = MeasureText("COOPER: ENN 360!", 5);
            DrawRectangle(cx - tw / 2 - 2, cy, tw + 4, 8, Color{ 15, 35, 75, 230 });
            DrawRectangleLines(cx - tw / 2 - 2, cy, tw + 4, 8, Color{ 90, 215, 255, 240 });
            VisualsEngine::DrawPixelText("COOPER: ENN 360!", cx - tw / 2, cy + 2, 5, Color{ 150, 235, 255, 255 });
        } else {

            int ix = (int)BgX(795.0f);
            int iy = (int)BgY(216.0f);
            int tw = MeasureText("INGRAHAM: THE ANGLE!", 5);
            DrawRectangle(ix - tw / 2 - 2, iy, tw + 4, 8, Color{ 65, 15, 35, 230 });
            DrawRectangleLines(ix - tw / 2 - 2, iy, tw + 4, 8, Color{ 255, 195, 60, 240 });
            VisualsEngine::DrawPixelText("INGRAHAM: THE ANGLE!", ix - tw / 2, iy + 2, 5, Color{ 255, 230, 110, 255 });
        }
    } else if (stageId == STAGE_CHICAGO_WRIGLEY) {

        for (int b = 0; b < 8; b++) {
            float bx = BgX(370.0f + b * 38.0f);
            float by = BgY(212.0f);
            bool lit = (((int)(time * 8.0f) + b) % 2) == 0;
            DrawCircle((int)bx, (int)by, lit ? 2.2f : 1.4f, lit ? Color{ 255, 240, 110, 250 } : Color{ 180, 60, 60, 190 });
        }

        for (int s = 0; s < 4; s++) {
            float phase = fmodf(time * 1.8f + s * 0.45f, 1.8f);
            float sx = BgX(88.0f + s * 9.0f) + sinf(time * 5.0f + s) * 2.5f;
            float sy = BgY(355.0f) - phase * 16.0f;
            unsigned char sa = (unsigned char)std::clamp((int)((1.8f - phase) * 110.0f), 0, 180);
            DrawCircle((int)sx, (int)sy, 2.5f + phase * 1.5f, Color{ 245, 250, 255, sa });
        }

        int cycle = ((int)(time * 0.7f)) % 4;
        if (cycle == 0) {

            int mx = (int)BgX(408.0f);
            int my = (int)BgY(302.0f);
            int tw = MeasureText("MICHELLE: YES WE CAN!", 5);
            DrawRectangle(mx - tw / 2 - 2, my, tw + 4, 9, Color{ 20, 45, 95, 235 });
            DrawRectangleLines(mx - tw / 2 - 2, my, tw + 4, 9, Color{ 255, 220, 70, 245 });
            VisualsEngine::DrawPixelText("MICHELLE: YES WE CAN!", mx - tw / 2, my + 1, 5, Color{ 255, 240, 110, 255 });
        } else if (cycle == 1) {

            int jx = (int)BgX(630.0f);
            int jy = (int)BgY(302.0f);
            int tw = MeasureText("JOHN CANDY: HEY CHICAGO!", 5);
            DrawRectangle(jx - tw / 2 - 2, jy, tw + 4, 9, Color{ 65, 35, 18, 235 });
            DrawRectangleLines(jx - tw / 2 - 2, jy, tw + 4, 9, Color{ 255, 210, 80, 245 });
            VisualsEngine::DrawPixelText("JOHN CANDY: HEY CHICAGO!", jx - tw / 2, jy + 1, 5, Color{ 255, 235, 120, 255 });
        } else if (cycle == 2) {

            int ax = (int)BgX(515.0f);
            int ay = (int)BgY(302.0f);
            int tw = MeasureText("AYKROYD: ON A MISSION!", 5);
            DrawRectangle(ax - tw / 2 - 2, ay, tw + 4, 9, Color{ 15, 20, 32, 235 });
            DrawRectangleLines(ax - tw / 2 - 2, ay, tw + 4, 9, Color{ 110, 220, 255, 245 });
            VisualsEngine::DrawPixelText("AYKROYD: ON A MISSION!", ax - tw / 2, ay + 1, 5, Color{ 165, 235, 255, 255 });
        } else {

            int dx = (int)BgX(750.0f);
            int dy = (int)BgY(302.0f);
            int tw = MeasureText("DITKA: DA BEARS!", 5);
            DrawRectangle(dx - tw / 2 - 2, dy, tw + 4, 9, Color{ 18, 30, 62, 235 });
            DrawRectangleLines(dx - tw / 2 - 2, dy, tw + 4, 9, Color{ 255, 135, 40, 245 });
            VisualsEngine::DrawPixelText("DITKA: DA BEARS!", dx - tw / 2, dy + 1, 5, Color{ 255, 195, 80, 255 });
        }
    }
}

inline void UpdateAndTrackFighters(Fighter& p1, Fighter& p2) {
    if (IsKeyPressed(KEY_F1)) {
        s_debugHitboxes = !s_debugHitboxes;
    }
    if (IsKeyPressed(KEY_F3)) {
        s_forceProceduralRig = !s_forceProceduralRig;
    }

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
        if ((st == FSTATE_SPECIAL || st == FSTATE_JUMP || (st == FSTATE_ATTACK && f.GetCurrentAttack().isHeavy)) && (f.GetStateFrame() % 3 == 0)) {
            CEAfterimage ghost{};
            ghost.pos = f.GetPosition();
            ghost.facingRight = f.IsFacingRight();
            ghost.state = st;
            ghost.frame = f.GetStateFrame();
            ghost.life = 9;
            s_afterimages[i].push_back(ghost);
        }
    }

    if (p1.PopSuperFlashTrigger()) {
        s_superFlashTimer = 24;
        s_superFlashPlayer = 0;
        s_superFlashMove = "TOTAL VETO FLURRY!";
        PushBanner(0, "SUPER COMBO!", p1.GetName().c_str(), Color{ 255, 215, 40, 255 });
    }
    if (p2.PopSuperFlashTrigger()) {
        s_superFlashTimer = 24;
        s_superFlashPlayer = 1;
        s_superFlashMove = "TOTAL VETO FLURRY!";
        PushBanner(1, "SUPER COMBO!", p2.GetName().c_str(), Color{ 255, 215, 40, 255 });
    }

    if (p1.PopCancelCallout()) PushBanner(0, "2-IN-1 CANCEL!", "SPECIAL BUFFER", Color{ 90, 230, 255, 255 });
    if (p2.PopCancelCallout()) PushBanner(1, "2-IN-1 CANCEL!", "SPECIAL BUFFER", Color{ 90, 230, 255, 255 });
    if (p1.PopReversalCallout()) {
        PushBanner(0, "REVERSAL!", "TOTAL VETO", Color{ 255, 110, 50, 255 });
        SpawnImpactJuice(p1.GetPosition(), true, true, "VETO!");
    }
    if (p2.PopReversalCallout()) {
        PushBanner(1, "REVERSAL!", "TOTAL VETO", Color{ 255, 110, 50, 255 });
        SpawnImpactJuice(p2.GetPosition(), true, true, "VETO!");
    }

    int c1 = p1.GetComboCount();
    int c2 = p2.GetComboCount();
    if (c1 > s_prevP1Hits && c1 >= 1) {
        if (!s_firstStrikeAwarded) {
            s_firstStrikeAwarded = true;
            PushBanner(0, "FIRST STRIKE!", p1.GetName().c_str(), Color{ 255, 230, 60, 255 });
        }
        if (c1 >= 2) {
            char buf[48];
            snprintf(buf, sizeof(buf), "%d HIT FILIBUSTER!", c1);
            PushBanner(0, buf, p1.GetCurrentAttack().name.c_str(), Color{ 255, 190, 40, 255 });
        }
        bool isSpec = (p1.GetState() == FSTATE_SPECIAL);
        SpawnImpactJuice(Vector2{ p2.GetPosition().x, p2.GetPosition().y - 36.0f }, p1.GetCurrentAttack().isHeavy, isSpec, isSpec ? "VETO!" : (p1.GetCurrentAttack().isHeavy ? "SMACK!" : nullptr));
    }
    if (c2 > s_prevP2Hits && c2 >= 1) {
        if (!s_firstStrikeAwarded) {
            s_firstStrikeAwarded = true;
            PushBanner(1, "FIRST STRIKE!", p2.GetName().c_str(), Color{ 255, 230, 60, 255 });
        }
        if (c2 >= 2) {
            char buf[48];
            snprintf(buf, sizeof(buf), "%d HIT FILIBUSTER!", c2);
            PushBanner(1, buf, p2.GetCurrentAttack().name.c_str(), Color{ 255, 120, 60, 255 });
        }
        bool isSpec = (p2.GetState() == FSTATE_SPECIAL);
        SpawnImpactJuice(Vector2{ p1.GetPosition().x, p1.GetPosition().y - 36.0f }, p2.GetCurrentAttack().isHeavy, isSpec, isSpec ? "REDACTED!" : (p2.GetCurrentAttack().isHeavy ? "OBJECTION!" : nullptr));
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
        st.pos.y -= 0.35f;
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
        int wbCenterX = (int)(f.GetPosition().x + dir * 39.0f);
        int wbW = 76;
        int wbX = wbCenterX - wbW / 2;
        int wbY = (int)(f.GetPosition().y - 80.0f);

        DrawRectangle(wbX - 1, wbY - 1, wbW + 2, 13, Color{ 220, 30, 40, 255 });
        DrawRectangle(wbX, wbY, wbW, 11, Color{ 16, 38, 105, 255 });
        VisualsEngine::DrawPixelText("DECISION 2028", wbX + 3, wbY + 1, 10, Color{ 255, 235, 80, 255 });

        int mapX = wbCenterX - 28;
        int mapY = wbY + 14;
        if (fr >= 2) {

            DrawRectangleLines(mapX + 2, mapY + 2, 12, 14, Color{ 35, 115, 255, 255 });
            DrawRectangleLines(mapX + 15, mapY + 2, 14, 11, Color{ 255, 45, 55, 255 });
        }
        if (fr >= 5) {

            DrawRectangleLines(mapX + 18, mapY + 14, 14, 10, Color{ 255, 45, 55, 255 });
            DrawLine(mapX + 12, mapY + 10, mapX + 46, mapY + 6, Color{ 255, 230, 60, 255 });
            DrawLine(mapX + 20, mapY + 18, mapX + 48, mapY + 20, Color{ 40, 220, 255, 255 });
        }
        if (fr >= 7) {

            int badgeX = wbCenterX + (f.IsFacingRight() ? 8 : -32);
            int badgeY = wbY + 25;
            DrawRectangle(badgeX - 2, badgeY - 1, 28, 12, Color{ 255, 250, 240, 245 });
            DrawRectangleLines(badgeX - 2, badgeY - 1, 28, 12, Color{ 225, 30, 40, 255 });
            VisualsEngine::DrawPixelText("270!", badgeX, badgeY, 10, Color{ 220, 20, 30, 255 });
        }

        int penX = mapX + 8 + ((fr * 7) % 40);
        int penY = mapY + 5 + ((fr * 5) % 18);
        DrawCircle(penX, penY, 2.5f, (fr % 2 == 0) ? Color{ 255, 45, 55, 255 } : Color{ 45, 130, 255, 255 });
    }

    if ((f.GetState() == FSTATE_ATTACK || f.GetState() == FSTATE_SPECIAL) && f.GetCurrentAttack().boxW > 0.0f) {
        int su = f.GetCurrentAttack().startupFrames;
        int ac = f.GetCurrentAttack().activeFrames;
        int fr = f.GetStateFrame();
        if (fr >= su && fr < su + ac) {
            float dir = f.IsFacingRight() ? 1.0f : -1.0f;
            Vector2 pStart{ f.GetPosition().x + dir * 10.0f, f.GetPosition().y - 36.0f };
            Vector2 pEnd{ f.GetPosition().x + dir * (f.GetCurrentAttack().reachX + 10.0f), f.GetPosition().y + f.GetCurrentAttack().reachY };
            Color smearOuter = (f.GetState() == FSTATE_SPECIAL) ? Color{ 255, 70, 40, 230 } : (f.GetCurrentAttack().isHeavy ? Color{ 255, 195, 40, 230 } : Color{ 120, 230, 255, 210 });
            DrawLineEx(Vector2{ pStart.x, pStart.y - 6.0f }, Vector2{ pEnd.x, pEnd.y - 2.0f }, 5.5f, smearOuter);
            DrawLineEx(pStart, pEnd, 3.0f, WHITE);
            DrawCircleLines((int)pEnd.x, (int)pEnd.y, f.GetCurrentAttack().isHeavy ? 14.0f : 9.0f, smearOuter);
        }
    }

    if (f.GetState() == FSTATE_HITSTUN && f.GetHealth() > 0) {
        float t = (float)GetTime() * 10.5f;
        int numStars = 5;
        float orbitRx = 18.0f;
        float orbitRy = 6.0f;
        DrawEllipseLines((int)f.GetPosition().x, (int)(f.GetPosition().y - 72.0f), orbitRx, orbitRy, Color{ 255, 230, 80, 140 });
        for (int s = 0; s < numStars; s++) {
            float ang = t + s * (2.0f * PI_F / (float)numStars);
            float sx = f.GetPosition().x + cosf(ang) * orbitRx;
            float sy = f.GetPosition().y - 72.0f + sinf(ang) * orbitRy;
            Color starCol = (s % 2 == 0) ? Color{ 255, 235, 40, 255 } : Color{ 255, 185, 30, 255 };
            DrawLineEx(Vector2{ sx - 4.0f, sy }, Vector2{ sx + 4.0f, sy }, 2.0f, starCol);
            DrawLineEx(Vector2{ sx, sy - 4.0f }, Vector2{ sx, sy + 4.0f }, 2.0f, starCol);
            DrawLine(sx - 2.5f, sy - 2.5f, sx + 2.5f, sy + 2.5f, WHITE);
            DrawLine(sx - 2.5f, sy + 2.5f, sx + 2.5f, sy - 2.5f, WHITE);
            DrawCircle((int)sx, (int)sy, 2.2f, WHITE);
        }
    }
}

inline void DrawWorldParticlesAndDebug(const Fighter& p1, const Fighter& p2) {
    for (const auto& p : s_particles) {
        if (!p.alive) continue;
        if (p.isPaper) {
            DrawRectanglePro(Rectangle{ p.pos.x, p.pos.y, p.size, p.size * 0.65f }, Vector2{ p.size * 0.5f, p.size * 0.32f }, p.rot, p.color);
        } else {
            DrawCircleV(p.pos, p.size, p.color);
        }
    }

    for (const auto& st : s_stamps) {
        if (!st.alive) continue;
        int tw = MeasureText(st.text.c_str(), 10);
        int bx = (int)st.pos.x - tw / 2 - 4;
        int by = (int)st.pos.y - 6;
        DrawRectangle(bx, by, tw + 8, 13, Color{ 15, 10, 15, 230 });
        DrawRectangleLines(bx, by, tw + 8, 13, st.color);
        VisualsEngine::DrawPixelText(st.text.c_str(), bx + 4, by + 2, 10, st.color);
    }

    if (s_debugHitboxes) {
        const Fighter* fighters[2] = { &p1, &p2 };
        for (int i = 0; i < 2; i++) {
            const Fighter& f = *fighters[i];
            Rectangle pb = f.GetPushbox();
            DrawRectangleRec(pb, Color{ 255, 235, 40, 50 });
            DrawRectangleLinesEx(pb, 1.0f, Color{ 255, 235, 40, 230 });

            auto hbs = f.GetHurtboxes();
            for (const auto& hb : hbs) {
                DrawRectangleRec(hb, Color{ 40, 240, 80, 55 });
                DrawRectangleLinesEx(hb, 1.0f, Color{ 40, 240, 80, 230 });
            }

            Rectangle atkBox{};
            AttackData atkData{};
            if (f.GetActiveHitbox(atkBox, atkData)) {
                DrawRectangleRec(atkBox, Color{ 255, 40, 40, 95 });
                DrawRectangleLinesEx(atkBox, 1.0f, Color{ 255, 40, 40, 255 });
            }
        }
    }
}

inline void DrawScreenHUDAndSuperFlash(const Fighter& p1, const Fighter& p2, const InputManager& in1) {

    int ceTw = MeasureText("CHAMPIONSHIP EDITION", 6);
    VisualsEngine::DrawPixelText("CHAMPIONSHIP EDITION", (SCREEN_WIDTH - ceTw) / 2, 35, 6, Color{ 255, 215, 60, 220 });

    for (const auto& b : s_banners) {
        float slide = std::min(1.0f, (float)(b.maxTimer - b.timer) / 6.0f);
        int bw = 122;
        int bh = 22;
        int bx = (b.playerIndex == 0) ? (int)(-bw + slide * (bw + 8)) : (int)(SCREEN_WIDTH - slide * (bw + 8));
        int by = 48 + b.playerIndex * 26;
        DrawRectangle(bx, by, bw, bh, Color{ 14, 16, 28, 235 });
        DrawRectangleLines(bx, by, bw, bh, b.color);
        VisualsEngine::DrawPixelText(b.text.c_str(), bx + 5, by + 3, 8, b.color);
        VisualsEngine::DrawPixelText(b.subtext.c_str(), bx + 5, by + 12, 6, WHITE);
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

    if (s_debugHitboxes) {
        DrawRectangle(6, SCREEN_HEIGHT - 42, SCREEN_WIDTH - 12, 36, Color{ 8, 10, 16, 225 });
        DrawRectangleLines(6, SCREEN_HEIGHT - 42, SCREEN_WIDTH - 12, 36, Color{ 80, 240, 120, 220 });
        char p1Info[128];
        snprintf(p1Info, sizeof(p1Info), "P1 F:%d ADV:%+d ST:%d %s",
                 p1.GetStateFrame(), p1.GetLastFrameAdvantage(), (int)p1.GetState(), p1.GetCurrentAttack().name.c_str());
        char p2Info[128];
        snprintf(p2Info, sizeof(p2Info), "P2 F:%d ADV:%+d ST:%d %s",
                 p2.GetStateFrame(), p2.GetLastFrameAdvantage(), (int)p2.GetState(), p2.GetCurrentAttack().name.c_str());
        VisualsEngine::DrawPixelText(p1Info, 10, SCREEN_HEIGHT - 39, 6, Color{ 110, 255, 140, 255 });
        VisualsEngine::DrawPixelText(p2Info, 10, SCREEN_HEIGHT - 29, 6, Color{ 255, 190, 100, 255 });

        std::string hist = "BUF: ";
        for (int a = 11; a >= 0; a--) {
            const auto& hf = in1.GetHistoryFrame(a);
            char c = '0' + (int)hf.relDir;
            hist.push_back(c);
            if (hf.buttonsPressed & BTN_LP) hist += "P";
            else if (hf.buttonsPressed & BTN_HP) hist += "H";
            else if (hf.buttonsPressed & BTN_LK) hist += "K";
            else if (hf.buttonsPressed & BTN_HK) hist += "S";
            hist.push_back(' ');
        }
        VisualsEngine::DrawPixelText(hist.c_str(), 10, SCREEN_HEIGHT - 18, 6, Color{ 140, 220, 255, 255 });
    }
}

}
