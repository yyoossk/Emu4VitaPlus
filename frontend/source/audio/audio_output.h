#pragma once
#include <utility>
#include <lockfree.hpp>
#include "thread_base.h"
#include "audio_define.h"

class AudioOutput : public ThreadBase
{
public:
    AudioOutput(uint32_t sample_size, uint32_t sample_rate, lockfree::spsc::BipartiteBuf<int16_t, AUDIO_OUTPUT_BUF_SIZE> *out_buf);
    virtual ~AudioOutput();
    void SetRate(uint32_t sample_size, uint32_t sample_rate);

private:
    static int
    _AudioThread(SceSize args, void *argp);

    int _port;
    uint32_t _sample_size;
    lockfree::spsc::BipartiteBuf<int16_t, AUDIO_OUTPUT_BUF_SIZE> *_out_buf;
};