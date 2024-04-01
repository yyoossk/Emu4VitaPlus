#pragma once
#include "buf_base.h"

#define AUDIO_BUF_COUNT 4
#define AUDIO_OUTPUT_COUNT 256

class AudioBuf : public BufBase<uint16_t *, AUDIO_BUF_COUNT>
{
public:
    AudioBuf()
    {
        for (auto &buf : _buf)
        {
            buf = new uint16_t[(AUDIO_OUTPUT_COUNT * 2) << 8];
        }
    };

    virtual ~AudioBuf()
    {
        for (auto &buf : _buf)
        {
            delete[] buf;
        }
    };
};