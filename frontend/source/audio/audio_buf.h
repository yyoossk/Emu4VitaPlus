#pragma once
#include "circle_buf.h"
#include "audio_define.h"
#include "log.h"

class AudioBuf : public CircleBuf<int16_t, (AUDIO_OUTPUT_BLOCK_SIZE << 10)>
{
public:
    int16_t *Read()
    {
        // LogDebug("%d %d %d %d", _read_pos, _write_pos, _write_pos - _read_pos, ((_write_pos - _read_pos) & (_total_size - 1)) < _block_size);

        if (AvailableSize() < AUDIO_OUTPUT_BLOCK_SIZE)
            return nullptr;
        size_t read_pos = _read_pos.load(std::memory_order_relaxed);

        int16_t *buf = _buf + read_pos;
        _read_pos += AUDIO_OUTPUT_BLOCK_SIZE;
        if (_read_pos >= (AUDIO_OUTPUT_BLOCK_SIZE << 10))
        {
            _read_pos = 0;
        }
        return buf;
    }
};