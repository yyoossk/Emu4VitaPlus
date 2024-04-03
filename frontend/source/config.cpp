#include <psp2/ctrl.h>
#include <toml++/toml.hpp>
#include "config.h"
#include "input.h"

Config::Config()
{
    key_maps = {
        {RETRO_JOYPAD_B, SCE_CTRL_UP},
    };
}

Config::~Config()
{
}