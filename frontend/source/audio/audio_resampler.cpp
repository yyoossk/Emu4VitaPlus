#include <stdint.h>
#include "audio_resampler.h"

#define SOUND_QUALITY 1

AudioResampler::AudioResampler(uint32_t in_rate, uint32_t out_rate, AudioOutput *output, lockfree::spsc::BipartiteBuf<int16_t, AUDIO_OUTPUT_BUF_SIZE> *out_buf)
    : ThreadBase(_ResampleThread),
      _in_rate(in_rate),
      _out_rate(out_rate),
      //_swr_ctx(nullptr),
      _output(output),
      _out_buf(out_buf)
{
    LogFunctionName;
    _speex = speex_resampler_init(1, in_rate, out_rate, SOUND_QUALITY, nullptr);
    // SetRate(in_rate, out_rate);
}

AudioResampler::~AudioResampler()
{
    LogFunctionName;
    speex_resampler_destroy(_speex);
    // swr_free(&_swr_ctx);
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
    speex_resampler_set_rate(_speex, in_rate, out_rate);
    // const AVChannelLayout channel = AV_CHANNEL_LAYOUT_STEREO;
    // _in_rate = in_rate;
    // _out_rate = out_rate;
    // swr_alloc_set_opts2(&_swr_ctx,
    //                     &channel, AV_SAMPLE_FMT_S16, _out_rate,
    //                     &channel, AV_SAMPLE_FMT_S16, _in_rate,
    //                     0, NULL);
    // swr_init(_swr_ctx);
}

void AudioResampler::Process(const int16_t *in, uint32_t in_size)
{
    // Lock();
    // size_t out_size = swr_get_out_samples(_swr_ctx, in_size); // GetOutSize(in_size);
    // int16_t *out = _buf->BeginWrite(out_size * 2);

    // // LogDebug("%d %d %d", out_size, in, swr_get_out_samples(_swr_ctx, in_size));

    // out_size = swr_convert(_swr_ctx, (uint8_t **)&out, out_size, (const uint8_t **)&in, in_size);
    // _buf->EndWrite(out_size * 2);
    // _output->Signal();
    LogFunctionName;

    in_size *= 2;
    size_t out_size = GetOutSize(in_size);
    size_t out_size1 = out_size;
    int16_t *out = _out_buf->WriteAcquire(out_size);
    if (out != nullptr)
    {
        speex_resampler_process_int(_speex, 0, in, &in_size, out, &out_size1);
        LogDebug("%d %d", in_size, out_size);
        _out_buf->WriteRelease(out_size);
        _output->Signal();
    }

    // size_t size = in_size * 2;
    // int16_t *write_ptr = _in_buf.WriteAcquire(size);
    // memcpy(write_ptr, in, size * sizeof(int16_t));
    // _in_buf.WriteRelease(size);
    // // Unlock();
    // Signal();
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

        // size_t out_size = swr_get_out_samples(resampler->_swr_ctx, in.second / 2); // resampler->GetOutSize(in.second / 2);
        // int16_t *out = resampler->_buf->BeginWrite(out_size * 2);
        // out_size = swr_convert(resampler->_swr_ctx, (uint8_t **)&out, out_size, (const uint8_t **)&(in.first), in.second / 2);
        // resampler->_in_buf.ReadRelease(in.second);
        // resampler->_buf->EndWrite(out_size * 2);

        size_t out_size = resampler->GetOutSize(in.second);
        size_t out_size1 = out_size;
        size_t in_size1 = in.second;
        int16_t *out = resampler->_out_buf->WriteAcquire(out_size);
        speex_resampler_process_int(resampler->_speex, 0, in.first, &in_size1, out, &out_size1);
        resampler->_in_buf.ReadRelease(in.second);
        resampler->_out_buf->WriteRelease(out_size);
        resampler->_output->Signal();
        // resampler->Unlock();
    }

    LogDebug("_Audio_ResempleThreadThread exit");
    sceKernelExitThread(0);

    return 0;
}