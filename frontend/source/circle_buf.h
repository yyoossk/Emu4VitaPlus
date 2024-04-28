#pragma once
#include <atomic>
#include <string.h>
#include "log.h"

template <typename T, size_t SIZE>
class CircleBuf
{
public:
    CircleBuf() : _read_pos(0),
                  _write_pos(0),
                  _tmp(nullptr),
                  _tmp_size(0),
                  _continue_write(true){};

    virtual ~CircleBuf()
    {
        if (_tmp != nullptr)
        {
            delete[] _tmp;
        }
    };

    T *WriteBegin(size_t size)
    {
        const size_t write_pos = _write_pos.load(std::memory_order_relaxed);
        const size_t read_pos = _read_pos.load(std::memory_order_acquire);
        if (size > FreeSize(write_pos, read_pos))
        {
            return nullptr;
        }

        _continue_write = write_pos + size < SIZE;

        return _continue_write ? _buf + write_pos : _GetTmpBuf(size);
    };

    void WriteEnd(size_t size)
    {
        size_t write_pos = _write_pos.load(std::memory_order_relaxed);
        if (_continue_write)
        {
            write_pos += size;
            _write_pos.store(write_pos, std::memory_order_release);
        }
        else
        {
            Write(_tmp, size);
        }
    }

    bool Write(const T *data, size_t size)
    {
        size_t write_pos = _write_pos.load(std::memory_order_relaxed);
        const size_t read_pos = _read_pos.load(std::memory_order_acquire);
        if (size > FreeSize(write_pos, read_pos))
        {
            return false;
        }

        if (write_pos + size < SIZE)
        {
            memcpy(_buf + write_pos, data, size * sizeof(T));
            write_pos += size;
        }
        else
        {
            size_t first_size = (SIZE - write_pos);
            size_t second_size = size - first_size;
            memcpy(_buf + write_pos, data, first_size * sizeof(T));
            memcpy(_buf, data + first_size, second_size * sizeof(T));
            write_pos = second_size;
        }

        _write_pos.store(write_pos, std::memory_order_release);

        return true;
    };

    T *ReadBegin(size_t *size)
    {
        const size_t write_pos = _write_pos.load(std::memory_order_acquire);
        const size_t read_pos = _read_pos.load(std::memory_order_relaxed);

        if (read_pos == write_pos)
        {
            *size = 0;
            return nullptr;
        }

        if (read_pos < write_pos)
        {
            *size = write_pos - read_pos;
        }
        else
        {
            *size = SIZE - read_pos;
        }

        return _buf + read_pos;
    };

    void ReadEnd(size_t size)
    {
        size_t read_pos = _read_pos.load(std::memory_order_relaxed) + size;
        if (read_pos == SIZE)
        {
            read_pos = 0;
        }
        _read_pos.store(read_pos, std::memory_order_release);
    };

    T *Read(size_t size)
    {
// LogDebug("%d %d %d %d", _read_pos, _write_pos, _write_pos - _read_pos, ((_write_pos - _read_pos) & (_total_size - 1)) < _block_size);
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
        if ((SIZE / size) * size != SIZE)
        {
            LogError("SIZE must be a multiple of size.");
        }
#endif
        if (AvailableSize() < size)
            return nullptr;

        size_t read_pos = _read_pos.load(std::memory_order_relaxed);
        T *buf = _buf + read_pos;

        // LogDebug("read_pos %d %d", read_pos, SIZE);
        read_pos += size;
        if (read_pos >= SIZE)
        {
            read_pos = 0;
        }
        _read_pos.store(read_pos, std::memory_order_release);

        return buf;
    }

    // the size can be written
    size_t FreeSize(const size_t write_pos, const size_t read_pos)
    {
        if (read_pos > write_pos)
        {
            return read_pos - write_pos;
        }
        else
        {
            return (SIZE - (write_pos - read_pos));
        }
    };

    // the size can be read
    size_t AvailableSize()
    {
        const size_t write_pos = _write_pos.load(std::memory_order_relaxed);
        const size_t read_pos = _read_pos.load(std::memory_order_relaxed);
        return SIZE - FreeSize(write_pos, read_pos);
    }

protected:
    T *_GetTmpBuf(size_t size)
    {
        if (size > _tmp_size)
        {
            if (_tmp != nullptr)
                delete[] _tmp;
            _tmp_size = size * 2;
            _tmp = new T[_tmp_size];
        }

        return _tmp;
    }

    T _buf[SIZE];
    std::atomic_size_t _read_pos;
    std::atomic_size_t _write_pos;

    T *_tmp;
    size_t _tmp_size;

    bool _continue_write;
};