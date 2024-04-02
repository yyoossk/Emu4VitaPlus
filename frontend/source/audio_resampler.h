#pragma once
#include <stdint.h>
#include <speex/speex_resampler.h>
#include "log.h"

#define SOUND_QUALITY 1

class AudioResampler
{
public:
    AudioResampler(uint32_t in_rate, uint32_t out_rate)
        : _in_rate(in_rate),
          _out_rate(out_rate),
          _out_buf(nullptr),
          _out_buf_size(0)
    {
        LogFunctionName;
        _speex = speex_resampler_init(1, in_rate, out_rate, SOUND_QUALITY, nullptr);
    };

    virtual ~AudioResampler()
    {
        LogFunctionName;
        speex_resampler_destroy(_speex);
        if (_out_buf != nullptr)
            delete[] _out_buf;
    };

    const int16_t *ProcessInt(const int16_t *in, uint32_t *in_size, uint32_t *out_size)
    {

        *out_size = (*in_size) * _out_rate / _in_rate;
        if (_out_buf == nullptr)
        {
            _out_buf_size = *out_size * 2;
            _out_buf = new int16_t[_out_buf_size];
        }
        else if (_out_buf_size < *out_size)
        {
            delete[] _out_buf;
            _out_buf_size = *out_size * 2;
            _out_buf = new int16_t[_out_buf_size];
        }

        int result = speex_resampler_process_int(_speex, 0, in, in_size, _out_buf, out_size);
        LogDebug("%d %d", result, *out_size);
        return _out_buf;
    };

    void SetRate(uint32_t in_rate, uint32_t out_rate)
    {
        LogFunctionName;
        _in_rate = in_rate;
        _out_rate = out_rate;
        speex_resampler_set_rate(_speex, in_rate, out_rate);
    };

private:
    SpeexResamplerState *_speex;
    uint32_t _in_rate, _out_rate;
    int16_t *_out_buf;
    size_t _out_buf_size;
};