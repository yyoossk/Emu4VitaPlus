#include <psp2/ctrl.h>
#include <libretro.h>
#include <stdio.h>
#include <unordered_map>
#include <string>
#include "global.h"
#include "config.h"
#include "input.h"
#include "log.h"

#ifndef TOML_EXCEPTIONS
#define TOML_EXCEPTIONS 0
#endif
#include <toml++/toml.hpp>

#define KEY_PAIR(K) \
    {               \
        K, #K       \
    }

static const std::unordered_map<uint32_t, std::string> PSV_KEYS = {
    KEY_PAIR(SCE_CTRL_CROSS),
    KEY_PAIR(SCE_CTRL_TRIANGLE),
    KEY_PAIR(SCE_CTRL_CIRCLE),
    KEY_PAIR(SCE_CTRL_SQUARE),
    KEY_PAIR(SCE_CTRL_SELECT),
    KEY_PAIR(SCE_CTRL_START),
    KEY_PAIR(SCE_CTRL_UP),
    KEY_PAIR(SCE_CTRL_DOWN),
    KEY_PAIR(SCE_CTRL_LEFT),
    KEY_PAIR(SCE_CTRL_RIGHT),
    KEY_PAIR(SCE_CTRL_L1),
    KEY_PAIR(SCE_CTRL_R1),
    KEY_PAIR(SCE_CTRL_L2),
    KEY_PAIR(SCE_CTRL_R2),
    KEY_PAIR(SCE_CTRL_L3),
    KEY_PAIR(SCE_CTRL_R3),
    KEY_PAIR(SCE_CTRL_LSTICK_UP),
    KEY_PAIR(SCE_CTRL_LSTICK_DOWN),
    KEY_PAIR(SCE_CTRL_LSTICK_LEFT),
    KEY_PAIR(SCE_CTRL_LSTICK_RIGHT),
    KEY_PAIR(SCE_CTRL_RSTICK_UP),
    KEY_PAIR(SCE_CTRL_RSTICK_DOWN),
    KEY_PAIR(SCE_CTRL_RSTICK_LEFT),
    KEY_PAIR(SCE_CTRL_RSTICK_RIGHT),
};

std::unordered_map<uint32_t, TEXT_ENUM> ControlTextMap = {
    {SCE_CTRL_CROSS, BUTTON_CROSS},
    {SCE_CTRL_TRIANGLE, BUTTON_TRIANGLE},
    {SCE_CTRL_CIRCLE, BUTTON_CIRCLE},
    {SCE_CTRL_SQUARE, BUTTON_SQUARE},
    {SCE_CTRL_SELECT, BUTTON_SELECT},
    {SCE_CTRL_START, BUTTON_START},
    {SCE_CTRL_UP, BUTTON_UP},
    {SCE_CTRL_DOWN, BUTTON_DOWN},
    {SCE_CTRL_LEFT, BUTTON_LEFT},
    {SCE_CTRL_RIGHT, BUTTON_RIGHT},
    {SCE_CTRL_L1, BUTTON_L1},
    {SCE_CTRL_R1, BUTTON_R1},
    {SCE_CTRL_L2, BUTTON_L2},
    {SCE_CTRL_R2, BUTTON_R2},
    {SCE_CTRL_L3, BUTTON_L3},
    {SCE_CTRL_R3, BUTTON_R3},
    {SCE_CTRL_LSTICK_UP, BUTTON_LEFT_ANALOG_UP},
    {SCE_CTRL_LSTICK_DOWN, BUTTON_LEFT_ANALOG_DOWN},
    {SCE_CTRL_LSTICK_LEFT, BUTTON_LEFT_ANALOG_LEFT},
    {SCE_CTRL_LSTICK_RIGHT, BUTTON_LEFT_ANALOG_RIGHT},
    {SCE_CTRL_RSTICK_UP, BUTTON_RIGHT_ANALOG_UP},
    {SCE_CTRL_RSTICK_DOWN, BUTTON_RIGHT_ANALOG_DOWN},
    {SCE_CTRL_RSTICK_LEFT, BUTTON_RIGHT_ANALOG_LEFT},
    {SCE_CTRL_RSTICK_RIGHT, BUTTON_RIGHT_ANALOG_RIGHT},
    {SCE_CTRL_PSBUTTON, BUTTON_HOME},
};

std::unordered_map<uint8_t, TEXT_ENUM> RetroTextMap = {
    {RETRO_DEVICE_ID_JOYPAD_B, BUTTON_B},
    {RETRO_DEVICE_ID_JOYPAD_Y, BUTTON_Y},
    {RETRO_DEVICE_ID_JOYPAD_SELECT, BUTTON_SELECT},
    {RETRO_DEVICE_ID_JOYPAD_START, BUTTON_START},
    {RETRO_DEVICE_ID_JOYPAD_UP, BUTTON_UP},
    {RETRO_DEVICE_ID_JOYPAD_DOWN, BUTTON_DOWN},
    {RETRO_DEVICE_ID_JOYPAD_LEFT, BUTTON_LEFT},
    {RETRO_DEVICE_ID_JOYPAD_RIGHT, BUTTON_RIGHT},
    {RETRO_DEVICE_ID_JOYPAD_A, BUTTON_A},
    {RETRO_DEVICE_ID_JOYPAD_X, BUTTON_X},
    {RETRO_DEVICE_ID_JOYPAD_L, BUTTON_L},
    {RETRO_DEVICE_ID_JOYPAD_R, BUTTON_R},
    {RETRO_DEVICE_ID_JOYPAD_L2, BUTTON_L2},
    {RETRO_DEVICE_ID_JOYPAD_R2, BUTTON_R2},
    {RETRO_DEVICE_ID_JOYPAD_L3, BUTTON_L3},
    {RETRO_DEVICE_ID_JOYPAD_R3, BUTTON_R3},
    {RETRO_DEVICE_ID_NONE, NONE},
};

Config::Config() : language(LANGUAGE_ENGLISH)
{
    LogFunctionName;
    Default();
    Load(APP_CONFIG_PATH);
}

Config::~Config()
{
    LogFunctionName;
    Save(APP_CONFIG_PATH);
}

void Config::Default()
{
    control_maps = {
#if defined(GBA_BUILD)
        {SCE_CTRL_UP, RETRO_DEVICE_ID_JOYPAD_UP},
        {SCE_CTRL_DOWN, RETRO_DEVICE_ID_JOYPAD_DOWN},
        {SCE_CTRL_LEFT, RETRO_DEVICE_ID_JOYPAD_LEFT},
        {SCE_CTRL_RIGHT, RETRO_DEVICE_ID_JOYPAD_RIGHT},
        {SCE_CTRL_CROSS, RETRO_DEVICE_ID_JOYPAD_B},
        {SCE_CTRL_TRIANGLE, RETRO_DEVICE_ID_JOYPAD_B, true},
        {SCE_CTRL_CIRCLE, RETRO_DEVICE_ID_JOYPAD_A},
        {SCE_CTRL_SQUARE, RETRO_DEVICE_ID_JOYPAD_A, true},
        {SCE_CTRL_SELECT, RETRO_DEVICE_ID_JOYPAD_SELECT},
        {SCE_CTRL_START, RETRO_DEVICE_ID_JOYPAD_START},
        {SCE_CTRL_L1, RETRO_DEVICE_ID_JOYPAD_L},
        {SCE_CTRL_R1, RETRO_DEVICE_ID_JOYPAD_R},
#endif
        {SCE_CTRL_L2},
        {SCE_CTRL_R2},
        {SCE_CTRL_L3},
        {SCE_CTRL_R3},
        {SCE_CTRL_LSTICK_UP, RETRO_DEVICE_ID_JOYPAD_UP},
        {SCE_CTRL_LSTICK_DOWN, RETRO_DEVICE_ID_JOYPAD_DOWN},
        {SCE_CTRL_LSTICK_LEFT, RETRO_DEVICE_ID_JOYPAD_LEFT},
        {SCE_CTRL_LSTICK_RIGHT, RETRO_DEVICE_ID_JOYPAD_RIGHT},
        {SCE_CTRL_RSTICK_UP},
        {SCE_CTRL_RSTICK_DOWN},
        {SCE_CTRL_RSTICK_LEFT},
        {SCE_CTRL_RSTICK_RIGHT},
    };

    hotkeys[SAVE_STATE] = SCE_CTRL_PSBUTTON | SCE_CTRL_SQUARE;
    hotkeys[LOAD_STATE] = SCE_CTRL_PSBUTTON | SCE_CTRL_TRIANGLE;
    hotkeys[GAME_SPEED_UP] = SCE_CTRL_PSBUTTON | SCE_CTRL_R1;
    hotkeys[GAME_SPEED_DOWN] = SCE_CTRL_PSBUTTON | SCE_CTRL_L1;
    hotkeys[GAME_REWIND] = SCE_CTRL_PSBUTTON | SCE_CTRL_RSTICK_LEFT;
    hotkeys[CONTROLLER_PORT_UP] = SCE_CTRL_PSBUTTON | SCE_CTRL_RSTICK_UP;
    hotkeys[CONTROLLER_PORT_DOWN] = SCE_CTRL_PSBUTTON | SCE_CTRL_RSTICK_DOWN;
    hotkeys[EXIT_GAME] = SCE_CTRL_PSBUTTON | SCE_CTRL_CROSS;

    graphics_config.size = CONFIG_DISPLAY_SIZE_FULL;
    graphics_config.ratio = CONFIG_DISPLAY_RATIO_BY_GAME_RESOLUTION;
    graphics_config.rotate = CONFIG_DISPLAY_ROTATE_DEFAULT;
    graphics_config.shader = CONFIG_GRAPHICS_SHADER_DEFAULT;
    graphics_config.smooth = CONFIG_GRAPHICS_SMOOTHER_NO;
    graphics_config.overlay_mode = CONFIG_GRAPHICS_OVERLAY_MODE_OVERLAY;
}

bool Config::Save(const char *path)
{
    LogFunctionName;
    toml::table keys;
    for (const auto &k : control_maps)
    {
        toml::table t{{"psv", k.psv},
                      {"retro", k.retro},
                      {"turbo", k.turbo}};
        keys.insert(PSV_KEYS.at(k.psv).c_str(), t);
    }

    toml::table main{
        {"language", gLanguageNames[language]},
    };

    auto tbl = toml::table{{"MAIN", main}, {"KEYS", keys}};
    std::stringstream s;
    s << tbl;

    FILE *fp = fopen(path, "wb");
    if (fp)
    {
        fputs(s.str().c_str(), fp);
        fclose(fp);
    }
    return true;
}

bool Config::Load(const char *path)
{
    toml::parse_result result = toml::parse_file(path);
    if (!result)
    {
        return false;
    }

    const toml::table tbl(std::move(result.table()));
    auto lang = tbl["MAIN"]["language"].ref<std::string>();
    for (size_t i = 0; i < LANGUAGE_COUNT; i++)
    {
        if (lang == gLanguageNames[i])
        {
            language = (LANGUAGE)i;
            break;
        }
    }

    const auto &keys = tbl["KEYS"];
    for (auto &k : control_maps)
    {
        const auto &t = keys[PSV_KEYS.at(k.psv)];
        k.retro = *(t["retro"].value<uint8_t>());
        k.turbo = *(t["turbo"].value<bool>());
    }

    return true;
}