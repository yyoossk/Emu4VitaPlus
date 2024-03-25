#include "emulator.h"
#include "log.h"

Emulator::Emulator()
{
    LogFunctionName;

    retro_get_system_info(&_info);
    LogInfo(_info.valid_extensions);
}

Emulator::~Emulator()
{
    LogFunctionName;
}