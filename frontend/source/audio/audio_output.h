#pragma once
#include "thread_base.h"
#include "audio_buf.h"

class AudioOutput : public ThreadBase
{
public:
    AudioOutput(uint32_t sample_size, uint32_t sample_rate, AudioBuf *buf);
    virtual ~AudioOutput();
    void SetRate(uint32_t sample_size, uint32_t sample_rate);

private:
    static int
    _AudioThread(SceSize args, void *argp);

    int _port;
    AudioBuf *_buf;
};