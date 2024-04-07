#pragma once
#include <stdint.h>
#include "log.h"

extern "C"
{
#include <libswresample/swresample.h>
}

#define SOUND_QUALITY 1

class AudioResampler
{
public:
    AudioResampler(uint32_t in_rate, uint32_t out_rate);
    virtual ~AudioResampler();

    uint32_t GetOutSize(uint32_t in_size);
    int ProcessInt(const int16_t *in, uint32_t in_size, int16_t *out, uint32_t out_size);
    void SetRate(uint32_t in_rate, uint32_t out_rate);

private:
    SwrContext *_swr_ctx;
    uint32_t _in_rate, _out_rate;
};