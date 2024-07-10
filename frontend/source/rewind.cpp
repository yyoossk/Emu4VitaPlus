#include <algorithm>
#include <libretro.h>
#include "defines.h"
#include "log.h"
#include "config.h"
#include "rewind.h"
#include "utils.h"
#include "emulator.h"

#define MIN_STATE_RATE 5
#define NEXT_STATE_PERIOD 50000
#define THRESHOLD_RATE 0.1

Rewind::Rewind()
    : ThreadBase(_RewindThread),
      _rewinding(false),
      _next_time(0),
      _contens(nullptr),
      _count(0),
      _tmp_buf(nullptr)
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

    Deinit();

    _state_size = retro_serialize_size();
    _aligned_state_size = ALIGN_UP_10H(_state_size);

    size_t buf_size = gConfig->rewind_buf_size << 20;
    if (buf_size < _aligned_state_size * MIN_STATE_RATE)
    {
        size_t min_size = _aligned_state_size * MIN_STATE_RATE >> 20;
        LogError(" the buffer size is too small, minimum required is %dMB", min_size);
        return false;
    }

    _threshold_size = _state_size * THRESHOLD_RATE;
    _tmp_buf = new uint8_t[_state_size];
    _contens = new RewindContens(buf_size);
    StopRewind();

    Start();

    return true;
}

void Rewind::Deinit()
{
    LogFunctionName;
    Stop();

    _blocks.Reset();
    if (_contens != nullptr)
    {
        delete _contens;
        _contens = nullptr;
    }

    if (_tmp_buf != nullptr)
    {
        delete[] _tmp_buf;
        _tmp_buf = nullptr;
    }
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
    if (_blocks.Current() == nullptr || _contens->ShouldSaveFull())
    {
        _SaveFullState(false);
    }
    else if (!_SaveDiffState())
    {
        _SaveFullState(true);
    }
}

void Rewind::_Rewind()
{
}

bool Rewind::_SaveDiffState()
{
    RewindBlock *current = _blocks.Current();
    _Serialize(_tmp_buf, _state_size);
    RewindBlock *full_block = current->type == BLOCK_FULL ? current : ((RewindDiffContent *)current->content)->full_block;
}

bool Rewind::_SaveFullState(bool from_tmp)
{
    RewindBlock *block = _blocks.Next();
    block->type = BLOCK_FULL;
    block->index = _count;
    block->size = sizeof(RewindFullContent) + _aligned_state_size;
    RewindFullContent *content = (RewindFullContent *)_contens->WriteBegin(block->size);
    block->content = content;

    content->magic = REWIND_BLOCK_MAGIC;
    content->index = _count;
    _count++;

    _contens->SetLastFull((uint8_t *)content);

    if (from_tmp)
    {
        memcpy(content->buf, _tmp_buf, _state_size);
        return true;
    }
    else
    {
        return _Serialize(content->buf, _state_size);
    }
}

bool Rewind::_Serialize(void *data, size_t size)
{
    gEmulator->Lock();
    bool result = retro_serialize(data, size);
    gEmulator->Unlock();
    return result;
}

bool Rewind::_UnSerialize(void *data, size_t size)
{
    gEmulator->Lock();
    bool result = retro_unserialize(data, size);
    gEmulator->Unlock();
    return result;
}