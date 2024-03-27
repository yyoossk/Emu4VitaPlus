#pragma once
#include <vector>
#include <libretro.h>
#include <vita2d.h>
#include <imgui_vita2d/imgui_vita.h>

#define TEXTURE_BUF_COUNT 4

class VideoTextureBuf
{
public:
    VideoTextureBuf(size_t num = TEXTURE_BUF_COUNT, size_t width = VITA_WIDTH, size_t height = VITA_HEIGHT);
    virtual ~VideoTextureBuf();
    vita2d_texture *Next();

private:
    std::vector<vita2d_texture *> _buf;
};

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

    const char *GetValidExtensions() const { return _info.valid_extensions; };

private:
    retro_system_info _info;
    SceGxmTextureFormat _videoPixelFormat;
    vita2d_texture *_texture_buf[TEXTURE_BUF_COUNT];
};