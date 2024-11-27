#pragma once
#include <unordered_map>
#include <array>
#include <string>
#include <stdint.h>
#include "language_define.h"

typedef std::unordered_map<uint32_t, std::array<std::string, LANGUAGE_COUNT>> t_RomNameMap;

class RomNameMap : public t_RomNameMap
{
public:
    bool Load(const char *path);

private:
};