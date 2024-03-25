#include "emulator.h"
#include "log.h"

static bool EnvironmentCallback(unsigned cmd, void *data)
{
    LogFunctionName;
    LogDebug("cmd: %d", cmd);
    return true;
}

static void VideoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch)
{
    LogFunctionName;
}

static size_t AudioSampleBatchCallback(const int16_t *data, size_t frames)
{
    LogFunctionName;
    return frames;
}

static void InputPollCallback()
{
    LogFunctionName;
}

static int16_t InputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id)
{
    LogFunctionName;
    return 0;
}

Emulator::Emulator()
{
    LogFunctionName;

    retro_set_environment(EnvironmentCallback);
    retro_set_video_refresh(VideoRefreshCallback);
    retro_set_audio_sample_batch(AudioSampleBatchCallback);
    retro_set_input_poll(InputPollCallback);
    retro_set_input_state(InputStateCallback);

    retro_get_system_info(&_info);
    LogInfo(_info.valid_extensions);
}

Emulator::~Emulator()
{
    LogFunctionName;
}