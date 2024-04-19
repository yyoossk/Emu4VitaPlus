#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include "global.h"

class MenuItem
{
public:
    MenuItem(size_t text_id, size_t *config, size_t sizeof_config, size_t config_text_start, size_t config_count);
    virtual ~MenuItem();

    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    virtual void Show(bool selected);
    virtual void OnActive(Input *input);
    size_t GetConfig() { return *_config & _config_mask; };
    void SetConfig(size_t value) { *_config = value & _config_mask; };

private:
    void _OnKeyUp(Input *input);
    void _OnKeyDown(Input *input);
    void _OnClick(Input *input);
    void _OnCancel(Input *input);

    size_t _text_id;

    size_t *_config;
    size_t _config_mask;
    size_t _config_text_start;
    size_t _config_count;
    size_t _old_config;

    bool _actived;
};