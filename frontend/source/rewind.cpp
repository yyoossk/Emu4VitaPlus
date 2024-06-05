#include <libretro.h>
#include "defines.h"
#include "log.h"
#include "config.h"
#include "rewind.h"
#include "utils.h"

//"REWD"
#define REWIND_BLOCK_MAGIC 0x44574552
#define MIN_STATE_RATE 5

bool RewindBlock::IsValid()
{
    return buf && buf->magic == REWIND_BLOCK_MAGIC && index == buf->index;
}

Rewind::Rewind() : ThreadBase(_RewindThread)
{
    LogFunctionName;
}

Rewind::~Rewind()
{
    LogFunctionName;
}

bool Rewind::Init()
{
    LogFunctionName;
    _state_size = retro_serialize_size();
    _aligned_state_size = ALIGN_UP_10H(_state_size);
    return true;
}

void Rewind::Deinit()
{
    LogFunctionName;
}

int Rewind::_RewindThread(SceSize args, void *argp)
{
    return 0;
}