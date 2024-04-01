#pragma once
#include <stdint.h>
#include <speex/speex_resampler.h>

#define SOUND_QUALITY 10

class AudioResampler
{
public:
    AudioResampler(uint32_t in_rate, uint32_t out_rate) :
    {
        _speex = speex_resampler_init(1, in_rate, out_rate, SOUND_QUALITY, nullptr);
    };

    virtual ~AudioResampler()
    {
        speex_resampler_destroy(_speex);
    };

    int ProcessInt(const int16_t *in, uint32_t *in_size, int16_t *out, uint32_t *out_size)
    {
        return speex_resampler_process_int(_speex, 0, in, in_size, out, out_size);
    };

    uint32_t GetInRate(){return _in_rate};
    uint32_t GetOutRate() { return _out_rate; }

private:
    SpeexResamplerState *_speex;
    uint32_t _in_rate, _out_rate;
};