#pragma once
#include <string.h>
#include "log.h"

#define AUDIO_OUTPUT_COUNT 512

template <size_t SAMPLES = AUDIO_OUTPUT_COUNT, bool STEREO = true>
class AudioBuf
{
public:
    AudioBuf()
        : _read_pos(0),
          _write_pos(0),
          _tmp_buf(nullptr),
          _tmp_buf_size(0)
    {
        LogFunctionName;
        _buf = new int16_t[_total_size];
    };

    virtual ~AudioBuf()
    {
        LogFunctionName;
        delete[] _buf;
    };

    int16_t *BeginWrite(size_t size)
    {
        _continue_write = _write_pos + size < _total_size;

        if (_continue_write)
        {
            return _buf + _write_pos;
        }
        else if (_tmp_buf == nullptr)
        {
        NEW_TMP_BUF:
            _tmp_buf_size = size << 1;
            _tmp_buf = new int16_t[_tmp_buf_size];
            return _tmp_buf;
        }
        else if (_tmp_buf_size < size)
        {
            delete[] _tmp_buf;
            goto NEW_TMP_BUF;
        }
        else
        {
            return _tmp_buf;
        }
    }

    void EndWrite(size_t size)
    {
        if (_continue_write)
        {
            _write_pos += size;
        }
        else
        {
            Write(_tmp_buf, size);
        }
    }

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
    const size_t _block_size = STEREO ? SAMPLES << 1 : SAMPLES;
    const size_t _total_size = _block_size << 8;

    int16_t *_buf;
    size_t _read_pos;
    size_t _write_pos;

    int16_t *_tmp_buf;
    size_t _tmp_buf_size;

    bool _continue_write;
};