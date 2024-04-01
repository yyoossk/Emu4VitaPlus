#pragma once
#include <speex/speex_resampler.h>
#include "thread_base.h"
#include "audio_resampler.h"

class Audio : public ThreadBase
{
public:
    Audio(uint32_t sample_rate);
    virtual ~Audio();

private:
    static int _PlayThread(SceSize args, void *argp);

    bool _GetSuitableSampleRate(uint32_t sample_rate, uint32_t *out_sample_rate);

    uint32_t _in_sample_rate;
    uint32_t _out_sample_rate;

    int _output_port;
    AudioResampler *_resampler;
};