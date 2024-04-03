#include <speex/speex_resampler.h>
#include <psp2/audioout.h>
#include <string.h>
#include "global.h"
#include "audio.h"
#include "log.h"

#define AUDIO_DEFAULT_SAMPLE_RATE 48000
#define SAMPLE_RATE_NEGLECT 50
const uint32_t SAMPLE_RATES[] = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};

Audio::Audio(uint32_t sample_rate)
    : ThreadBase(_AudioThread),
      _in_sample_rate(sample_rate),
      _resampler(nullptr)
{
    LogFunctionName;

    _buf = new AudioBuf<>;

    if (!_GetSuitableSampleRate(sample_rate, &_out_sample_rate))
    {
        _resampler = new AudioResampler(sample_rate, _out_sample_rate);
    }
    LogDebug("_in_sample_rate: %d _out_sample_rate:%d _resampler:%08x", _in_sample_rate, _out_sample_rate, _resampler);

    _output_port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_VOICE, AUDIO_OUTPUT_COUNT, _out_sample_rate, SCE_AUDIO_OUT_MODE_STEREO);
}

Audio::~Audio()
{
    LogFunctionName;
    sceAudioOutReleasePort(_output_port);
    if (_resampler)
    {
        delete _resampler;
    }
    delete _buf;
}

void Audio::SetSampleRate(uint32_t sample_rate)
{
    LogFunctionName;
    if (_in_sample_rate == sample_rate)
    {
        return;
    }

    if (!_GetSuitableSampleRate(sample_rate, &_out_sample_rate))
    {
        if (_resampler == nullptr)
        {
            _resampler = new AudioResampler(sample_rate, _out_sample_rate);
        }
        else
        {
            _resampler->SetRate(sample_rate, _out_sample_rate);
        }

        sceAudioOutSetConfig(_output_port, AUDIO_OUTPUT_COUNT, _out_sample_rate, SCE_AUDIO_OUT_MODE_STEREO);
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

int Audio::_AudioThread(SceSize args, void *argp)
{
    LogFunctionName;
    Audio *audio = *(Audio **)argp;
    int16_t *buf;
    while (audio->IsRunning())
    {
        audio->Lock();
        while ((buf = audio->_buf->Read()) == nullptr)
        {
            audio->Wait();
        }
        audio->Unlock();
        sceAudioOutOutput(audio->_output_port, buf);
    }
    return 0;
}

size_t Audio::SendAudioSample(const int16_t *data, size_t frames)
{
    size_t in_size = frames * 2;

    const int16_t *in = data;
    if (_resampler != nullptr)
    {
        size_t out_size;
        in = _resampler->ProcessInt(data, &in_size, &out_size);
        in_size = out_size;
    }

    _buf->Write(in, in_size);

    // if (full)
    LogDebug("SendAudioSample lock");
    Lock();
    Signal();
    Unlock();
    LogDebug("SendAudioSample unlock");
    return frames;
}