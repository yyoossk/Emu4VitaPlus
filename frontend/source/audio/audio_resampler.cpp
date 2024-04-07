#include "audio_resampler.h"

AudioResampler::AudioResampler(uint32_t in_rate, uint32_t out_rate, SceKernelLwCondWork *end_cond)
    : ThreadBase(_ResampleThread),
      _swr_ctx(nullptr),
      _end_cond(end_cond)
{
    LogFunctionName;
    SetRate(in_rate, out_rate);
}

AudioResampler::~AudioResampler()
{
    LogFunctionName;
    swr_free(&_swr_ctx);
}

uint32_t AudioResampler::GetOutSize(uint32_t in_size)
{
    return in_size * _out_rate / _in_rate;
}

int AudioResampler::ProcessInt(const int16_t *in, uint32_t in_size, int16_t *out, uint32_t out_size)
{
    return swr_convert(_swr_ctx, (uint8_t **)&out, out_size, (const uint8_t **)&in, in_size);
}

void AudioResampler::SetRate(uint32_t in_rate, uint32_t out_rate)
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
}

int AudioResampler::_ResampleThread(SceSize args, void *argp)
{
    return 0;
}