#include <psp2/audioout.h>
#include "audio_output.h"
#include "log.h"

AudioOutput::AudioOutput(uint32_t sample_size, uint32_t sample_rate, AudioBuf *buf)
    : ThreadBase(_AudioThread),
      _buf(buf)
{
    _port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_VOICE, sample_size, sample_rate, SCE_AUDIO_OUT_MODE_STEREO);
}

AudioOutput::~AudioOutput()
{
    sceAudioOutReleasePort(_port);
}

void AudioOutput::SetRate(uint32_t sample_size, uint32_t sample_rate)
{
    sceAudioOutSetConfig(_port, sample_size, sample_rate, SCE_AUDIO_OUT_MODE_STEREO);
}

int AudioOutput::_AudioThread(SceSize args, void *argp)
{
    LogFunctionName;

    CLASS_POINT(AudioOutput, output, argp);
    int16_t *buf;
    while (output->IsRunning())
    {
        // output->Lock();
        while ((buf = output->_buf->Read()) == nullptr)
        {
            output->Wait();
            //  LogDebug("audio wait resampler");
        }
        // output->Unlock();
        sceAudioOutOutput(output->_port, buf);
    }

    LogDebug("_AudioThread exit");
    sceKernelExitThread(0);

    return 0;
}