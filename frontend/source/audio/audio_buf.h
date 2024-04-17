#pragma once
#include "circle_buf.h"
#include "audio_define.h"

typedef CircleBuf<int16_t, AUDIO_OUTPUT_BUF_SIZE> AudioBuf;
