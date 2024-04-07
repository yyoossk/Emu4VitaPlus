#pragma once
#include <stdint.h>
#include "thread_base.h"
#include "log.h"

extern "C"
{
#include <libswresample/swresample.h>
}

#define SOUND_QUALITY 1

class AudioResampler : public ThreadBase
{
public:
    AudioResampler(uint32_t in_rate, uint32_t out_rate, SceKernelLwCondWork *end_cond);
    virtual ~AudioResampler();

    uint32_t GetOutSize(uint32_t in_size);
    int ProcessInt(const int16_t *in, uint32_t in_size, int16_t *out, uint32_t out_size);
    void SetRate(uint32_t in_rate, uint32_t out_rate);

private:
    static int _ResampleThread(SceSize args, void *argp);

    SwrContext *_swr_ctx;
    uint32_t _in_rate, _out_rate;

    SceKernelLwCondWork *_end_cond;
};