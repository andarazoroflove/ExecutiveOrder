#include "Common.h"
#include "Shaders.h"
#include "AudioEngine.h"
#include "InputManager.h"
#include "Fighter.h"
#include "Collision.h"
#include "Stages.h"
#include "Visuals.h"
#include "AIController.h"
#include "AssetManager.h"
#include "rlgl.h"
#include <memory>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <ctime>

static volatile sig_atomic_t s_quitRequested = 0;

static void CleanExitSignalHandler(int sig) {
    (void)sig;
    s_quitRequested = 1;
}

static void CrashSignalHandler(int sig) {
    const char* sigName = "UNKNOWN";
    if (sig == SIGSEGV) sigName = "SIGSEGV (Segmentation Fault)";
    else if (sig == SIGABRT) sigName = "SIGABRT (Aborted)";
    else if (sig == SIGFPE) sigName = "SIGFPE (Floating Point Exception)";
    else if (sig == SIGILL) sigName = "SIGILL (Illegal Instruction)";

    fprintf(stderr, "\n==================================================\n");
    fprintf(stderr, "FATAL CRASH DETECTED: Signal %d - %s\n", sig, sigName);
    fprintf(stderr, "==================================================\n");
    fflush(stderr);

    FILE* f = fopen("crash_log.txt", "w");
    if (f) {
        fprintf(f, "FATAL CRASH DETECTED: Signal %d - %s\n", sig, sigName);
        fflush(f);
        fclose(f);
    }
    _Exit(sig);
}

static int MoveSecretCharCursor(int cur, int dx, int dy) {
    if (dx != 0) {
        return (cur + dx + CHARACTER_COUNT) % CHARACTER_COUNT;
    }
    if (dy != 0) {
        int row = (cur < 10) ? 0 : ((cur < 20) ? 1 : 2);
        int col = (cur < 10) ? cur : ((cur < 20) ? (cur - 10) : (cur - 20));
        int nextRow = (row + dy + 3) % 3;
        int maxCol = (nextRow < 2) ? 9 : 8;
        if (col > maxCol) col = maxCol;
        if (nextRow == 0) return col;
        if (nextRow == 1) return 10 + col;
        return 20 + col;
    }
    return cur;
}

int main(int argc, char* argv[]) {
    std::srand((unsigned int)std::time(nullptr));
    std::signal(SIGTERM, CleanExitSignalHandler);
    std::signal(SIGINT, CleanExitSignalHandler);
    std::signal(SIGSEGV, CrashSignalHandler);
    std::signal(SIGABRT, CrashSignalHandler);
    std::signal(SIGFPE, CrashSignalHandler);
    std::signal(SIGILL, CrashSignalHandler);

    bool windowedMode = false;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--windowed") == 0) {
            windowedMode = true;
        }
    }

    ChangeDirectory(GetApplicationDirectory());

#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
    SetConfigFlags(FLAG_VSYNC_HINT);
#else
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
#endif
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Executive Veto II Turbo: Global Conflict");
    if (!IsWindowReady()) {
        TraceLog(LOG_FATAL, "ExecutiveVetoTurbo: Failed to initialize window / graphics context.");
        return 1;
    }
    SetExitKey(KEY_NULL);
    SetTargetFPS(TARGET_FPS);
#if !defined(GRAPHICS_API_OPENGL_ES2) && !defined(__aarch64__)
    SetWindowMinSize(1280, 720);
#endif

    static const char* s_gamepadDb =
        "19000000010000000100000001000000,ANBERNIC-keys,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,guide:b10,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,leftstick:b11,rightstick:b12,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,platform:Linux,\n"
        "19000000010000000001000000010000,ANBERNIC-keys,a:b0,b:b1,x:b3,y:b2,back:b8,guide:b6,start:b7,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpleft:h0.8,dpdown:h0.4,dpright:h0.2,leftx:a0,lefty:a1,lefttrigger:b9,righttrigger:b10,platform:Linux,\n"
        "19000000010000000100000001010000,Anbernic RG353V,platform:Linux,a:b1,b:b0,x:b2,y:b3,back:b8,start:b9,guide:b10,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,leftstick:b11,rightstick:b12,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
        "19000000010000000100000002010000,retrogame_joypad,platform:Linux,a:b1,b:b0,x:b2,y:b3,back:b8,start:b9,guide:b10,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,leftstick:b11,rightstick:b12,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
        "19000000010000000200000002000000,retrogame_joypad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,dpup:b10,dpdown:b11,dpleft:b12,dpright:b13,leftx:a0,lefty:a1,\n"
        "19000000010000000100000000010000,retrogame_joypad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,\n"
        "03000000091200000031000000010000,OpenSimHardware OSH PB Controller,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,\n"
        "19000000010000000100000000000000,GO-Advance Gamepad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,\n"
        "19000000010000000100000002000000,GO-Advance Gamepad (rev 1.1),platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,dpup:b10,dpdown:b11,dpleft:b12,dpright:b13,leftx:a0,lefty:a1,\n"
        "030000005e0400008e02000010010000,Microsoft X-Box 360 pad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b6,start:b7,guide:b8,leftshoulder:b4,rightshoulder:b5,leftstick:b9,rightstick:b10,leftx:a0,lefty:a1,rightx:a3,righty:a4,lefttrigger:a2,righttrigger:a5,dpup:h0.1,dpright:h0.2,dpdown:h0.4,dpleft:h0.8,\n";
    SetGamepadMappings(s_gamepadDb);
    if (FileExists("gamecontrollerdb.txt")) {
        char* fileMappings = LoadFileText("gamecontrollerdb.txt");
        if (fileMappings) {
            SetGamepadMappings(fileMappings);
            UnloadFileText(fileMappings);
        }
    }

#if defined(GRAPHICS_API_OPENGL_ES2) || defined(__aarch64__)
    auto ToggleAppFullscreen = [&]() {
        ToggleBorderlessWindowed();
    };
    if (!windowedMode) {
        ToggleAppFullscreen();
    }
#else
    int initMon = GetCurrentMonitor();
    int initMonW = GetMonitorWidth(initMon);
    int initMonH = GetMonitorHeight(initMon);
    Vector2 initMonPos = GetMonitorPosition(initMon);
    if (initMonW <= 0) initMonW = 1920;
    if (initMonH <= 0) initMonH = 1080;

    int savedWinH = std::clamp((int)(initMonH * 0.75f), 720, std::max(720, initMonH - 96));
    int savedWinW = std::clamp((savedWinH * 16) / 9, 1280, std::max(1280, initMonW - 96));
    int savedWinX = (int)initMonPos.x + std::max(0, (initMonW - savedWinW) / 2);
    int savedWinY = (int)initMonPos.y + std::max(24, (initMonH - savedWinH) / 2);

    SetWindowSize(savedWinW, savedWinH);
    SetWindowPosition(savedWinX, savedWinY);

    bool isDesktopFullscreen = false;
    bool mainLoopStarted = false;
    auto SetDesktopFullscreen = [&](bool wantFull) {
        if (wantFull && !isDesktopFullscreen) {
            if (IsWindowMaximized()) {
                RestoreWindow();
            } else if (mainLoopStarted) {
                Vector2 wp = GetWindowPosition();
                int cw = GetRenderWidth();
                int ch = GetRenderHeight();
                if (cw >= 1280 && ch >= 720) {
                    savedWinW = cw;
                    savedWinH = ch;
                    savedWinX = (int)wp.x;
                    savedWinY = (int)wp.y;
                }
            }
            int mon = GetCurrentMonitor();
            int mw = GetMonitorWidth(mon);
            int mh = GetMonitorHeight(mon);
            Vector2 mp = GetMonitorPosition(mon);
            if (mw <= 0) mw = initMonW;
            if (mh <= 0) mh = initMonH;

            SetWindowMinSize(0, 0);
            SetWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
            SetWindowSize(mw, mh);
            SetWindowPosition((int)mp.x, (int)mp.y);
            SetWindowFocused();
            HideCursor();
            isDesktopFullscreen = true;
        } else if (!wantFull && isDesktopFullscreen) {
            ShowCursor();
            ClearWindowState(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
            SetWindowMinSize(1280, 720);
            SetWindowSize(std::max(1280, savedWinW), std::max(720, savedWinH));
            SetWindowPosition(savedWinX, savedWinY);
            SetWindowFocused();
            isDesktopFullscreen = false;
        }
    };
    auto ToggleAppFullscreen = [&]() {
        SetDesktopFullscreen(!isDesktopFullscreen);
    };
    if (!windowedMode) {
        SetDesktopFullscreen(true);
    }
#endif

    AssetManager::Init();
    const char* iconPath = AssetManager::FileExists("assets/app_icon.png") ? "assets/app_icon.png" : "../assets/app_icon.png";
    if (AssetManager::FileExists(iconPath)) {
        Image appIcon = AssetManager::LoadImage(iconPath);
        if (appIcon.data != nullptr) {
            SetWindowIcon(appIcon);
            UnloadImage(appIcon);
        }
    }

    RenderTexture2D screenTarget = LoadRenderTexture(INTERNAL_WIDTH, INTERNAL_HEIGHT);
    SetTextureFilter(screenTarget.texture, TEXTURE_FILTER_BILINEAR);

    Shader crtShader = LoadShaderFromMemory(nullptr, CRT_FRAGMENT_SHADER);
    int scanlineLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "scanlineIntensity") : -1;
    int curvatureLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "curvature") : -1;
    int bloomLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "bloom") : -1;

    float scanlineVal = 0.28f;
    float curvatureVal = 0.03f;
    float bloomVal = 0.22f;
    if (crtShader.id > 0) {
        if (scanlineLoc >= 0) SetShaderValue(crtShader, scanlineLoc, &scanlineVal, SHADER_UNIFORM_FLOAT);
        if (curvatureLoc >= 0) SetShaderValue(crtShader, curvatureLoc, &curvatureVal, SHADER_UNIFORM_FLOAT);
        if (bloomLoc >= 0) SetShaderValue(crtShader, bloomLoc, &bloomVal, SHADER_UNIFORM_FLOAT);
    }

    Shader flagShader = LoadShaderFromMemory(nullptr, FLAG_FRAGMENT_SHADER);
    int flagTimeLoc = (flagShader.id > 0) ? GetShaderLocation(flagShader, "time") : -1;

    Texture2D flagTexture = VisualsEngine::CreateProceduralFlagTexture();

    VisualsEngine::Init();
    AudioEngine::Instance().Init();
    AudioEngine::Instance().SetBgmTrack(BGM_MENU_SPLASH);

    InputManager inputP1;
    InputManager inputP2;
    StageManager stageMgr;
    stageMgr.Init();
    AIController cpuAI;

    CharacterId p1CharId = CHAR_COMMANDER;
    CharacterId p2CharId = CHAR_TECH_VISIONARY;
    StageId currentStage = STAGE_REFLECTING_POOL;

    bool defeatedFighters[CHARACTER_COUNT] = { false };
    int matchesWon = 0;
    int continuesRemaining = 2;
    bool bonusStagesPlayed[BONUS_STAGE_COUNT] = { false };
    int easterEggBonusCursor = 0;
    bool playerLocked = false;
    bool choosingOpponent = false;
    bool opponentLocked = false;
    int worldActIndex = 0;

    static const StageId homeStages[CHARACTER_COUNT] = {
        STAGE_WHITE_HOUSE_MMA,
        STAGE_CONGRESSIONAL_FLOOR,
        STAGE_DEMOLISHED_EAST_WING,
        STAGE_REFLECTING_POOL,
        STAGE_SMITHSONIAN_AERO,
        STAGE_LINCOLN_MEMORIAL,
        STAGE_PROGRESS_AVENUE,
        STAGE_THE_HOOD,
        STAGE_TOMB_UNKNOWN,
        STAGE_DEMOLISHED_EAST_WING,
        STAGE_PRESS_BRIEFING,
        STAGE_PENTAGON_ALTAR,
        STAGE_WTC_PLAZA,
        STAGE_COMET_PING_PONG,
        STAGE_FLORIDA_KEYS,
        STAGE_ENN_NEWS_STUDIO,
        STAGE_CHICAGO_WRIGLEY,
        STAGE_CHICAGO_WRIGLEY,
        STAGE_CONGRESSIONAL_FLOOR,
        STAGE_WHITE_HOUSE_MMA,
        STAGE_SMITHSONIAN_AERO,
        STAGE_RYUGYONG_HOTEL,
        STAGE_WESTERN_WALL,
        STAGE_SOVIET_FOUNDRY,
        STAGE_PALACE_COURTYARD,
        STAGE_LINCOLN_BEDROOM,
        STAGE_PERFORMING_ARTS,
        STAGE_WYOMING_RODEO,
        STAGE_TEHRAN_CASBAH
    };

    auto CountDefeatedOriginals = [&]() -> int {
        int count = 0;
        for (int s = 0; s < PLAYABLE_CHARACTER_COUNT; s++) {
            int cid = PLAYABLE_ROSTER[s];
            if (cid != (int)p1CharId && defeatedFighters[cid]) {
                count++;
            }
        }
        return count;
    };

    auto GetStageForOpponent = [&](CharacterId opp) -> StageId {
        if ((int)opp >= 0 && (int)opp < CHARACTER_COUNT) {
            return homeStages[(int)opp];
        }
        return STAGE_DEMOLISHED_EAST_WING;
    };

    auto GetCurrentWorldAct = [&]() -> int {
        if (!defeatedFighters[CHAR_KIM]) return 0;
        if (!defeatedFighters[CHAR_BEBE]) return 1;
        if (!defeatedFighters[CHAR_PALE_MOTH]) return 2;
        if (!defeatedFighters[CHAR_THE_DICTATOR]) return 3;
        if (!defeatedFighters[CHAR_AYATOLLAH]) return 4;
        return 5;
    };

    auto PickNextAvailableOpponent = [&](int startSlot) -> CharacterId {
        int origDefeated = CountDefeatedOriginals();
        if (defeatedFighters[CHAR_PALM_BEACH_PETE]) {
            int act = GetCurrentWorldAct();
            if (act == 0) return CHAR_KIM;
            if (act == 1) return CHAR_BEBE;
            if (act == 2) return CHAR_PALE_MOTH;
            if (act == 3) return CHAR_THE_DICTATOR;
            return CHAR_AYATOLLAH;
        }
        if (defeatedFighters[CHAR_WAR_PIG]) {
            if (!defeatedFighters[CHAR_SOCIALIST]) return CHAR_SOCIALIST;
            if (!defeatedFighters[CHAR_DICK]) return CHAR_DICK;
            if (!defeatedFighters[CHAR_BILLARY]) return CHAR_BILLARY;
            if (!defeatedFighters[CHAR_A_NEW_HOPE]) return CHAR_A_NEW_HOPE;
            return CHAR_PALM_BEACH_PETE;
        }
        if (origDefeated >= (PLAYABLE_CHARACTER_COUNT - 1)) {
            if (!defeatedFighters[CHAR_SUCKRATERY_RKF]) return CHAR_SUCKRATERY_RKF;
            if (!defeatedFighters[CHAR_THE_PUNDIT]) return CHAR_THE_PUNDIT;
            return CHAR_WAR_PIG;
        }
        for (int step = 0; step < PLAYABLE_CHARACTER_COUNT; step++) {
            int slot = (startSlot + step + PLAYABLE_CHARACTER_COUNT) % PLAYABLE_CHARACTER_COUNT;
            int cid = PLAYABLE_ROSTER[slot];
            if (cid != (int)p1CharId && !defeatedFighters[cid]) {
                return (CharacterId)cid;
            }
        }
        return CHAR_COASTAL_GOV;
    };

    auto IsHardBossOpponent = [](CharacterId opp) -> bool {
        return opp == CHAR_WAR_PIG || opp == CHAR_PALM_BEACH_PETE ||
               opp == CHAR_KIM || opp == CHAR_BEBE ||
               opp == CHAR_PALE_MOTH || opp == CHAR_THE_DICTATOR ||
               opp == CHAR_AYATOLLAH;
    };

    BonusStageState bonusState{};
    bool bonusFreeFacingRight = true;
    InputManager bonusMoveInput;

    p2CharId = PickNextAvailableOpponent(1);
    currentStage = GetStageForOpponent(p2CharId);
    stageMgr.SetStage(currentStage);

    std::unique_ptr<Fighter> p1 = std::make_unique<Fighter>(0, p1CharId, 320.0f);
    std::unique_ptr<Fighter> p2 = std::make_unique<Fighter>(1, p2CharId, 480.0f);
    VisualsEngine::PrefetchMatchFighters((int)p1CharId, (int)p2CharId);

    std::vector<SparkEffect> sparks;

    GameState gameState = STATE_SPLASH;
    float stateTimer = 0.0f;
    bool logoSlammed = false;
    bool enableCrt = false;

    int roundNumber = 1;
    int roundSeconds = 99;
    float roundTickAccum = 0.0f;
    float simAccum = 0.0f;
    int fightIntroTimer = 100;
    bool fightActive = false;
    bool roundOver = false;
    float matchEndTimer = 0.0f;
    bool p1WasKo = false;
    bool p2WasKo = false;
    bool bonusFromEasterEgg = false;
    bool isGamePaused = false;

    bool finishHimTriggeredThisRound = false;
    int finishHimTimer = 0;
    int finishHimTargetPlayer = -1;
    float youWinVoiceTimer = -1.0f;
    float perfectVoiceTimer = -1.0f;

    bool isSecretMode = false;
    int secretStep = 0;
    int secretP1 = 0;
    int secretP2 = 1;
    int secretStage = 0;
    int secretDifficulty = (int)AI_DIFF_NORMAL;

    float cameraX = 400.0f;
    float cameraShake = 0.0f;

    auto ResetFightRoundState = [&](bool resetRoundOne) {
        if (resetRoundOne) {
            roundNumber = 1;
        }
        roundSeconds = 99;
        roundTickAccum = 0.0f;
        simAccum = 0.0f;
        fightIntroTimer = 110;
        fightActive = false;
        roundOver = false;
        matchEndTimer = 0.0f;
        finishHimTriggeredThisRound = false;
        finishHimTimer = 0;
        finishHimTargetPlayer = -1;
        youWinVoiceTimer = -1.0f;
        perfectVoiceTimer = -1.0f;
        ChampionshipEdition::ResetRoundFX();
        AudioEngine::SetCriticalTempo(false);
    };

    auto AddBonusPopup = [&](const char* txt, float x, float y, Color col) {
        BonusPopup pop{};
        pop.alive = true;
        snprintf(pop.text, sizeof(pop.text), "%s", txt);
        pop.pos = Vector2{ x, y };
        pop.life = 0.0f;
        pop.maxLife = 0.95f;
        pop.col = col;
        bonusState.popups.push_back(pop);
    };

    auto SpawnBonusDebris = [&](int kind, float x, float y, float vx, float vy, Color col = WHITE, float maxLife = 1.4f, int targetPile = -1) {
        BonusDebris d{};
        d.alive = true;
        d.kind = kind;
        d.pos = Vector2{ x, y };
        d.vel = Vector2{ vx, vy };
        d.angle = (float)(rand() % 360);
        d.angVel = (float)((rand() % 380) - 190);
        d.life = 0.0f;
        d.maxLife = maxLife;
        d.col = col;
        d.targetPile = targetPile;
        bonusState.debris.push_back(d);
    };

    auto StartBonusStage = [&](BonusStageType bType, bool fromEasterEgg) {
        bonusFromEasterEgg = fromEasterEgg;
        bonusState = BonusStageState{};
        bonusState.type = bType;
        bonusState.timer = 30.0f;
        bonusState.maxTimer = 30.0f;
        bonusFreeFacingRight = true;
        bonusMoveInput.Reset();
        sparks.clear();
        gameState = STATE_BONUS_CAR;
        stateTimer = 0.0f;

        float startX = 92.0f;
        int bgmTrack = BGM_BONUS_CAR;

        if (bType == BONUS_STAGE_TESLA) {
            bonusState.maxHp = 1350.0f;
            bonusState.hp = bonusState.maxHp;
            startX = 96.0f;
            bgmTrack = BGM_BONUS_CAR;
        } else if (bType == BONUS_STAGE_POOL_TAR) {
            bonusState.cracksGoal = 18;
            bonusState.cracksSealed = 0;
            bonusState.crackSpawnTimer = 1.38f;

            const float initXs[3] = { 142.0f, 228.0f, 316.0f };
            for (int i = 0; i < 3; i++) {
                PoolCrackSlot c{};
                c.active = true;
                c.state = 0;
                c.x = initXs[i];
                c.y = GROUND_Y + 3.0f + (i % 2) * 5.0f;
                bonusState.cracks.push_back(c);
            }
            startX = 72.0f;
            bgmTrack = BGM_BONUS_POOL;
        } else if (bType == BONUS_STAGE_BOOK_FIRE) {
            const float pileXs[4] = { 66.0f, 150.0f, 234.0f, 318.0f };
            for (int i = 0; i < 4; i++) {
                bonusState.bookPiles[i].x = pileXs[i];
                bonusState.bookPiles[i].maxHp = 210.0f;
                bonusState.bookPiles[i].hp = 210.0f;
                bonusState.bookPiles[i].hitFlash = 0.0f;
            }
            bonusState.villainThrowTimer[0] = 0.45f;
            bonusState.villainThrowTimer[1] = 1.15f;
            bonusState.villainThrowTimer[2] = 1.85f;
            startX = 36.0f;
            bgmTrack = BGM_BONUS_BOOKS;
        } else if (bType == BONUS_STAGE_BERLIN_WALL) {
            bonusState.maxHp = 1620.0f;
            bonusState.hp = bonusState.maxHp;
            for (int i = 0; i < 9; i++) bonusState.blockHp[i] = 180.0f;
            startX = 114.0f;
            bgmTrack = BGM_BONUS_BERLIN;
        }

        if (!p1 || p1->GetCharacterId() != p1CharId) {
            p1 = std::make_unique<Fighter>(0, p1CharId, startX);
        } else {
            p1->ResetRound(startX, true);
        }
        p1->SetActiveTaunt("", 0);
        VisualsEngine::PrefetchMatchFighters((int)p1CharId, (int)p1CharId);

        AudioEngine::Instance().SetBgmTrack(bgmTrack);
        AudioEngine::Instance().PlaySfx(SFX_TITLE_SLAM);
        AudioEngine::Instance().PlaySfx(SFX_VOICE_FIGHT);
    };

    auto StepBonusStage = [&](float dt) -> bool {
        bonusState.elapsed += dt;
        if (bonusState.targetShake > 0.0f) {
            bonusState.targetShake = std::max(0.0f, bonusState.targetShake - dt * 3.5f);
        }

        const RawInputState& raw = inputP1.GetCurrentState();
        if (raw.right && !raw.left) bonusFreeFacingRight = true;
        else if (raw.left && !raw.right) bonusFreeFacingRight = false;

        bool anyAttackPressed =
            inputP1.IsButtonPressed(BTN_LP) || inputP1.IsButtonPressed(BTN_HP) ||
            inputP1.IsButtonPressed(BTN_LK) || inputP1.IsButtonPressed(BTN_HK) ||
            inputP1.IsButtonPressed(BTN_SPECIAL);
        bool anyAttackHeld =
            raw.lp || raw.hp || raw.lk || raw.hk || raw.special;

        bool activePlay = (!bonusState.cleared && bonusState.timer > 0.0f);

        if (activePlay) {
            bonusState.timer = std::max(0.0f, bonusState.timer - dt);

            if (bonusState.type == BONUS_STAGE_TESLA) {
                if (bonusState.gunCooldown > 0.0f) {
                    bonusState.gunCooldown = std::max(0.0f, bonusState.gunCooldown - dt);
                }
                Rectangle carHurtbox = { (float)SCREEN_WIDTH * 0.5f - 88.0f, GROUND_Y - 72.0f, 176.0f, 72.0f };
                Fighter carDummy(1, CHAR_TECH_VISIONARY, SCREEN_WIDTH * 0.5f);
                if (inputP1.IsButtonPressed(BTN_SPECIAL) &&
                    (p1->GetCharacterId() == CHAR_SLEEPY_JOE || p1->GetCharacterId() == CHAR_COUCHIE || p1->GetCharacterId() == CHAR_THE_DICTATOR)) {
                    RawInputState smashSim = raw;
                    smashSim.special = false;
                    smashSim.hp = true;
                    inputP1.UpdateSimulated(smashSim, p1->IsFacingRight());
                }
                p1->UpdatePhysics(inputP1, carDummy);
                p1->SetPositionX(std::clamp(p1->GetPosition().x, 26.0f, (float)SCREEN_WIDTH - 26.0f));

                auto applyCarDamage = [&](float dmg, float hitX, float hitY, bool heavy) {
                    bonusState.hp = std::max(0.0f, bonusState.hp - dmg);
                    bonusState.targetShake = 1.0f;
                    cameraShake = heavy ? 5.0f : 2.5f;

                    SparkEffect sp{};
                    sp.pos = Vector2{ hitX, hitY };
                    sp.isBlock = false;
                    sp.alive = true;
                    sp.frame = 0;
                    sp.maxFrames = 8;
                    sparks.push_back(sp);

                    for (int g = 0; g < 4; g++) {
                        SpawnBonusDebris(0, hitX, hitY, (float)((rand() % 180) - 90), -(float)(70 + (rand() % 110)));
                    }

                    AudioEngine::Instance().PlaySfx(heavy ? SFX_HIT_HEAVY : SFX_HIT_LIGHT);
                    AudioEngine::Instance().PlaySfx(SFX_METAL_CRUNCH, (float)(88 + (rand() % 24)) / 100.0f);

                    float hpRatio = bonusState.hp / bonusState.maxHp;
                    int curTier = 0;
                    if (hpRatio <= 0.01f)      curTier = 7;
                    else if (hpRatio <= 0.16f) curTier = 6;
                    else if (hpRatio <= 0.31f) curTier = 5;
                    else if (hpRatio <= 0.46f) curTier = 4;
                    else if (hpRatio <= 0.60f) curTier = 3;
                    else if (hpRatio <= 0.74f) curTier = 2;
                    else if (hpRatio <= 0.88f) curTier = 1;

                    if (curTier > bonusState.lastDamageTier) {
                        bonusState.lastDamageTier = curTier;
                        AudioEngine::Instance().PlaySfx(SFX_GLASS_BREAK);
                        if (!bonusState.carAlarmPlayed) {
                            bonusState.carAlarmPlayed = true;
                            AudioEngine::Instance().PlaySfx(SFX_CAR_ALARM);
                        }
                        float cx = SCREEN_WIDTH * 0.5f;
                        if (curTier == 2) {
                            SpawnBonusDebris(1, cx + 10.0f, GROUND_Y - 42.0f, 85.0f, -135.0f, WHITE, 2.2f);
                            AddBonusPopup("MIRROR SMASHED!", cx, GROUND_Y - 76.0f, Color{ 255, 230, 60, 255 });
                        } else if (curTier == 4) {
                            SpawnBonusDebris(4, cx + 45.0f, GROUND_Y - 22.0f, 110.0f, -120.0f, WHITE, 2.4f);
                            AddBonusPopup("BUMPER RIPPED OFF!", cx, GROUND_Y - 80.0f, Color{ 255, 185, 45, 255 });
                        } else if (curTier == 5) {
                            SpawnBonusDebris(2, cx + 28.0f, GROUND_Y - 24.0f, 95.0f, -145.0f, WHITE, 2.8f);
                            AddBonusPopup("WHEEL POPPED OFF!", cx, GROUND_Y - 82.0f, Color{ 255, 145, 40, 255 });
                        } else if (curTier == 6) {
                            AudioEngine::Instance().PlaySfx(SFX_ELEC_SPARK);
                            SpawnBonusDebris(3, cx - 12.0f, GROUND_Y - 40.0f, -105.0f, -165.0f, WHITE, 2.8f);
                            SpawnBonusDebris(2, cx - 42.0f, GROUND_Y - 24.0f, -88.0f, -135.0f, WHITE, 2.8f);
                            AddBonusPopup("DOORS & AXLES GONE!", cx, GROUND_Y - 86.0f, Color{ 80, 235, 255, 255 });
                        }
                    }

                    if (bonusState.hp <= 0.0f && !bonusState.cleared) {
                        bonusState.cleared = true;
                        SpawnBonusDebris(3, SCREEN_WIDTH * 0.5f, GROUND_Y - 44.0f, 75.0f, -185.0f, WHITE, 3.0f);
                        SpawnBonusDebris(2, SCREEN_WIDTH * 0.5f - 20.0f, GROUND_Y - 26.0f, -115.0f, -155.0f, WHITE, 3.0f);
                        AudioEngine::Instance().PlaySfx(SFX_TITLE_SLAM);
                        AudioEngine::Instance().PlaySfx(SFX_FIREBALL_IMPACT);
                        AudioEngine::Instance().PlaySfx(SFX_VOICE_PERFECT);
                        cameraShake = 8.5f;
                    }
                };

                Rectangle hitbox{};
                AttackData atkData{};
                if (bonusState.gunCooldown <= 0.0f && p1->GetActiveHitbox(hitbox, atkData) && !p1->HasHitConnected()) {
                    if (CheckCollisionRecs(hitbox, carHurtbox)) {
                        p1->SetHitConnected(true);
                        p1->SetHitstop(std::min(4, atkData.hitstopFrames));
                        bonusState.gunCooldown = 0.22f;
                        float dmg = atkData.isSpecial ? 35.0f : (atkData.isHeavy ? 27.0f : 23.0f);
                        applyCarDamage(dmg, hitbox.x + hitbox.width * 0.5f, hitbox.y + hitbox.height * 0.5f, atkData.isHeavy);
                    }
                }
                for (auto& proj : p1->GetProjectiles()) {
                    if (!proj.active) continue;
                    Rectangle pBox = { proj.pos.x - proj.width * 0.5f, proj.pos.y - proj.height * 0.5f, proj.width, proj.height };
                    if (CheckCollisionRecs(pBox, carHurtbox)) {
                        proj.active = false;
                        applyCarDamage(35.0f, proj.pos.x, proj.pos.y, true);
                    }
                }

            } else if (bonusState.type == BONUS_STAGE_POOL_TAR) {

                RawInputState moveOnly{};
                moveOnly.up = raw.up;
                moveOnly.down = raw.down;
                moveOnly.left = raw.left;
                moveOnly.right = raw.right;
                bonusMoveInput.UpdateSimulated(moveOnly, bonusFreeFacingRight);
                Fighter freeDummy(1, CHAR_TECH_VISIONARY, p1->GetPosition().x + (bonusFreeFacingRight ? 140.0f : -140.0f));
                p1->UpdatePhysics(bonusMoveInput, freeDummy);
                if (raw.right && !raw.left) p1->SetPositionX(p1->GetPosition().x + 58.0f * dt);
                if (raw.left && !raw.right) p1->SetPositionX(p1->GetPosition().x - 58.0f * dt);
                p1->SetPositionX(std::clamp(p1->GetPosition().x, 26.0f, (float)SCREEN_WIDTH - 26.0f));
                p1->SetFacingRight(bonusFreeFacingRight);
                p1->SetActiveTaunt("", 0);

                if (bonusState.gunRecoil > 0.0f) bonusState.gunRecoil = std::max(0.0f, bonusState.gunRecoil - dt * 6.0f);
                if (bonusState.gunCooldown > 0.0f) bonusState.gunCooldown = std::max(0.0f, bonusState.gunCooldown - dt);

                bonusState.barrelSpawnTimer -= dt;
                if (bonusState.barrelSpawnTimer <= 0.0f) {
                    bonusState.barrelSpawnTimer = 2.4f;
                    RollingBarrel br{};
                    br.alive = true;
                    bool fromLeft = ((rand() % 2) == 0);
                    br.x = fromLeft ? -12.0f : (float)SCREEN_WIDTH + 12.0f;
                    br.y = 148.0f;
                    br.vx = fromLeft ? 78.0f : -78.0f;
                    bonusState.barrels.push_back(br);
                }
                for (auto& br : bonusState.barrels) {
                    if (!br.alive) continue;
                    br.x += br.vx * dt;
                    br.angle += (br.vx * dt) * 0.12f;
                    if (br.x < -24.0f || br.x > SCREEN_WIDTH + 24.0f) br.alive = false;
                }

                int activeLeaks = 0;
                for (const auto& c : bonusState.cracks) {
                    if (c.active && c.state == 0) activeLeaks++;
                }
                bool canSpawnMore = ((int)bonusState.cracks.size() < bonusState.cracksGoal);
                if (activeLeaks == 0 && canSpawnMore && bonusState.crackSpawnTimer > 1.02f) {
                    bonusState.crackSpawnTimer = 1.02f;
                }
                bonusState.crackSpawnTimer -= dt;
                if (activeLeaks < 3 && canSpawnMore && bonusState.crackSpawnTimer <= 0.0f) {
                    bonusState.crackSpawnTimer = 1.38f;
                    float bestX = 56.0f + (float)(rand() % 272);
                    for (int tries = 0; tries < 10; tries++) {
                        float cand = 44.0f + (float)(rand() % 296);
                        bool ok = (fabsf(cand - p1->GetPosition().x) > 36.0f);
                        for (const auto& c : bonusState.cracks) {
                            if (c.active && c.state == 0 && fabsf(c.x - cand) < 46.0f) { ok = false; break; }
                        }
                        if (ok) { bestX = cand; break; }
                    }
                    PoolCrackSlot nc{};
                    nc.active = true;
                    nc.state = 0;
                    nc.x = bestX;
                    nc.y = GROUND_Y + 2.0f + (float)(rand() % 9);
                    bonusState.cracks.push_back(nc);
                }

                auto sealCrack = [&](PoolCrackSlot& c) {
                    c.state = 1;
                    c.animTimer = 0.0f;
                    bonusState.cracksSealed++;
                    AudioEngine::Instance().PlaySfx(SFX_TAR_SPLAT, (float)(92 + (rand() % 18)) / 100.0f);
                    for (int k = 0; k < 8; k++) {
                        SpawnBonusDebris(5, c.x, c.y - 6.0f, (float)((rand() % 150) - 75), -(float)(55 + (rand() % 95)), WHITE, 0.8f);
                    }
                    AddBonusPopup("TAR SEALED!", c.x, c.y - 30.0f, Color{ 80, 225, 255, 255 });
                    if (bonusState.cracksSealed >= bonusState.cracksGoal && !bonusState.cleared) {
                        bonusState.cleared = true;
                        AudioEngine::Instance().PlaySfx(SFX_TITLE_SLAM);
                        AudioEngine::Instance().PlaySfx(SFX_CROWD_CHEER);
                        AudioEngine::Instance().PlaySfx(SFX_VOICE_PERFECT);
                        cameraShake = 6.0f;
                    }
                };

                if ((anyAttackPressed || anyAttackHeld) && bonusState.gunCooldown <= 0.0f) {
                    bonusState.gunCooldown = 0.25f;
                    bonusState.gunRecoil = 1.0f;
                    float dir = bonusFreeFacingRight ? 1.0f : -1.0f;
                    Vector2 pPos = p1->GetPosition();

                    for (auto& c : bonusState.cracks) {
                        if (c.active && c.state == 0) {
                            float dx = c.x - pPos.x;
                            if ((dir > 0.0f && dx >= -18.0f && dx <= 52.0f) ||
                                (dir < 0.0f && dx <= 18.0f && dx >= -52.0f)) {
                                sealCrack(c);
                                break;
                            }
                        }
                    }

                    TarGlobShot shot{};
                    shot.alive = true;
                    shot.pos = Vector2{ pPos.x + dir * 28.0f, pPos.y - 34.0f };
                    shot.vel = Vector2{ dir * 255.0f, 95.0f };
                    bonusState.tarShots.push_back(shot);
                    AudioEngine::Instance().PlaySfx(SFX_WHOOSH_LIGHT, 1.25f, 0.55f);
                }

                for (auto& shot : bonusState.tarShots) {
                    if (!shot.alive) continue;
                    shot.life += dt;
                    shot.vel.y += 280.0f * dt;
                    shot.pos.x += shot.vel.x * dt;
                    shot.pos.y += shot.vel.y * dt;

                    for (auto& c : bonusState.cracks) {
                        if (c.active && c.state == 0 && fabsf(shot.pos.x - c.x) <= 34.0f && shot.pos.y >= c.y - 26.0f) {
                            sealCrack(c);
                            shot.alive = false;
                            break;
                        }
                    }
                    if (shot.alive && (shot.pos.y >= GROUND_Y + 12.0f || shot.pos.x < 0.0f || shot.pos.x > SCREEN_WIDTH)) {
                        for (int k = 0; k < 4; k++) {
                            SpawnBonusDebris(5, shot.pos.x, GROUND_Y + 4.0f, (float)((rand() % 90) - 45), -(float)(40 + (rand() % 55)), WHITE, 0.55f);
                        }
                        shot.alive = false;
                    }
                }

                for (auto& c : bonusState.cracks) {
                    if (!c.active) continue;
                    c.animTimer += dt;
                    if (c.state == 1 && c.animTimer >= 0.28f) {
                        c.state = 2;
                    }
                }

            } else if (bonusState.type == BONUS_STAGE_BOOK_FIRE) {

                RawInputState moveOnly{};
                moveOnly.up = raw.up;
                moveOnly.down = raw.down;
                moveOnly.left = raw.left;
                moveOnly.right = raw.right;
                bonusMoveInput.UpdateSimulated(moveOnly, bonusFreeFacingRight);
                Fighter freeDummy(1, CHAR_TECH_VISIONARY, p1->GetPosition().x + (bonusFreeFacingRight ? 140.0f : -140.0f));
                p1->UpdatePhysics(bonusMoveInput, freeDummy);
                if (raw.right && !raw.left) p1->SetPositionX(p1->GetPosition().x + 58.0f * dt);
                if (raw.left && !raw.right) p1->SetPositionX(p1->GetPosition().x - 58.0f * dt);
                p1->SetPositionX(std::clamp(p1->GetPosition().x, 24.0f, (float)SCREEN_WIDTH - 24.0f));
                p1->SetFacingRight(bonusFreeFacingRight);
                p1->SetActiveTaunt("", 0);

                if (bonusState.bucketSwingTimer > 0.0f) bonusState.bucketSwingTimer = std::max(0.0f, bonusState.bucketSwingTimer - dt);
                if (bonusState.bucketCooldown > 0.0f) bonusState.bucketCooldown = std::max(0.0f, bonusState.bucketCooldown - dt);
                for (int i = 0; i < 4; i++) {
                    if (bonusState.bookPiles[i].hitFlash > 0.0f) {
                        bonusState.bookPiles[i].hitFlash = std::max(0.0f, bonusState.bookPiles[i].hitFlash - dt * 5.0f);
                    }
                }

                const float vilXs[3] = { 92.0f, 192.0f, 294.0f };
                const Color bookCols[4] = {
                    Color{ 185, 35, 35, 255 }, Color{ 35, 90, 185, 255 },
                    Color{ 40, 140, 65, 255 }, Color{ 155, 95, 35, 255 }
                };
                for (int v = 0; v < 3; v++) {
                    if (bonusState.villainAnimTimer[v] > 0.0f) bonusState.villainAnimTimer[v] = std::max(0.0f, bonusState.villainAnimTimer[v] - dt);
                    if (bonusState.villainQuipTimer[v] > 0.0f) bonusState.villainQuipTimer[v] = std::max(0.0f, bonusState.villainQuipTimer[v] - dt);

                    bonusState.villainThrowTimer[v] -= dt;
                    if (bonusState.villainThrowTimer[v] <= 0.0f) {
                        bonusState.villainThrowTimer[v] = 2.15f + (float)(rand() % 45) * 0.01f;
                        bonusState.villainAnimTimer[v] = 0.55f;
                        bonusState.villainQuipTimer[v] = 0.85f;

                        int bestPile = (v == 0) ? 0 : (v == 1 ? 1 + (rand() % 2) : 3);
                        for (int k = 0; k < 4; k++) {
                            int cand = (bestPile + k) % 4;
                            if (bonusState.bookPiles[cand].hp > 0.0f) { bestPile = cand; break; }
                        }
                        float tx = bonusState.bookPiles[bestPile].x;
                        float flightT = 0.65f;
                        float vx = (tx - vilXs[v]) / flightT;
                        float vy = -115.0f;
                        SpawnBonusDebris(8, vilXs[v], 132.0f, vx, vy, bookCols[rand() % 4], flightT + 0.15f, bestPile);
                    }
                }

                if ((anyAttackPressed || anyAttackHeld) && bonusState.bucketCooldown <= 0.0f) {
                    bonusState.bucketCooldown = 0.44f;
                    bonusState.bucketSwingTimer = 0.26f;
                    bonusState.bucketFacingRight = bonusFreeFacingRight;
                    float dir = bonusFreeFacingRight ? 1.0f : -1.0f;
                    Vector2 pPos = p1->GetPosition();

                    AudioEngine::Instance().PlaySfx(SFX_WATER_SPLASH, (float)(94 + (rand() % 14)) / 100.0f);

                    for (int w = 0; w < 14; w++) {
                        float wx = pPos.x + dir * (14.0f + (rand() % 38));
                        float wy = pPos.y - 40.0f + (rand() % 16);
                        SpawnBonusDebris(6, wx, wy, dir * (float)(85 + (rand() % 130)), -(float)(35 + (rand() % 80)), WHITE, 0.65f);
                    }

                    for (auto& d : bonusState.debris) {
                        if (d.alive && d.kind == 8) {
                            float dx = d.pos.x - pPos.x;
                            if ((dir > 0.0f && dx >= -12.0f && dx <= 86.0f) || (dir < 0.0f && dx <= 12.0f && dx >= -86.0f)) {
                                d.alive = false;
                                AddBonusPopup("BOOK SAVED!", d.pos.x, d.pos.y - 10.0f, Color{ 90, 245, 130, 255 });
                            }
                        }
                    }

                    bool hitAnyFire = false;
                    for (int i = 0; i < 4; i++) {
                        auto& pile = bonusState.bookPiles[i];
                        if (pile.hp <= 0.0f) continue;
                        float dx = pile.x - pPos.x;
                        if ((dir > 0.0f && dx >= -16.0f && dx <= 72.0f) ||
                            (dir < 0.0f && dx <= 16.0f && dx >= -72.0f)) {
                            pile.hp = std::max(0.0f, pile.hp - 22.5f);
                            pile.hitFlash = 1.0f;
                            hitAnyFire = true;
                            for (int s = 0; s < 5; s++) {
                                SpawnBonusDebris(7, pile.x + (rand() % 28 - 14), GROUND_Y - 26.0f, (float)(rand() % 26 - 13), -(float)(38 + rand() % 36), WHITE, 0.85f);
                            }
                            if (pile.hp <= 0.0f) {
                                AddBonusPopup("PILE EXTINGUISHED!", pile.x, GROUND_Y - 68.0f, Color{ 80, 250, 130, 255 });
                            } else {
                                AddBonusPopup("DOUSED!", pile.x, GROUND_Y - 60.0f, Color{ 95, 220, 255, 255 });
                            }
                        }
                    }
                    if (hitAnyFire) {
                        AudioEngine::Instance().PlaySfx(SFX_STEAM_HISS, (float)(92 + (rand() % 18)) / 100.0f);
                    }

                    bool allOut = true;
                    for (int i = 0; i < 4; i++) {
                        if (bonusState.bookPiles[i].hp > 0.0f) { allOut = false; break; }
                    }
                    if (allOut && !bonusState.cleared) {
                        bonusState.cleared = true;
                        AudioEngine::Instance().PlaySfx(SFX_TITLE_SLAM);
                        AudioEngine::Instance().PlaySfx(SFX_CROWD_CHEER);
                        AudioEngine::Instance().PlaySfx(SFX_VOICE_PERFECT);
                        cameraShake = 6.5f;
                    }
                }

            } else if (bonusState.type == BONUS_STAGE_BERLIN_WALL) {
                if (bonusState.gunCooldown > 0.0f) {
                    bonusState.gunCooldown = std::max(0.0f, bonusState.gunCooldown - dt);
                }
                float wallCenterX = (float)SCREEN_WIDTH * 0.5f + 16.0f;
                Rectangle wallHurtbox = { wallCenterX - 74.0f, GROUND_Y - 102.0f, 148.0f, 102.0f };
                Fighter wallDummy(1, CHAR_TECH_VISIONARY, wallCenterX);
                if (inputP1.IsButtonPressed(BTN_SPECIAL) &&
                    (p1->GetCharacterId() == CHAR_SLEEPY_JOE || p1->GetCharacterId() == CHAR_COUCHIE || p1->GetCharacterId() == CHAR_THE_DICTATOR)) {
                    RawInputState smashSim = raw;
                    smashSim.special = false;
                    smashSim.hk = true;
                    inputP1.UpdateSimulated(smashSim, p1->IsFacingRight());
                }
                p1->UpdatePhysics(inputP1, wallDummy);
                p1->SetPositionX(std::clamp(p1->GetPosition().x, 26.0f, (float)SCREEN_WIDTH - 26.0f));

                auto spawnEastGermanWave = [&](int count) {
                    for (int k = 0; k < count; k++) {
                        EastGermanRunner eg{};
                        eg.active = true;
                        eg.variant = (int)(bonusState.eastGermans.size() + k) % 4;
                        eg.x = wallCenterX + (float)((k - count / 2) * 14);
                        eg.y = GROUND_Y - 2.0f + (float)((k % 3) * 3 - 3);
                        bool goLeft = ((bonusState.eastGermans.size() % 2) == 0);
                        eg.facingRight = !goLeft;
                        eg.vx = (goLeft ? -1.0f : 1.0f) * (46.0f + (float)(rand() % 42));
                        eg.animPhase = (float)(rand() % 100) * 0.06f;
                        bonusState.eastGermans.push_back(eg);
                    }
                };

                auto applyWallDamage = [&](float dmg, float hitX, float hitY, bool isKick) {
                    bonusState.hp = std::max(0.0f, bonusState.hp - dmg);
                    bonusState.targetShake = 1.0f;
                    cameraShake = isKick ? 5.5f : 3.0f;

                    for (int b = 0; b < 9; b++) {
                        int order = (b * 5 + 2) % 9;
                        if (bonusState.blockHp[order] > 0.0f) {
                            bonusState.blockHp[order] = std::max(0.0f, bonusState.blockHp[order] - dmg);
                            break;
                        }
                    }

                    SparkEffect sp{};
                    sp.pos = Vector2{ hitX, hitY };
                    sp.isBlock = false;
                    sp.alive = true;
                    sp.frame = 0;
                    sp.maxFrames = 8;
                    sparks.push_back(sp);

                    const Color grafCols[4] = {
                        Color{ 255, 65, 195, 255 }, Color{ 50, 235, 255, 255 },
                        Color{ 255, 230, 45, 255 }, Color{ 120, 255, 90, 255 }
                    };
                    for (int c = 0; c < 4; c++) {
                        SpawnBonusDebris(9, hitX, hitY, (float)((rand() % 190) - 95), -(float)(75 + (rand() % 120)), WHITE, 1.8f);
                        SpawnBonusDebris(10, hitX, hitY, (float)((rand() % 210) - 105), -(float)(85 + (rand() % 130)), grafCols[c % 4], 1.6f);
                    }

                    AudioEngine::Instance().PlaySfx(isKick ? SFX_HIT_HEAVY : SFX_HIT_LIGHT);
                    AudioEngine::Instance().PlaySfx(SFX_WALL_CRUMBLE, (float)(90 + (rand() % 22)) / 100.0f);

                    float hpRatio = bonusState.hp / bonusState.maxHp;
                    int tier = (hpRatio <= 0.0f) ? 3 : (hpRatio <= 0.34f ? 2 : (hpRatio <= 0.68f ? 1 : 0));
                    if (tier > bonusState.lastDamageTier) {
                        bonusState.lastDamageTier = tier;
                        if (tier == 1) AddBonusPopup("WALL CRACKING!", wallCenterX, GROUND_Y - 88.0f, Color{ 255, 230, 60, 255 });
                        else if (tier == 2) AddBonusPopup("ALMOST THROUGH!", wallCenterX, GROUND_Y - 92.0f, Color{ 90, 240, 255, 255 });
                    }

                    if (bonusState.hp <= 0.0f && !bonusState.cleared) {
                        bonusState.cleared = true;
                        for (int b = 0; b < 9; b++) bonusState.blockHp[b] = 0.0f;
                        for (int c = 0; c < 14; c++) {
                            SpawnBonusDebris(9, wallCenterX + (rand() % 50 - 25), GROUND_Y - 45.0f, (float)((rand() % 240) - 120), -(float)(90 + (rand() % 150)), WHITE, 2.6f);
                            SpawnBonusDebris(10, wallCenterX + (rand() % 50 - 25), GROUND_Y - 45.0f, (float)((rand() % 240) - 120), -(float)(90 + (rand() % 150)), grafCols[c % 4], 2.4f);
                        }
                        spawnEastGermanWave(6);
                        AddBonusPopup("FREIHEIT!! EAST GERMANS FREE!", wallCenterX, GROUND_Y - 96.0f, Color{ 255, 235, 55, 255 });
                        AudioEngine::Instance().PlaySfx(SFX_TITLE_SLAM);
                        AudioEngine::Instance().PlaySfx(SFX_WALL_CRUMBLE, 0.82f);
                        AudioEngine::Instance().PlaySfx(SFX_CROWD_CHEER);
                        AudioEngine::Instance().PlaySfx(SFX_VOICE_PERFECT);
                        cameraShake = 9.0f;
                    }
                };

                Rectangle hitbox{};
                AttackData atkData{};
                if (bonusState.gunCooldown <= 0.0f && p1->GetActiveHitbox(hitbox, atkData) && !p1->HasHitConnected()) {
                    if (CheckCollisionRecs(hitbox, wallHurtbox)) {
                        p1->SetHitConnected(true);
                        p1->SetHitstop(std::min(4, atkData.hitstopFrames));
                        bonusState.gunCooldown = 0.22f;
                        bool isKick = (atkData.moveType == MOVE_LIGHT_KICK || atkData.moveType == MOVE_HEAVY_KICK);
                        float dmg = atkData.isSpecial ? 35.0f : (isKick ? (atkData.isHeavy ? 28.5f : 25.5f) : (atkData.isHeavy ? 26.0f : 22.5f));
                        applyWallDamage(dmg, hitbox.x + hitbox.width * 0.5f, hitbox.y + hitbox.height * 0.5f, isKick || atkData.isHeavy);
                    }
                }
                for (auto& proj : p1->GetProjectiles()) {
                    if (!proj.active) continue;
                    Rectangle pBox = { proj.pos.x - proj.width * 0.5f, proj.pos.y - proj.height * 0.5f, proj.width, proj.height };
                    if (CheckCollisionRecs(pBox, wallHurtbox)) {
                        proj.active = false;
                        applyWallDamage(35.0f, proj.pos.x, proj.pos.y, true);
                    }
                }
            }
        } else {

            if (!bonusState.endVoicePlayed) {
                bonusState.endVoicePlayed = true;
                if (bonusState.cleared) {
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_YOU_WIN);
                } else {
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_VETOED);
                }
            }

            bonusState.postTimer += dt;
            RawInputState idleRaw{};
            bonusMoveInput.UpdateSimulated(idleRaw, bonusFreeFacingRight);
            Fighter idleDummy(1, CHAR_TECH_VISIONARY, SCREEN_WIDTH * 0.5f);
            p1->UpdatePhysics(bonusMoveInput, idleDummy);
            p1->SetPositionX(std::clamp(p1->GetPosition().x, 26.0f, (float)SCREEN_WIDTH - 26.0f));

            if (bonusState.type == BONUS_STAGE_BERLIN_WALL && bonusState.cleared) {
                bonusState.runnerSpawnTimer -= dt;
                if (bonusState.runnerSpawnTimer <= 0.0f && bonusState.eastGermans.size() < 14) {
                    bonusState.runnerSpawnTimer = 0.28f;
                    float wallCenterX = (float)SCREEN_WIDTH * 0.5f + 16.0f;
                    int idx = (int)bonusState.eastGermans.size();
                    EastGermanRunner eg{};
                    eg.active = true;
                    eg.variant = idx % 4;
                    eg.x = wallCenterX + (float)((rand() % 18) - 9);
                    eg.y = GROUND_Y - 1.0f + (float)((rand() % 8) - 3);
                    bool goLeft = ((idx % 2) == 0);
                    eg.facingRight = !goLeft;
                    eg.vx = (goLeft ? -1.0f : 1.0f) * (50.0f + (float)(rand() % 38));
                    bonusState.eastGermans.push_back(eg);
                    const char* joyQuips[3] = { "BANANAS!!", "PRETZELS!!", "FREIHEIT!!" };
                    float popX = wallCenterX + (goLeft ? -48.0f : 48.0f) + (float)((idx % 3) * 14 - 14);
                    float popY = GROUND_Y - 56.0f - (float)((idx % 3) * 11);
                    AddBonusPopup(joyQuips[idx % 3], popX, popY, Color{ 110, 255, 140, 255 });
                }
            }

            float outroLen = (bonusState.type == BONUS_STAGE_BERLIN_WALL && bonusState.cleared) ? 3.6f : 2.3f;
            if (bonusState.postTimer >= outroLen) {
                return true;
            }
        }

        for (auto& eg : bonusState.eastGermans) {
            if (!eg.active) continue;
            eg.x += eg.vx * dt;
            eg.animPhase += dt * 15.0f;
            if (eg.x < -30.0f || eg.x > SCREEN_WIDTH + 30.0f) eg.active = false;
        }

        for (auto& d : bonusState.debris) {
            if (!d.alive) continue;
            d.life += dt;
            if (d.life >= d.maxLife) {
                d.alive = false;
                continue;
            }
            if (d.kind == 7) {
                d.pos.x += d.vel.x * dt;
                d.pos.y += d.vel.y * dt;
            } else {
                d.vel.y += 320.0f * dt;
                d.pos.x += d.vel.x * dt;
                d.pos.y += d.vel.y * dt;
                d.angle += d.angVel * dt;
                if (d.kind == 8 && d.pos.y >= GROUND_Y - 14.0f) {

                    if (d.targetPile >= 0 && d.targetPile < 4) {
                        auto& pile = bonusState.bookPiles[d.targetPile];
                        if (pile.hp > 0.0f) {
                            pile.hp = std::min(pile.maxHp, pile.hp + 2.5f);
                        }
                    }
                    d.alive = false;
                } else if (d.pos.y >= GROUND_Y + 2.0f) {
                    d.pos.y = GROUND_Y + 2.0f;
                    d.vel.y = -fabsf(d.vel.y) * 0.42f;
                    d.vel.x *= 0.78f;
                }
            }
        }

        for (auto& pop : bonusState.popups) {
            if (!pop.alive) continue;
            pop.life += dt;
            pop.pos.y -= 22.0f * dt;
            if (pop.life >= pop.maxLife) pop.alive = false;
        }

        for (auto& sp : sparks) {
            if (!sp.alive) continue;
            sp.frame++;
            if (sp.frame >= sp.maxFrames) sp.alive = false;
        }
        if (cameraShake > 0.01f) cameraShake *= 0.88f;
        else cameraShake = 0.0f;
        return false;
    };

    while (!WindowShouldClose() && !s_quitRequested) {
        InputManager::BeginRenderFrame();
        float dt = GetFrameTime();
        bool altHeld = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
        bool enterPressed = !altHeld && (InputManager::WasKeyPressed(KEY_ENTER) || InputManager::WasKeyPressed(KEY_KP_ENTER));

        bool crtTogglePressed = InputManager::WasKeyPressed(KEY_F1) || InputManager::WasKeyPressed(KEY_F2);
        for (int pad = 0; pad < 4; pad++) {
            if (IsGamepadAvailable(pad) && IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_THUMB)) {
                crtTogglePressed = true;
            }
        }
        if (crtTogglePressed) {
            enableCrt = !enableCrt;
            AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR, enableCrt ? 1.25f : 0.90f, 0.85f);
        }
        stateTimer += dt;
        AudioEngine::Instance().Update();

        bool killRequested = WindowShouldClose() || (s_quitRequested != 0);
        if (InputManager::WasKeyPressed(KEY_ESCAPE) && gameState != STATE_SECRET_SELECT) {
            killRequested = true;
        }
        if (!altHeld && IsKeyDown(KEY_BACKSPACE) && (IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_KP_ENTER))) {
            killRequested = true;
        }
        for (int pad = 0; pad < 4; pad++) {
            if (!IsGamepadAvailable(pad)) continue;
            if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_LEFT) &&
                IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                killRequested = true;
                break;
            }
        }
        if (killRequested) {
            break;
        }

#if !defined(GRAPHICS_API_OPENGL_ES2) && !defined(__aarch64__)
        mainLoopStarted = true;
#endif
        if (InputManager::WasKeyPressed(KEY_F11) ||
            (altHeld && (InputManager::WasKeyPressed(KEY_ENTER) || InputManager::WasKeyPressed(KEY_KP_ENTER)))) {
            ToggleAppFullscreen();
        }

        if (InputManager::WasKeyPressed(KEY_F12)) {
            TakeScreenshot("screenshot.png");
        }

        static bool s_selectTapPending = false;
        bool anySelectPressed = false;
        bool anySelectDown = false;
        bool anySelectChordModifier = false;
        for (int pad = 0; pad < 4; pad++) {
            if (!IsGamepadAvailable(pad)) continue;
            if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_LEFT)) anySelectPressed = true;
            if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_LEFT)) anySelectDown = true;
            if (IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT) ||
                IsGamepadButtonDown(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ||
                IsGamepadButtonDown(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
                anySelectChordModifier = true;
            }
        }
        if (anySelectPressed && (gameState == STATE_SPLASH || gameState == STATE_SELECT_MAP)) {
            s_selectTapPending = true;
        }
        if (anySelectDown && anySelectChordModifier) {
            s_selectTapPending = false;
        }
        bool tabTurbo = InputManager::WasKeyPressed(KEY_TAB);
        bool padTurbo = false;
        if (!anySelectDown && s_selectTapPending) {
            s_selectTapPending = false;
            padTurbo = true;
        }
        if (tabTurbo || ((gameState == STATE_SPLASH || gameState == STATE_SELECT_MAP) && padTurbo)) {
            ChampionshipEdition::s_turboSpeed = (TurboSpeed)(((int)ChampionshipEdition::s_turboSpeed + 1) % 3);
            AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR, 1.15f, 1.0f);
        }

        if ((gameState == STATE_SPLASH || gameState == STATE_SELECT_MAP || gameState == STATE_WORLD_MAP ||
             gameState == STATE_BONUS_CAR) && inputP1.IsSecretMenuComboPressed()) {
            s_selectTapPending = false;
            isGamePaused = false;
            bonusFromEasterEgg = false;
            AudioEngine::Instance().PlaySfx(SFX_COIN);
            AudioEngine::Instance().PlaySfx(SFX_SUPER_FREEZE, 1.0f, 0.9f);
            AudioEngine::Instance().SetBgmTrack(BGM_CHAR_SELECT);
            gameState = STATE_SECRET_SELECT;
            isSecretMode = true;
            secretStep = 0;
            secretP1 = (int)p1CharId;
            secretP2 = (int)p2CharId;
            stateTimer = 0.0f;
        }

        if ((gameState == STATE_SPLASH || gameState == STATE_SELECT_MAP || gameState == STATE_WORLD_MAP ||
             gameState == STATE_SECRET_SELECT || gameState == STATE_BONUS_CAR) && inputP1.IsTriggerEasterEggPressed()) {
            s_selectTapPending = false;
            isGamePaused = false;
            BonusStageType chosen = (BonusStageType)(easterEggBonusCursor % BONUS_STAGE_COUNT);
            easterEggBonusCursor = (easterEggBonusCursor + 1) % BONUS_STAGE_COUNT;
            StartBonusStage(chosen, true);
        }

        switch (gameState) {
        case STATE_SPLASH: {
            if (!logoSlammed && stateTimer >= 0.8f) {
                logoSlammed = true;
                cameraShake = 4.5f;
                AudioEngine::Instance().PlaySfx(SFX_TITLE_SLAM);
                AudioEngine::Instance().PlaySfx(SFX_VOICE_TITLE);
            }

            inputP1.Update(0, true);

            bool startPressed = false;
            if (enterPressed || InputManager::WasKeyPressed(KEY_SPACE) ||
                InputManager::WasKeyPressed(KEY_ONE) || InputManager::WasKeyPressed(KEY_FIVE) ||
                InputManager::WasKeyPressed(KEY_U) || InputManager::WasKeyPressed(KEY_Z) ||
                InputManager::WasKeyPressed(KEY_I) || InputManager::WasKeyPressed(KEY_X) ||
                InputManager::WasKeyPressed(KEY_J) || InputManager::WasKeyPressed(KEY_C) ||
                InputManager::WasKeyPressed(KEY_K) || InputManager::WasKeyPressed(KEY_V) ||
                InputManager::WasKeyPressed(KEY_O) || InputManager::WasKeyPressed(KEY_E)) {
                startPressed = true;
            }
            for (int pad = 0; pad < 4; pad++) {
                if (!IsGamepadAvailable(pad)) continue;
                if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_UP)) {
                    startPressed = true;
                    break;
                }
            }
            if (inputP1.IsButtonPressed(BTN_START) ||
                inputP1.IsButtonPressed(BTN_LP) || inputP1.IsButtonPressed(BTN_HP) ||
                inputP1.IsButtonPressed(BTN_LK) || inputP1.IsButtonPressed(BTN_HK) ||
                inputP1.IsButtonPressed(BTN_SPECIAL)) {
                startPressed = true;
            }

            if (startPressed) {
                AudioEngine::Instance().PlaySfx(SFX_COIN);
                AudioEngine::Instance().SetBgmTrack(BGM_CHAR_SELECT);
                gameState = STATE_SELECT_MAP;
                isSecretMode = false;
                continuesRemaining = 2;
                ChampionshipEdition::s_continuesRemaining = 2;
                playerLocked = false;
                opponentLocked = false;
                ChampionshipEdition::s_opponentLocked = false;
                choosingOpponent = false;
                p1CharId = CHAR_COMMANDER;
                p2CharId = PickNextAvailableOpponent(1);
                currentStage = homeStages[(int)p1CharId];
                stageMgr.SetStage(currentStage);
                stateTimer = 0.0f;
            }
            break;
        }

        case STATE_SELECT_MAP: {
            inputP1.Update(0, true);
            ChampionshipEdition::s_opponentLocked = opponentLocked;

            int origDefeated = CountDefeatedOriginals();

            if (defeatedFighters[CHAR_PALM_BEACH_PETE]) {
                worldActIndex = GetCurrentWorldAct();
                if (worldActIndex >= 5) {
                    if (p1CharId == CHAR_COMMANDER && !isSecretMode) {
                        gameState = STATE_CTHULHU_ENDING;
                        stateTimer = 0.0f;
                        AudioEngine::Instance().StopBgm();
                        AudioEngine::Instance().PlaySfx(SFX_CTHULHU_ROAR);
                    } else {
                        gameState = STATE_ENDING_CELEBRATION;
                        stateTimer = 0.0f;
                        AudioEngine::Instance().StopBgm();
                        AudioEngine::Instance().PlaySfx(SFX_JESUS_FANFARE);
                    }
                } else {
                    gameState = STATE_WORLD_MAP;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(BGM_WORLD_CONFLICT_MAP);
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_GLOBAL_CONFLICT);
                }
                break;
            }

            if (defeatedFighters[CHAR_WAR_PIG]) {
                opponentLocked = true;
                ChampionshipEdition::s_opponentLocked = true;
                AudioEngine::Instance().SetBgmTrack(BGM_US_MAP);
                if (!defeatedFighters[CHAR_SOCIALIST]) {
                    p2CharId = CHAR_SOCIALIST;
                    currentStage = STAGE_WTC_PLAZA;
                } else if (!defeatedFighters[CHAR_DICK]) {
                    p2CharId = CHAR_DICK;
                    currentStage = STAGE_WYOMING_RODEO;
                } else if (!defeatedFighters[CHAR_BILLARY]) {
                    p2CharId = CHAR_BILLARY;
                    currentStage = STAGE_COMET_PING_PONG;
                } else if (!defeatedFighters[CHAR_A_NEW_HOPE]) {
                    p2CharId = CHAR_A_NEW_HOPE;
                    currentStage = STAGE_CHICAGO_WRIGLEY;
                } else {
                    p2CharId = CHAR_PALM_BEACH_PETE;
                    currentStage = STAGE_FLORIDA_KEYS;
                }
                stageMgr.SetStage(currentStage);
            } else if (origDefeated >= (PLAYABLE_CHARACTER_COUNT - 1)) {
                opponentLocked = true;
                ChampionshipEdition::s_opponentLocked = true;
                AudioEngine::Instance().SetBgmTrack(BGM_CHAR_SELECT);
                if (!defeatedFighters[CHAR_SUCKRATERY_RKF]) {
                    p2CharId = CHAR_SUCKRATERY_RKF;
                    currentStage = STAGE_PRESS_BRIEFING;
                } else if (!defeatedFighters[CHAR_THE_PUNDIT]) {
                    p2CharId = CHAR_THE_PUNDIT;
                    currentStage = STAGE_ENN_NEWS_STUDIO;
                } else {
                    p2CharId = CHAR_WAR_PIG;
                    currentStage = STAGE_PENTAGON_ALTAR;
                }
                stageMgr.SetStage(currentStage);
            } else if (!playerLocked) {
                AudioEngine::Instance().SetBgmTrack(BGM_CHAR_SELECT);
                int slot = GetPlayableSlotForChar((int)p1CharId);
                int p1Col = slot % 8;
                int p1Row = slot / 8;
                bool p1Changed = false;

                if (InputManager::WasKeyPressed(KEY_LEFT) || InputManager::WasKeyPressed(KEY_A) || inputP1.IsLeftPressed()) {
                    p1Col = (p1Col + 7) % 8;
                    p1Changed = true;
                } else if (InputManager::WasKeyPressed(KEY_RIGHT) || InputManager::WasKeyPressed(KEY_D) || inputP1.IsRightPressed()) {
                    p1Col = (p1Col + 1) % 8;
                    p1Changed = true;
                }
                if (InputManager::WasKeyPressed(KEY_UP) || InputManager::WasKeyPressed(KEY_W) || inputP1.IsUpPressed() ||
                    InputManager::WasKeyPressed(KEY_DOWN) || InputManager::WasKeyPressed(KEY_S) || inputP1.IsDownPressed()) {
                    p1Row = 1 - p1Row;
                    p1Changed = true;
                }

                if (p1Changed) {
                    p1CharId = (CharacterId)PLAYABLE_ROSTER[p1Row * 8 + p1Col];
                    currentStage = homeStages[(int)p1CharId];
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }
            } else {
                opponentLocked = false;
                ChampionshipEdition::s_opponentLocked = false;
                AudioEngine::Instance().SetBgmTrack(BGM_CHAR_SELECT);

                if (matchesWon == 0 && InputManager::WasKeyPressed(KEY_BACKSPACE)) {
                    playerLocked = false;
                    choosingOpponent = false;
                    currentStage = homeStages[(int)p1CharId];
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_BLOCK, 1.0f);
                    stateTimer = 0.0f;
                    break;
                }

                bool p2Changed = false;
                int curSlot = GetPlayableSlotForChar((int)p2CharId);

                if (InputManager::WasKeyPressed(KEY_RIGHT) || InputManager::WasKeyPressed(KEY_D) || inputP1.IsRightPressed()) {
                    for (int step = 1; step < PLAYABLE_CHARACTER_COUNT; step++) {
                        int nextCid = PLAYABLE_ROSTER[(curSlot + step) % PLAYABLE_CHARACTER_COUNT];
                        if (nextCid != (int)p1CharId && !defeatedFighters[nextCid]) {
                            p2CharId = (CharacterId)nextCid;
                            p2Changed = true;
                            break;
                        }
                    }
                } else if (InputManager::WasKeyPressed(KEY_LEFT) || InputManager::WasKeyPressed(KEY_A) || inputP1.IsLeftPressed()) {
                    for (int step = 1; step < PLAYABLE_CHARACTER_COUNT; step++) {
                        int prevCid = PLAYABLE_ROSTER[(curSlot - step + PLAYABLE_CHARACTER_COUNT) % PLAYABLE_CHARACTER_COUNT];
                        if (prevCid != (int)p1CharId && !defeatedFighters[prevCid]) {
                            p2CharId = (CharacterId)prevCid;
                            p2Changed = true;
                            break;
                        }
                    }
                }
                if (InputManager::WasKeyPressed(KEY_UP) || InputManager::WasKeyPressed(KEY_W) || inputP1.IsUpPressed() ||
                    InputManager::WasKeyPressed(KEY_DOWN) || InputManager::WasKeyPressed(KEY_S) || inputP1.IsDownPressed()) {
                    int targetSlot = (curSlot + 8) % PLAYABLE_CHARACTER_COUNT;
                    for (int step = 0; step < PLAYABLE_CHARACTER_COUNT; step++) {
                        int candCid = PLAYABLE_ROSTER[(targetSlot + step) % PLAYABLE_CHARACTER_COUNT];
                        if (candCid != (int)p1CharId && !defeatedFighters[candCid]) {
                            p2CharId = (CharacterId)candCid;
                            p2Changed = true;
                            break;
                        }
                    }
                }

                if (p2Changed) {
                    currentStage = GetStageForOpponent(p2CharId);
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsButtonPressed(BTN_TAUNT) || inputP1.IsButtonPressed(BTN_SPECIAL)) {
                    static const StageId normalStages[12] = {
                        STAGE_DEMOLISHED_EAST_WING,
                        STAGE_REFLECTING_POOL,
                        STAGE_CONGRESSIONAL_FLOOR,
                        STAGE_WHITE_HOUSE_MMA,
                        STAGE_LINCOLN_MEMORIAL,
                        STAGE_SMITHSONIAN_AERO,
                        STAGE_TOMB_UNKNOWN,
                        STAGE_CHICAGO_WRIGLEY,
                        STAGE_PERFORMING_ARTS,
                        STAGE_THE_HOOD,
                        STAGE_PROGRESS_AVENUE,
                        STAGE_LINCOLN_BEDROOM
                    };
                    int curIdx = 0;
                    for (int k = 0; k < 12; k++) {
                        if (normalStages[k] == currentStage) {
                            curIdx = k;
                            break;
                        }
                    }
                    curIdx = inputP1.IsButtonPressed(BTN_SPECIAL) ? ((curIdx + 1) % 12) : ((curIdx + 11) % 12);
                    currentStage = normalStages[curIdx];
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }
            }

            bool canConfirm = (stateTimer >= 0.25f);
            bool padConfirm = false;
            for (int pad = 0; pad < 4; pad++) {
                if (!IsGamepadAvailable(pad)) continue;
                if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_UP)) {
                    padConfirm = true;
                    break;
                }
            }

            bool specialConfirm = (!playerLocked || opponentLocked) && inputP1.IsButtonPressed(BTN_SPECIAL);
            if (canConfirm && (padConfirm || enterPressed || InputManager::WasKeyPressed(KEY_SPACE) ||
                inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                inputP1.IsButtonPressed(BTN_HK) || specialConfirm)) {

                if (!playerLocked) {
                    playerLocked = true;
                    opponentLocked = false;
                    ChampionshipEdition::s_opponentLocked = false;
                    choosingOpponent = true;
                    int p1Slot = GetPlayableSlotForChar((int)p1CharId);
                    p2CharId = PickNextAvailableOpponent(p1Slot + 1);
                    currentStage = GetStageForOpponent(p2CharId);
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                    stateTimer = 0.0f;
                } else {
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);

                    if (defeatedFighters[CHAR_WAR_PIG] || origDefeated >= (PLAYABLE_CHARACTER_COUNT - 1)) {
                        currentStage = GetStageForOpponent(p2CharId);
                    }
                    stageMgr.SetStage(currentStage);
                    opponentLocked = false;
                    ChampionshipEdition::s_opponentLocked = false;

                    cpuAI.SetDifficulty(IsHardBossOpponent(p2CharId) ? AI_DIFF_HARD : AI_DIFF_NORMAL);
                    cpuAI.Reset();
                    inputP2.Reset();

                    p1 = std::make_unique<Fighter>(0, p1CharId, 320.0f);
                    p2 = std::make_unique<Fighter>(1, p2CharId, 480.0f);
                    VisualsEngine::PrefetchMatchFighters((int)p1CharId, (int)p2CharId);

                    AudioEngine::Instance().StopBgm();
                    AudioEngine::Instance().PlaySfx(SFX_MATCH_START_FANFARE);

                    gameState = STATE_VERSUS;
                    stateTimer = 0.0f;
                }
            }
            break;
        }

        case STATE_WORLD_MAP: {
            inputP1.Update(0, true);
            worldActIndex = GetCurrentWorldAct();

            if (worldActIndex >= 5) {
                if (p1CharId == CHAR_COMMANDER && !isSecretMode) {
                    gameState = STATE_CTHULHU_ENDING;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().StopBgm();
                    AudioEngine::Instance().PlaySfx(SFX_CTHULHU_ROAR);
                } else {
                    gameState = STATE_ENDING_CELEBRATION;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().StopBgm();
                    AudioEngine::Instance().PlaySfx(SFX_JESUS_FANFARE);
                }
                break;
            }

            static const CharacterId worldBossOrder[5] = {
                CHAR_KIM, CHAR_BEBE, CHAR_PALE_MOTH, CHAR_THE_DICTATOR, CHAR_AYATOLLAH
            };
            static const StageId worldStageOrder[5] = {
                STAGE_RYUGYONG_HOTEL, STAGE_WESTERN_WALL, STAGE_SOVIET_FOUNDRY, STAGE_PALACE_COURTYARD, STAGE_TEHRAN_CASBAH
            };

            p2CharId = worldBossOrder[worldActIndex];
            currentStage = worldStageOrder[worldActIndex];
            stageMgr.SetStage(currentStage);

            bool confirmPressed = (enterPressed || InputManager::WasKeyPressed(KEY_SPACE) ||
                inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                inputP1.IsButtonPressed(BTN_HK) || inputP1.IsButtonPressed(BTN_SPECIAL));

            if (stateTimer >= 3.8f || (stateTimer >= 0.75f && confirmPressed)) {
                AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                AudioEngine::Instance().StopBgm();
                AudioEngine::Instance().PlaySfx(SFX_MATCH_START_FANFARE);

                cpuAI.SetDifficulty(AI_DIFF_HARD);
                cpuAI.Reset();
                inputP2.Reset();

                p1 = std::make_unique<Fighter>(0, p1CharId, 320.0f);
                p2 = std::make_unique<Fighter>(1, p2CharId, 480.0f);
                VisualsEngine::PrefetchMatchFighters((int)p1CharId, (int)p2CharId);

                gameState = STATE_VERSUS;
                stateTimer = 0.0f;
            }
            break;
        }

        case STATE_SECRET_SELECT: {
            inputP1.Update(0, true);

            if (inputP1.IsButtonPressed(BTN_SELECT) || InputManager::WasKeyPressed(KEY_ESCAPE) || InputManager::WasKeyPressed(KEY_BACKSPACE)) {
                AudioEngine::Instance().PlaySfx(SFX_BLOCK, 1.0f);
                AudioEngine::Instance().SetBgmTrack(BGM_MENU_SPLASH);
                gameState = STATE_SPLASH;
                isSecretMode = false;
                stateTimer = 0.0f;
                break;
            }

            if (secretStep == 0 || secretStep == 1) {
                int& curChar = (secretStep == 0) ? secretP1 : secretP2;
                if (inputP1.IsLeftPressed() || InputManager::WasKeyPressed(KEY_LEFT) || InputManager::WasKeyPressed(KEY_A)) {
                    curChar = MoveSecretCharCursor(curChar, -1, 0);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsRightPressed() || InputManager::WasKeyPressed(KEY_RIGHT) || InputManager::WasKeyPressed(KEY_D)) {
                    curChar = MoveSecretCharCursor(curChar, +1, 0);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsUpPressed() || InputManager::WasKeyPressed(KEY_UP) || InputManager::WasKeyPressed(KEY_W)) {
                    curChar = MoveSecretCharCursor(curChar, 0, -1);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsDownPressed() || InputManager::WasKeyPressed(KEY_DOWN) || InputManager::WasKeyPressed(KEY_S)) {
                    curChar = MoveSecretCharCursor(curChar, 0, +1);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }

                if (enterPressed || InputManager::WasKeyPressed(KEY_SPACE) ||
                    inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                    inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                    inputP1.IsButtonPressed(BTN_HK) || inputP1.IsButtonPressed(BTN_SPECIAL)) {
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                    if (secretStep == 1) {
                        secretStage = (int)GetStageForOpponent((CharacterId)secretP2);
                    }
                    secretStep++;
                    stateTimer = 0.0f;
                }
            } else if (secretStep == 2) {
                if (inputP1.IsLeftPressed() || InputManager::WasKeyPressed(KEY_LEFT) || InputManager::WasKeyPressed(KEY_A)) {
                    secretStage = (secretStage - 1 + STAGE_COUNT) % STAGE_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsRightPressed() || InputManager::WasKeyPressed(KEY_RIGHT) || InputManager::WasKeyPressed(KEY_D)) {
                    secretStage = (secretStage + 1) % STAGE_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsUpPressed() || InputManager::WasKeyPressed(KEY_UP) || InputManager::WasKeyPressed(KEY_W)) {
                    secretStage = (secretStage - 6 + STAGE_COUNT) % STAGE_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsDownPressed() || InputManager::WasKeyPressed(KEY_DOWN) || InputManager::WasKeyPressed(KEY_S)) {
                    secretStage = (secretStage + 6) % STAGE_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }

                if (enterPressed || InputManager::WasKeyPressed(KEY_SPACE) ||
                    inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                    inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                    inputP1.IsButtonPressed(BTN_HK) || inputP1.IsButtonPressed(BTN_SPECIAL)) {
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                    secretStep = 3;
                    stateTimer = 0.0f;
                }
            } else if (secretStep == 3) {
                if (inputP1.IsUpPressed() || InputManager::WasKeyPressed(KEY_UP) || InputManager::WasKeyPressed(KEY_W)) {
                    secretDifficulty = (secretDifficulty - 1 + 3) % 3;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsDownPressed() || InputManager::WasKeyPressed(KEY_DOWN) || InputManager::WasKeyPressed(KEY_S)) {
                    secretDifficulty = (secretDifficulty + 1) % 3;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }

                if (enterPressed || InputManager::WasKeyPressed(KEY_SPACE) ||
                    inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                    inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                    inputP1.IsButtonPressed(BTN_HK) || inputP1.IsButtonPressed(BTN_SPECIAL)) {
                    AudioEngine::Instance().PlaySfx(SFX_MATCH_START_FANFARE);

                    p1CharId = (CharacterId)secretP1;
                    p2CharId = (CharacterId)secretP2;
                    currentStage = (StageId)secretStage;
                    stageMgr.SetStage(currentStage);

                    AIDifficulty chosenDiff = (AIDifficulty)secretDifficulty;
                    cpuAI.SetDifficulty(chosenDiff);
                    cpuAI.Reset();
                    inputP2.Reset();

                    p1 = std::make_unique<Fighter>(0, p1CharId, 320.0f);
                    p2 = std::make_unique<Fighter>(1, p2CharId, 480.0f);
                    VisualsEngine::PrefetchMatchFighters((int)p1CharId, (int)p2CharId);

                    p1WasKo = false;
                    p2WasKo = false;
                    ResetFightRoundState(true);

                    gameState = STATE_FIGHT;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(AudioEngine::GetStageBgmTrack(currentStage));
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_ROUND1);
                }
            }
            break;
        }

        case STATE_VERSUS: {
            if (stateTimer >= 1.8f) {
                gameState = STATE_FIGHT;
                stateTimer = 0.0f;
                p1WasKo = false;
                p2WasKo = false;
                ResetFightRoundState(true);
                p1->ResetRound(320.0f, true);
                p2->ResetRound(480.0f, false);
                cpuAI.Reset();
                inputP2.Reset();
                AudioEngine::Instance().SetBgmTrack(AudioEngine::GetStageBgmTrack(currentStage));
                AudioEngine::Instance().PlaySfx(SFX_VOICE_ROUND1);
            }
            break;
        }

        case STATE_FIGHT: {
            bool pausePressed = false;
            if (enterPressed || InputManager::WasKeyPressed(KEY_P)) {
                pausePressed = true;
            }
            for (int pad = 0; pad < 2; pad++) {
                if (IsGamepadAvailable(pad) && IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    pausePressed = true;
                }
            }

            if (pausePressed && !roundOver) {
                isGamePaused = !isGamePaused;
                AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
            }

            if (isGamePaused) {
                break;
            }

            inputP1.LatchRenderFrame(0);

            if (!fightActive && !roundOver) {
                fightIntroTimer--;

                if (p1->GetState() == FSTATE_KNOCKDOWN_GROUND && fightIntroTimer <= 75) {
                    p1->SetState(FSTATE_WAKEUP);
                }
                if (p2->GetState() == FSTATE_KNOCKDOWN_GROUND && fightIntroTimer <= 75) {
                    p2->SetState(FSTATE_WAKEUP);
                }

                if (p1->GetState() == FSTATE_WAKEUP) p1->TickWakeup();
                if (p2->GetState() == FSTATE_WAKEUP) p2->TickWakeup();

                if (fightIntroTimer == 50) {
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_FIGHT);
                } else if (fightIntroTimer <= 0) {
                    fightActive = true;
                    p1WasKo = false;
                    p2WasKo = false;
                }
            }

            if (fightActive) {
                bool anyFinisherRunning = (p1->IsPerformingFinisher() || p2->IsPerformingFinisher());
                roundTickAccum += dt;
                if (roundTickAccum >= 1.0f) {
                    roundTickAccum -= 1.0f;
                    if (roundSeconds > 0 && !ChampionshipEdition::s_finishHimActive && !anyFinisherRunning) {
                        roundSeconds--;
                    }
                }

                AudioEngine::SetCriticalTempo(
                    (p1->GetHealth() > 0 && p1->GetHealth() <= 25) ||
                    (p2->GetHealth() > 0 && p2->GetHealth() <= 25)
                );

                float turboMult = 1.0f + 0.15f * (float)std::clamp((int)ChampionshipEdition::s_turboSpeed, 0, 2);
                float frameUnits = (dt >= 0.012f && dt <= 0.022f) ? 1.0f : (std::min(dt, 0.05f) * 60.0f);
                simAccum += frameUnits * turboMult;
                int simSteps = std::clamp((int)(simAccum + 0.0001f), 0, 3);
                simAccum = std::max(0.0f, simAccum - (float)simSteps);

                int p1RoundsNeeded = (p2CharId == CHAR_WAR_PIG || p2CharId == CHAR_PALM_BEACH_PETE || p2CharId == CHAR_AYATOLLAH) ? 1 : 2;
                int p2RoundsNeeded = 2;

                for (int stepIdx = 0; stepIdx < simSteps && fightActive; stepIdx++) {
                    inputP1.Update(0, p1->IsFacingRight(), stepIdx > 0);
                    cpuAI.Update(inputP2, *p2, *p1, FIXED_DT);

                    if (ChampionshipEdition::s_finishHimActive) {
                        bool triggerFinisher = false;
                        if (finishHimTargetPlayer == 1 && inputP1.IsButtonPressed(BTN_FINISHER)) {
                            triggerFinisher = true;
                        } else if (finishHimTargetPlayer == 0 && inputP2.IsButtonPressed(BTN_FINISHER)) {
                            triggerFinisher = true;
                        }
                        if (triggerFinisher) {
                            ChampionshipEdition::s_finishHimActive = false;
                            finishHimTimer = 0;
                            Fighter& atk = (finishHimTargetPlayer == 1) ? *p1 : *p2;
                            Fighter& vic = (finishHimTargetPlayer == 1) ? *p2 : *p1;
                            atk.StartFinisher(vic);
                            ChampionshipEdition::s_superFlashTimer = 12;
                            ChampionshipEdition::s_crowdExciteTimer = 180;
                            cameraShake = 4.5f;
                        }
                    }

                    if (ChampionshipEdition::s_superFlashTimer > 0) {
                        ChampionshipEdition::s_superFlashTimer--;
                    } else {
                        p1->UpdatePhysics(inputP1, *p2);
                        p2->UpdatePhysics(inputP2, *p1);

                        if (!p1->IsPerformingFinisher() && !p2->IsPerformingFinisher()) {
                            CollisionSystem::ResolvePushboxes(*p1, *p2);
                            CollisionSystem::ResolveCombat(*p1, *p2, sparks, cameraShake);
                        }
                    }
                    ChampionshipEdition::UpdateAndTrackFighters(*p1, *p2);

                    if (p1->IsPerformingFinisher() || p2->IsPerformingFinisher()) {
                        Fighter& atk = p1->IsPerformingFinisher() ? *p1 : *p2;
                        Fighter& vic = p1->IsPerformingFinisher() ? *p2 : *p1;
                        if (atk.GetStateFrame() == 54) {
                            CharacterId aId = atk.GetCharacterId();
                            vic.TriggerVetoedKO();
                            if (aId == CHAR_BEBE) vic.TriggerStarBurst(52);
                            if (aId == CHAR_FLOTUS || aId == CHAR_MTG || aId == CHAR_POPULIST) vic.TriggerElectroShock(46);
                            if (aId == CHAR_W) vic.TriggerJengaCollapse(68);
                            ChampionshipEdition::s_vetoedKoActive = true;
                            ChampionshipEdition::s_crowdExciteTimer = 180;
                            AudioEngine::Instance().PlaySfx(SFX_FINISHER_OBLITERATE, 1.0f, 1.0f);
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_VETOED, 1.0f, 1.0f);
                            cameraShake = 9.5f;
                            for (int s = 0; s < 2; s++) {
                                SparkEffect sp{};
                                sp.alive = true;
                                sp.isBlock = false;
                                sp.pos = Vector2{ vic.GetPosition().x + (s == 0 ? -24.0f : 24.0f), GROUND_Y - 22.0f - s * 8.0f };
                                sp.frame = s * 2;
                                sp.maxFrames = 12;
                                sparks.push_back(sp);
                            }
                        } else if (atk.GetStateFrame() >= 90) {
                            fightActive = false;
                            roundOver = true;
                            isGamePaused = false;
                            matchEndTimer = 0.0f;
                            ChampionshipEdition::s_vetoedKoActive = true;
                            AudioEngine::SetCriticalTempo(false);
                            AudioEngine::Instance().PlaySfx(SFX_CROWD_CHEER, 1.0f, 0.90f);
                            atk.AddRoundWin();
                            atk.SetState(FSTATE_VICTORY);
                            if (&atk == p1.get()) {
                                youWinVoiceTimer = 0.45f;
                                perfectVoiceTimer = (p1->GetHealth() >= 100) ? 1.10f : -1.0f;
                            }
                        }
                        continue;
                    }

                    if (!finishHimTriggeredThisRound) {
                        if (p2->GetHealth() <= 0 && (p1->GetRoundsWon() + 1 >= p1RoundsNeeded)) {
                            finishHimTriggeredThisRound = true;
                            finishHimTimer = 180;
                            finishHimTargetPlayer = 1;
                            ChampionshipEdition::s_finishHimActive = true;
                            p2->EnterFinishHimDizzy();
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_FINISH_HIM);
                        } else if (p1->GetHealth() <= 0 && (p2->GetRoundsWon() + 1 >= p2RoundsNeeded)) {
                            finishHimTriggeredThisRound = true;
                            finishHimTimer = 180;
                            finishHimTargetPlayer = 0;
                            ChampionshipEdition::s_finishHimActive = true;
                            p1->EnterFinishHimDizzy();
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_FINISH_HIM);
                        }
                    } else if (ChampionshipEdition::s_finishHimActive) {
                        Fighter* dizzyTarget = (finishHimTargetPlayer == 1) ? p2.get() : p1.get();
                        finishHimTimer--;
                        if (dizzyTarget->GetHealth() <= 0 || dizzyTarget->GetState() != FSTATE_FINISH_HIM_DIZZY) {
                            dizzyTarget->TriggerVetoedKO();
                            ChampionshipEdition::s_finishHimActive = false;
                        } else if (finishHimTimer <= 0) {
                            dizzyTarget->TriggerVetoedKO();
                            ChampionshipEdition::s_finishHimActive = false;
                        }
                    }

                    if (!ChampionshipEdition::s_finishHimActive &&
                        (p1->GetHealth() <= 0 || p2->GetHealth() <= 0 || roundSeconds <= 0)) {
                        fightActive = false;
                        roundOver = true;
                        isGamePaused = false;
                        matchEndTimer = 0.0f;
                        ChampionshipEdition::s_vetoedKoActive = true;
                        ChampionshipEdition::s_crowdExciteTimer = 150;
                        AudioEngine::SetCriticalTempo(false);

                        AudioEngine::Instance().PlaySfx(SFX_VOICE_VETOED);
                        AudioEngine::Instance().PlaySfx(SFX_CROWD_CHEER, 1.0f, 0.90f);

                        if (p1->GetHealth() > p2->GetHealth()) {
                            p1->AddRoundWin();
                            p1->SetState(FSTATE_VICTORY);
                            youWinVoiceTimer = 0.65f;
                            perfectVoiceTimer = (p1->GetHealth() >= 100) ? 1.30f : -1.0f;
                        } else if (p2->GetHealth() > p1->GetHealth()) {
                            p2->AddRoundWin();
                            p2->SetState(FSTATE_VICTORY);
                        }
                    }
                }
            } else if (roundOver) {
                matchEndTimer += dt;
                if (youWinVoiceTimer > 0.0f) {
                    youWinVoiceTimer -= dt;
                    if (youWinVoiceTimer <= 0.0f) {
                        AudioEngine::Instance().PlaySfx(SFX_VOICE_YOU_WIN);
                    }
                }
                if (perfectVoiceTimer > 0.0f) {
                    perfectVoiceTimer -= dt;
                    if (perfectVoiceTimer <= 0.0f) {
                        AudioEngine::Instance().PlaySfx(SFX_VOICE_PERFECT);
                    }
                }

                inputP1.Update(0, p1->IsFacingRight());
                p1->UpdatePhysics(inputP1, *p2);
                p2->UpdatePhysics(inputP2, *p1);
                ChampionshipEdition::UpdateAndTrackFighters(*p1, *p2);

                float roundOutroDuration = (p2CharId == CHAR_PALE_MOTH || p1CharId == CHAR_PALE_MOTH ||
                                            p2CharId == CHAR_W || p1CharId == CHAR_W) ? 2.5f : 2.0f;
                if (matchEndTimer >= roundOutroDuration) {
                    int p1RoundsNeeded = (p2CharId == CHAR_WAR_PIG || p2CharId == CHAR_PALM_BEACH_PETE || p2CharId == CHAR_AYATOLLAH) ? 1 : 2;
                    int p2RoundsNeeded = 2;
                    if (p1->GetRoundsWon() >= p1RoundsNeeded || p2->GetRoundsWon() >= p2RoundsNeeded) {
                        stateTimer = 0.0f;
                        if (p1->GetRoundsWon() >= p1RoundsNeeded) {
                            if (!isSecretMode) {
                                matchesWon++;
                                defeatedFighters[(int)p2CharId] = true;
                            }
                            AudioEngine::Instance().SetBgmTrack(BGM_VICTORY_SCREEN);
                            AudioEngine::Instance().PlaySfx(SFX_VICTORY_FANFARE);
                            gameState = STATE_VICTORY_SPLASH;
                        } else {
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_TOTAL_VETO);
                            if (isSecretMode) {
                                AudioEngine::Instance().SetBgmTrack(BGM_VICTORY_SCREEN);
                                gameState = STATE_VICTORY_SPLASH;
                            } else {
                                gameState = STATE_MATCH_OVER;
                            }
                        }
                    } else {
                        bool p1Died = (p1->GetHealth() <= 0);
                        bool p2Died = (p2->GetHealth() <= 0);
                        p1WasKo = p1Died;
                        p2WasKo = p2Died;

                        roundNumber++;
                        ResetFightRoundState(false);
                        p1->ResetRound(320.0f, true, p1Died);
                        p2->ResetRound(480.0f, false, p2Died);
                        cpuAI.Reset();
                        inputP2.Reset();
                        if (roundNumber == 2) {
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_ROUND2);
                        } else if (p1->GetRoundsWon() > 0 && p2->GetRoundsWon() > 0) {
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_FINAL_ROUND);
                        } else {
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_ROUND3);
                        }
                    }
                }
            }

            for (auto& sp : sparks) {
                if (!sp.alive) continue;
                sp.frame++;
                if (sp.frame >= sp.maxFrames) sp.alive = false;
            }
            sparks.erase(std::remove_if(sparks.begin(), sparks.end(), [](const SparkEffect& s) { return !s.alive; }), sparks.end());

            float midX = (p1->GetPosition().x + p2->GetPosition().x) * 0.5f;
            float targetCamX = std::clamp(midX, STAGE_MIN_X + SCREEN_WIDTH * 0.5f, STAGE_MAX_X - SCREEN_WIDTH * 0.5f);
            cameraX += (targetCamX - cameraX) * 0.12f;

            if (cameraShake > 0.01f) cameraShake *= 0.88f;
            else cameraShake = 0.0f;
            break;
        }

        case STATE_VICTORY_SPLASH: {
            inputP1.Update(0, true);
            bool padConfirm = false;
            for (int pad = 0; pad < 4; pad++) {
                if (!IsGamepadAvailable(pad)) continue;
                if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_FACE_UP) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_LEFT_TRIGGER_1) ||
                    IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_RIGHT_TRIGGER_1)) {
                    padConfirm = true;
                    break;
                }
            }
            bool wantAdvance = (stateTimer >= 6.5f) ||
                (stateTimer >= 0.40f && (padConfirm ||
                 enterPressed || InputManager::WasKeyPressed(KEY_SPACE) ||
                 inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                 inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                 inputP1.IsButtonPressed(BTN_HK) || inputP1.IsButtonPressed(BTN_SPECIAL) ||
                 inputP1.IsButtonPressed(BTN_TAUNT)));
            if (wantAdvance) {
                if (isSecretMode) {
                    gameState = STATE_SECRET_SELECT;
                    secretStep = 0;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(BGM_CHAR_SELECT);
                    break;
                }

                if (matchesWon == 3 && !bonusStagesPlayed[BONUS_STAGE_TESLA]) {
                    bonusStagesPlayed[BONUS_STAGE_TESLA] = true;
                    StartBonusStage(BONUS_STAGE_TESLA, false);
                } else if (matchesWon == 6 && !bonusStagesPlayed[BONUS_STAGE_POOL_TAR]) {
                    bonusStagesPlayed[BONUS_STAGE_POOL_TAR] = true;
                    StartBonusStage(BONUS_STAGE_POOL_TAR, false);
                } else if (matchesWon == 9 && !bonusStagesPlayed[BONUS_STAGE_BOOK_FIRE]) {
                    bonusStagesPlayed[BONUS_STAGE_BOOK_FIRE] = true;
                    StartBonusStage(BONUS_STAGE_BOOK_FIRE, false);
                } else if (matchesWon == 12 && !bonusStagesPlayed[BONUS_STAGE_BERLIN_WALL]) {
                    bonusStagesPlayed[BONUS_STAGE_BERLIN_WALL] = true;
                    StartBonusStage(BONUS_STAGE_BERLIN_WALL, false);
                } else if (defeatedFighters[CHAR_PALM_BEACH_PETE]) {

                    worldActIndex = GetCurrentWorldAct();
                    if (worldActIndex >= 5) {
                        if (p1CharId == CHAR_COMMANDER) {
                            gameState = STATE_CTHULHU_ENDING;
                            stateTimer = 0.0f;
                            AudioEngine::Instance().StopBgm();
                            AudioEngine::Instance().PlaySfx(SFX_CTHULHU_ROAR);
                        } else {
                            gameState = STATE_ENDING_CELEBRATION;
                            stateTimer = 0.0f;
                            AudioEngine::Instance().StopBgm();
                            AudioEngine::Instance().PlaySfx(SFX_JESUS_FANFARE);
                        }
                    } else {
                        gameState = STATE_WORLD_MAP;
                        stateTimer = 0.0f;
                        AudioEngine::Instance().SetBgmTrack(BGM_WORLD_CONFLICT_MAP);
                        AudioEngine::Instance().PlaySfx(SFX_VOICE_GLOBAL_CONFLICT);
                    }
                } else {
                    opponentLocked = false;
                    ChampionshipEdition::s_opponentLocked = false;
                    int curSlot = GetPlayableSlotForChar((int)p2CharId);
                    p2CharId = PickNextAvailableOpponent(curSlot + 1);
                    currentStage = GetStageForOpponent(p2CharId);
                    stageMgr.SetStage(currentStage);
                    gameState = STATE_SELECT_MAP;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(defeatedFighters[CHAR_WAR_PIG] ? BGM_US_MAP : BGM_CHAR_SELECT);
                }
            }
            break;
        }

        case STATE_BONUS_CAR: {
            bool useFreeDir = (bonusState.type == BONUS_STAGE_POOL_TAR || bonusState.type == BONUS_STAGE_BOOK_FIRE);
            inputP1.Update(0, useFreeDir ? bonusFreeFacingRight : p1->IsFacingRight());

            bool pausePressed = false;
            if (enterPressed || InputManager::WasKeyPressed(KEY_P)) {
                pausePressed = true;
            }
            for (int pad = 0; pad < 2; pad++) {
                if (IsGamepadAvailable(pad) && IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    pausePressed = true;
                }
            }

            if (pausePressed && !bonusState.cleared && bonusState.timer > 0.0f) {
                isGamePaused = !isGamePaused;
                AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
            }

            if (isGamePaused) {
                break;
            }

            if (StepBonusStage(std::min(dt, 0.05f))) {
                if (defeatedFighters[CHAR_PALM_BEACH_PETE] && !bonusFromEasterEgg) {
                    worldActIndex = GetCurrentWorldAct();
                    gameState = STATE_WORLD_MAP;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(BGM_WORLD_CONFLICT_MAP);
                } else {
                    opponentLocked = false;
                    ChampionshipEdition::s_opponentLocked = false;
                    if (!bonusFromEasterEgg) {
                        int curSlot = GetPlayableSlotForChar((int)p2CharId);
                        p2CharId = PickNextAvailableOpponent(curSlot + 1);
                        currentStage = GetStageForOpponent(p2CharId);
                        stageMgr.SetStage(currentStage);
                    }
                    bonusFromEasterEgg = false;
                    gameState = STATE_SELECT_MAP;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(defeatedFighters[CHAR_WAR_PIG] ? BGM_US_MAP : BGM_CHAR_SELECT);
                }
            }
            sparks.erase(std::remove_if(sparks.begin(), sparks.end(), [](const SparkEffect& s) { return !s.alive; }), sparks.end());
            break;
        }

        case STATE_MATCH_OVER: {
            inputP1.Update(0, true);
            ChampionshipEdition::s_continuesRemaining = continuesRemaining;
            ChampionshipEdition::s_isSecretMatch = isSecretMode;

            bool actionPressed = (enterPressed || InputManager::WasKeyPressed(KEY_SPACE) ||
                inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_COIN) ||
                inputP1.IsButtonPressed(BTN_LP) || inputP1.IsButtonPressed(BTN_HP) ||
                inputP1.IsButtonPressed(BTN_LK) || inputP1.IsButtonPressed(BTN_HK) ||
                inputP1.IsButtonPressed(BTN_SPECIAL));

            if (isSecretMode) {
                if (stateTimer >= 5.0f || actionPressed) {
                    gameState = STATE_SECRET_SELECT;
                    secretStep = 0;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(BGM_CHAR_SELECT);
                }
                break;
            }

            if (continuesRemaining > 0) {
                if (actionPressed && stateTimer >= 0.35f) {
                    continuesRemaining--;
                    ChampionshipEdition::s_continuesRemaining = continuesRemaining;
                    AudioEngine::Instance().PlaySfx(SFX_COIN);
                    AudioEngine::Instance().PlaySfx(SFX_MATCH_START_FANFARE);

                    cpuAI.Reset();
                    inputP2.Reset();
                    p1 = std::make_unique<Fighter>(0, p1CharId, 320.0f);
                    p2 = std::make_unique<Fighter>(1, p2CharId, 480.0f);
                    VisualsEngine::PrefetchMatchFighters((int)p1CharId, (int)p2CharId);
                    AudioEngine::Instance().StopBgm();

                    gameState = STATE_VERSUS;
                    stateTimer = 0.0f;
                    break;
                }
                if (stateTimer >= 10.0f) {
                    continuesRemaining = 2;
                    ChampionshipEdition::s_continuesRemaining = 2;
                    matchesWon = 0;
                    for (int b = 0; b < BONUS_STAGE_COUNT; b++) bonusStagesPlayed[b] = false;
                    playerLocked = false;
                    opponentLocked = false;
                    ChampionshipEdition::s_opponentLocked = false;
                    choosingOpponent = false;
                    for (int i = 0; i < CHARACTER_COUNT; i++) defeatedFighters[i] = false;

                    p1CharId = CHAR_COMMANDER;
                    p2CharId = PickNextAvailableOpponent(1);
                    currentStage = GetStageForOpponent(p2CharId);
                    stageMgr.SetStage(currentStage);
                    gameState = STATE_SELECT_MAP;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(BGM_CHAR_SELECT);
                }
            } else {
                if (stateTimer >= 5.0f || (actionPressed && stateTimer >= 0.35f)) {
                    continuesRemaining = 2;
                    ChampionshipEdition::s_continuesRemaining = 2;
                    matchesWon = 0;
                    for (int b = 0; b < BONUS_STAGE_COUNT; b++) bonusStagesPlayed[b] = false;
                    playerLocked = false;
                    opponentLocked = false;
                    ChampionshipEdition::s_opponentLocked = false;
                    choosingOpponent = false;
                    for (int i = 0; i < CHARACTER_COUNT; i++) defeatedFighters[i] = false;

                    p1CharId = CHAR_COMMANDER;
                    p2CharId = PickNextAvailableOpponent(1);
                    currentStage = GetStageForOpponent(p2CharId);
                    stageMgr.SetStage(currentStage);
                    gameState = STATE_SELECT_MAP;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(BGM_CHAR_SELECT);
                }
            }
            break;
        }

        case STATE_ENDING_CELEBRATION: {
            inputP1.Update(0, true);
            if (stateTimer >= 2.0f && (enterPressed || InputManager::WasKeyPressed(KEY_SPACE) ||
                inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                inputP1.IsButtonPressed(BTN_HK) || inputP1.IsButtonPressed(BTN_SPECIAL))) {

                matchesWon = 0;
                for (int b = 0; b < BONUS_STAGE_COUNT; b++) bonusStagesPlayed[b] = false;
                playerLocked = false;
                opponentLocked = false;
                ChampionshipEdition::s_opponentLocked = false;
                choosingOpponent = false;
                for (int i = 0; i < CHARACTER_COUNT; i++) defeatedFighters[i] = false;

                p1CharId = CHAR_COMMANDER;
                p2CharId = PickNextAvailableOpponent(1);
                currentStage = homeStages[(int)p2CharId];
                stageMgr.SetStage(currentStage);
                gameState = STATE_SPLASH;
                stateTimer = 0.0f;
                logoSlammed = false;
                AudioEngine::Instance().SetBgmTrack(BGM_MENU_SPLASH);
            }
            break;
        }

        case STATE_CTHULHU_ENDING: {
            static bool sharkSfxPlayed = false;
            static bool snoreSfxPlayed = false;
            if (stateTimer < 0.1f) {
                sharkSfxPlayed = false;
                snoreSfxPlayed = false;
            }
            if (stateTimer >= 5.0f && !sharkSfxPlayed) {
                sharkSfxPlayed = true;
                AudioEngine::Instance().PlaySfx(SFX_SHARK_CHOMP);
            }
            if (stateTimer >= 10.0f && !snoreSfxPlayed) {
                snoreSfxPlayed = true;
                AudioEngine::Instance().PlaySfx(SFX_CTHULHU_SNORE);
            }

            inputP1.Update(0, true);
            bool skip = (stateTimer >= 12.0f && (enterPressed || InputManager::WasKeyPressed(KEY_SPACE) ||
                         inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                         inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                         inputP1.IsButtonPressed(BTN_HK) || inputP1.IsButtonPressed(BTN_SPECIAL)));
            if (stateTimer >= 18.0f || skip) {
                matchesWon = 0;
                for (int b = 0; b < BONUS_STAGE_COUNT; b++) bonusStagesPlayed[b] = false;
                playerLocked = false;
                opponentLocked = false;
                ChampionshipEdition::s_opponentLocked = false;
                choosingOpponent = false;
                for (int i = 0; i < CHARACTER_COUNT; i++) defeatedFighters[i] = false;

                p1CharId = CHAR_COMMANDER;
                p2CharId = PickNextAvailableOpponent(1);
                currentStage = homeStages[(int)p2CharId];
                stageMgr.SetStage(currentStage);
                gameState = STATE_SPLASH;
                stateTimer = 0.0f;
                logoSlammed = false;
                AudioEngine::Instance().SetBgmTrack(BGM_MENU_SPLASH);
            }
            break;
        }
        }

        BeginTextureMode(screenTarget);
        ClearBackground(BLACK);
        rlPushMatrix();
        rlScalef((float)RENDER_SCALE, (float)RENDER_SCALE, 1.0f);

        float camRenderX = cameraX - SCREEN_WIDTH * 0.5f;
        if (cameraShake > 0.1f) {
            camRenderX += ((float)(rand() % 100) / 100.0f - 0.5f) * cameraShake * 2.0f;
        }
        ChampionshipEdition::s_camRenderX = camRenderX;

        switch (gameState) {
        case STATE_SPLASH: {
            if (flagShader.id > 0 && flagTimeLoc >= 0) SetShaderValue(flagShader, flagTimeLoc, &stateTimer, SHADER_UNIFORM_FLOAT);
            VisualsEngine::DrawCinematicIntro(stateTimer, logoSlammed, flagShader, flagTexture);
            break;
        }
        case STATE_SELECT_MAP: {
            VisualsEngine::DrawStageAndCharSelect((int)currentStage, (int)p1CharId, (int)p2CharId, stateTimer, defeatedFighters, playerLocked, (playerLocked || CountDefeatedOriginals() >= (PLAYABLE_CHARACTER_COUNT - 1)));
            break;
        }
        case STATE_WORLD_MAP: {
            VisualsEngine::DrawWorldConflictMap(worldActIndex, (int)p1CharId, stateTimer, defeatedFighters);
            break;
        }
        case STATE_SECRET_SELECT: {
            VisualsEngine::DrawSecretSelectScreen(secretStep, secretP1, secretP2, secretStage, secretDifficulty, stateTimer);
            break;
        }
        case STATE_VERSUS: {
            VisualsEngine::DrawVersusScreen(*p1, *p2, stateTimer);
            break;
        }
        case STATE_VICTORY_SPLASH: {
            int p1RoundsToWin = (p2CharId == CHAR_WAR_PIG || p2CharId == CHAR_PALM_BEACH_PETE || p2CharId == CHAR_AYATOLLAH) ? 1 : 2;
            int winnerChar = (p1 && p1->GetRoundsWon() >= p1RoundsToWin) ? (int)p1CharId : (int)p2CharId;
            int loserChar = (winnerChar == (int)p1CharId) ? (int)p2CharId : (int)p1CharId;
            VisualsEngine::DrawVictorySplash(winnerChar, loserChar, stateTimer);
            break;
        }
        case STATE_BONUS_CAR: {
            VisualsEngine::DrawBonusStage(bonusState, *p1, sparks);
            if (isGamePaused) {
                VisualsEngine::DrawPauseOverlay(stateTimer);
            }
            break;
        }
        case STATE_FIGHT: {
            stageMgr.DrawFarLayer(camRenderX, stateTimer);
            stageMgr.DrawMidLayer(camRenderX, stateTimer);
            stageMgr.DrawPlayfield(camRenderX, stateTimer);

            rlPushMatrix();
            rlTranslatef(-camRenderX, 0.0f, 0.0f);

            VisualsEngine::DrawFinisherBackdrop(*p1, *p2, camRenderX);

            VisualsEngine::DrawDropShadow(*p1);
            VisualsEngine::DrawDropShadow(*p2);

            VisualsEngine::DrawFighterSprite(*p1);
            VisualsEngine::DrawFighterSprite(*p2);

            VisualsEngine::DrawFinisherWorldFX(*p1, *p2);

            VisualsEngine::DrawProjectiles(*p1);
            VisualsEngine::DrawProjectiles(*p2);

            VisualsEngine::DrawSparks(sparks);
            ChampionshipEdition::DrawWorldParticlesAndDebug(*p1, *p2);

            rlPopMatrix();

            stageMgr.DrawForeground(camRenderX, stateTimer);

            VisualsEngine::DrawBattleHUD(*p1, *p2, roundSeconds, roundNumber, fightIntroTimer, p1WasKo, p2WasKo);
            ChampionshipEdition::DrawScreenHUDAndSuperFlash(*p1, *p2, inputP1);

            if (!fightActive && fightIntroTimer > 0) {
                if (fightIntroTimer > 50) {
                    if (roundNumber >= 3 && p1->GetRoundsWon() > 0 && p2->GetRoundsWon() > 0) {
                        int tw = MeasureText("FINAL ROUND", 22);
                        VisualsEngine::DrawPixelText("FINAL ROUND", (SCREEN_WIDTH - tw) / 2, (SCREEN_HEIGHT / 2) - 16, 22, Color{ 255, 230, 40, 255 });
                    } else {
                        char roundBuf[32];
                        snprintf(roundBuf, sizeof(roundBuf), "ROUND %d", roundNumber);
                        int tw = MeasureText(roundBuf, 22);
                        VisualsEngine::DrawPixelText(roundBuf, (SCREEN_WIDTH - tw) / 2, (SCREEN_HEIGHT / 2) - 16, 22, Color{ 255, 230, 40, 255 });
                    }
                } else {
                    int tw = MeasureText("FIGHT!", 28);
                    VisualsEngine::DrawPixelText("FIGHT!", (SCREEN_WIDTH - tw) / 2, (SCREEN_HEIGHT / 2) - 20, 28, Color{ 255, 50, 50, 255 });
                }
            } else if (roundOver && !ChampionshipEdition::s_vetoedKoActive) {
                int tw = MeasureText("TIME OVER!", 24);
                VisualsEngine::DrawPixelText("TIME OVER!", (SCREEN_WIDTH - tw) / 2, (SCREEN_HEIGHT / 2) - 18, 24, Color{ 255, 210, 50, 255 });
            }

            if (isGamePaused) {
                VisualsEngine::DrawPauseOverlay(stateTimer);
            }
            break;
        }
        case STATE_MATCH_OVER: {
            stageMgr.DrawFarLayer(camRenderX, stateTimer);
            stageMgr.DrawMidLayer(camRenderX, stateTimer);
            stageMgr.DrawPlayfield(camRenderX, stateTimer);

            rlPushMatrix();
            rlTranslatef(-camRenderX, 0.0f, 0.0f);
            VisualsEngine::DrawDropShadow(*p1);
            VisualsEngine::DrawDropShadow(*p2);
            VisualsEngine::DrawFighterSprite(*p1);
            VisualsEngine::DrawFighterSprite(*p2);
            rlPopMatrix();

            int roundsToWin = (p2CharId == CHAR_WAR_PIG || p2CharId == CHAR_PALM_BEACH_PETE || p2CharId == CHAR_AYATOLLAH) ? 1 : 2;
            const Fighter& winner = (p1->GetRoundsWon() >= roundsToWin) ? *p1 : *p2;
            VisualsEngine::DrawMatchOver(winner, stateTimer);
            break;
        }
        case STATE_ENDING_CELEBRATION: {
            VisualsEngine::DrawEndingCelebration(stateTimer);
            break;
        }
        case STATE_CTHULHU_ENDING: {
            VisualsEngine::DrawCthulhuEnding(stateTimer);
            break;
        }
        }

        rlPopMatrix();
        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        int outW = GetRenderWidth();
        int outH = GetRenderHeight();
        if (outW <= 0) outW = GetScreenWidth();
        if (outH <= 0) outH = GetScreenHeight();

        rlViewport(0, 0, outW, outH);
        rlMatrixMode(RL_PROJECTION);
        rlLoadIdentity();
        rlOrtho(0.0, (double)outW, (double)outH, 0.0, 0.0, 1.0);
        rlMatrixMode(RL_MODELVIEW);
        rlLoadIdentity();

        float scale = std::min((float)outW / (float)SCREEN_WIDTH, (float)outH / (float)SCREEN_HEIGHT);
        int scaledW = std::max(1, (int)roundf((float)SCREEN_WIDTH * scale));
        int scaledH = std::max(1, (int)roundf((float)SCREEN_HEIGHT * scale));
        int offsetX = (outW - scaledW) / 2;
        int offsetY = (outH - scaledH) / 2;

        Rectangle srcRec = { 0.0f, 0.0f, (float)screenTarget.texture.width, -(float)screenTarget.texture.height };
        Rectangle dstRec = { (float)offsetX, (float)offsetY, (float)scaledW, (float)scaledH };

        if (enableCrt && crtShader.id > 0) {
            BeginShaderMode(crtShader);
            DrawTexturePro(screenTarget.texture, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            EndShaderMode();
        } else {
            DrawTexturePro(screenTarget.texture, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }

        rlDrawRenderBatchActive();

        EndDrawing();
    }

    UnloadTexture(flagTexture);
    if (flagShader.id > 0) UnloadShader(flagShader);
    if (crtShader.id > 0) UnloadShader(crtShader);
    UnloadRenderTexture(screenTarget);
    AudioEngine::Instance().Shutdown();
    VisualsEngine::Shutdown();
    stageMgr.Shutdown();
    CloseWindow();

    return 0;
}
