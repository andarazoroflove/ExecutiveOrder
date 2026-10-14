#pragma once
#include "Common.h"
#include <string>

struct VenueDetails {
    std::string name;
    std::string location;
    Color skyTop;
    Color skyBottom;
    Color midArchColor;
    Color floorColor;
    Color hazardAccent;
};

class StageManager {
public:
    StageManager();
    ~StageManager();

    void Init();
    void Shutdown();

    void SetStage(StageId id);
    StageId GetCurrentStage() const { return m_stageId; }
    const VenueDetails& GetCurrentVenue() const { return m_venues[m_stageId]; }

    void DrawFarLayer(float camX, float time);
    void DrawMidLayer(float camX, float time);
    void DrawPlayfield(float camX, float time);
    void DrawForeground(float camX, float time);

private:
    void DrawEastWing(float camX, float time);
    void DrawReflectingPool(float camX, float time);
    void DrawCongressionalFloor(float camX, float time);
    void DrawMMARing(float camX, float time);

    StageId m_stageId = STAGE_DEMOLISHED_EAST_WING;
    VenueDetails m_venues[STAGE_COUNT];
    Texture2D m_stageTextures[STAGE_COUNT]{};
    bool m_texturesLoaded = false;
};
