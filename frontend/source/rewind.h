#pragma once
#include <stdint.h>
#include "thread_base.h"
#include "circle_buf.h"

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

struct RewindBuf
{
    uint32_t magic;
    uint32_t index;
};

struct RewindBlock
{
    BlockType type;
    uint32_t index;
    RewindBuf *buf;
    uint32_t size;

    bool IsValid();
};

struct RewindFullBuf : RewindBuf
{
    uint8_t buf[];
};

struct RewindDiffBuf : RewindBuf
{
    RewindBlock *full_block;
    uint32_t num;
    DiffArea areas[];
};

class RewindBuf
{
public:
    RewindBuf(size_t total);
    virtual ~RewindBuf();

private:
    uint8_t *_buf;
    size_t _total;
};

class RewindBlocks
{
public:
    RewindBlocks(size_t total);
    virtual ~RewindBlocks();

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

    CircleBuf<RewindBlock> _blocks{BLOCK_SIZE};
    CircleBuf<uint8_t> *_buf;
};