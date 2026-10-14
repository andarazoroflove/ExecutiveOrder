#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include "raylib.h"

constexpr int SCREEN_WIDTH = 384;
constexpr int SCREEN_HEIGHT = 224;
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr int TARGET_FPS = 60;
constexpr float FIXED_DT = 1.0f / 60.0f;
constexpr float PI_F = 3.14159265358979323846f;

constexpr float STAGE_MIN_X = 40.0f;
constexpr float STAGE_MAX_X = 760.0f;
constexpr float STAGE_TOTAL_WIDTH = 800.0f;
constexpr float GROUND_Y = 196.0f;
constexpr float MAX_FIGHTER_DISTANCE = 330.0f;

enum CharacterId {
    CHAR_COMMANDER = 0,
    CHAR_HIGH_BENCH = 1,
    CHAR_REDCAP = 2,
    CHAR_COASTAL_GOV = 3,
    CHAR_TECH_VISIONARY = 4,
    CHAR_FILIBUSTER = 5,
    CHAR_MASCOT = 6,
    CHAR_POPULIST = 7,
    CHAR_SLEEPY_JOE = 8,
    CHAR_MTG = 9,
    CHAR_SUCKRATERY_RKF = 10,
    CHAR_WAR_PIG = 11,
    CHAR_SOCIALIST = 12,
    CHAR_BILLARY = 13,
    CHAR_PALM_BEACH_PETE = 14,
    CHAR_THE_PUNDIT = 15,
    CHAR_A_NEW_HOPE = 16,
    CHARACTER_COUNT = 17
};
constexpr int PLAYABLE_CHARACTER_COUNT = 10;

enum StageId {
    STAGE_DEMOLISHED_EAST_WING = 0,
    STAGE_REFLECTING_POOL = 1,
    STAGE_CONGRESSIONAL_FLOOR = 2,
    STAGE_WHITE_HOUSE_MMA = 3,
    STAGE_PRESS_BRIEFING = 4,
    STAGE_PENTAGON_ALTAR = 5,
    STAGE_WTC_PLAZA = 6,
    STAGE_COMET_PING_PONG = 7,
    STAGE_FLORIDA_KEYS = 8,
    STAGE_LINCOLN_MEMORIAL = 9,
    STAGE_SMITHSONIAN_AERO = 10,
    STAGE_TOMB_UNKNOWN = 11,
    STAGE_ENN_NEWS_STUDIO = 12,
    STAGE_CHICAGO_WRIGLEY = 13,
    STAGE_COUNT = 14
};

enum GameState {
    STATE_SPLASH,
    STATE_SELECT_MAP,
    STATE_SECRET_SELECT,
    STATE_VERSUS,
    STATE_FIGHT,
    STATE_VICTORY_SPLASH,
    STATE_BONUS_CAR,
    STATE_MATCH_OVER,
    STATE_ENDING_CELEBRATION,
    STATE_CTHULHU_ENDING
};

enum AttackType {
    ATTACK_NONE = 0,
    ATTACK_MID,
    ATTACK_LOW,
    ATTACK_OVERHEAD,
    ATTACK_THROW,
    ATTACK_PROJECTILE
};

enum AttackMoveType {
    MOVE_LIGHT_PUNCH = 0,
    MOVE_HEAVY_PUNCH = 1,
    MOVE_LIGHT_KICK = 2,
    MOVE_HEAVY_KICK = 3,
    MOVE_SPECIAL = 4
};

enum InputButtons : uint16_t {
    BTN_NONE = 0,
    BTN_LP = (1 << 0),
    BTN_HP = (1 << 1),
    BTN_LK = (1 << 2),
    BTN_HK = (1 << 3),
    DIR_UP = (1 << 4),
    DIR_DOWN = (1 << 5),
    DIR_LEFT = (1 << 6),
    DIR_RIGHT = (1 << 7),
    BTN_START = (1 << 8),
    BTN_COIN = (1 << 9),
    BTN_SPECIAL = (1 << 10),
    BTN_TAUNT = (1 << 11),
    BTN_SELECT = (1 << 12)
};

enum AIDifficulty {
    AI_DIFF_EASY = 0,
    AI_DIFF_NORMAL = 1,
    AI_DIFF_HARD = 2
};

struct Palette16 {
    Color colors[16];
};

inline Rectangle MakeRect(float x, float y, float w, float h) {
    return Rectangle{ x, y, w, h };
}

inline Vector2 MakeVec2(float x, float y) {
    return Vector2{ x, y };
}
