#include <psp2/audioout.h>
#include "audio_output.h"
#include "log.h"

AudioOutput::AudioOutput(uint32_t sample_size, uint32_t sample_rate, AudioBuf *buf)
    : ThreadBase(_AudioThread),
      _sample_size(sample_size * 2),
      _out_buf(buf)
{
    _port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_VOICE, sample_size, sample_rate, SCE_AUDIO_OUT_MODE_STEREO);
}

AudioOutput::~AudioOutput()
{
    sceAudioOutReleasePort(_port);
}

void AudioOutput::SetRate(uint32_t sample_size, uint32_t sample_rate)
{
    _sample_size = sample_size * 2;
    sceAudioOutSetConfig(_port, sample_size, sample_rate, SCE_AUDIO_OUT_MODE_STEREO);
}

int AudioOutput::_AudioThread(SceSize args, void *argp)
{
    LogFunctionName;

    CLASS_POINTER(AudioOutput, output, argp);
    int16_t *buf;
    while (output->IsRunning())
    {
        while ((buf = output->_out_buf->Read(AUDIO_OUTPUT_BLOCK_SIZE)) == nullptr)
        {
            output->Wait();
            if (!output->IsRunning())
                break;
        }
        sceAudioOutOutput(output->_port, buf);
    }

    LogDebug("_AudioThread exit");
    sceKernelExitThread(0);

    return 0;
}