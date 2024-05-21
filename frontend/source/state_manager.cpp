#include <libretro.h>
#include <stdio.h>
#include "state_manager.h"
#include "defines.h"
#include "log.h"
#include "emulator.h"

#define STATE_SCREENSHOT_HEIGHT 96

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

    _valid = true;
    File::GetCreateTime(_state_path.c_str(), &_create_time);

END:
    delete[] buf;

    result &= gEmulator->SaveScreenShot(_image_path.c_str(), STATE_SCREENSHOT_HEIGHT);

    if (result)
    {
        _valid = true;
        File::GetCreateTime(_state_path.c_str(), &_create_time);
        if (_texture)
        {
            vita2d_free_texture(_texture);
        }
        _texture = vita2d_load_JPEG_file(_image_path.c_str());
    }

    return result;
}

bool State::Load()
{
    LogFunctionName;
    FILE *fp = fopen(_state_path.c_str(), "rb");
    if (!fp)
    {
        LogError("failed to open file %s for reading", _state_path.c_str());
        return false;
    }

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *buf = new char[size];
    fread(buf, size, 1, fp);
    fclose(fp);

    bool result = retro_unserialize(buf, size);

    delete[] buf;

    return result;
}

bool State::Remove()
{
    LogFunctionName;
    bool result = File::Remove(_state_path.c_str());
    result &= File::Remove(_image_path.c_str());
    if (_texture)
    {
        vita2d_free_texture(_texture);
        _texture = nullptr;
    }
    return result;
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
        uint32_t height = STATE_SCREENSHOT_HEIGHT;
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
