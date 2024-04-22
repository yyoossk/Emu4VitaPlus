#include <speex/speex_resampler.h>
#include <psp2/audioout.h>
#include <string.h>
#include "audio.h"
#include "log.h"
#include "global.h"

const uint32_t SAMPLE_RATES[] = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};

Audio::Audio(uint32_t sample_rate)
    : _in_sample_rate(0),
      _resampler(nullptr),
      _output(nullptr)
{
    LogFunctionName;

    SetSampleRate(sample_rate);
    LogDebug("_in_sample_rate: %d _out_sample_rate:%d _resampler:%08x", _in_sample_rate, _out_sample_rate, _resampler);
}

Audio::~Audio()
{
    LogFunctionName;
    if (_resampler)
    {
        delete _resampler;
    }
    delete _output;
}

void Audio::SetSampleRate(uint32_t sample_rate)
{
    LogFunctionName;
    if (_in_sample_rate == sample_rate)
    {
        return;
    }

    _in_sample_rate = sample_rate;

    bool need_resample = !_GetSuitableSampleRate(sample_rate, &_out_sample_rate);
    if (_output == nullptr)
    {
        _output = new AudioOutput(AUDIO_OUTPUT_COUNT, _out_sample_rate, &_out_buf);
        _output->Start();
    }
    else
    {
        _output->SetRate(AUDIO_OUTPUT_COUNT, _out_sample_rate);
    }

    if (need_resample)
    {
        if (_resampler == nullptr)
        {
            _resampler = new AudioResampler(sample_rate, _out_sample_rate, _output, &_out_buf);
            _resampler->Start();
        }
        else
        {
            _resampler->SetRate(sample_rate, _out_sample_rate);
        }
    }
}

bool Audio::_GetSuitableSampleRate(uint32_t sample_rate, uint32_t *out_sample_rate)
{
    LogFunctionName;

    for (size_t i = 0; i < sizeof(SAMPLE_RATES) / sizeof(uint32_t); i++)
    {
        if ((sample_rate >= SAMPLE_RATES[i] - SAMPLE_RATE_NEGLECT) && (sample_rate <= SAMPLE_RATES[i] + SAMPLE_RATE_NEGLECT))
        {
            *out_sample_rate = SAMPLE_RATES[i];
            return true;
        }
    }

    *out_sample_rate = AUDIO_DEFAULT_SAMPLE_RATE;
    return false;
}

size_t Audio::SendAudioSample(const int16_t *data, size_t frames)
{
    if (_resampler != nullptr)
    {
        _resampler->Process(data, frames);
    }
    else
    {
        _out_buf.Write(data, frames * 2);
        _output->Signal();
    }

    return frames;
}