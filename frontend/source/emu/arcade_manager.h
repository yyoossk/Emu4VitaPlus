#pragma once
#include <stdint.h>
#include <unordered_set>
#include "cache_manager.h"

#define DAT_BIN_PATH "app0:assets/arcade_dat.bin"

class ArcadeManager : public CacheManager
{
public:
    ArcadeManager();
    virtual ~ArcadeManager();
    const char *GetCachedPath(const char *path);

private:
    void _Load();

    std::unordered_set<uint32_t> _name_hash;
    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> _roms;
    char *_names;
};
