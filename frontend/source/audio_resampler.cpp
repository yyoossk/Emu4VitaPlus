#include "audio_resampler.h"
#define SOUND_QUALITY 10

AudioResampler::AudioResampler(uint32_t in_rate, uint32_t out_rate)
{
    _speex = speex_resampler_init(1, in_rate, out_rate, SOUND_QUALITY, nullptr);
}

AudioResampler ::~AudioResampler()
{
    speex_resampler_destroy(_speex);
}