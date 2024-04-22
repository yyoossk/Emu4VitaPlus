#pragma once
#include <stdint.h>
#include <vector>
#include "config_types.h"
#include "language.h"

class Config
{
public:
    Config();
    virtual ~Config();

    bool Load(const char *path);
    bool Save(const char *path);

    std::vector<KeyMapConfig> key_maps;
    GraphicsConfig graphics_config;

private:
};