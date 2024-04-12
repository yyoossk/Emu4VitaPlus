#include <stdint.h>
#include "audio_resampler.h"
#include <stdio.h>
#include "global.h"

AudioResampler::AudioResampler(uint32_t in_rate, uint32_t out_rate, AudioOutput *output, AudioBuf *buf)
    : ThreadBase(_ResampleThread),
      _swr_ctx(nullptr),
      _output(output),
      _buf(buf)
{
    LogFunctionName;
    SetRate(in_rate, out_rate);
}

AudioResampler::~AudioResampler()
{
    LogFunctionName;
    swr_free(&_swr_ctx);
}

uint32_t AudioResampler::GetOutSize(uint32_t in_size)
{
    return in_size * _out_rate / _in_rate;
}

void AudioResampler::SetRate(uint32_t in_rate, uint32_t out_rate)
{
    LogFunctionName;
    const AVChannelLayout channel = AV_CHANNEL_LAYOUT_STEREO;
    _in_rate = in_rate;
    _out_rate = out_rate;
    swr_alloc_set_opts2(&_swr_ctx,
                        &channel, AV_SAMPLE_FMT_S16, _out_rate,
                        &channel, AV_SAMPLE_FMT_S16, _in_rate,
                        0, NULL);
    swr_init(_swr_ctx);
}

void AudioResampler::Process(const int16_t *in, uint32_t in_size)
{
    // Lock();
    size_t size = in_size * 2;
    int16_t *write_ptr = _in_buf.WriteAcquire(size);
    memcpy(write_ptr, in, size * sizeof(int16_t));
    _in_buf.WriteRelease(size);
    // Unlock();
    Signal();
}

int AudioResampler::_ResampleThread(SceSize args, void *argp)
{
    LogFunctionName;

    CLASS_POINT(AudioResampler, resampler, argp);

    while (resampler->IsRunning())
    {
        auto in = resampler->_in_buf.ReadAcquire();

        while (in.first == nullptr)
        {
            resampler->Wait();
            in = resampler->_in_buf.ReadAcquire();
        }
        // resampler->Lock();

        // auto in = resampler->_in_buf.ReadAcquire();
        size_t out_size = resampler->GetOutSize(in.second / 2);
        int16_t *out = resampler->_buf->BeginWrite(out_size * 2);
        out_size = swr_convert(resampler->_swr_ctx, (uint8_t **)&out, out_size, (const uint8_t **)&(in.first), in.second / 2);
        resampler->_in_buf.ReadRelease(in.second);
        resampler->_buf->EndWrite(out_size * 2);
        resampler->_output->Signal();
        // resampler->Unlock();
    }

    LogDebug("_Audio_ResempleThreadThread exit");
    sceKernelExitThread(0);

    return 0;
}