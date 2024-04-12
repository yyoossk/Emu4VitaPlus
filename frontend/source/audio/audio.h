#pragma once
#include <lockfree.hpp>
#include "audio_define.h"
#include "audio_output.h"
#include "audio_resampler.h"

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

    lockfree::spsc::BipartiteBuf<int16_t, AUDIO_OUTPUT_BUF_SIZE> _out_buf;
};