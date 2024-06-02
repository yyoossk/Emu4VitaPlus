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

#define REWIND_BUF_HEADER \
    uint32_t magic;       \
    uint32_t index;

struct RewindBuf
{
    uint32_t magic;
    uint32_t index;
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

struct RewindBlock
{
    BlockType type;
    uint32_t index;
    RewindBuf *buf;
    uint32_t size;

    bool IsValid();
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
    size_t _state_size;
    size_t _aligned_state_size;
    size_t _threshold_size;

    CircleBuf<RewindBlock, BLOCK_SIZE> _blocks;
};