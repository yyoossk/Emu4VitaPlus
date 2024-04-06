#pragma once
#include <stdint.h>
#include "log.h"

extern "C"
{
#include <libswresample/swresample.h>
}

#define SOUND_QUALITY 1

class AudioResampler
{
public:
    AudioResampler(uint32_t in_rate, uint32_t out_rate) : _swr_ctx(nullptr)
    {
        LogFunctionName;
        SetRate(in_rate, out_rate);
    };

    virtual ~AudioResampler()
    {
        LogFunctionName;
        swr_free(&_swr_ctx);
    };

    uint32_t GetOutSize(uint32_t in_size)
    {
        return in_size * _out_rate / _in_rate;
    };

    int ProcessInt(const int16_t *in, uint32_t in_size, int16_t *out, uint32_t out_size)
    {
        return swr_convert(_swr_ctx, (uint8_t **)&out, out_size, (const uint8_t **)&in, in_size);
    };

    void SetRate(uint32_t in_rate, uint32_t out_rate)
    {
        LogFunctionName;
        const AVChannelLayout channel = AV_CHANNEL_LAYOUT_STEREO;
        _in_rate = in_rate;
        _out_rate = out_rate;
        swr_alloc_set_opts2(&_swr_ctx,
                            &channel, AV_SAMPLE_FMT_S16, _out_rate,
                            &channel, AV_SAMPLE_FMT_S16, _in_rate,
                            0, NULL);
        swr_init(_swr_ctx);
    };

private:
    SwrContext *_swr_ctx;
    uint32_t _in_rate, _out_rate;
};