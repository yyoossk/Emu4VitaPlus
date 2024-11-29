#pragma once
#include <stdint.h>
#include <unordered_set>
#include <vector>
#include "cache_manager.h"

class ArcadeManager : public CacheManager
{
public:
    ArcadeManager();
    virtual ~ArcadeManager();
    const char *GetCachedPath(const char *path);
    const char *GetRomName(const char *path) const;
    bool NeedReplace(const char *path);

private:
    void _Load();

    std::unordered_set<uint32_t> _name_hash;
    std::unordered_map<uint32_t, std::vector<uint32_t>> _roms;
    char *_names;
};
