#pragma once
#include <string.h>
#include "log.h"

#define AUDIO_OUTPUT_COUNT 256

template <size_t SAMPLES = AUDIO_OUTPUT_COUNT, bool STEREO = true>
class AudioBuf
{
public:
    AudioBuf() : _read_pos(0), _write_pos(0)
    {
        LogFunctionName;
        _buf = new int16_t[_total_size];
    };

    virtual ~AudioBuf()
    {
        LogFunctionName;
        delete[] _buf;
    };

    void Write(const int16_t *data, size_t size)
    {
        if (_write_pos + size < _total_size)
        {
            memcpy(_buf + _write_pos, data, size * sizeof(int16_t));
            _write_pos += size;
        }
        else
        {
            size_t first_size = (_total_size - _write_pos);
            size_t second_size = size - first_size;
            memcpy(_buf + _write_pos, data, first_size * sizeof(int16_t));
            memcpy(_buf, data + first_size, second_size * sizeof(int16_t));
            _write_pos = second_size;
        }
    };

    int16_t *Read()
    {
        if (((_write_pos - _read_pos) & (_total_size - 1)) < _block_size)
            return nullptr;

        int16_t *buf = _buf + _read_pos;
        _read_pos += _block_size;
        if (_read_pos >= _total_size)
        {
            _read_pos = 0;
        }
        return buf;
    }

private:
    const size_t _block_size = STEREO ? SAMPLES : SAMPLES << 1;
    const size_t _total_size = _block_size << 8;

    int16_t *_buf;
    size_t _read_pos;
    size_t _write_pos;
};