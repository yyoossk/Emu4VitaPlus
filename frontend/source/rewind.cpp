#include <algorithm>
#include <libretro.h>
#include "defines.h"
#include "log.h"
#include "config.h"
#include "rewind.h"
#include "utils.h"
#include "emulator.h"

//"REWD"
#define REWIND_BLOCK_MAGIC 0x44574552
#define MIN_STATE_RATE 5
#define NEXT_STATE_PERIOD 50000

bool RewindBlock::IsValid()
{
    return buf && buf->magic == REWIND_BLOCK_MAGIC && buf->index == index;
}

Rewind::Rewind()
    : ThreadBase(_RewindThread),
      _rewinding(false),
      _next_time(0)
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

void Rewind::StartRewind()
{
    _rewinding = true;
}

void Rewind::StopRewind()
{
    _rewinding = false;
}

int Rewind::_RewindThread(SceSize args, void *argp)
{
    CLASS_POINTER(Rewind, rewind, argp);

    while (rewind->IsRunning())
    {
        int current_time = sceKernelGetProcessTimeWide();
        if ((!rewind->_rewinding) && current_time < rewind->_next_time)
        {
            sceKernelDelayThread(rewind->_next_time - current_time);
            current_time = sceKernelGetProcessTimeWide();
        }

        rewind->_rewinding ? rewind->_Rewind() : rewind->_SaveState();
        rewind->_next_time = current_time + std::max((uint64_t)NEXT_STATE_PERIOD, gEmulator->GetMsPerFrame());
    }

    return 0;
}

void Rewind::_SaveState()
{
}

void Rewind::_Rewind()
{
}

bool Rewind::_SaveFullState()
{
    // RewindBlock *block = _blocks.WriteBegin(1);
    return true;
}