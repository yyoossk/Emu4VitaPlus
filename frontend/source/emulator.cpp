
#include <algorithm>
#include <string>
#include <string.h>
#include <stdint.h>
#include "global.h"
#include "emulator.h"
#include "log.h"

static bool _EnvironmentCallback(unsigned cmd, void *data)
{
    return gEmulator->EnvironmentCallback(cmd, data);
}

static void _VideoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch)
{
    return gEmulator->VideoRefreshCallback(data, width, height, pitch);
}

static size_t _AudioSampleBatchCallback(const int16_t *data, size_t frames)
{
    return gEmulator->AudioSampleBatchCallback(data, frames);
}

static void _InputPollCallback()
{
    return gEmulator->InputPollCallback();
}

static int16_t _InputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id)
{
    return gEmulator->InputStateCallback(port, device, index, id);
}

bool Emulator::EnvironmentCallback(unsigned cmd, void *data)
{
    LogFunctionNameLimited;
    LogTrace("cmd: %u", cmd);
    switch (cmd)
    {
    case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
        if (data)
        {
            *(const char **)data = CORE_DIR;
        }
        break;

    case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
        _SetPixelFormat(*(retro_pixel_format *)data);
        break;

    case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
        break;

    case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
        break;

    case RETRO_ENVIRONMENT_GET_VARIABLE:
    {
        retro_variable *var = (retro_variable *)data;
        LogDebug("retro_variable %s", var->key);
    }
    break;

    case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
        *(bool *)data = false;
        break;

    default:
        return false;
    }

    return true;
}

void Emulator::VideoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch)
{
    LogFunctionNameLimited;

    TextureInfo texture_info;
    if (_texture_buf == nullptr)
    {
        _texture_buf = new TextureBuf(_video_pixel_format, width, height);
        _texture_buf->Current(&texture_info);
    }
    else if (_texture_buf->GetWidth() != width || _texture_buf->GetHeight() != height)
    {
        delete _texture_buf;
        _texture_buf = new TextureBuf(_video_pixel_format, width, height);
        _texture_buf->Current(&texture_info);
    }
    else
    {
        _texture_buf->Next(&texture_info);
    }

    _texture_buf->Lock(texture_info.index);

    unsigned out_pitch = vita2d_texture_get_stride(texture_info.texture);
    uint8_t *out = (uint8_t *)vita2d_texture_get_datap(texture_info.texture);
    uint8_t *in = (uint8_t *)data;

    if (pitch == out_pitch)
    {
        memcpy(out, in, pitch * height);
    }
    else
    {
        for (unsigned i = 0; i < height; i++)
        {
            memcpy(out, in, pitch);
            in += pitch;
            out += out_pitch;
        }
    }

    _texture_buf->Unlock(texture_info.index);
}

size_t Emulator::AudioSampleBatchCallback(const int16_t *data, size_t frames)
{
    LogFunctionNameLimited;
    return frames;
}

void Emulator::InputPollCallback()
{
    LogFunctionNameLimited;
}

int16_t Emulator::InputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id)
{
    LogFunctionNameLimited;
    return 0;
}

Emulator::Emulator() : _texture_buf(nullptr), _speed(1.f)
{
    LogFunctionName;

    retro_set_environment(_EnvironmentCallback);
    retro_set_video_refresh(_VideoRefreshCallback);
    retro_set_audio_sample_batch(_AudioSampleBatchCallback);
    retro_set_input_poll(_InputPollCallback);
    retro_set_input_state(_InputStateCallback);

    retro_get_system_info(&_info);
    retro_get_system_av_info(&_av_info);

    retro_init();
}

Emulator::~Emulator()
{
    LogFunctionName;
    if (_texture_buf)
    {
        delete _texture_buf;
    }

    retro_deinit();
}

bool Emulator::LoadGame(const char *path)
{
    LogFunctionName;
    retro_game_info game_info;
    game_info.path = path;
    game_info.data = nullptr;
    game_info.size = 0;
    game_info.meta = nullptr;

    return retro_load_game(&game_info);
}

void Emulator::UnloadGame()
{
    LogFunctionName;
    retro_unload_game();
}

void Emulator::Run()
{
    retro_run();
}

void Emulator::Show()
{
    if (_texture_buf != nullptr)
    {
        TextureInfo texture_info;
        _texture_buf->Current(&texture_info);
        _texture_buf->Lock(texture_info.index);
        vita2d_draw_texture(texture_info.texture, 0, 0);
        _texture_buf->Unlock(texture_info.index);
    }
}

void Emulator::_SetPixelFormat(retro_pixel_format format)
{
    LogFunctionName;

    SceGxmTextureFormat old_format = _video_pixel_format;
    switch (format)
    {
    case RETRO_PIXEL_FORMAT_0RGB1555:
        _video_pixel_format = SCE_GXM_TEXTURE_FORMAT_X1U5U5U5_1RGB;
        break;
    case RETRO_PIXEL_FORMAT_XRGB8888:
        _video_pixel_format = SCE_GXM_TEXTURE_FORMAT_X8U8U8U8_1RGB;
        break;
    case RETRO_PIXEL_FORMAT_RGB565:
        _video_pixel_format = SCE_GXM_TEXTURE_FORMAT_U5U6U5_RGB;
        break;
    default:
        LogWarn("unknown pixel format: %d", format);
        break;
    }

    LogDebug("_video_pixel_format: %d", _video_pixel_format);
    if (_texture_buf != nullptr && old_format != _video_pixel_format)
    {
        delete _texture_buf;
        _texture_buf = nullptr;
    }
}