#pragma once
#include <stdint.h>
#include <vector>
#include <unordered_map>
#include <string>
#include "libretro.h"
#include "config_types.h"
#include "language_define.h"
#include "defines.h"

extern std::unordered_map<uint32_t, TEXT_ENUM> ControlTextMap;
extern std::unordered_map<uint8_t, TEXT_ENUM> RetroTextMap;

class Config
{
public:
    Config();
    virtual ~Config();

    bool Load(const char *path = APP_CONFIG_PATH);
    bool Save(const char *path = APP_CONFIG_PATH);

    void Default();
    void DefaultControlMap();
    void DefaultHotKey();
    void DefaultGraphics();
    retro_language GetRetroLanguage();

    uint32_t language;
    std::vector<ControlMapConfig> control_maps;
    uint32_t graphics_config[GRAPHICS_CONFIG_COUNT];
    uint32_t hotkeys[HOT_KEY_COUNT];

    static std::unordered_map<uint32_t, TEXT_ENUM> ControlTextMap;
    static std::unordered_map<uint8_t, TEXT_ENUM> RetroTextMap;

private:
};

extern Config *gConfig;
