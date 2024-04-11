#include "audio_resampler.h"

AudioResampler::AudioResampler(uint32_t in_rate, uint32_t out_rate, AudioOutput *output, AudioBuf *buf)
    : ThreadBase(_ResampleThread),
      _swr_ctx(nullptr),
      _in_buf(nullptr),
      _in_buf_size(0),
      _in_size(0),
      _output(output),
      _buf(buf)
{
    LogFunctionName;
    SetRate(in_rate, out_rate);

    _resample_done_sema = sceKernelCreateSema("resample_done_sema", 0, 1, 1, NULL);
}

AudioResampler::~AudioResampler()
{
    LogFunctionName;
    swr_free(&_swr_ctx);
    sceKernelDeleteSema(_resample_done_sema);
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
    sceKernelWaitSema(_resample_done_sema, 1, NULL);
    Lock();
    _in_size = in_size;
    if (_in_buf_size < _in_size)
    {
        delete[] _in_buf;
        _in_buf = nullptr;
    }

    if (_in_buf == nullptr)
    {
        _in_buf_size = _in_size << 4;
        _in_buf = new int16_t[_in_buf_size];
    }

    memcpy(_in_buf, in, in_size * sizeof(int16_t) * 2);
    Unlock();
    Signal();
}

int AudioResampler::_ResampleThread(SceSize args, void *argp)
{
    LogFunctionName;

    CLASS_POINT(AudioResampler, resampler, argp);

    while (resampler->IsRunning())
    {
        resampler->Wait();
        resampler->Lock();
        size_t out_size = resampler->GetOutSize(resampler->_in_size);
        int16_t *out = resampler->_buf->BeginWrite(out_size * 2);
        size_t in_size = resampler->_in_size;
        int16_t *in = resampler->_in_buf;
        out_size = swr_convert(resampler->_swr_ctx, (uint8_t **)&out, out_size, (const uint8_t **)&in, in_size);
        resampler->_buf->EndWrite(out_size * 2);
        resampler->_output->Signal();
        resampler->Unlock();
        sceKernelSignalSema(resampler->_resample_done_sema, 1);
    }

    LogDebug("_Audio_ResempleThreadThread exit");
    sceKernelExitThread(0);

    return 0;
}