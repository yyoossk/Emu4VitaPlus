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
    return buf && buf->magic == REWIND_BLOCK_MAGIC && buf->index == index;
}

Rewind::Rewind()
    : ThreadBase(_RewindThread)
{
    LogFunctionName;
}

Rewind::~Rewind()
{
    LogFunctionName;
    Deinit();
}

bool Rewind::Init()
{
    LogFunctionName;
    _state_size = retro_serialize_size();
    _aligned_state_size = ALIGN_UP_10H(_state_size);

    size_t buf_size = gConfig->rewind_buf_size << 20;
    if (buf_size < _aligned_state_size * MIN_STATE_RATE)
    {
        size_t min_size = _aligned_state_size * MIN_STATE_RATE >> 20;
        LogError(" the buffer size is too small, minimum required is %dMB", min_size);
        return false;
    }

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

bool Rewind::_SaveFullState()
{
    // RewindBlock *block = _blocks.WriteBegin(1);
    return true;
}