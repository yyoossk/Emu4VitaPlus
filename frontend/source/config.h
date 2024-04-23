#pragma once
#include <stdint.h>
#include <vector>
#include "config_types.h"
#include "language_define.h"

class Config
{
public:
    Config();
    virtual ~Config();

    bool Load(const char *path);
    bool Save(const char *path);

    LANGUAGE language;
    std::vector<ControlMapConfig> control_maps;
    GraphicsConfig graphics_config;

private:
};