#pragma once
#include "Common.h"
#include "Fighter.h"
#include "Collision.h"
#include "Stages.h"
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
