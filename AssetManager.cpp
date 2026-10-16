#include "AssetManager.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <cstdint>
#include <cstdio>

#ifdef _WIN32

typedef void* HRSRC;
typedef void* HGLOBAL;
typedef void* HMODULE;
typedef void* LPVOID;
typedef unsigned long DWORD;

extern "C" {
__declspec(dllimport) HRSRC __stdcall FindResourceA(HMODULE hModule, const char* lpName, const char* lpType);
__declspec(dllimport) HGLOBAL __stdcall LoadResource(HMODULE hModule, HRSRC hResInfo);
__declspec(dllimport) LPVOID __stdcall LockResource(HGLOBAL hResData);
__declspec(dllimport) DWORD __stdcall SizeofResource(HMODULE hModule, HRSRC hResInfo);
}

#define WIN_MAKEINTRESOURCEA(i) ((const char*)((uintptr_t)((uint16_t)(i))))
#define WIN_RT_RCDATA WIN_MAKEINTRESOURCEA(10)

#ifdef EO_PAK_FILE
__asm__(
    ".section .eopak,\"dr\"\n"
    ".p2align 4\n"
    ".globl eo_pak_start\n"
    "eo_pak_start:\n"
    ".incbin \"" EO_PAK_FILE "\"\n"
    ".globl eo_pak_end\n"
    "eo_pak_end:\n"
    ".text\n"
);
extern "C" const uint8_t eo_pak_start[];
extern "C" const uint8_t eo_pak_end[];
#endif
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#endif

namespace AssetManager {

struct Entry {
    uint32_t offset;
    uint32_t size;
};

static bool s_initialized = false;
static bool s_hasEmbedded = false;
static const uint8_t* s_pakData = nullptr;
static uint32_t s_pakSize = 0;
static const uint8_t* s_payloadData = nullptr;
static uint8_t s_xorKey = 0x5A;
static std::unordered_map<std::string, Entry> s_entries;

static std::string NormalizePath(const std::string& p) {
    std::string s = p;
    std::replace(s.begin(), s.end(), '\\', '/');
    while (s.rfind("./", 0) == 0) s = s.substr(2);
    while (s.rfind("../", 0) == 0) s = s.substr(3);
    for (char& c : s) c = (char)tolower((unsigned char)c);
    return s;
}

void Init() {
    if (s_initialized) return;
    s_initialized = true;

#ifdef _WIN32

#ifdef EO_PAK_FILE
    // Preferred: pak embedded via .incbin into its own read-only PE section.
    // Keeping it out of .rsrc keeps the resource section tiny so Explorer can
    // extract the application icon reliably.
    {
        volatile uintptr_t pakBegin = (uintptr_t)eo_pak_start;
        volatile uintptr_t pakEnd = (uintptr_t)eo_pak_end;
        if (pakEnd > pakBegin) {
            s_pakData = (const uint8_t*)pakBegin;
            s_pakSize = (uint32_t)(pakEnd - pakBegin);
        }
    }
#endif

    if (!s_pakData || s_pakSize < 16) {
        HRSRC hRes = FindResourceA(nullptr, WIN_MAKEINTRESOURCEA(100), WIN_RT_RCDATA);
        if (hRes) {
            HGLOBAL hGlobal = LoadResource(nullptr, hRes);
            if (hGlobal) {
                s_pakData = (const uint8_t*)LockResource(hGlobal);
                s_pakSize = (uint32_t)SizeofResource(nullptr, hRes);
            }
        }
    }
#endif

    if (!s_pakData || s_pakSize < 16) {
        const char* candidatePaths[] = {
            "assets.pak",
            "ExecutiveOrder/assets.pak",
            "../assets.pak",
            "/roms/ports/ExecutiveOrder/assets.pak",
            "/roms2/ports/ExecutiveOrder/assets.pak"
        };
        std::string foundPath;
        std::string appDirPak = std::string(GetApplicationDirectory()) + "assets.pak";
        if (::FileExists(appDirPak.c_str())) {
            foundPath = appDirPak;
        } else {
            for (const char* cp : candidatePaths) {
                if (::FileExists(cp)) {
                    foundPath = cp;
                    break;
                }
            }
        }

        if (!foundPath.empty()) {
            FILE* f = fopen(foundPath.c_str(), "rb");
            if (f) {
                fseek(f, 0, SEEK_END);
                long sz = ftell(f);
                fseek(f, 0, SEEK_SET);
                if (sz >= 16) {
                    static std::vector<uint8_t> fileBuf;
                    fileBuf.resize(sz);
                    if (fread(fileBuf.data(), 1, sz, f) == (size_t)sz) {
                        s_pakData = fileBuf.data();
                        s_pakSize = (uint32_t)sz;
                    }
                }
                fclose(f);
            }
        }
    }

    if (!s_pakData || s_pakSize < 16) return;

    if (memcmp(s_pakData, "EO2PAK1\0", 8) != 0) return;

    uint32_t fileCount = *(const uint32_t*)(s_pakData + 8);
    s_xorKey = s_pakData[12];

    const uint8_t* ptr = s_pakData + 16;
    for (uint32_t i = 0; i < fileCount; i++) {
        if (ptr + 2 > s_pakData + s_pakSize) break;
        uint16_t nameLen = *(const uint16_t*)ptr;
        ptr += 2;
        if (ptr + nameLen + 8 > s_pakData + s_pakSize) break;
        std::string name((const char*)ptr, nameLen);
        ptr += nameLen;
        uint32_t offset = *(const uint32_t*)ptr;
        ptr += 4;
        uint32_t size = *(const uint32_t*)ptr;
        ptr += 4;

        Entry e{ offset, size };
        s_entries[NormalizePath(name)] = e;

        if (name.rfind("assets/", 0) == 0) {
            s_entries[NormalizePath(name.substr(7))] = e;
        }
    }

    s_payloadData = ptr;
    s_hasEmbedded = !s_entries.empty();
}

bool HasEmbeddedAssets() {
    Init();
    return s_hasEmbedded;
}

bool FileExists(const char* path) {
    if (!path || !path[0]) return false;
    Init();
    if (s_hasEmbedded) {
        std::string norm = NormalizePath(path);
        if (s_entries.find(norm) != s_entries.end()) return true;
    }
    return ::FileExists(path);
}

static bool GetDecryptedData(const char* path, std::vector<uint8_t>& outBuf) {
    Init();
    if (!s_hasEmbedded || !s_payloadData) return false;
    std::string norm = NormalizePath(path);
    auto it = s_entries.find(norm);
    if (it == s_entries.end()) return false;

    const Entry& e = it->second;
    const uint8_t* src = s_payloadData + e.offset;
    outBuf.resize(e.size);
    for (uint32_t i = 0; i < e.size; i++) {
        outBuf[i] = src[i] ^ s_xorKey;
    }
    return true;
}

Texture2D LoadTexture(const char* path) {
    std::vector<uint8_t> buf;
    if (GetDecryptedData(path, buf)) {
        const char* ext = GetFileExtension(path);
        if (!ext || !ext[0]) ext = ".png";
        Image img = LoadImageFromMemory(ext, buf.data(), (int)buf.size());
        if (img.data != nullptr) {
            Texture2D tex = LoadTextureFromImage(img);
            UnloadImage(img);
            return tex;
        }
    }
    return ::LoadTexture(path);
}

Image LoadImage(const char* path) {
    std::vector<uint8_t> buf;
    if (GetDecryptedData(path, buf)) {
        const char* ext = GetFileExtension(path);
        if (!ext || !ext[0]) ext = ".png";
        return LoadImageFromMemory(ext, buf.data(), (int)buf.size());
    }
    return ::LoadImage(path);
}

}
