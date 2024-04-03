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
          _out_rate(out_rate)
    {
        LogFunctionName;
        _speex = speex_resampler_init(1, in_rate, out_rate, SOUND_QUALITY, nullptr);
    };

    virtual ~AudioResampler()
    {
        LogFunctionName;
        speex_resampler_destroy(_speex);
    };

    uint32_t GetOutSize(uint32_t in_size)
    {
        return in_size * _out_rate / _in_rate;
    };

    int ProcessInt(const int16_t *in, uint32_t *in_size, int16_t *out, uint32_t *out_size)
    {
        return speex_resampler_process_int(_speex, 0, in, in_size, out, out_size);
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
};