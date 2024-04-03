#include <psp2/ctrl.h>
#include <toml++/toml.hpp>
#include "config.h"
#include "input.h"

Config::Config()
{
    key_maps = {
        {RETRO_JOYPAD_UP, SCE_CTRL_UP},
        {RETRO_JOYPAD_RIGHT, SCE_CTRL_RIGHT},
        {RETRO_JOYPAD_DOWN, SCE_CTRL_DOWN},
        {RETRO_JOYPAD_LEFT, SCE_CTRL_LEFT},
        {RETRO_JOYPAD_START, SCE_CTRL_START},
        {RETRO_JOYPAD_SELECT, SCE_CTRL_SELECT},
    };
}

Config::~Config()
{
}