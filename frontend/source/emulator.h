#pragma once
#include <libretro.h>

class Emulator
{
public:
    Emulator();
    virtual ~Emulator();

private:
    retro_system_info _info;
};