#include <libretro.h>
#include <stdio.h>
#include "state_manager.h"
#include "defines.h"
#include "log.h"
#include "emulator.h"

StateManager *gStateManager = nullptr;

State::State()
    : _valid(false),
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

void State::Init(const char *name, int index)
{
    LogFunctionName;
    char path[SCE_FIOS_PATH_MAX];

    if (index == AUTO_STATE_INDEX)
    {
        snprintf(path, SCE_FIOS_PATH_MAX, CORE_SAVESTATES_DIR "/%s/state_auto.bin", name);
    }
    else
    {
        snprintf(path, SCE_FIOS_PATH_MAX, CORE_SAVESTATES_DIR "/%s/state_%02d.bin", name, index);
    }

    _state_path = path;
    _valid = File::Exist(path);
    if (_valid)
    {
        File::GetCreateTime(path, &_create_time);
    }

    if (index == AUTO_STATE_INDEX)
    {
        snprintf(path, SCE_FIOS_PATH_MAX, CORE_SAVESTATES_DIR "/%s/state_auto.jpg", name);
    }
    else
    {
        snprintf(path, SCE_FIOS_PATH_MAX, CORE_SAVESTATES_DIR "/%s/state_%02d.jpg", name, index);
    }

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

StateManager::StateManager()
{
    uint32_t height = SCREENSHOT_HEIGHT;
    uint32_t width = height * gEmulator->GetAspectRatio();
    _empty_texture = vita2d_create_empty_texture(width, height);
    const auto stride = vita2d_texture_get_stride(_empty_texture) / 4;
    auto texture_data = (uint32_t *)vita2d_texture_get_datap(_empty_texture);
    for (auto y = 0; y < height; ++y)
        for (auto x = 0; x < width; ++x)
            texture_data[y * stride + x] = 0xff00cc00;
}

StateManager::~StateManager()
{
    if (_empty_texture)
    {
        vita2d_free_texture(_empty_texture);
    }
}

void StateManager::Init(const char *name)
{
    for (int i = 0; i < MAX_STATES; i++)
    {
        _states->Init(name, i);
    }
}

vita2d_texture *StateManager::Texture(int index)
{
    vita2d_texture *tex = _states[index].Texture();
    return tex ? tex : _empty_texture;
}