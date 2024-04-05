#include <psp2/ctrl.h>
#include <libretro.h>
#include <stdio.h>
#include "config.h"
#include "input.h"

#ifndef TOML_EXCEPTIONS
#define TOML_EXCEPTIONS 0
#endif
#include <toml++/toml.hpp>

Config::Config()
{
    key_maps = {
#if defined(GBA_BUILD)
        {SCE_CTRL_CROSS, RETRO_DEVICE_ID_JOYPAD_B},
        {SCE_CTRL_TRIANGLE, RETRO_DEVICE_ID_JOYPAD_B, true},
        {SCE_CTRL_CIRCLE, RETRO_DEVICE_ID_JOYPAD_Y},
        {SCE_CTRL_SQUARE, RETRO_DEVICE_ID_JOYPAD_Y, true},
        {SCE_CTRL_SELECT, RETRO_DEVICE_ID_JOYPAD_SELECT},
        {SCE_CTRL_START, RETRO_DEVICE_ID_JOYPAD_START},
        {SCE_CTRL_UP, RETRO_DEVICE_ID_JOYPAD_UP},
        {SCE_CTRL_DOWN, RETRO_DEVICE_ID_JOYPAD_DOWN},
        {SCE_CTRL_LEFT, RETRO_DEVICE_ID_JOYPAD_LEFT},
        {SCE_CTRL_RIGHT, RETRO_DEVICE_ID_JOYPAD_RIGHT},
        {SCE_CTRL_L1, RETRO_DEVICE_ID_JOYPAD_L},
        {SCE_CTRL_R1, RETRO_DEVICE_ID_JOYPAD_R},
#endif
        {SCE_CTRL_LSTICK_UP, SCE_CTRL_UP},
        {SCE_CTRL_LSTICK_DOWN, SCE_CTRL_DOWN},
        {SCE_CTRL_LSTICK_LEFT, SCE_CTRL_LEFT},
        {SCE_CTRL_LSTICK_RIGHT, SCE_CTRL_RIGHT},
    };
}

Config::~Config()
{
}

bool Config::Save(const char *path)
{
    toml::array keys;
    for (const auto &k : key_maps)
    {
        toml::table t{{"psv", k.psv},
                      {"retro", k.retro},
                      {"turbo", k.turbo}};
        keys.emplace_back(t);
    }
    auto tbl = toml::table{{"KEY", keys}};
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