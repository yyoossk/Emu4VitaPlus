#pragma once
#include <stdint.h>
#include <speex/speex_resampler.h>

class AudioResampler
{
public:
    AudioResampler(uint32_t in_rate, uint32_t out_rate);
    virtual ~AudioResampler();

private:
    SpeexResamplerState *_speex;
};