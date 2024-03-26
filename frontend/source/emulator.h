#pragma once
#include <libretro.h>

class Emulator
{
public:
    Emulator();
    virtual ~Emulator();
    const char *GetValidExtensions() const { return _info.valid_extensions; };

private:
    retro_system_info _info;
};