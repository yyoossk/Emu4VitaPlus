#pragma once
#include <stdint.h>
#include <string.h>
#include "thread_base.h"
#include "utils.h"

#define BLOCK_SIZE 0x400

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
    RewindContent *buf;
    uint32_t size;

    bool IsValid();
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
    };

private:
    uint8_t *_data;
    size_t _total_bytes;
    size_t _current;
};

class RewindBlocks
{
public:
    RewindBlocks(size_t total)
        : _total(total)
    {
        _blocks = new RewindBlock[total];
        Reset();
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

private:
    bool _SaveFullState();

    size_t _state_size;
    size_t _aligned_state_size;
    size_t _threshold_size;

    RewindBlocks _blocks{BLOCK_SIZE};
};