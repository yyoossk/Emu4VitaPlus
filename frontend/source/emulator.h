#pragma once
#include <vector>
#include <libretro.h>
#include "texture_buf.h"
#include "delay.h"

class Emulator
{
public:
    Emulator();
    virtual ~Emulator();

    bool EnvironmentCallback(unsigned cmd, void *data);
    void VideoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch);
    size_t AudioSampleBatchCallback(const int16_t *data, size_t frames);
    void InputPollCallback();
    int16_t InputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id);

    bool LoadGame(const char *path);
    void UnloadGame();
    void Run();
    void Show();
    void SetSpeed(double speed);

    const char *GetValidExtensions() const { return _info.valid_extensions; };

private:
    retro_system_info _info;
    retro_system_av_info _av_info;
    SceGxmTextureFormat _video_pixel_format;
    TextureBuf *_texture_buf;
    double _speed;
    Delay _delay;

    void _SetPixelFormat(retro_pixel_format format);
};