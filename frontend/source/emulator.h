#pragma once
#include <vector>
#include <libretro.h>
#include "texture_buf.h"
#include "audio.h"
#include "delay.h"

class Emulator
{
public:
    Emulator();
    virtual ~Emulator();

    bool LoadGame(const char *path);
    void UnloadGame();
    void Run();
    void Show();
    void SetSpeed(double speed);

    const char *GetValidExtensions() const { return _info.valid_extensions; };
    double GetSampleRate() const { return _av_info.timing.sample_rate; };

    friend bool EnvironmentCallback(unsigned cmd, void *data);
    friend void VideoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch);
    friend size_t AudioSampleBatchCallback(const int16_t *data, size_t frames);
    friend void InputPollCallback();
    friend int16_t InputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id);

private:
    retro_system_info _info;
    retro_system_av_info _av_info;
    SceGxmTextureFormat _video_pixel_format;
    TextureBuf *_texture_buf;
    Audio *_audio;

    double _speed;
    Delay _delay;

    void _SetPixelFormat(retro_pixel_format format);
};