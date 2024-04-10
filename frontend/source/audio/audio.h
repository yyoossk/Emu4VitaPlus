#pragma once
#include "audio_resampler.h"
#include "audio_output.h"
#include "audio_buf.h"

class Audio
{
public:
    Audio(uint32_t sample_rate);
    virtual ~Audio();

    void SetSampleRate(uint32_t sample_rate);
    size_t SendAudioSample(const int16_t *data, size_t frames);

private:
    bool _GetSuitableSampleRate(uint32_t sample_rate, uint32_t *out_sample_rate);

    uint32_t _in_sample_rate;
    uint32_t _out_sample_rate;

    AudioResampler *_resampler;
    AudioOutput *_output;

    AudioBuf *_buf;
};