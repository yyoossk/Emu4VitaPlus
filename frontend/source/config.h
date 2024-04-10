#pragma once
#include <stdint.h>
#include <vector>
#include "language.h"

struct KeyMapConfig
{
    uint32_t psv;
    uint8_t retro;
    bool turbo = false;
};

class Config
{
public:
    Config();
    virtual ~Config();

    bool Load(const char *path);
    bool Save(const char *path);

    std::vector<KeyMapConfig> key_maps;
    LANGUAGE language;

private:
};