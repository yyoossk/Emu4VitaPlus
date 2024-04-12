#pragma once
#include <utility>
#include <stdint.h>
#include <lockfree.hpp>
#include "audio_define.h"
#include "thread_base.h"
#include "audio_output.h"
#include "log.h"

#include <speex/speex_resampler.h>

// extern "C"
// {
// #include <libswresample/swresample.h>
// }

class AudioResampler : public ThreadBase
{
public:
    AudioResampler(uint32_t in_rate, uint32_t out_rate, AudioOutput *output, lockfree::spsc::BipartiteBuf<int16_t, AUDIO_OUTPUT_BUF_SIZE> *out_buf);
    virtual ~AudioResampler();

    uint32_t GetOutSize(uint32_t in_size);

    void SetRate(uint32_t in_rate, uint32_t out_rate);
    void Process(const int16_t *in, uint32_t in_size);

private:
    static int _ResampleThread(SceSize args, void *argp);

    // SwrContext *_swr_ctx;
    SpeexResamplerState *_speex;
    uint32_t _in_rate, _out_rate;
    lockfree::spsc::BipartiteBuf<int16_t, 0x10000> _in_buf;

    AudioOutput *_output;
    lockfree::spsc::BipartiteBuf<int16_t, AUDIO_OUTPUT_BUF_SIZE> *_out_buf;
};