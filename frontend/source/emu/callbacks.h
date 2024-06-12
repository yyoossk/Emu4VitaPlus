#pragma once
#include <stdint.h>

bool EnvironmentCallback(unsigned cmd, void *data);
void VideoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch);
size_t AudioSampleBatchCallback(const int16_t *data, size_t frames);
void InputPollCallback();
int16_t InputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id);