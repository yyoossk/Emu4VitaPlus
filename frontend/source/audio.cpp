#include <speex/speex_resampler.h>
#include <psp2/audioout.h>
#include "audio.h"
#include "log.h"

#define AUDIO_DEFAULT_SAMPLE_RATE 48000
#define SAMPLE_RATE_NEGLECT 50
const uint32_t SAMPLE_RATES[] = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};

#define SOUND_QUALITY 1
#define AUDIO_SAMPLES 256

Audio::Audio(uint32_t sample_rate) : ThreadBase(_PlayThread), _in_sample_rate(sample_rate)
{
    LogFunctionName;
    _out_sample_rate = _GetSuitableSampleRate(sample_rate);
    _speex = speex_resampler_init(1, _in_sample_rate, _out_sample_rate, SOUND_QUALITY, NULL);
    _output_port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_VOICE, AUDIO_SAMPLES, _out_sample_rate, SCE_AUDIO_OUT_MODE_STEREO);

    LogDebug("_in_sample_rate: %d _out_sample_rate:%d", _in_sample_rate, _out_sample_rate);
}

Audio::~Audio()
{
    LogFunctionName;
    sceAudioOutReleasePort(_output_port);
    speex_resampler_destroy(_speex);
}

uint32_t Audio::_GetSuitableSampleRate(uint32_t sample_rate)
{
    LogFunctionName;

    for (size_t i = 0; i < sizeof(SAMPLE_RATES) / sizeof(uint32_t); i++)
    {
        if ((sample_rate >= SAMPLE_RATES[i] - SAMPLE_RATE_NEGLECT) && (sample_rate <= SAMPLE_RATES[i] + SAMPLE_RATE_NEGLECT))
        {
            return SAMPLE_RATES[i];
        }
    }

    return AUDIO_DEFAULT_SAMPLE_RATE;
}

int Audio::_PlayThread(SceSize args, void *argp)
{

    return 0;
}