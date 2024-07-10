#pragma once
#include <stdint.h>
#include <string.h>
#include "thread_base.h"
#include "utils.h"

#define BLOCK_SIZE 0x400
//"REWD"
#define REWIND_BLOCK_MAGIC 0x44574552

enum BlockType
{
    BLOCK_FULL,
    BLOCK_DIFF
};

struct DiffArea
{
    uint32_t offset;
    uint32_t size;
};

struct RewindContent
{
    uint32_t magic;
    uint32_t index;
};

struct RewindBlock
{
    BlockType type;
    uint32_t index;
    RewindContent *content;
    uint32_t size;

    bool IsValid()
    {
        return content && content->magic == REWIND_BLOCK_MAGIC && content->index == index;
    }
};

struct RewindFullContent : RewindContent
{
    uint8_t buf[];
};

struct RewindDiffContent : RewindContent
{
    RewindBlock *full_block;
    uint32_t num;
    DiffArea areas[];
};

class RewindContens
{
public:
    RewindContens(size_t total_bytes)
        : _total_bytes(total_bytes),
          _current(0)
    {
        _data = new uint8_t[total_bytes];
    };

    virtual ~RewindContens()
    {
        delete[] _data;
    }

    uint8_t *GetData() { return _data; };

    uint8_t *WriteBegin(size_t max_size)
    {
        if (_current + max_size >= _total_bytes)
        {
            return _data;
        }
        else
        {
            return _data + _current;
        }
    }

    void WriteEnd(size_t size)
    {
        _current += size;
    }

    bool ShouldSaveFull()
    {
        size_t dist = _current - _last_full;
        if (dist < 0)
        {
            dist = _total_bytes - dist;
        }
        return dist * 2 > _total_bytes;
    }

    void SetLastFull(uint8_t *last_full) { _last_full = last_full - _data; }

private:
    uint8_t *_data;
    size_t _total_bytes;
    size_t _current;
    size_t _last_full;
};

class RewindBlocks
{
public:
    RewindBlocks(size_t total)
        : _total(total)
    {
        _blocks = new RewindBlock[total];
    };

    virtual ~RewindBlocks()
    {
        delete[] _blocks;
    };

    void Reset()
    {
        _current = _total;
        memset(_blocks, 0, _total * sizeof(RewindBlock));
    }

    RewindBlock *Current()
    {
        return _current == _total ? nullptr : (_blocks + _current);
    }

    RewindBlock *Next()
    {
        LOOP_PLUS_ONE(_current, _total);
        return _blocks + _current;
    };

    RewindBlock *Prev()
    {
        LOOP_MINUS_ONE(_current, _total);
        return _blocks + _current;
    };

private:
    RewindBlock *_blocks;
    size_t _total;
    size_t _current;
};

class Rewind : public ThreadBase
{
public:
    Rewind();
    virtual ~Rewind();
    static int _RewindThread(SceSize args, void *argp);

    bool Init();
    void Deinit();
    void StartRewind();
    void StopRewind();

private:
    void _SaveState();
    void _Rewind();

    bool _SaveFullState(bool from_tmp = false);
    bool _SaveDiffState();

    bool _Serialize(void *data, size_t size);
    bool _UnSerialize(void *data, size_t size);

    bool _rewinding;
    size_t _state_size;
    size_t _aligned_state_size;
    size_t _threshold_size;
    int _next_time;
    uint32_t _count;
    uint8_t *_tmp_buf;

    RewindBlocks _blocks{BLOCK_SIZE};
    RewindContens *_contens;
};