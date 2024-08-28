#include <stdint.h>
#include "audio_resampler.h"

AudioResampler::AudioResampler(uint32_t in_rate, uint32_t out_rate, AudioOutput *output, AudioBuf *out_buf)
    : ThreadBase(_ResampleThread),
      _in_rate(in_rate),
      _out_rate(out_rate),
      _output(output),
      _out_buf(out_buf),
#if RESAMPLER == SWR
      _swr_ctx(nullptr)
#elif RESAMPLER == SPEEX
      _speex(nullptr)
#endif
{
    LogFunctionName;
    SetRate(in_rate, out_rate);
}

AudioResampler::~AudioResampler()
{
    LogFunctionName;
#if RESAMPLER == SWR
    if (_swr_ctx != nullptr)
    {
        swr_free(&_swr_ctx);
    }
#elif RESAMPLER == SPEEX
    if (_speex != nullptr)
    {
        speex_resampler_destroy(_speex);
    }
#endif
}

uint32_t AudioResampler::GetOutSize(uint32_t in_size)
{
    return in_size * _out_rate / _in_rate;
}

void AudioResampler::SetRate(uint32_t in_rate, uint32_t out_rate)
{
    LogFunctionName;

    _in_rate = in_rate;
    _out_rate = out_rate;

#if RESAMPLER == SWR
    if (_swr_ctx != nullptr)
    {
        swr_free(&_swr_ctx);
    }
    const AVChannelLayout channel = AV_CHANNEL_LAYOUT_STEREO;
    swr_alloc_set_opts2(&_swr_ctx,
                        &channel, AV_SAMPLE_FMT_S16, _out_rate,
                        &channel, AV_SAMPLE_FMT_S16, _in_rate,
                        0, NULL);
    swr_init(_swr_ctx);
#elif RESAMPLER == SPEEX
    if (_speex != nullptr)
    {
        speex_resampler_destroy(_speex);
    }

    _speex = speex_resampler_init(1, _in_rate, _out_rate, SOUND_QUALITY, nullptr);
#endif
}

void AudioResampler::Process(const int16_t *in, uint32_t in_size)
{
    size_t size = in_size * 2;
    int16_t *write_ptr = _in_buf.WriteBegin(size);
    if (write_ptr)
    {
        memcpy(write_ptr, in, size * sizeof(int16_t));
        _in_buf.WriteEnd(size);
        Signal();
    }
}

int AudioResampler::_ResampleThread(SceSize args, void *argp)
{
    LogFunctionName;

    CLASS_POINTER(AudioResampler, resampler, argp);

    while (resampler->IsRunning())
    {
        size_t in_size;
        int16_t *in = resampler->_in_buf.ReadBegin(&in_size);

        while (in == nullptr)
        {
            resampler->Wait();
            in = resampler->_in_buf.ReadBegin(&in_size);
        }

        size_t out_size = resampler->GetOutSize(in_size); // swr_get_out_samples(resampler->_swr_ctx, in_size / 2);
        int16_t *out = resampler->_out_buf->WriteBegin(out_size);
#if RESAMPLER == SWR
        out_size = swr_convert(resampler->_swr_ctx, (uint8_t **)&out, out_size / 2, (const uint8_t **)&in, in_size / 2);
#elif RESAMPLER == SPEEX
        speex_resampler_process_int(resampler->_speex, 0, in, &in_size, out, &out_size);
#endif
        resampler->_in_buf.ReadEnd(in_size);
        resampler->_out_buf->WriteEnd(out_size);
        resampler->_output->Signal();

        resampler->LogCpuId("AudioResampler");
    }

    LogDebug("_Audio_ResempleThreadThread exit");
    sceKernelExitThread(0);

    return 0;
}

size_t AudioResampler::GetInBufOccupancy()
{
    return _in_buf.FreeSize() * 100 / _in_buf.TotalSize();
}