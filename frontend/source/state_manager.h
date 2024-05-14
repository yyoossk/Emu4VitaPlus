#pragma once
#include <string>
#include "vita2d.h"
#include "file.h"

#define MAX_STATES 6
#define AUTO_STATE_INDEX 0
#define SCREENSHOT_HEIGHT 96

class State
{
public:
    State();
    virtual ~State();

    void Init(const char *name, int index);
    bool Save();
    bool Load();
    bool Valid() { return _valid; };
    SceDateTime CreateTime() { return _create_time; };
    vita2d_texture *Texture() { return _texture; };

private:
    bool _valid;
    std::string _state_path;
    std::string _image_path;
    SceDateTime _create_time;
    vita2d_texture *_texture;
};

class StateManager
{
public:
    StateManager();
    virtual ~StateManager();

    void Init(const char *name);
    bool Valid(int index) { return _states[index].Valid(); };
    bool Load(int index) { return _states[index].Save(); };
    bool Save(int index) { return _states[index].Load(); }
    SceDateTime CreateTime(int index) { return _states[index].CreateTime(); };
    vita2d_texture *Texture(int index);

private:
    State _states[MAX_STATES];
    vita2d_texture *_empty_texture;
};

extern StateManager *gStateManager;