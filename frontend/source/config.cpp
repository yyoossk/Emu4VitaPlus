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
        {RETRO_JOYPAD_B, SCE_CTRL_CIRCLE},
        {RETRO_JOYPAD_A, SCE_CTRL_CROSS},
        {RETRO_JOYPAD_X, SCE_CTRL_SQUARE},
        {RETRO_JOYPAD_Y, SCE_CTRL_TRIANGLE},
    };
}

Config::~Config()
{
}