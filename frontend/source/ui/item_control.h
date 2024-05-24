#pragma once
#include <stdint.h>
#include "item_selectable.h"
#include "config.h"

static uint8_t RetroKeys[] = {
    RETRO_DEVICE_ID_NONE,
    RETRO_DEVICE_ID_JOYPAD_UP,
    RETRO_DEVICE_ID_JOYPAD_DOWN,
    RETRO_DEVICE_ID_JOYPAD_LEFT,
    RETRO_DEVICE_ID_JOYPAD_RIGHT,
#if defined(GBA_BUILD)
    RETRO_DEVICE_ID_JOYPAD_A,
    RETRO_DEVICE_ID_JOYPAD_B,
    RETRO_DEVICE_ID_JOYPAD_L,
    RETRO_DEVICE_ID_JOYPAD_R,
    RETRO_DEVICE_ID_JOYPAD_START,
    RETRO_DEVICE_ID_JOYPAD_SELECT,
#elif defined(ARC_BUILD)
    RETRO_DEVICE_ID_JOYPAD_A,
    RETRO_DEVICE_ID_JOYPAD_B,
    RETRO_DEVICE_ID_JOYPAD_X,
    RETRO_DEVICE_ID_JOYPAD_Y,
    RETRO_DEVICE_ID_JOYPAD_L,
    RETRO_DEVICE_ID_JOYPAD_R,
    RETRO_DEVICE_ID_JOYPAD_L2,
    RETRO_DEVICE_ID_JOYPAD_R2,
#elif defined(SFC_BUILD)
    RETRO_DEVICE_ID_JOYPAD_A,
    RETRO_DEVICE_ID_JOYPAD_B,
    RETRO_DEVICE_ID_JOYPAD_X,
    RETRO_DEVICE_ID_JOYPAD_Y,
    RETRO_DEVICE_ID_JOYPAD_L,
    RETRO_DEVICE_ID_JOYPAD_R,
    RETRO_DEVICE_ID_JOYPAD_START,
    RETRO_DEVICE_ID_JOYPAD_SELECT,
#else
#error "unknown build"
#endif
};

#define RETRO_KEYS_SIZE (sizeof(RetroKeys) / sizeof(RetroKeys[0]))

class ItemControl : public ItemSelectable
{
public:
    ItemControl(ControlMapConfig *control_map)
        : ItemSelectable(Config::ControlTextMap.at(control_map->psv)),
          _control_map(control_map){};
    virtual ~ItemControl(){};

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
        return TEXT(Config::RetroTextMap.at(RetroKeys[index]));
    };

    size_t _GetIndex() override
    {
        for (uint32_t i = 0; i < RETRO_KEYS_SIZE; i++)
        {
            if (RetroKeys[i] == _control_map->retro)
            {
                return i;
            }
        }
        return 0;
    };

    void _SetIndex(size_t index) override
    {
        _control_map->retro = RetroKeys[index];
    };

    void _OnClick(Input *input) override
    {
        ItemSelectable::_OnClick(input);
        gConfig->Save();
    };

    ControlMapConfig *_control_map;
};