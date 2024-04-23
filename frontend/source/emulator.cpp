#include <algorithm>
#include <string>
#include <string.h>
#include <stdint.h>
#include "emulator.h"
#include "log.h"
#include "global.h"

#define CORE_OPTIONS_VERSION 2

bool EnvironmentCallback(unsigned cmd, void *data)
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
        gEmulator->_SetPixelFormat(*(retro_pixel_format *)data);
        break;

    case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
        break;

    case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
        break;

    case RETRO_ENVIRONMENT_GET_VARIABLE:
    {
        // retro_variable *var = (retro_variable *)data;
        LogDebug("RETRO_ENVIRONMENT_GET_VARIABLE: %s", ((retro_variable *)data)->key);
    }
    break;

    case RETRO_ENVIRONMENT_SET_VARIABLES:
    {
        LogDebug("RETRO_ENVIRONMENT_SET_VARIABLES: %s", ((retro_variable *)data)->key);
    }
    break;

    case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
        *(bool *)data = false;
        break;

    case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
    {
        retro_system_av_info *av_info = (retro_system_av_info *)data;
        // if (_av_info.timing.fps != av_info->timing.fps)
        // {
        //     LogDebug("av_info.timing.fps %d", av_info->timing.fps);
        // }
        memcpy(&gEmulator->_av_info, av_info, sizeof(retro_system_av_info));
        gEmulator->_audio->SetSampleRate(av_info->timing.sample_rate);
    }
    break;

    case RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION:
    {
        if (data)
            *(unsigned *)data = CORE_OPTIONS_VERSION;
    }
    break;

    case RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE:
        if (data)
        {
            *(int *)data = 3;
        }
        break;

    default:
        LogDebug("UNSUPPORTED cmd:%d", cmd);
        return false;
    }

    return true;
}

void VideoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch)
{
    LogFunctionNameLimited;

    vita2d_texture *texture;
    if (gEmulator->_texture_buf == nullptr || gEmulator->_texture_buf->GetWidth() != width || gEmulator->_texture_buf->GetHeight() != height)
    {
        if (gEmulator->_texture_buf != nullptr)
        {
            delete gEmulator->_texture_buf;
        }
        gEmulator->_texture_buf = new TextureBuf(gEmulator->_video_pixel_format, width, height);
        gEmulator->_SetVideoSize(width, height);
    }

    gEmulator->_texture_buf->Lock();
    texture = gEmulator->_texture_buf->Next();

    unsigned out_pitch = vita2d_texture_get_stride(texture);
    uint8_t *out = (uint8_t *)vita2d_texture_get_datap(texture);
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

    gEmulator->_texture_buf->Unlock();
}

size_t AudioSampleBatchCallback(const int16_t *data, size_t frames)
{
    LogFunctionNameLimited;

    return gEmulator->_audio->SendAudioSample(data, frames);
}

void InputPollCallback()
{
    LogFunctionNameLimited;
}

int16_t InputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id)
{
    LogFunctionNameLimited;
    if (device == RETRO_DEVICE_JOYPAD)
    {
        if (id >= 16)
        {
            LogError("InputStateCallback, wrong id %d", id);
            return 0;
        }

        return gEmulator->_input.GetKeyStates() & gEmulator->_keys[id];
    }
    else
    {
        return 0;
    }
}

Emulator::Emulator()
    : _texture_buf(nullptr), _keys{0}
{
    LogFunctionName;

    retro_set_environment(EnvironmentCallback);
    retro_set_video_refresh(VideoRefreshCallback);
    retro_set_audio_sample_batch(AudioSampleBatchCallback);
    retro_set_input_poll(InputPollCallback);
    retro_set_input_state(InputStateCallback);

    retro_init();
    retro_get_system_info(&_info);
    retro_get_system_av_info(&_av_info);

    SetSpeed(1.0);

    retro_set_controller_port_device(0, RETRO_DEVICE_JOYPAD);
    _SetupKeys();

    _audio = new Audio(_av_info.timing.sample_rate);
}

Emulator::~Emulator()
{
    LogFunctionName;

    if (_texture_buf)
    {
        delete _texture_buf;
    }

    if (_audio)
    {
        delete _audio;
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
    _delay.Wait();
    _input.Poll();
    retro_run();
}

void Emulator::Show()
{
    if (_texture_buf == nullptr)
    {
        return;
    }

    _texture_buf->Lock();
    vita2d_draw_texture_part_scale_rotate(_texture_buf->Current(),
                                          VITA_WIDTH / 2, VITA_HEIGHT / 2, _video_rect.x, _video_rect.y,
                                          _texture_buf->GetWidth(), _texture_buf->GetHeight(),
                                          _video_rect.width / _texture_buf->GetWidth(),
                                          _video_rect.height / _texture_buf->GetHeight(),
                                          0.f);
    // LogDebug("%f %f %f %f", _video_rect.x, _video_rect.y,
    //          _video_rect.width, _video_rect.height);
    _texture_buf->Unlock();
}

void Emulator::SetSpeed(double speed)
{
    LogFunctionName;
    _speed = speed;
    _delay.SetInterval(1000000ull / (uint64_t)(_av_info.timing.fps * speed));
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

void Emulator::_SetupKeys()
{
    LogFunctionName;
    memset(_keys, 0, sizeof(_keys));
    for (const auto &k : gConfig->control_maps)
    {
        if (k.retro >= 16)
        {
            LogError("wrong key config: %d %08x", k.retro, k.psv);
            continue;
        }
        _keys[k.retro] |= k.psv;
        if (k.turbo)
        {
            _input.SetTurbo(k.psv);
        }
    }

    // for (int i = 0; i < 16; i++)
    // {
    //     LogDebug("%d %08x", i, _keys[i]);
    // }
}

void Emulator::_SetVideoSize(uint32_t width, uint32_t height)
{
    float aspect_ratio = .0f;

    switch (gConfig->graphics_config.ratio)
    {
    case CONFIG_DISPLAY_RATIO_BY_DEVICE_SCREEN:
        aspect_ratio = (float)VITA_WIDTH / (float)VITA_HEIGHT;
        break;

    case CONFIG_DISPLAY_RATIO_8_7:
        aspect_ratio = 8.f / 7.f;
        break;

    case CONFIG_DISPLAY_RATIO_4_3:
        aspect_ratio = 4.f / 3.f;
        break;

    case CONFIG_DISPLAY_RATIO_3_2:
        aspect_ratio = 3.f / 2.f;
        break;

    case CONFIG_DISPLAY_RATIO_16_9:
        aspect_ratio = 16.f / 9.f;
        break;

    case CONFIG_DISPLAY_RATIO_BY_GAME_RESOLUTION:
    default:
        break;
    }

    if (aspect_ratio > 0.f && (width / aspect_ratio) < height)
    {
        width = height * aspect_ratio;
    }
    else
    {
        aspect_ratio = (float)width / (float)height;
    }

    switch (gConfig->graphics_config.size)
    {
    case CONFIG_DISPLAY_SIZE_2X:
        width *= 2;
        height *= 2;
        break;

    case CONFIG_DISPLAY_SIZE_3X:
        width *= 3;
        height *= 3;
        break;

    case CONFIG_DISPLAY_SIZE_FULL:
        width = VITA_HEIGHT * aspect_ratio;
        height = VITA_HEIGHT;
        break;

    case CONFIG_DISPLAY_SIZE_1X:
    default:
        break;
    }

    _video_rect.width = width;
    _video_rect.height = height;
}