#include <algorithm>
#include <libretro.h>
#include "defines.h"
#include "log.h"
#include "config.h"
#include "rewind.h"
#include "utils.h"
#include "emulator.h"
#include "app.h"

#define MIN_STATE_RATE 5
#define NEXT_STATE_PERIOD 50000
#define THRESHOLD_RATE 0.1

#define DIFF_STEP 0x10
#define MEMCMP2(SIZE) memcmp_##SIZE
#define MEMCMP(SIZE) MEMCMP2(SIZE)
#define MEMCMP_DIFF_STEP MEMCMP(DIFF_STEP)

RewindManager::RewindManager()
    : ThreadBase(_RewindThread),
      _rewinding(false),
      _next_time(0),
      _contens(nullptr),
      _count(0),
      _tmp_buf(nullptr),
      _last_full_block(nullptr)
{
    LogFunctionName;
}

RewindManager::~RewindManager()
{
    LogFunctionName;
    Deinit();
}

bool RewindManager::Init()
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

void RewindManager::Deinit()
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

void RewindManager::StartRewind()
{
    _rewinding = true;
}

void RewindManager::StopRewind()
{
    _rewinding = false;
}

int RewindManager::_RewindThread(SceSize args, void *argp)
{
    CLASS_POINTER(RewindManager, rewind, argp);

    while (rewind->IsRunning())
    {
        if (gStatus != APP_STATUS_RUN_GAME)
        {
            sceKernelDelayThread(500000);
            continue;
        }

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

void RewindManager::_SaveState()
{
    if (_last_full_block == nullptr || _contens->GetDistance((uint8_t *)_last_full_block->content) * 2 > _contens->GetSize())
    {
        _SaveFullState(_blocks.Next(), false);
    }
    else if (_Serialize(_tmp_buf, _state_size))
    {
        if (!_SaveDiffState(_blocks.Next()))
        {
            _SaveFullState(_blocks.Current(), true);
        }
    }
}

void RewindManager::_Rewind()
{
}

static inline int memcmp_0x10(const void *src, const void *dst)
{
    const uint32_t *s = (const uint32_t *)src;
    const uint32_t *d = (const uint32_t *)dst;
    return s[0] != d[0] || s[1] != d[1] || s[2] != d[2] || s[3] != d[3];
}

bool RewindManager::_SaveDiffState(RewindBlock *block)
{
    if (!_Serialize(_tmp_buf, _state_size))
    {
        return false;
    }

    uint8_t *old_state = ((RewindFullContent *)(_last_full_block->content))->buf;
    uint8_t *new_state = _tmp_buf;
    bool last_diff = false;
    size_t offset = 0;
    size_t diff_size = sizeof(RewindDiffContent);
    uint32_t tail_size = _state_size % DIFF_STEP;
    size_t chunk_size = _state_size - tail_size;

    RewindDiffContent *content = (RewindDiffContent *)_contens->WriteBegin(_threshold_size);
    DiffArea *areas = content->areas;

    block->type = BLOCK_DIFF;
    block->index = _count;
    block->content = content;
    content->magic = REWIND_BLOCK_MAGIC;
    content->index = _count;
    content->full_block = _last_full_block;
    content->num = 0;
    _count++;

    for (; offset < chunk_size; offset += DIFF_STEP)
    {
        if (MEMCMP_DIFF_STEP(old_state + offset, new_state + offset) == 0)
        {
            if (last_diff)
            {
                last_diff = false;
                areas[content->num].size = offset - areas[content->num].offset;
                diff_size += sizeof(DiffArea) + areas[content->num].size;
                content->num++;
                if (diff_size > _threshold_size)
                {
                    return false;
                }
            }
        }
        else
        {
            if (!last_diff)
            {
                last_diff = true;
                areas[content->num].offset = offset;
            }
        }
    }

    if (tail_size > 0 && memcmp(old_state + offset, new_state + offset, tail_size) != 0)
    {
        if (last_diff)
        {
            areas[content->num].size = _state_size - areas[content->num].offset;
        }
        else
        {
            areas[content->num].offset = offset;
            areas[content->num].size = tail_size;
        }

        diff_size += sizeof(DiffArea) + areas[content->num].size;
        content->num++;
    }
    else if (last_diff)
    {
        areas[content->num].size = offset - areas[content->num].offset;
        diff_size += sizeof(DiffArea) + areas[content->num].size;
        content->num++;
    }

    if (diff_size > _threshold_size)
    {
        return false;
    }

    block->size = ALIGN_UP_10H(diff_size);
    uint8_t *buf = (uint8_t *)content + sizeof(RewindDiffContent) + content->num * sizeof(DiffArea);
    for (size_t i = 0; i < content->num; i++)
    {
        memcpy(buf, buf + areas[i].offset, areas[i].size);
        buf += areas[i].size;
    }

    return true;
}

bool RewindManager::_SaveFullState(RewindBlock *block, bool from_tmp)
{
    block->type = BLOCK_FULL;
    block->index = _count;
    block->size = sizeof(RewindFullContent) + _aligned_state_size;
    RewindFullContent *content = (RewindFullContent *)_contens->WriteBegin(block->size);
    block->content = content;

    content->magic = REWIND_BLOCK_MAGIC;
    content->index = _count;
    _count++;

    bool result = true;
    if (from_tmp)
    {
        memcpy(content->buf, _tmp_buf, _state_size);
    }
    else
    {
        result = _Serialize(content->buf, _state_size);
    }

    if (result)
    {
        _last_full_block = block;
        _contens->WriteEnd(block->size);
    }

    return result;
}

bool RewindManager::_Serialize(void *data, size_t size)
{
    gEmulator->Lock();
    bool result = retro_serialize(data, size);
    gEmulator->Unlock();
    return result;
}

bool RewindManager::_UnSerialize(void *data, size_t size)
{
    gEmulator->Lock();
    bool result = retro_unserialize(data, size);
    gEmulator->Unlock();
    return result;
}