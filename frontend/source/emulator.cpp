#include <algorithm>
#include <string>
#include <string.h>
#include <stdint.h>
#include <imgui_vita2d/imgui_impl_vita2d.h>
#include <jpeglib.h>
#include "emulator.h"
#include "video.h"
#include "app.h"
#include "defines.h"
#include "core_options.h"
#include "state_manager.h"
#include "config.h"
#include "ui.h"
#include "log.h"
#include "file.h"

extern "C"
{
#include <libswscale/swscale.h>
}

#define CORE_OPTIONS_VERSION 2

Emulator *gEmulator = nullptr;

void RetroLog(retro_log_level level, const char *fmt, ...)
{
    LogFunctionName;

    va_list list;
    char str[512];

    va_start(list, fmt);
    vsprintf(str, fmt, list);
    va_end(list);

    switch (level)
    {
    case RETRO_LOG_DEBUG:
        LogDebug(str);
        break;
    case RETRO_LOG_INFO:
        LogInfo(str);
        break;
    case RETRO_LOG_WARN:
        LogWarn(str);
        break;
    case RETRO_LOG_ERROR:
        LogWarn(str);
        break;
    }

    gUi->AppendLog(str);
}

bool EnvironmentCallback(unsigned cmd, void *data)
{
    LogFunctionNameLimited;
    LogTrace("  cmd: %u", cmd);

    switch (cmd)
    {
    case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
        if (data)
        {
            *(const char **)data = CORE_SYSTEM_DIR;
        }
        break;

    case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
        gEmulator->_SetPixelFormat(*(retro_pixel_format *)data);
        break;

    // case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
    //     break;
    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS:
        gConfig->core_options.Load((retro_core_option_definition *)data);
        gUi->UpdateCoreOptions();
        break;

    case RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE:
        LogDebug("  RETRO_ENVIRONMENT_SET_DISK_CONTROL_INTERFACE");
        break;

    case RETRO_ENVIRONMENT_GET_VARIABLE:
        if (data)
        {
            return gConfig->core_options.Get((retro_variable *)data);
        }
        else
        {
            return false;
        }

        break;

    case RETRO_ENVIRONMENT_SET_VARIABLES:
        LogDebug("  RETRO_ENVIRONMENT_SET_VARIABLES: %s", ((retro_variable *)data)->key);
        break;

    case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
        *(bool *)data = false;
        break;

    case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
        if (data)
        {
            ((retro_log_callback *)data)->log = RetroLog;
        }
        break;

    case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
        if (data)
        {
            char path[SCE_FIOS_PATH_MAX];
            snprintf(path, SCE_FIOS_PATH_MAX, CORE_SAVEFILES_DIR "/%s", gEmulator->_current_name.c_str());
            *(const char **)data = path;
        }
        break;

    case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
        memcpy(&gEmulator->_av_info, data, sizeof(retro_system_av_info));
        gEmulator->_audio.SetSampleRate(gEmulator->_av_info.timing.sample_rate);
        break;

    case RETRO_ENVIRONMENT_GET_LANGUAGE:
        LogDebug("  RETRO_ENVIRONMENT_GET_LANGUAGE");
        if (data)
        {
            *(retro_language *)data = gConfig->GetRetroLanguage();
        }
        break;

    case RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION:
        if (data)
            *(unsigned *)data = CORE_OPTIONS_VERSION;
        break;

    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_INTL:
        gConfig->core_options.Load((retro_core_options_intl *)data);
        gUi->UpdateCoreOptions();
        break;

    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_V2_INTL:
        gConfig->core_options.Load((retro_core_options_v2_intl *)data);
        gUi->UpdateCoreOptions();
        break;

    case RETRO_ENVIRONMENT_GET_CURRENT_SOFTWARE_FRAMEBUFFER:
        return gEmulator->GetCurrentSoftwareFramebuffer((retro_framebuffer *)data);

    case RETRO_ENVIRONMENT_GET_AUDIO_VIDEO_ENABLE:
        if (data)
        {
            *(int *)data = 3;
        }
        break;

    case RETRO_ENVIRONMENT_GET_INPUT_BITMASKS:
        LogDebug("  RETRO_ENVIRONMENT_GET_INPUT_BITMASKS");
        break;

    default:
        LogDebug("  unsupported cmd:%d", cmd);
        return false;
    }

    return true;
}

void VideoRefreshCallback(const void *data, unsigned width, unsigned height, size_t pitch)
{
    LogFunctionNameLimited;

    if (!data)
    {
        LogDebug("video data is NULL");
        return;
    }

    if (gEmulator->_texture_buf == nullptr || gEmulator->_texture_buf->GetWidth() != width || gEmulator->_texture_buf->GetHeight() != height)
    {
        if (gEmulator->_texture_buf != nullptr)
        {
            delete gEmulator->_texture_buf;
        }
        gEmulator->_texture_buf = new TextureBuf(gEmulator->_video_pixel_format, width, height);
        gEmulator->_SetVideoSize(width, height);
    }
    else if (gEmulator->_soft_frame_buf_render)
    {
        return;
    }

    gEmulator->_texture_buf->Lock();
    vita2d_texture *texture = gEmulator->_texture_buf->Next();

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
    return gEmulator->_audio.SendAudioSample(data, frames);
}

void InputPollCallback()
{
    LogFunctionNameLimited;
}

int16_t InputStateCallback(unsigned port, unsigned device, unsigned index, unsigned id)
{
    LogFunctionNameLimited;
    if (device != RETRO_DEVICE_JOYPAD || port != 0)
    {
        return 0;
    }
    // LogDebug("%d %d %d %d", port, device, index, id);
    uint32_t key_states = gEmulator->_input.GetKeyStates();

    if (id == RETRO_DEVICE_ID_JOYPAD_MASK)
    {
        gEmulator->_input.ClearKeyStates(gEmulator->_keys_mask);
        uint64_t keys = key_states;
        int16_t state = 0;
        for (size_t i = 0; i < 16; i++)
        {
            if (keys & gEmulator->_keys[i])
            {
                state |= (1 << i);
            }
        }
        return state;
    }
    else if (id >= 16)
    {
        LogError("  InputStateCallback, wrong id %d", id);
        return 0;
    }
    else
    {
        // gEmulator->_input.ClearKeyStates(gEmulator->_keys[id]);
        return key_states & gEmulator->_keys[id];
    }
}

Emulator::Emulator()
    : _texture_buf(nullptr),
      _keys{0},
      _soft_frame_buf_render(false),
      _current_tex(nullptr),
      _info{0},
      _av_info{0}
{
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

void Emulator::Init()
{
    LogFunctionName;

    retro_set_environment(EnvironmentCallback);
    retro_set_video_refresh(VideoRefreshCallback);
    retro_set_audio_sample_batch(AudioSampleBatchCallback);
    retro_set_input_poll(InputPollCallback);
    retro_set_input_state(InputStateCallback);

    retro_init();
    retro_get_system_info(&_info);

    retro_set_controller_port_device(0, RETRO_DEVICE_JOYPAD);
    _SetupKeys();
}

bool Emulator::LoadGame(const char *path)
{
    LogFunctionName;

    gStatus = APP_STATUS_BOOT;

    _current_name = File::GetStem(path);
    gStateManager->Init(_current_name.c_str());

    retro_game_info game_info = {0};
    game_info.path = path;

    _soft_frame_buf_render = false;

    bool result = retro_load_game(&game_info);
    if (result)
    {
        retro_get_system_av_info(&_av_info);
        SetSpeed(1.0);
        gStatus = APP_STATUS_RUN_GAME;
    }
    else
    {
        gStatus = APP_STATUS_SHOW_UI;
    }

    gUi->ClearLogs();

    return result;
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

    if (_soft_frame_buf_render)
    {
        _texture_buf->Unlock();
    }
}

void Emulator::Reset()
{
    LogFunctionName;
    retro_reset();
}

void Emulator::Show()
{
    if (_texture_buf == nullptr)
    {
        return;
    }

    _texture_buf->Lock();
    // LogDebug("Show _texture_buf->Current() %08x", _texture_buf->Current());
    _current_tex = _texture_buf->Current();
    vita2d_draw_texture_part_scale_rotate(_current_tex,
                                          VITA_WIDTH / 2, VITA_HEIGHT / 2, _video_rect.x, _video_rect.y,
                                          _texture_buf->GetWidth(), _texture_buf->GetHeight(),
                                          _video_rect.width / _texture_buf->GetWidth(),
                                          _video_rect.height / _texture_buf->GetHeight(),
                                          0.f);

    // LogDebug("%f %f %f %f", _video_rect.x, _video_rect.y, _video_rect.width, _video_rect.height);
    // LogDebug("%f %f", _video_rect.width / _texture_buf->GetWidth(), _video_rect.height / _texture_buf->GetHeight());

    _texture_buf->Unlock();
}

void Emulator::SetSpeed(double speed)
{
    LogFunctionName;
    _speed = speed;
    _delay.SetInterval(1000000ull / (uint64_t)(_av_info.timing.fps * speed));
}

bool Emulator::GetCurrentSoftwareFramebuffer(retro_framebuffer *fb)
{
    LogFunctionNameLimited;
    if (!fb || _texture_buf == nullptr)
    {
        return false;
    }

    _texture_buf->Lock();
    // LogDebug("GetCurrentSoftwareFramebuffer _texture_buf->Current() %08x", _texture_buf->Current());
    _soft_frame_buf_render = true;
    vita2d_texture *texture = _texture_buf->Next();
    if (texture == _current_tex)
    {
        LogWarn("same texture: %x %x", texture, _current_tex);
        texture = _texture_buf->Next();
    }

    fb->data = vita2d_texture_get_datap(texture);
    fb->width = vita2d_texture_get_width(texture);
    fb->height = vita2d_texture_get_height(texture);
    fb->pitch = vita2d_texture_get_stride(texture);
    fb->format = _retro_pixel_format;
    fb->access_flags = RETRO_MEMORY_ACCESS_WRITE | RETRO_MEMORY_ACCESS_READ;
    fb->memory_flags = RETRO_MEMORY_TYPE_CACHED;

    return true;
}

void Emulator::_SetPixelFormat(retro_pixel_format format)
{
    LogFunctionName;

    _retro_pixel_format = format;
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
        LogWarn("  unknown pixel format: %d", format);
        break;
    }

    LogDebug("  _video_pixel_format: %d", _video_pixel_format);
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
    _keys_mask = 0;
    for (const auto &k : gConfig->control_maps)
    {
        if (k.retro == 0xff)
        {
            continue;
        }

        if (k.retro >= 16)
        {
            LogError("  wrong key config: %d %08x", k.retro, k.psv);
            continue;
        }
        _keys[k.retro] |= k.psv;
        _keys_mask |= k.psv;
        if (k.turbo)
        {
            _input.SetTurbo(k.psv);
        }
    }

    _input.SetKeyUpCallback(SCE_CTRL_PSBUTTON, std::bind(&Emulator::_OnPsButton, this, &_input));
    // for (int i = 0; i < 16; i++)
    // {
    //     LogDebug("%d %08x", i, _keys[i]);
    // }
}

void Emulator::_SetVideoSize(uint32_t width, uint32_t height)
{
    float aspect_ratio = .0f;

    switch (gConfig->graphics[DISPLAY_RATIO])
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

    switch (gConfig->graphics[DISPLAY_SIZE])
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

void Emulator::_OnPsButton(Input *input)
{
    LogFunctionName;
    gVideo->Lock();
    gStatus = APP_STATUS_SHOW_UI_IN_GAME;
    gVideo->Unlock();
}

static void ConvertTextureToRGB888(vita2d_texture *texture, uint8_t *dst, size_t width = 0, size_t height = 0)
{
    AVPixelFormat src_format = AV_PIX_FMT_NONE;
    SceGxmTextureFormat tex_format = vita2d_texture_get_format(texture);
    size_t src_width = vita2d_texture_get_width(texture);
    size_t src_height = vita2d_texture_get_height(texture);
    int src_stride = vita2d_texture_get_stride(texture);
    uint8_t *src = (uint8_t *)vita2d_texture_get_datap(texture);

    if (width == 0)
    {
        width = src_width;
    }

    if (height == 0)
    {
        height = src_height;
    }

    int dst_stride = width * 3;
    LogDebug("  texture format: %d", tex_format);
    switch (tex_format)
    {
    case SCE_GXM_TEXTURE_FORMAT_X1U5U5U5_1RGB:
        src_format = AV_PIX_FMT_RGB555LE;
        break;

    case SCE_GXM_TEXTURE_FORMAT_X8U8U8U8_1RGB:
        src_format = AV_PIX_FMT_ARGB;
        break;

    case SCE_GXM_TEXTURE_FORMAT_U5U6U5_RGB:
        src_format = AV_PIX_FMT_RGB565LE;
        break;

    default:
        LogError("  unknown sce gxm format: %d", tex_format);
        return;
    }

    SwsContext *sws_ctx = sws_getContext(src_width, src_height, src_format,
                                         width, height, AV_PIX_FMT_RGB24,
                                         SWS_BILINEAR, NULL, NULL, NULL);
    sws_scale(sws_ctx, &src, &src_stride, 0, src_height, &dst, &dst_stride);
    sws_freeContext(sws_ctx);
}

bool Emulator::SaveScreenShot(const char *name, size_t height)
{
    LogFunctionName;

    FILE *fp = fopen(name, "wb");
    if (!fp)
    {
        return false;
    }

    if (height == 0)
    {
        height = vita2d_texture_get_height(_texture_buf->Current());
    }

    float scale = float(height) / _texture_buf->GetHeight();
    size_t width = _texture_buf->GetWidth() * scale;
    uint8_t *buf = new uint8_t[width * height * 3];
    ConvertTextureToRGB888(_texture_buf->Current(), buf, width, height);
    int dst_stride = 3 * width;

    jpeg_compress_struct cinfo;
    jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.in_color_space = JCS_RGB;
    cinfo.input_components = 3;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 90, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    uint8_t *p = buf;
    for (size_t i = 0; i < height; i++)
    {
        jpeg_write_scanlines(&cinfo, (JSAMPARRAY)&p, 1);
        p += dst_stride;
    }

    jpeg_finish_compress(&cinfo);
    fclose(fp);
    jpeg_destroy_compress(&cinfo);

    delete[] buf;

    return true;
}