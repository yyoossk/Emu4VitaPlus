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
    KEY_PAIR(SCE_CTRL_LSTICK_UP),
    KEY_PAIR(SCE_CTRL_LSTICK_DOWN),
    KEY_PAIR(SCE_CTRL_LSTICK_LEFT),
    KEY_PAIR(SCE_CTRL_LSTICK_RIGHT),
};

Config::Config()
{
    LogFunctionName;
    key_maps = {
#if defined(GBA_BUILD)
        {SCE_CTRL_CROSS, RETRO_DEVICE_ID_JOYPAD_B},
        {SCE_CTRL_TRIANGLE, RETRO_DEVICE_ID_JOYPAD_B, true},
        {SCE_CTRL_CIRCLE, RETRO_DEVICE_ID_JOYPAD_A},
        {SCE_CTRL_SQUARE, RETRO_DEVICE_ID_JOYPAD_A, true},
        {SCE_CTRL_SELECT, RETRO_DEVICE_ID_JOYPAD_SELECT},
        {SCE_CTRL_START, RETRO_DEVICE_ID_JOYPAD_START},
        {SCE_CTRL_UP, RETRO_DEVICE_ID_JOYPAD_UP},
        {SCE_CTRL_DOWN, RETRO_DEVICE_ID_JOYPAD_DOWN},
        {SCE_CTRL_LEFT, RETRO_DEVICE_ID_JOYPAD_LEFT},
        {SCE_CTRL_RIGHT, RETRO_DEVICE_ID_JOYPAD_RIGHT},
        {SCE_CTRL_L1, RETRO_DEVICE_ID_JOYPAD_L},
        {SCE_CTRL_R1, RETRO_DEVICE_ID_JOYPAD_R},
#endif
        {SCE_CTRL_LSTICK_UP, RETRO_DEVICE_ID_JOYPAD_UP},
        {SCE_CTRL_LSTICK_DOWN, RETRO_DEVICE_ID_JOYPAD_DOWN},
        {SCE_CTRL_LSTICK_LEFT, RETRO_DEVICE_ID_JOYPAD_LEFT},
        {SCE_CTRL_LSTICK_RIGHT, RETRO_DEVICE_ID_JOYPAD_RIGHT},
    };

    language = LANGUAGE_ENGLISH;

    Load(APP_CONFIG_PATH);
}

Config::~Config()
{
    LogFunctionName;
    Save(APP_CONFIG_PATH);
}

bool Config::Save(const char *path)
{
    LogFunctionName;
    toml::table keys;
    for (const auto &k : key_maps)
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

    const toml::table *tbl = &result.table();

    return true;
}