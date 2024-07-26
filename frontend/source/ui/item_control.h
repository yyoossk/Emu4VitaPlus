#pragma once
#include <stdint.h>
#include "item_selectable.h"
#include "config.h"
#include "defines.h"

class ItemControl : public ItemSelectable
{
public:
    ItemControl(ControlMapConfig *control_map)
        : ItemSelectable(Config::ControlTextMap.at(control_map->psv)),
          _control_map(control_map) {};
    virtual ~ItemControl() {};

    void Show(bool selected) override
    {
        ItemSelectable::Show(selected);
        ImGui::SameLine();
        _control_map->turbo ? ImGui::Text(TEXT(TURBO)) : ImGui::TextDisabled(TEXT(TURBO));
    };

    void OnOption(Input *input) override
    {
        _control_map->turbo = !_control_map->turbo;
    };

private:
    size_t _GetTotalCount() override
    {
        return RETRO_KEYS_SIZE;
    };

    const char *_GetOptionString(size_t index) override
    {
        return TEXT(Config::RetroTextMap.at(RETRO_KEYS[index]));
    };

    size_t _GetIndex() override
    {
        for (uint32_t i = 0; i < RETRO_KEYS_SIZE; i++)
        {
            if (RETRO_KEYS[i] == _control_map->retro)
            {
                return i;
            }
        }
        return 0;
    };

    void _SetIndex(size_t index) override
    {
        _control_map->retro = RETRO_KEYS[index];
    };

    void _OnClick(Input *input) override
    {
        ItemSelectable::_OnClick(input);
        gConfig->Save();
    };

    ControlMapConfig *_control_map;
};