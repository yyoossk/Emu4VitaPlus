#pragma once
#include <stdint.h>
#include <vector>
#include "config_types.h"
#include "language_define.h"
#include "global.h"

class Config
{
public:
    Config();
    virtual ~Config();

    bool Load(const char *path = APP_CONFIG_PATH);
    bool Save(const char *path = APP_CONFIG_PATH);

    void Default();

    LANGUAGE language;
    std::vector<ControlMapConfig> control_maps;
    GraphicsConfig graphics_config;
    uint32_t hotkeys[HOT_KEY_COUNT];

private:
};