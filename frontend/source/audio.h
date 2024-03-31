#pragma once
#include "thread_base.h"
#include <speex/speex_resampler.h>

class Audio : public ThreadBase
{
public:
    Audio(uint32_t sample_rate);
    virtual ~Audio();

private:
    static int _PlayThread(SceSize args, void *argp);

    uint32_t _GetSuitableSampleRate(uint32_t sample_rate);

    uint32_t _in_sample_rate;
    uint32_t _out_sample_rate;
    SpeexResamplerState *_speex;

    int _output_port;
};