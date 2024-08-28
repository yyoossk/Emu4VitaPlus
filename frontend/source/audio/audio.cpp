#include <speex/speex_resampler.h>
#include <psp2/audioout.h>
#include "audio.h"
#include "config.h"
#include "log.h"

const uint32_t SAMPLE_RATES[] = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};

Audio::Audio()
    : _in_sample_rate(0),
      _resampler(nullptr),
      _output(nullptr),
      _buf_status_callback(nullptr)
{
    LogFunctionName;

    // SetSampleRate(sample_rate);
    // LogDebug("_in_sample_rate: %d _out_sample_rate:%d _resampler:%08x", _in_sample_rate, _out_sample_rate, _resampler);
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
    LogDebug("  sample_rate: %d", sample_rate);
    if (_in_sample_rate == sample_rate || sample_rate == 0)
    {
        return;
    }

    _in_sample_rate = sample_rate;

    bool need_resample = !_GetSuitableSampleRate(sample_rate, &_out_sample_rate);
    LogDebug("  need_resample: %d _out_sample_rate: %d", need_resample, _out_sample_rate);

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
        else if (SAMPLE_RATES[i] < sample_rate)
        {
            *out_sample_rate = SAMPLE_RATES[i];
        }
    }

    return false;
}

size_t Audio::SendAudioSample(const int16_t *data, size_t frames)
{
    static uint32_t status_count = 0;
    if (_resampler != nullptr)
    {
        _resampler->Process(data, frames);
        if (_buf_status_callback)
        {
            if (status_count & 0x7f == 0)
            {
                size_t occupancy = _resampler->GetInBufOccupancy();
                _buf_status_callback(gConfig->mute, occupancy, occupancy < 25);
                LogDebug("  _buf_status_callback: %d %d %d", gConfig->mute, occupancy, occupancy < 25);
            }
            status_count++;
        }
    }
    else
    {
        _out_buf.Write(data, frames * 2);
        _output->Signal();
    }

    return frames;
}