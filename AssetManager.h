#pragma once
#include "raylib.h"

namespace AssetManager {
    void Init();
    bool HasEmbeddedAssets();
    bool FileExists(const char* path);
    Texture2D LoadTexture(const char* path);
    Image LoadImage(const char* path);
    Sound LoadSound(const char* path);
}
