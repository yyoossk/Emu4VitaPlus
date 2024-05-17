#include <libretro.h>
#include <stdio.h>
#include <jpeglib.h>
#include "state_manager.h"
#include "defines.h"
#include "log.h"
#include "emulator.h"

StateManager *gStateManager = nullptr;
vita2d_texture *State::_empty_texture = nullptr;

State::State(const char *slot_name)
    : _slot_name(slot_name),
      _valid(false),
      _texture(nullptr)
{
}

State::~State()
{
    LogFunctionName;
    if (_texture)
    {
        vita2d_free_texture(_texture);
    }
}

void State::Init(const char *game_name)
{
    LogFunctionName;

    char path[SCE_FIOS_PATH_MAX];
    snprintf(path, SCE_FIOS_PATH_MAX, CORE_SAVESTATES_DIR "/%s/state_%s.bin", game_name, _slot_name.c_str());

    _state_path = path;
    _valid = File::Exist(path);
    if (_valid)
    {
        File::GetCreateTime(path, &_create_time);
    }

    snprintf(path, SCE_FIOS_PATH_MAX, CORE_SAVESTATES_DIR "/%s/state_%s.jpg", game_name, _slot_name.c_str());
    _image_path = path;
    if (_texture)
    {
        vita2d_free_texture(_texture);
    }
    _texture = vita2d_load_JPEG_file(path);
}

bool State::Save()
{
    LogFunctionName;
    FILE *fp;

    size_t size = retro_serialize_size();
    char *buf = new char[size];

    bool result = retro_serialize(buf, size);
    if (!result)
    {
        LogError("run retro_serialize failed");
        goto END;
    }

    fp = fopen(_state_path.c_str(), "wb");
    if (!fp)
    {
        LogError("failed to open file %s for writing", _state_path.c_str());
        goto END;
    }

    fwrite(buf, size, 1, fp);
    fclose(fp);

END:
    delete[] buf;
    return result;
}

bool _SaveTexture(vita2d_texture *texture, const char *name)
{
    FILE *fp = fopen(name, "wb");
    if (!fp)
    {
        return false;
    }

    struct jpeg_compress_struct cinfo;
    jpeg_create_compress(&cinfo);
    // jpeg_stdio_dest(&cinfo, fp);
    jpeg_destroy_compress(&cinfo);
    fclose(fp);
    return true;
}

StateManager::StateManager()
{
    states[0] = new State("auto");
    for (int i = 1; i < MAX_STATES; i++)
    {
        char buf[3];
        snprintf(buf, 3, "%02d", i);
        states[i] = new State(buf);
    }

    if (State::_empty_texture == nullptr)
    {
        uint32_t height = SCREENSHOT_HEIGHT;
        uint32_t width = height * gEmulator->GetAspectRatio();
        State::_empty_texture = vita2d_create_empty_texture(width, height);
        const auto stride = vita2d_texture_get_stride(State::_empty_texture) / 4;
        auto texture_data = (uint32_t *)vita2d_texture_get_datap(State::_empty_texture);
        for (auto y = 0; y < height; ++y)
            for (auto x = 0; x < width; ++x)
                texture_data[y * stride + x] = 0xff00cc00;
    }
}

StateManager::~StateManager()
{
    if (State::_empty_texture != nullptr)
    {
        vita2d_free_texture(State::_empty_texture);
        State::_empty_texture = nullptr;
    }
    for (size_t i = 0; i < MAX_STATES; i++)
    {
        delete states[i];
    }
}

void StateManager::Init(const char *name)
{
    states[0]->Init(name);
    for (int i = 1; i < MAX_STATES; i++)
    {
        states[i]->Init(name);
    }
}
