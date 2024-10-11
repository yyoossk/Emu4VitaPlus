#include <algorithm>
#include <string>
#include <string.h>
#include <stdint.h>
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
#include "profiler.h"

extern "C"
{
#include <libswscale/swscale.h>
}

Emulator *gEmulator = nullptr;

Emulator::Emulator()
    : _texture_buf(nullptr),
      _keys{0},
      _soft_frame_buf_render(false),
      _info{0},
      _av_info{0},
      _graphics_config_changed(false),
      _frame_count(0),
      _core_options_update_display_callback(nullptr),
      _arcade_manager(nullptr)
{
    sceKernelCreateLwMutex(&_run_mutex, "run_mutex", 0, 0, NULL);
    _InitArcadeManager();
}

Emulator::~Emulator()
{
    LogFunctionName;

    if (_texture_buf)
    {
        delete _texture_buf;
    }

    if (_arcade_manager)
    {
        delete _arcade_manager;
    }

    sceKernelDeleteLwMutex(&_run_mutex);

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

    _SetupKeys();
}

bool Emulator::LoadRom(const char *path, const char *entry_name, uint32_t crc32)
{
    LogFunctionName;

    LogDebug(" %s %s %08x", path, entry_name, crc32);

    if (!File::Exist(path))
    {
        return false;
    }

    gStatus.Set(APP_STATUS_BOOT);

    _current_name = File::GetStem(path);
    gStateManager->Init(_current_name.c_str());

    retro_game_info game_info = {0};
    _soft_frame_buf_render = false;
    char *buf = nullptr;
    const char *_path = path;
    if (*entry_name)
    {
        _path = _archive_manager.GetCachedPath(crc32, path, entry_name);
    }

    if (_info.need_fullpath)
    {
        LogDebug("  load rom from path");
        game_info.path = _path;
    }
    else
    {
        LogDebug("  load rom from memory");
        game_info.size = File::ReadFile(_path, (void **)&buf);
        if (game_info.size == 0)
        {
            LogError("failed to read rom: %s", _path);
            return false;
        }
        game_info.data = buf;
    }

    bool result = retro_load_game(&game_info);
    if (result)
    {
        retro_get_system_av_info(&_av_info);
        retro_set_controller_port_device(0, RETRO_DEVICE_JOYPAD);
        SetSpeed(1.0);
        gUi->ClearLogs();
        if ((!gConfig->mute) && (!_audio.Inited()))
        {
            _audio.Init(_av_info.timing.sample_rate);
        }

        gConfig->last_rom = path;
        gConfig->Save();

        Load();
        retro_run();

        _frame_count = 0;

        if (gConfig->rewind)
        {
            _rewind_manager.Init();
        }

        gStatus.Set(APP_STATUS_RUN_GAME);
    }
    else
    {
        LogError("  load rom failed: %s", _current_name.c_str());
        gStatus.Set(APP_STATUS_SHOW_UI);
    }

    if (buf)
    {
        delete[] buf;
    }

    LogDebug("  LoadRom result: %d", result);

    if (_LoadCheats(path))
    {
        gUi->UpdateCheatOptions();
        _cheats.Start();
    }

    return result;
}

void Emulator::UnloadGame()
{
    LogFunctionName;
    APP_STATUS status = gStatus.Get();
    gStatus.Set(APP_STATUS_SHOW_UI);

    if (status & (APP_STATUS_RUN_GAME | APP_STATUS_SHOW_UI_IN_GAME))
    {
        _rewind_manager.Deinit();
        Lock();
        _cheats.Stop(true);
        gStateManager->states[0]->Save();
        Save();
        retro_unload_game();
        Unlock();
    }
}

void Emulator::Run()
{
    LogFunctionNameLimited;
    _input.Poll();

    if (gConfig->rewind)
    {
        switch (gStatus.Get())
        {
        case APP_STATUS_REWIND_GAME:
            _rewind_manager.Wait();
            break;
        case APP_STATUS_RUN_GAME:
            _rewind_manager.Signal();
            break;
        default:
            return;
        }
    }

    BeginProfile("retro_run");

    Lock();
    _audio.NotifyBufStatus();
    retro_run();
    Unlock();

    EndProfile("retro_run");

    if (_soft_frame_buf_render && _texture_buf)
    {
        _texture_buf->Unlock();
    }

    _delay.Wait();
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
    if (gStatus.Get() & (APP_STATUS_RUN_GAME | APP_STATUS_SHOW_UI_IN_GAME))
    {
        gStatus.Set(APP_STATUS_BOOT);
        retro_reset();
        gStatus.Set(APP_STATUS_RUN_GAME);
    }
}

void Emulator::SetSpeed(double speed)
{
    LogFunctionName;
    LogDebug("  fps: %f", _av_info.timing.fps);
    _speed = speed;
    _delay.SetInterval(1000000ull / (uint64_t)(_av_info.timing.fps * speed));
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

std::string Emulator::_SaveDirPath()
{
    return std::string(CORE_SAVEFILES_DIR) + "/" + gEmulator->_current_name;
}

std::string Emulator::_SaveNamePath(uint32_t id)
{
    std::string s = _SaveDirPath() + "/" + gEmulator->_current_name;
    switch (id)
    {
    case RETRO_MEMORY_SAVE_RAM:
        s += ".srm";
        break;
    case RETRO_MEMORY_RTC:
        s += ".rtc";
        break;
    default:
        s += ".unk";
    }
    return s;
}

static const uint32_t RETRO_MEMORY_IDS[] = {RETRO_MEMORY_SAVE_RAM, RETRO_MEMORY_RTC};

void Emulator::Save()
{
    LogFunctionName;

    std::string path = _SaveDirPath();
    if (!File::Exist(path.c_str()))
    {
        File::MakeDirs(path.c_str());
    }

    for (auto id : RETRO_MEMORY_IDS)
    {
        size_t size = retro_get_memory_size(id);
        void *data = retro_get_memory_data(id);
        if (size == 0 || data == NULL)
        {
            LogDebug("failed to save: %d", id);
            continue;
        }

        FILE *fp = fopen(_SaveNamePath(id).c_str(), "wb");
        if (fp)
        {
            fwrite(data, size, 1, fp);
            fclose(fp);
            LogDebug("%s saved", _SaveNamePath(id).c_str());
        }
    }
}

void Emulator::Load()
{
    LogFunctionName;

    for (auto id : RETRO_MEMORY_IDS)
    {
        size_t size = retro_get_memory_size(id);
        void *data = retro_get_memory_data(id);
        if (size == 0 || data == NULL)
        {
            LogDebug("failed to load: %d", id);
            continue;
        }

        FILE *fp = fopen(_SaveNamePath(id).c_str(), "rb");
        if (fp)
        {
            fseek(fp, 0, SEEK_END);

            if (size == ftell(fp))
            {
                fseek(fp, 0, SEEK_SET);
                fread(data, size, 1, fp);
                LogDebug("%s loaded", _SaveNamePath(id).c_str());
            }
            fclose(fp);
        }
    }
}

void Emulator::ChangeRewindConfig()
{
    LogFunctionName;

    if (gStatus.Get() & (APP_STATUS_RUN_GAME | APP_STATUS_SHOW_UI_IN_GAME))
    {
        if (gConfig->rewind)
        {
            _rewind_manager.Init();
        }
        else
        {
            _rewind_manager.Deinit();
        }
    }
}

void Emulator::ChangeAudioConfig()
{
    LogFunctionName;

    if (gConfig->mute)
    {
        _audio.Deinit();
    }
    else
    {
        _audio.Init(gEmulator->_av_info.timing.sample_rate);
    }
}

void Emulator::CoreOptionUpdate()
{
    LogFunctionName;

    if (_core_options_update_display_callback != nullptr)
    {
        _core_options_update_display_callback();
    }
}

bool Emulator::_LoadCheats(const char *path)
{
    LogFunctionName;

    char p[SCE_FIOS_PATH_MAX];
    std::string dir = File::GetDir(path);
    std::string stem = File::GetStem(path);

    snprintf(p, SCE_FIOS_PATH_MAX, "%s/%s.cht", dir.c_str(), stem.c_str());
    LogDebug(p);
    if (_cheats.Load(p))
    {
        return true;
    }

    snprintf(p, SCE_FIOS_PATH_MAX, "%s/%s/%s.cht", dir.c_str(), CHEAT_DIR_NAME, stem.c_str());
    LogDebug(p);
    if (_cheats.Load(p))
    {
        return true;
    }

    snprintf(p, SCE_FIOS_PATH_MAX, "%s/%s.cht", CORE_CHEATS_DIR, stem.c_str());
    LogDebug(p);
    if (_cheats.Load(p))
    {
        return true;
    }

    return false;
}

void Emulator::ChangeCheatConfig()
{
    LogFunctionName;
    _cheats.Signal();
}