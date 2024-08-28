#pragma once
#include <libretro.h>
#include "audio_define.h"
#include "audio_output.h"
#include "audio_buf.h"
#include "audio_resampler.h"

class Audio
{
public:
    Audio();
    virtual ~Audio();

    void SetSampleRate(uint32_t sample_rate);
    size_t SendAudioSample(const int16_t *data, size_t frames);
    bool Inited() { return _output != nullptr; };
    void SetBufStatusCallback(retro_audio_buffer_status_callback_t callback) { _buf_status_callback = callback; };

private:
    bool _GetSuitableSampleRate(uint32_t sample_rate, uint32_t *out_sample_rate);

    uint32_t _in_sample_rate;
    uint32_t _out_sample_rate;

    AudioResampler *_resampler;
    AudioOutput *_output;
    retro_audio_buffer_status_callback_t _buf_status_callback;
    AudioBuf _out_buf{AUDIO_OUTPUT_BUF_SIZE};
};