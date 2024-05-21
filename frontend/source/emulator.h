#pragma once
#include <string>
#include <libretro.h>
#include <stdint.h>
#include "texture_buf.h"
#include "audio.h"
#include "delay.h"
#include "input.h"

struct Rect
{
    float x = 0.f;
    float y = 0.f;
    float width = 0.f;
    float height = 0.f;
};

class Emulator
{
public:
    Emulator();
    virtual ~Emulator();

    bool LoadGame(const char *path);
    void UnloadGame();
    void Reset();
    void Run();
    void Show();
    void SetSpeed(double speed);
    const char *CurrentName() { return _current_name.c_str(); };

    const char *GetValidExtensions() const { return _info.valid_extensions; };
    double GetSampleRate() const { return _av_info.timing.sample_rate; };
    float GetAspectRatio() { return _av_info.geometry.aspect_ratio; };

    friend bool
    EnvironmentCallback(unsigned cmd, void *data);
    friend void VideoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch);
    friend size_t AudioSampleBatchCallback(const int16_t *data, size_t frames);
    friend void InputPollCallback();
    friend int16_t InputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id);
    bool SaveScreenShot(const char *name, size_t height = 0);

private:
    std::string _current_name;
    retro_system_info _info;
    retro_system_av_info _av_info;

    SceGxmTextureFormat _video_pixel_format;
    TextureBuf *_texture_buf;
    Rect _video_rect;

    Audio *_audio;
    Input _input;
    uint32_t _keys[16];
    double _speed;
    Delay _delay;

    void _SetPixelFormat(retro_pixel_format format);
    void _SetVideoSize(uint32_t width, uint32_t height);
    void _SetupKeys();
    void _OnPsButton(Input *input);
    void _LoadCoreOptions(retro_core_options_intl *options);
};

extern Emulator *gEmulator;