#pragma once
#include "Common.h"

struct BoxAABB {
    Rectangle rect;
    bool active = false;
};

struct SparkEffect {
    Vector2 pos;
    bool isBlock = false;
    int frame = 0;
    int maxFrames = 6;
    bool alive = false;
};

class Fighter;

class CollisionSystem {
public:
    static bool CheckAABB(const Rectangle& a, const Rectangle& b);
    static Rectangle GetIntersection(const Rectangle& a, const Rectangle& b);

    static void ResolvePushboxes(Fighter& f1, Fighter& f2);

    static void ResolveCombat(Fighter& p1, Fighter& p2, std::vector<SparkEffect>& sparks, float& cameraShake);
};
