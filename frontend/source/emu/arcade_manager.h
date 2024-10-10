#pragma once

#ifdef ARC_BUILD
#include <stdint.h>
#include <unordered_map>
#include <unordered_set>

#define DAT_BIN_PATH "app0:assets/arcade_dat.bin"

class ArcadeManager
{
public:
    ArcadeManager();
    virutal ~ArcadeManager();

private:
    void _Load(const char *path = DAT_BIN_PATH);

    std::unordered_set<uint32_t> _name_hash;
    std::unordered_map<uint32_t, std::unordered_set<uint32_t>> _roms;
    char *_names;
}

#endif