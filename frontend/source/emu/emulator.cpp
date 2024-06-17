#include <algorithm>
#include <string>
#include <string.h>
#include <stdint.h>
#include <imgui_vita2d/imgui_impl_vita2d.h>
#include <jpeglib.h>
#include "emulator.h"
#include "callbacks.h"
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

Emulator *gEmulator = nullptr;

Emulator::Emulator()
    : _texture_buf(nullptr),
      _keys{0},
      _soft_frame_buf_render(false),
      _current_tex(nullptr),
      _info{0},
      _av_info{0}
{
    sceKernelCreateLwMutex(&_run_mutex, "run_mutex", 0, 0, NULL);
    _video_semaid = sceKernelCreateSema("video_sema", 0, 0, 1, NULL);
}

Emulator::~Emulator()
{
    LogFunctionName;

    if (_texture_buf)
    {
        delete _texture_buf;
    }

    sceKernelDeleteLwMutex(&_run_mutex);
    sceKernelDeleteSema(_video_semaid);

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
    _soft_frame_buf_render = false;
    char *buf = nullptr;

    if (_info.need_fullpath)
    {
        LogDebug("  load rom from path");
        game_info.path = path;
    }
    else
    {
        LogDebug("  load rom from memory");
        game_info.size = File::GetSize(path);
        buf = new char[game_info.size];
        if (!File::ReadFile(path, buf, game_info.size))
        {
            LogError("failed to read rom: %s", path);
            delete[] buf;
            return false;
        }
        game_info.data = buf;
    }

    bool result = retro_load_game(&game_info);
    if (result)
    {
        retro_get_system_av_info(&_av_info);
        SetSpeed(1.0);
        gStatus = APP_STATUS_RUN_GAME;
        if (!_audio.Inited())
        {
            _audio.SetSampleRate(_av_info.timing.sample_rate);
        }
    }
    else
    {
        LogError("load rom failed: %s", _current_name.c_str());
        gStatus = APP_STATUS_SHOW_UI;
    }

    gUi->ClearLogs();
    if (buf)
    {
        delete[] buf;
    }

    return result;
}

void Emulator::UnloadGame()
{
    LogFunctionName;
    Lock();
    gStateManager->states[0]->Save();
    retro_unload_game();
    gStatus = APP_STATUS_SHOW_UI;
    Unlock();
}

void Emulator::Run()
{
    _delay.Wait();
    _input.Poll();

    Lock();
    retro_run();
    Unlock();

    if (_soft_frame_buf_render && _texture_buf)
    {
        _texture_buf->Unlock();
        sceKernelSignalSema(_video_semaid, 1);
    }
}

void Emulator::Lock()
{
    sceKernelLockLwMutex(&_run_mutex, 1, NULL);
}

void Emulator::Unlock()
{
    sceKernelUnlockLwMutex(&_run_mutex, 1);
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

    if (gStatus == APP_STATUS_RUN_GAME)
    {
        SceUInt time = 15000;
        sceKernelWaitSema(_video_semaid, 1, &time);
    }

    // size_t count = 0;
    // while (_current_tex == _texture_buf->Current() && count < 10)
    // {
    //     SceUInt timeout = 1000;
    //     sceKernelWaitSema(_video_semaid, 1, &timeout);
    //     count++;
    // }

    // if (count >= 10 && gStatus == APP_STATUS_RUN_GAME)
    // {
    //     LogDebug("skip frame");
    //     return;
    // }
    // sceKernelWaitSema(_video_semaid, 1, NULL);

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
    LogDebug("  fps: %f", _av_info.timing.fps);
    _speed = speed;
    _delay.SetInterval(1000000ull / (uint64_t)(_av_info.timing.fps * speed));
}

bool Emulator::GetCurrentSoftwareFramebuffer(retro_framebuffer *fb)
{
    LogFunctionNameLimited;

    return false;

    if (!fb || _texture_buf == nullptr)
    {
        return false;
    }

    _texture_buf->Lock();
    // LogDebug("GetCurrentSoftwareFramebuffer _texture_buf->Current() %08x", _texture_buf->Current());
    _soft_frame_buf_render = true;
    vita2d_texture *texture = _texture_buf->Current();
    // if (texture == _current_tex)
    // {
    //     // LogWarn("same texture: %x %x", texture, _current_tex);
    //     texture = _texture_buf->Next();
    // }

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