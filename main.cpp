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
        if (strcmp(argv[i], "--windowed") == 0) windowedMode = true;
    }

    ChangeDirectory(GetApplicationDirectory());

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Executive Order II: Total Veto - Championship Edition");
    if (!IsWindowReady()) {
        TraceLog(LOG_FATAL, "ExecutiveOrder: Failed to initialize window / graphics context.");
        return 1;
    }
    SetTargetFPS(TARGET_FPS);

    static const char* s_gamepadDb =
        "19000000010000000100000001000000,ANBERNIC-keys,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,guide:b6,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,platform:Linux,\n"
        "19000000010000000100000001010000,Anbernic RG353V,platform:Linux,a:b1,b:b0,x:b2,y:b3,back:b8,start:b9,guide:b10,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,leftstick:b11,rightstick:b12,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
        "19000000010000000100000002010000,retrogame_joypad,platform:Linux,a:b1,b:b0,x:b2,y:b3,back:b8,start:b9,guide:b10,leftshoulder:b4,rightshoulder:b5,lefttrigger:b6,righttrigger:b7,leftstick:b11,rightstick:b12,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,rightx:a2,righty:a3,\n"
        "19000000010000000200000002000000,retrogame_joypad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,dpup:b10,dpdown:b11,dpleft:b12,dpright:b13,leftx:a0,lefty:a1,\n"
        "19000000010000000100000000010000,retrogame_joypad,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,\n"
        "03000000091200000031000000010000,OpenSimHardware OSH PB Controller,platform:Linux,a:b0,b:b1,x:b2,y:b3,back:b8,start:b9,leftshoulder:b4,rightshoulder:b5,dpup:h0.1,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,leftx:a0,lefty:a1,\n"
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

    if (!windowedMode) {
        ToggleBorderlessWindowed();
    }

    AssetManager::Init();
    const char* iconPath = AssetManager::FileExists("assets/app_icon.png") ? "assets/app_icon.png" : "../assets/app_icon.png";
    if (AssetManager::FileExists(iconPath)) {
        Image appIcon = AssetManager::LoadImage(iconPath);
        if (appIcon.data != nullptr) {
            SetWindowIcon(appIcon);
            UnloadImage(appIcon);
        }
    }

    RenderTexture2D screenTarget = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    SetTextureFilter(screenTarget.texture, TEXTURE_FILTER_POINT);

    Shader crtShader = LoadShaderFromMemory(nullptr, CRT_FRAGMENT_SHADER);
    int scanlineLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "scanlineIntensity") : -1;
    int curvatureLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "curvature") : -1;
    int bloomLoc = (crtShader.id > 0) ? GetShaderLocation(crtShader, "bloom") : -1;

    float scanlineVal = 0.35f;
    float curvatureVal = 0.015f;
    float bloomVal = 0.15f;
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
    bool bonusStagePlayed = false;
    bool playerLocked = false;
    bool choosingOpponent = false;
    bool opponentLocked = false;

    static const StageId homeStages[PLAYABLE_CHARACTER_COUNT] = {
        STAGE_WHITE_HOUSE_MMA,
        STAGE_CONGRESSIONAL_FLOOR,
        STAGE_DEMOLISHED_EAST_WING,
        STAGE_REFLECTING_POOL,
        STAGE_SMITHSONIAN_AERO,
        STAGE_CONGRESSIONAL_FLOOR,
        STAGE_LINCOLN_MEMORIAL,
        STAGE_LINCOLN_MEMORIAL,
        STAGE_TOMB_UNKNOWN,
        STAGE_DEMOLISHED_EAST_WING
    };

    auto CountDefeatedOriginals = [&]() -> int {
        int count = 0;
        for (int i = 0; i < PLAYABLE_CHARACTER_COUNT; i++) {
            if (i != (int)p1CharId && defeatedFighters[i]) {
                count++;
            }
        }
        return count;
    };

    auto GetStageForOpponent = [&](CharacterId opp) -> StageId {
        if (opp == CHAR_SUCKRATERY_RKF) return STAGE_PRESS_BRIEFING;
        if (opp == CHAR_THE_PUNDIT) return STAGE_ENN_NEWS_STUDIO;
        if (opp == CHAR_WAR_PIG) return STAGE_PENTAGON_ALTAR;
        if (opp == CHAR_SOCIALIST) return STAGE_WTC_PLAZA;
        if (opp == CHAR_A_NEW_HOPE) return STAGE_CHICAGO_WRIGLEY;
        if (opp == CHAR_BILLARY) return STAGE_COMET_PING_PONG;
        if (opp == CHAR_PALM_BEACH_PETE) return STAGE_FLORIDA_KEYS;
        if ((int)opp >= 0 && (int)opp < PLAYABLE_CHARACTER_COUNT) return homeStages[(int)opp];
        return STAGE_DEMOLISHED_EAST_WING;
    };

    auto PickNextAvailableOpponent = [&](int startIdx) -> CharacterId {
        int origDefeated = CountDefeatedOriginals();
        if (defeatedFighters[CHAR_WAR_PIG]) {
            if (!defeatedFighters[CHAR_SOCIALIST]) return CHAR_SOCIALIST;
            if (!defeatedFighters[CHAR_A_NEW_HOPE]) return CHAR_A_NEW_HOPE;
            if (!defeatedFighters[CHAR_BILLARY]) return CHAR_BILLARY;
            if (!defeatedFighters[CHAR_PALM_BEACH_PETE]) return CHAR_PALM_BEACH_PETE;
            return CHAR_PALM_BEACH_PETE;
        }
        if (origDefeated >= 9) {
            if (!defeatedFighters[CHAR_SUCKRATERY_RKF]) return CHAR_SUCKRATERY_RKF;
            if (!defeatedFighters[CHAR_THE_PUNDIT]) return CHAR_THE_PUNDIT;
            if (!defeatedFighters[CHAR_WAR_PIG]) return CHAR_WAR_PIG;
            return CHAR_WAR_PIG;
        }
        for (int step = 0; step < PLAYABLE_CHARACTER_COUNT; step++) {
            int idx = (startIdx + step) % PLAYABLE_CHARACTER_COUNT;
            if (idx != (int)p1CharId && !defeatedFighters[idx]) {
                return (CharacterId)idx;
            }
        }
        return CHAR_COASTAL_GOV;
    };

    const float BONUS_CAR_MAX_HP = 1200.0f;
    float bonusCarHp = BONUS_CAR_MAX_HP;
    float bonusTimer = 30.0f;
    bool bonusCarAlarmPlayed = false;
    int bonusLastStage = 0;
    float bonusPostTimer = 0.0f;
    bool bonusBooYaaPlayed = false;

    p2CharId = PickNextAvailableOpponent(1);
    currentStage = GetStageForOpponent(p2CharId);
    stageMgr.SetStage(currentStage);

    std::unique_ptr<Fighter> p1 = std::make_unique<Fighter>(0, p1CharId, 320.0f);
    std::unique_ptr<Fighter> p2 = std::make_unique<Fighter>(1, p2CharId, 480.0f);

    std::vector<SparkEffect> sparks;

    GameState gameState = STATE_SPLASH;
    float stateTimer = 0.0f;
    bool logoSlammed = false;
    bool enableCrt = true;

    int roundNumber = 1;
    int roundSeconds = 99;
    float roundTickAccum = 0.0f;
    int fightIntroTimer = 100;
    bool fightActive = false;
    bool roundOver = false;
    float matchEndTimer = 0.0f;
    bool p1WasKo = false;
    bool p2WasKo = false;
    bool bonusFromEasterEgg = false;
    bool isGamePaused = false;

    bool isSecretMode = false;
    int secretStep = 0;
    int secretP1 = 0;
    int secretP2 = 1;
    int secretStage = 0;
    int secretDifficulty = (int)AI_DIFF_NORMAL;

    float cameraX = 400.0f;
    float cameraShake = 0.0f;

    while (!WindowShouldClose() && !s_quitRequested) {
        float dt = GetFrameTime();
        if (IsKeyPressed(KEY_F2)) enableCrt = !enableCrt;
        stateTimer += dt;
        AudioEngine::Instance().Update();

            bool killRequested = WindowShouldClose() || (s_quitRequested != 0);

            if (IsKeyPressed(KEY_ESCAPE) || IsKeyDown(KEY_ESCAPE) ||
                IsKeyPressed(KEY_BACKSPACE) || IsKeyDown(KEY_BACKSPACE) ||
                IsKeyPressed(KEY_TAB) || IsKeyDown(KEY_TAB)) {
                killRequested = true;
            }

            for (int pad = 0; pad < 4; pad++) {
                if (IsGamepadAvailable(pad)) {
                    if (IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_LEFT) ||
                        IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE_LEFT) ||
                        IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE) ||
                        IsGamepadButtonDown(pad, GAMEPAD_BUTTON_MIDDLE)) {
                        killRequested = true;
                        break;
                    }

                    const char* padName = GetGamepadName(pad);
                    if (padName) {
                        std::string nameLower = padName;
                        for (char& c : nameLower) c = (char)tolower((unsigned char)c);
                        if (nameLower.find("anbernic") != std::string::npos ||
                            nameLower.find("retrogame") != std::string::npos ||
                            nameLower.find("opensim") != std::string::npos ||
                            nameLower.find("go-advance") != std::string::npos) {
                            if (IsGamepadButtonPressed(pad, 8) || IsGamepadButtonDown(pad, 8) ||
                                IsGamepadButtonPressed(pad, 10) || IsGamepadButtonDown(pad, 10)) {
                                killRequested = true;
                                break;
                            }
                        }
                    }
                }
            }

            if (inputP1.IsButtonPressed(BTN_SELECT) || inputP1.IsButtonHeld(BTN_SELECT)) {
                killRequested = true;
            }

            if (killRequested) {
                break;
            }

        if (IsKeyPressed(KEY_F11) || ((IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)) && IsKeyPressed(KEY_ENTER))) {
            ToggleBorderlessWindowed();
        }

        if (IsKeyPressed(KEY_F1)) {
            enableCrt = !enableCrt;
        }

        if (IsKeyPressed(KEY_F12)) {
            TakeScreenshot("screenshot.png");
        }

        switch (gameState) {
        case STATE_SPLASH: {
            if (!logoSlammed && stateTimer >= 0.8f) {
                logoSlammed = true;
                cameraShake = 4.0f;
                AudioEngine::Instance().PlaySfx(SFX_TITLE_SLAM);
            }

                inputP1.Update(0, true);

                if (inputP1.IsSecretMenuComboPressed()) {
                    AudioEngine::Instance().PlaySfx(SFX_COIN);
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_HADOUKEN, 0.8f);
                    gameState = STATE_SECRET_SELECT;
                    isSecretMode = true;
                    secretStep = 0;
                    secretP1 = (int)p1CharId;
                    secretP2 = (int)p2CharId;
                    stateTimer = 0.0f;
                    break;
                }

                bool startPressed = false;
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ONE) ||
                    IsKeyPressed(KEY_U) || IsKeyPressed(KEY_I) || IsKeyPressed(KEY_C) || IsKeyPressed(KEY_V)) {
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
                if (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_COIN) ||
                    inputP1.IsButtonPressed(BTN_LP) || inputP1.IsButtonPressed(BTN_HP) ||
                    inputP1.IsButtonPressed(BTN_LK) || inputP1.IsButtonPressed(BTN_HK)) {
                    startPressed = true;
                }

                if (startPressed) {
                    AudioEngine::Instance().PlaySfx(SFX_COIN);
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

            if (defeatedFighters[CHAR_WAR_PIG]) {
                opponentLocked = true;
                ChampionshipEdition::s_opponentLocked = true;
                if (!defeatedFighters[CHAR_SOCIALIST]) {
                    p2CharId = CHAR_SOCIALIST;
                    currentStage = STAGE_WTC_PLAZA;
                } else if (!defeatedFighters[CHAR_A_NEW_HOPE]) {
                    p2CharId = CHAR_A_NEW_HOPE;
                    currentStage = STAGE_CHICAGO_WRIGLEY;
                } else if (!defeatedFighters[CHAR_BILLARY]) {
                    p2CharId = CHAR_BILLARY;
                    currentStage = STAGE_COMET_PING_PONG;
                } else if (!defeatedFighters[CHAR_PALM_BEACH_PETE]) {
                    p2CharId = CHAR_PALM_BEACH_PETE;
                    currentStage = STAGE_FLORIDA_KEYS;
                } else {
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
                stageMgr.SetStage(currentStage);
            } else if (origDefeated >= 9) {
                opponentLocked = true;
                ChampionshipEdition::s_opponentLocked = true;
                if (!defeatedFighters[CHAR_SUCKRATERY_RKF]) {
                    p2CharId = CHAR_SUCKRATERY_RKF;
                    currentStage = STAGE_PRESS_BRIEFING;
                } else if (!defeatedFighters[CHAR_THE_PUNDIT]) {
                    p2CharId = CHAR_THE_PUNDIT;
                    currentStage = STAGE_ENN_NEWS_STUDIO;
                } else if (!defeatedFighters[CHAR_WAR_PIG]) {
                    p2CharId = CHAR_WAR_PIG;
                    currentStage = STAGE_PENTAGON_ALTAR;
                } else {
                    gameState = STATE_MATCH_OVER;
                    stateTimer = 0.0f;
                    break;
                }
                stageMgr.SetStage(currentStage);
            } else if (!playerLocked) {
                int p1Col = (int)p1CharId % 5;
                int p1Row = (int)p1CharId / 5;
                bool p1Changed = false;

                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || inputP1.IsLeftPressed()) {
                    p1Col = (p1Col + 4) % 5;
                    p1Changed = true;
                }
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || inputP1.IsRightPressed()) {
                    p1Col = (p1Col + 1) % 5;
                    p1Changed = true;
                }
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || inputP1.IsUpPressed() ||
                    IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || inputP1.IsDownPressed()) {
                    p1Row = 1 - p1Row;
                    p1Changed = true;
                }

                if (p1Changed) {
                    p1CharId = (CharacterId)(p1Row * 5 + p1Col);
                    currentStage = homeStages[(int)p1CharId];
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }
            } else if (!opponentLocked) {
                bool p2Changed = false;

                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || inputP1.IsRightPressed()) {
                    int cur = (int)p2CharId;
                    for (int step = 1; step < PLAYABLE_CHARACTER_COUNT; step++) {
                        int next = (cur + step) % PLAYABLE_CHARACTER_COUNT;
                        if (next != (int)p1CharId && !defeatedFighters[next]) {
                            p2CharId = (CharacterId)next;
                            p2Changed = true;
                            break;
                        }
                    }
                }
                if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || inputP1.IsLeftPressed()) {
                    int cur = (int)p2CharId;
                    for (int step = 1; step < PLAYABLE_CHARACTER_COUNT; step++) {
                        int prev = (cur - step + PLAYABLE_CHARACTER_COUNT) % PLAYABLE_CHARACTER_COUNT;
                        if (prev != (int)p1CharId && !defeatedFighters[prev]) {
                            p2CharId = (CharacterId)prev;
                            p2Changed = true;
                            break;
                        }
                    }
                }
                if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || inputP1.IsUpPressed() ||
                    IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || inputP1.IsDownPressed()) {
                    int target = ((int)p2CharId + 5) % PLAYABLE_CHARACTER_COUNT;
                    for (int step = 0; step < PLAYABLE_CHARACTER_COUNT; step++) {
                        int cand = (target + step) % PLAYABLE_CHARACTER_COUNT;
                        if (cand != (int)p1CharId && !defeatedFighters[cand]) {
                            p2CharId = (CharacterId)cand;
                            p2Changed = true;
                            break;
                        }
                    }
                }

                if (p2Changed) {
                    currentStage = homeStages[(int)p2CharId];
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }
            } else {
                static const StageId normalStages[7] = {
                    STAGE_DEMOLISHED_EAST_WING,
                    STAGE_REFLECTING_POOL,
                    STAGE_CONGRESSIONAL_FLOOR,
                    STAGE_WHITE_HOUSE_MMA,
                    STAGE_LINCOLN_MEMORIAL,
                    STAGE_SMITHSONIAN_AERO,
                    STAGE_TOMB_UNKNOWN
                };
                int curIdx = 0;
                for (int k = 0; k < 7; k++) {
                    if (normalStages[k] == currentStage) {
                        curIdx = k;
                        break;
                    }
                }
                bool stageChanged = false;
                if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || inputP1.IsRightPressed() ||
                    IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || inputP1.IsDownPressed()) {
                    curIdx = (curIdx + 1) % 7;
                    stageChanged = true;
                } else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || inputP1.IsLeftPressed() ||
                           IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || inputP1.IsUpPressed()) {
                    curIdx = (curIdx + 6) % 7;
                    stageChanged = true;
                }
                if (stageChanged) {
                    currentStage = normalStages[curIdx];
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }
            }

            if (inputP1.IsTriggerEasterEggPressed()) {
                bonusStagePlayed = true;
                bonusFromEasterEgg = true;
                gameState = STATE_BONUS_CAR;
                stateTimer = 0.0f;
                bonusCarHp = BONUS_CAR_MAX_HP;
                bonusTimer = 30.0f;
                bonusCarAlarmPlayed = false;
                bonusLastStage = 0;
                bonusPostTimer = 0.0f;
                bonusBooYaaPlayed = false;
                p1 = std::make_unique<Fighter>(0, p1CharId, 100.0f);

                AudioEngine::Instance().StopBgm();
                AudioEngine::Instance().SetBgmTrack(5);
                AudioEngine::Instance().PlaySfx(SFX_TITLE_SLAM);
                AudioEngine::Instance().PlaySfx(SFX_VOICE_FIGHT);
                break;
            }

            bool canConfirm = (stateTimer >= 0.25f);

            if (canConfirm && (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))) {

                if (!playerLocked) {
                    playerLocked = true;
                    opponentLocked = false;
                    ChampionshipEdition::s_opponentLocked = false;
                    choosingOpponent = true;
                    p2CharId = PickNextAvailableOpponent((int)p1CharId + 1);
                    currentStage = GetStageForOpponent(p2CharId);
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                    stateTimer = 0.0f;
                } else if (!opponentLocked && !defeatedFighters[CHAR_WAR_PIG] && origDefeated < 9) {
                    opponentLocked = true;
                    ChampionshipEdition::s_opponentLocked = true;
                    currentStage = homeStages[(int)p2CharId];
                    stageMgr.SetStage(currentStage);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                    stateTimer = 0.0f;
                } else {
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);

                    if (defeatedFighters[CHAR_WAR_PIG] || origDefeated >= 9) {
                        currentStage = GetStageForOpponent(p2CharId);
                    }
                    stageMgr.SetStage(currentStage);
                    opponentLocked = false;
                    ChampionshipEdition::s_opponentLocked = false;

                    cpuAI.SetDifficulty((p2CharId == CHAR_WAR_PIG || p2CharId == CHAR_PALM_BEACH_PETE) ? AI_DIFF_HARD : AI_DIFF_NORMAL);
                    cpuAI.Reset();
                    inputP2.Reset();

                    p1 = std::make_unique<Fighter>(0, p1CharId, 320.0f);
                    p2 = std::make_unique<Fighter>(1, p2CharId, 480.0f);

                    AudioEngine::Instance().StopBgm();
                    AudioEngine::Instance().PlaySfx(SFX_MATCH_START_FANFARE);

                    gameState = STATE_VERSUS;
                    stateTimer = 0.0f;
                }
            }
            break;
        }

        case STATE_SECRET_SELECT: {
            inputP1.Update(0, true);

            if (inputP1.IsButtonPressed(BTN_SELECT) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
                AudioEngine::Instance().PlaySfx(SFX_BLOCK, 1.0f);
                gameState = STATE_SPLASH;
                isSecretMode = false;
                stateTimer = 0.0f;
                break;
            }

            if (secretStep == 0) {
                if (inputP1.IsLeftPressed()) {
                    secretP1 = (secretP1 - 1 + CHARACTER_COUNT) % CHARACTER_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsRightPressed()) {
                    secretP1 = (secretP1 + 1) % CHARACTER_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsUpPressed()) {
                    if (secretP1 >= 8) secretP1 = std::min(7, secretP1 - 8);
                    else secretP1 = std::min(16, secretP1 + 8);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsDownPressed()) {
                    if (secretP1 < 8) secretP1 = std::min(16, secretP1 + 8);
                    else secretP1 = secretP1 - 8;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }

                if (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                    inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                    inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                    secretStep = 1;
                    stateTimer = 0.0f;
                }
            }

            else if (secretStep == 1) {
                if (inputP1.IsLeftPressed()) {
                    secretP2 = (secretP2 - 1 + CHARACTER_COUNT) % CHARACTER_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsRightPressed()) {
                    secretP2 = (secretP2 + 1) % CHARACTER_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsUpPressed()) {
                    if (secretP2 >= 8) secretP2 = std::min(7, secretP2 - 8);
                    else secretP2 = std::min(16, secretP2 + 8);
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsDownPressed()) {
                    if (secretP2 < 8) secretP2 = std::min(16, secretP2 + 8);
                    else secretP2 = secretP2 - 8;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }

                if (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                    inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                    inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                    secretStep = 2;
                    stateTimer = 0.0f;
                }
            }

            else if (secretStep == 2) {
                if (inputP1.IsLeftPressed()) {
                    secretStage = (secretStage - 1 + STAGE_COUNT) % STAGE_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsRightPressed()) {
                    secretStage = (secretStage + 1) % STAGE_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsUpPressed()) {
                    secretStage = (secretStage - 4 + STAGE_COUNT) % STAGE_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsDownPressed()) {
                    secretStage = (secretStage + 4) % STAGE_COUNT;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }

                if (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                    inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                    inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
                    secretStep = 3;
                    stateTimer = 0.0f;
                }
            }

            else if (secretStep == 3) {
                if (inputP1.IsUpPressed()) {
                    secretDifficulty = (secretDifficulty - 1 + 3) % 3;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                } else if (inputP1.IsDownPressed()) {
                    secretDifficulty = (secretDifficulty + 1) % 3;
                    AudioEngine::Instance().PlaySfx(SFX_SELECT_CURSOR);
                }

                if (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                    inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                    inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                    AudioEngine::Instance().PlaySfx(SFX_MATCH_START_FANFARE);

                    p1CharId = (CharacterId)secretP1;
                    p2CharId = (CharacterId)secretP2;
                    currentStage = (StageId)secretStage;
                    stageMgr.SetStage(currentStage);

                    cpuAI.SetDifficulty((AIDifficulty)secretDifficulty);
                    cpuAI.Reset();
                    inputP2.Reset();

                    p1 = std::make_unique<Fighter>(0, p1CharId, 320.0f);
                    p2 = std::make_unique<Fighter>(1, p2CharId, 480.0f);

                    roundNumber = 1;
                    roundSeconds = 99;
                    fightIntroTimer = 110;
                    fightActive = false;
                    roundOver = false;
                    matchEndTimer = 0.0f;
                    p1WasKo = false;
                    p2WasKo = false;

                    gameState = STATE_FIGHT;
                    stateTimer = 0.0f;
                    int bgmTrack = (currentStage == STAGE_LINCOLN_MEMORIAL || currentStage == STAGE_CHICAGO_WRIGLEY) ? 2 : ((currentStage == STAGE_SMITHSONIAN_AERO) ? 1 : ((currentStage == STAGE_TOMB_UNKNOWN || currentStage == STAGE_ENN_NEWS_STUDIO) ? 3 : ((currentStage >= 4) ? ((currentStage == STAGE_PRESS_BRIEFING) ? 3 : 4) : ((int)currentStage + 1))));
                    AudioEngine::Instance().SetBgmTrack(bgmTrack);
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_ROUND1);
                }
            }
            break;
        }

        case STATE_VERSUS: {

            if (stateTimer >= 1.8f) {
                gameState = STATE_FIGHT;
                stateTimer = 0.0f;
                fightIntroTimer = 110;
                fightActive = false;
                roundOver = false;
                roundSeconds = 99;
                roundNumber = 1;
                p1WasKo = false;
                p2WasKo = false;
                ChampionshipEdition::ResetRoundFX();
                p1->ResetRound(320.0f, true);
                p2->ResetRound(480.0f, false);
                cpuAI.Reset();
                inputP2.Reset();
                int bgmTrack = (currentStage == STAGE_LINCOLN_MEMORIAL || currentStage == STAGE_CHICAGO_WRIGLEY) ? 2 : ((currentStage == STAGE_SMITHSONIAN_AERO) ? 1 : ((currentStage == STAGE_TOMB_UNKNOWN || currentStage == STAGE_ENN_NEWS_STUDIO) ? 3 : ((currentStage >= 4) ? ((currentStage == STAGE_PRESS_BRIEFING) ? 3 : 4) : ((int)currentStage + 1))));
                AudioEngine::Instance().SetBgmTrack(bgmTrack);
                AudioEngine::Instance().PlaySfx(SFX_VOICE_ROUND1);
            }
            break;
        }

        case STATE_FIGHT: {

            bool pausePressed = false;
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_P) || inputP1.IsButtonPressed(BTN_START)) {
                pausePressed = true;
            }
            for (int pad = 0; pad < 2; pad++) {
                if (IsGamepadAvailable(pad) && IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    pausePressed = true;
                }
            }

            if (pausePressed && fightIntroTimer <= 0 && !roundOver) {
                isGamePaused = !isGamePaused;
                AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
            }

            if (isGamePaused) {
                break;
            }

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

            inputP1.Update(0, p1->IsFacingRight());
            cpuAI.Update(inputP2, *p2, *p1, dt);

            if (fightActive) {

                roundTickAccum += dt;
                if (roundTickAccum >= 1.0f) {
                    roundTickAccum -= 1.0f;
                    if (roundSeconds > 0) roundSeconds--;
                }

                if (ChampionshipEdition::s_superFlashTimer > 0) {
                    ChampionshipEdition::s_superFlashTimer--;
                } else {
                    p1->UpdatePhysics(inputP1, *p2);
                    p2->UpdatePhysics(inputP2, *p1);

                    CollisionSystem::ResolvePushboxes(*p1, *p2);
                    CollisionSystem::ResolveCombat(*p1, *p2, sparks, cameraShake);
                }
                ChampionshipEdition::UpdateAndTrackFighters(*p1, *p2);

                if (p1->GetHealth() <= 0 || p2->GetHealth() <= 0 || roundSeconds <= 0) {
                    fightActive = false;
                    roundOver = true;
                    isGamePaused = false;
                    matchEndTimer = 0.0f;

                    AudioEngine::Instance().PlaySfx(SFX_VOICE_BOO_YAA);

                    if (p1->GetHealth() > p2->GetHealth()) {
                        p1->AddRoundWin();
                    } else if (p2->GetHealth() > p1->GetHealth()) {
                        p2->AddRoundWin();
                    }
                }
            } else if (roundOver) {

                matchEndTimer += dt;
                p1->UpdatePhysics(inputP1, *p2);
                p2->UpdatePhysics(inputP2, *p1);
                ChampionshipEdition::UpdateAndTrackFighters(*p1, *p2);

                if (matchEndTimer >= 1.5f) {

                    int p1RoundsNeeded = (p2CharId == CHAR_WAR_PIG || p2CharId == CHAR_PALM_BEACH_PETE) ? 1 : 2;
                    int p2RoundsNeeded = 2;
                    if (p1->GetRoundsWon() >= p1RoundsNeeded || p2->GetRoundsWon() >= p2RoundsNeeded) {
                        stateTimer = 0.0f;
                        if (p1->GetRoundsWon() >= p1RoundsNeeded) {
                            if (!isSecretMode) {
                                matchesWon++;
                                defeatedFighters[(int)p2CharId] = true;
                            }
                            AudioEngine::Instance().StopBgm();
                            AudioEngine::Instance().PlaySfx(SFX_VICTORY_FANFARE);
                            gameState = STATE_VICTORY_SPLASH;
                        } else {
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_TOTAL_VETO);
                            if (isSecretMode) {
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
                        roundSeconds = 99;
                        fightIntroTimer = 110;
                        fightActive = false;
                        roundOver = false;
                        ChampionshipEdition::ResetRoundFX();
                        p1->ResetRound(320.0f, true, p1Died);
                        p2->ResetRound(480.0f, false, p2Died);
                        cpuAI.Reset();
                        inputP2.Reset();
                        if (roundNumber == 2) {
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_ROUND2);
                        } else {
                            AudioEngine::Instance().PlaySfx(SFX_VOICE_FINAL_ROUND);
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
            if (stateTimer >= 6.0f || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
                inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                inputP1.IsButtonPressed(BTN_HK)) {

                if (isSecretMode) {
                    gameState = STATE_SECRET_SELECT;
                    secretStep = 0;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(0);
                    break;
                }

                if (matchesWon == 3 && !bonusStagePlayed) {
                    bonusStagePlayed = true;
                    gameState = STATE_BONUS_CAR;
                    stateTimer = 0.0f;
                    bonusCarHp = BONUS_CAR_MAX_HP;
                    bonusTimer = 30.0f;
                    bonusCarAlarmPlayed = false;
                    bonusLastStage = 0;
                    bonusPostTimer = 0.0f;
                    p1->ResetRound(100.0f, true);
                    AudioEngine::Instance().SetBgmTrack(5);
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_FIGHT);
                } else if (defeatedFighters[CHAR_PALM_BEACH_PETE]) {

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
                    opponentLocked = false;
                    ChampionshipEdition::s_opponentLocked = false;
                    p2CharId = PickNextAvailableOpponent((int)p2CharId + 1);
                    currentStage = GetStageForOpponent(p2CharId);
                    stageMgr.SetStage(currentStage);
                    gameState = STATE_SELECT_MAP;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(0);
                }
            }
            break;
        }

        case STATE_BONUS_CAR: {
            inputP1.Update(0, p1->IsFacingRight());

            bool pausePressed = false;
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_P) || inputP1.IsButtonPressed(BTN_START)) {
                pausePressed = true;
            }
            for (int pad = 0; pad < 2; pad++) {
                if (IsGamepadAvailable(pad) && IsGamepadButtonPressed(pad, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
                    pausePressed = true;
                }
            }

            if (pausePressed && bonusCarHp > 0.0f && bonusTimer > 0.0f) {
                isGamePaused = !isGamePaused;
                AudioEngine::Instance().PlaySfx(SFX_SELECT_CONFIRM);
            }

            if (isGamePaused) {
                break;
            }

            if (bonusCarHp > 0.0f && bonusTimer > 0.0f) {
                bonusTimer = std::max(0.0f, bonusTimer - dt);

                Rectangle carHurtbox = { (float)SCREEN_WIDTH / 2.0f - 77.0f, GROUND_Y - 60.0f, 154.0f, 60.0f };

                Fighter carDummy(1, CHAR_TECH_VISIONARY, SCREEN_WIDTH / 2.0f);
                p1->UpdatePhysics(inputP1, carDummy);

                Rectangle hitbox{};
                AttackData atkData{};
                if (p1->GetActiveHitbox(hitbox, atkData) && !p1->HasHitConnected()) {
                    if (CheckCollisionRecs(hitbox, carHurtbox)) {
                        p1->SetHitConnected(true);
                        p1->SetHitstop(atkData.hitstopFrames);
                        cameraShake = atkData.isHeavy ? 5.0f : 2.5f;

                        float dmg = (float)atkData.damage;
                        bonusCarHp = std::max(0.0f, bonusCarHp - dmg);

                        SparkEffect sp{};
                        sp.pos = Vector2{ (hitbox.x + hitbox.width * 0.5f), (hitbox.y + hitbox.height * 0.5f) };
                        sp.isBlock = false;
                        sp.alive = true;
                        sp.frame = 0;
                        sp.maxFrames = 8;
                        sparks.push_back(sp);

                        AudioEngine::Instance().PlaySfx(atkData.isHeavy ? SFX_HIT_HEAVY : SFX_HIT_LIGHT);
                        AudioEngine::Instance().PlaySfx(SFX_METAL_CRUNCH, (float)(90 + (rand() % 20)) / 100.0f);

                        float hpRatio = bonusCarHp / BONUS_CAR_MAX_HP;
                        int curStage = (hpRatio <= 0.0f) ? 3 : ((hpRatio <= 0.25f) ? 3 : ((hpRatio <= 0.50f) ? 2 : ((hpRatio <= 0.75f) ? 1 : 0)));
                        if (curStage > bonusLastStage) {
                            bonusLastStage = curStage;
                            AudioEngine::Instance().PlaySfx(SFX_GLASS_BREAK);
                            if (!bonusCarAlarmPlayed) {
                                bonusCarAlarmPlayed = true;
                                AudioEngine::Instance().PlaySfx(SFX_CAR_ALARM);
                            }
                        }
                        if (curStage == 3 && (rand() % 3 == 0)) {
                            AudioEngine::Instance().PlaySfx(SFX_ELEC_SPARK);
                        }

                        if (bonusCarHp <= 0.0f) {
                            AudioEngine::Instance().PlaySfx(SFX_TITLE_SLAM);
                            AudioEngine::Instance().PlaySfx(SFX_FIREBALL_IMPACT);
                            cameraShake = 8.0f;
                        }
                    }
                }
            } else {

                if (!bonusBooYaaPlayed) {
                    bonusBooYaaPlayed = true;
                    AudioEngine::Instance().PlaySfx(SFX_VOICE_BOO_YAA);
                }

                bonusPostTimer += dt;
                Fighter carDummy(1, CHAR_TECH_VISIONARY, SCREEN_WIDTH / 2.0f);
                p1->UpdatePhysics(inputP1, carDummy);

                if (bonusPostTimer >= 2.0f) {
                    opponentLocked = false;
                    ChampionshipEdition::s_opponentLocked = false;
                    p2CharId = PickNextAvailableOpponent((int)p2CharId + 1);
                    currentStage = GetStageForOpponent(p2CharId);
                    stageMgr.SetStage(currentStage);
                    gameState = STATE_SELECT_MAP;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(0);
                }
            }

            for (auto& sp : sparks) {
                if (!sp.alive) continue;
                sp.frame++;
                if (sp.frame >= sp.maxFrames) sp.alive = false;
            }

            if (cameraShake > 0.01f) cameraShake *= 0.88f;
            else cameraShake = 0.0f;
            break;
        }

        case STATE_MATCH_OVER: {
            inputP1.Update(0, true);
            ChampionshipEdition::s_continuesRemaining = continuesRemaining;
            ChampionshipEdition::s_isSecretMatch = isSecretMode;

            bool actionPressed = (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) ||
                inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_COIN) ||
                inputP1.IsButtonPressed(BTN_LP) || inputP1.IsButtonPressed(BTN_HP) ||
                inputP1.IsButtonPressed(BTN_LK) || inputP1.IsButtonPressed(BTN_HK));

            if (isSecretMode) {
                if (stateTimer >= 5.0f || actionPressed) {
                    gameState = STATE_SECRET_SELECT;
                    secretStep = 0;
                    stateTimer = 0.0f;
                    AudioEngine::Instance().SetBgmTrack(0);
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
                    AudioEngine::Instance().StopBgm();

                    gameState = STATE_VERSUS;
                    stateTimer = 0.0f;
                    break;
                }
                if (stateTimer >= 10.0f) {
                    continuesRemaining = 2;
                    ChampionshipEdition::s_continuesRemaining = 2;
                    matchesWon = 0;
                    bonusStagePlayed = false;
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
                    AudioEngine::Instance().SetBgmTrack(0);
                }
            } else {
                if (stateTimer >= 5.0f || (actionPressed && stateTimer >= 0.35f)) {
                    continuesRemaining = 2;
                    ChampionshipEdition::s_continuesRemaining = 2;
                    matchesWon = 0;
                    bonusStagePlayed = false;
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
                    AudioEngine::Instance().SetBgmTrack(0);
                }
            }
            break;
        }
        case STATE_ENDING_CELEBRATION: {
            inputP1.Update(0, true);
            if (stateTimer >= 2.0f && (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))) {

                matchesWon = 0;
                bonusStagePlayed = false;
                playerLocked = false;
                choosingOpponent = false;
                for (int i = 0; i < CHARACTER_COUNT; i++) defeatedFighters[i] = false;

                p1CharId = CHAR_COMMANDER;
                p2CharId = PickNextAvailableOpponent(1);
                currentStage = homeStages[(int)p2CharId];
                stageMgr.SetStage(currentStage);
                gameState = STATE_SPLASH;
                stateTimer = 0.0f;
                logoSlammed = false;
                AudioEngine::Instance().SetBgmTrack(0);
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
                bool skip = (stateTimer >= 12.0f && (inputP1.IsButtonPressed(BTN_START) || inputP1.IsButtonPressed(BTN_LP) ||
                             inputP1.IsButtonPressed(BTN_HP) || inputP1.IsButtonPressed(BTN_LK) ||
                             inputP1.IsButtonPressed(BTN_HK) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)));
                if (stateTimer >= 18.0f || skip) {

                    matchesWon = 0;
                    bonusStagePlayed = false;
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
                    AudioEngine::Instance().SetBgmTrack(0);
                }

            break;
        }
        }

        BeginTextureMode(screenTarget);
        ClearBackground(BLACK);

        float camRenderX = cameraX - SCREEN_WIDTH * 0.5f;
        if (cameraShake > 0.1f) {
            camRenderX += ((float)(rand() % 100) / 100.0f - 0.5f) * cameraShake * 2.0f;
        }

        switch (gameState) {
        case STATE_SPLASH: {
            if (flagShader.id > 0 && flagTimeLoc >= 0) SetShaderValue(flagShader, flagTimeLoc, &stateTimer, SHADER_UNIFORM_FLOAT);
            VisualsEngine::DrawCinematicIntro(stateTimer, logoSlammed, flagShader, flagTexture);
            break;
        }
        case STATE_SELECT_MAP: {
            VisualsEngine::DrawStageAndCharSelect((int)currentStage, (int)p1CharId, (int)p2CharId, stateTimer, defeatedFighters, playerLocked, (playerLocked || CountDefeatedOriginals() >= 7));
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
            int p1RoundsToWin = (p2CharId == CHAR_WAR_PIG || p2CharId == CHAR_PALM_BEACH_PETE) ? 1 : 2;
            int winnerChar = (p1 && p1->GetRoundsWon() >= p1RoundsToWin) ? (int)p1CharId : (int)p2CharId;
            int loserChar = (winnerChar == (int)p1CharId) ? (int)p2CharId : (int)p1CharId;
            VisualsEngine::DrawVictorySplash(winnerChar, loserChar, stateTimer);
            break;
        }
        case STATE_BONUS_CAR: {
            VisualsEngine::DrawBonusStage(bonusCarHp, BONUS_CAR_MAX_HP, bonusTimer, *p1, sparks);
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

            VisualsEngine::DrawDropShadow(*p1);
            VisualsEngine::DrawDropShadow(*p2);

            VisualsEngine::DrawFighterSprite(*p1);
            VisualsEngine::DrawFighterSprite(*p2);

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
                    if (roundNumber == 3) {
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
            } else if (p1->GetHealth() <= 0 || p2->GetHealth() <= 0 || roundSeconds <= 0) {
                int tw = MeasureText("TOTAL VETO!", 26);
                VisualsEngine::DrawPixelText("TOTAL VETO!", (SCREEN_WIDTH - tw) / 2, (SCREEN_HEIGHT / 2) - 18, 26, Color{ 255, 60, 60, 255 });
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

            int roundsToWin = (p2CharId == CHAR_WAR_PIG || p2CharId == CHAR_PALM_BEACH_PETE) ? 1 : 2;
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

        EndTextureMode();

        BeginDrawing();
        ClearBackground(BLACK);

        float scale = std::min((float)GetScreenWidth() / (float)SCREEN_WIDTH, (float)GetScreenHeight() / (float)SCREEN_HEIGHT);
        int scaledW = (int)(SCREEN_WIDTH * scale);
        int scaledH = (int)(SCREEN_HEIGHT * scale);
        int offsetX = (GetScreenWidth() - scaledW) / 2;
        int offsetY = (GetScreenHeight() - scaledH) / 2;

        Rectangle srcRec = { 0.0f, 0.0f, (float)screenTarget.texture.width, -(float)screenTarget.texture.height };
        Rectangle dstRec = { (float)offsetX, (float)offsetY, (float)scaledW, (float)scaledH };

        if (enableCrt && crtShader.id > 0) {
            BeginShaderMode(crtShader);
            DrawTexturePro(screenTarget.texture, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
            EndShaderMode();
        } else {
            DrawTexturePro(screenTarget.texture, srcRec, dstRec, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
        }

        DrawText(ChampionshipEdition::s_debugHitboxes ? "[F1: BOXES ON] [F2: CRT] [F3: 12-PART RIG]" : "[F1: DEBUG BOXES] [F2: CRT] [F3: 12-PART RIG]", 10, 8, 12, Color{ 140, 220, 180, 220 });

        char padStatus[96];
        if (InputManager::IsGamepadActive()) {
            snprintf(padStatus, sizeof(padStatus), "[PAD: %.26s]", InputManager::GetActiveControllerName(0));
            DrawText(padStatus, 140, 8, 12, Color{ 80, 240, 140, 240 });
        } else {
            DrawText("[PAD: KEYBOARD (CONNECT GAMEPAD)]", 140, 8, 12, Color{ 180, 180, 195, 200 });
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
