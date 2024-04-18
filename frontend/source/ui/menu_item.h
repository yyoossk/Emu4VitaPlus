#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include "global.h"

class MenuItem
{
public:
    MenuItem(size_t text_id, size_t *config, size_t sizeof_config, size_t config_text_start, size_t config_count);
    virtual ~MenuItem();

    void Show(bool selected);
    void OnClick();

private:
    size_t _text_id;

    size_t *_config;
    size_t _config_mask;
    size_t _config_text_start;
    size_t _config_count;
};