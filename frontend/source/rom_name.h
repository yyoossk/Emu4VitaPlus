#pragma once
#include <unordered_map>
#include <stdint.h>
#include <string>
#include "language_define.h"

class RomNameMap
{
public:
    RomNameMap(const char *path = nullptr);
    virtual ~RomNameMap();

    void Load();
    bool Load(const char *path);
    bool Load(const std::string &path);
    const char *GetName(uint32_t crc) const;

private:
    std::unordered_map<uint32_t, uint32_t> _map;
    char *_names;
};