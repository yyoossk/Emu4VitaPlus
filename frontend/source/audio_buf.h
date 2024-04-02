#pragma once
#include <string.h>
#include "log.h"
#define AUDIO_BUF_COUNT 1

#define AUDIO_BUF_SIZE(S) (S << 9)

class AudioBuf
{
public:
    AudioBuf(size_t size)
        : _size(AUDIO_BUF_SIZE(size)),
          _read_pos(0),
          _write_pos(0),
          _next_read_pos(0)
    {
        _buf = new int16_t[_size];
        sceKernelCreateLwMutex(&_mutex, "buf_mutex", 0, 1, nullptr);
    };

    virtual ~AudioBuf()
    {
        delete[] _buf;
        sceKernelDeleteLwMutex(&_mutex);
    };

    void Write(const int16_t *data, size_t size)
    {
        // LogDebug("Write %d %d %d", _size, _read_pos, _write_pos);
        if (_write_pos + size < _size)
        {
            memcpy(_buf + _write_pos, data, size);
            _write_pos += size;
        }
        else
        {
            size_t first_size = (_size - _write_pos);
            size_t second_size = size - first_size;
            memcpy(_buf + _write_pos, data, first_size * sizeof(int16_t));
            memcpy(_buf, data + first_size, second_size * sizeof(int16_t));
            _write_pos = second_size;
        }
        _next_read_pos = _write_pos;
    };

    int16_t *Read()
    {
        // LogDebug("Read %d %d %d", _size, _read_pos, _write_pos);
        int16_t *buf = _buf + _read_pos;
        _read_pos = _next_read_pos;
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
    size_t _next_read_pos;
};