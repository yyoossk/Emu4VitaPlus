#include <psp2/audioout.h>
#include "audio_output.h"
#include "log.h"

AudioOutput::AudioOutput(uint32_t sample_size, uint32_t sample_rate, lockfree::spsc::BipartiteBuf<int16_t, AUDIO_OUTPUT_BUF_SIZE> *out_buf)
    : ThreadBase(_AudioThread),
      _sample_size(sample_size * 2),
      _out_buf(out_buf)
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
    while (output->IsRunning())
    {
        // output->Lock();
        auto out = output->_out_buf->ReadAcquire();
        while (out.second < output->_sample_size)
        {
            LogDebug("wait %08x %u", out.first, out.second);
            if (out.first != nullptr)
                output->_out_buf->ReadRelease(0);
            output->Wait();
            LogDebug("audio wait resampler");
            out = output->_out_buf->ReadAcquire();
        }
        // output->Unlock();
        LogDebug("%08x %u", out.first, out.second);
        sceAudioOutOutput(output->_port, out.first);
        output->_out_buf->ReadRelease(output->_sample_size);
    }

    LogDebug("_AudioThread exit");
    sceKernelExitThread(0);

    return 0;
}