#pragma once
#include "thread_base.h"
#include "audio_resampler.h"
#include "audio_buf.h"

class Audio : public ThreadBase
{
public:
    Audio(uint32_t sample_rate);
    virtual ~Audio();

    void SetSampleRate(uint32_t sample_rate);
    size_t SendAudioSample(const int16_t *data, size_t frames);

private:
    static int _AudioThread(SceSize args, void *argp);

    bool _GetSuitableSampleRate(uint32_t sample_rate, uint32_t *out_sample_rate);

    uint32_t _in_sample_rate;
    uint32_t _out_sample_rate;

    int _output_port;
    AudioResampler *_resampler;

    AudioBuf<> *_buf;
};