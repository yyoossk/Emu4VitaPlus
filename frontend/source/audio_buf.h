#pragma once
#include <string.h>
#include "log.h"

#define AUDIO_OUTPUT_COUNT 256
#define AUDIO_BUF_BLOCK_SIZE (AUDIO_OUTPUT_COUNT << 1)
#define AUDIO_BUF_TOTAL_SIZE (AUDIO_BUF_BLOCK_SIZE << 9)

class AudioBuf
{
public:
    AudioBuf(size_t size = AUDIO_BUF_TOTAL_SIZE)
        : _size(size),
          _read_pos(0),
          _write_pos(0)
    {
        _buf = new int16_t[_size];
        sceKernelCreateLwMutex(&_mutex, "buf_mutex", 0, 1, nullptr);
    };

    virtual ~AudioBuf()
    {
        delete[] _buf;
        sceKernelDeleteLwMutex(&_mutex);
    };

    bool Write(const int16_t *data, size_t size)
    {
        // LogDebug("Write %08x %d %d %d", data, _size, _read_pos, _write_pos);
        bool block_full;
        if (_write_pos + size < _size)
        {
            memcpy(_buf + _write_pos, data, size * sizeof(int16_t));
            _write_pos += size;
            block_full = (_write_pos - _read_pos >= AUDIO_BUF_BLOCK_SIZE);
        }
        else
        {
            size_t first_size = (_size - _write_pos);
            size_t second_size = size - first_size;
            memcpy(_buf + _write_pos, data, first_size * sizeof(int16_t));
            memcpy(_buf, data + first_size, second_size * sizeof(int16_t));
            _write_pos = second_size;
            block_full = true;
        }
        //     LogDebug("%d %d", _read_pos, _write_pos);
        return block_full;
    };

    inline int16_t *Read()
    {
        if (_write_pos - _read_pos < AUDIO_BUF_BLOCK_SIZE)
            return nullptr;

        LogDebug("Read %d %d %d", _size, _read_pos, _write_pos);

        int16_t *buf = _buf + _read_pos;
        _read_pos += AUDIO_BUF_BLOCK_SIZE;
        if (_read_pos >= AUDIO_BUF_TOTAL_SIZE)
        {
            _read_pos = 0;
        }
        return buf;
    }

    void Lock() { sceKernelLockLwMutex(&_mutex, 1, nullptr); };
    void Unlock() { sceKernelUnlockLwMutex(&_mutex, 1); };

private:
    SceKernelLwMutexWork _mutex;
    int16_t *_buf;
    size_t _size;
    size_t _read_pos;
    size_t _write_pos;
};