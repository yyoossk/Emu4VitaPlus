#include <psp2/ctrl.h>
#include <unordered_map>
#include <stdint.h>
#include "item_control.h"
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
#endif
};

#define RETRO_KEYS_SIZE (sizeof(RetroKeys) / sizeof(RetroKeys[0]))

ItemControl::ItemControl(ControlMapConfig *control_map)
    : ItemBase(ControlTextMap[control_map->psv]),
      _control_map(control_map),
      _actived(false)
{
}

ItemControl::~ItemControl()
{
}

void ItemControl::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_UP, std::bind(&ItemControl::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_DOWN, std::bind(&ItemControl::_OnKeyDown, this, input), true);
    input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&ItemControl::_OnClick, this, input));
    input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&ItemControl::_OnCancel, this, input));
}

void ItemControl::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}

void ItemControl::Show(bool selected)
{
    const char *text = GetText();
    ImGui::Selectable(text, selected);
    ImGui::NextColumn();

    bool is_popup = ImGui::IsPopupOpen(text);

    if (_actived && !is_popup)
    {
        ImGui::OpenPopup(text);
    }

    if (MyBeginCombo(text, TEXT(RetroTextMap[_control_map->retro]), ImGuiComboFlags_NoArrowButton))
    {
        if (!_actived && is_popup)
        {
            ImGui::CloseCurrentPopup();
        }
        for (size_t i = 0; i < RETRO_KEYS_SIZE; i++)
        {
            ImGui::Selectable(TEXT(RetroTextMap[RetroKeys[i]]), _control_map->retro == RetroKeys[i]);
            if (_control_map->retro == RetroKeys[i])
            {
                ImGui::SetItemDefaultFocus();
                if (ImGui::GetScrollMaxY() > 0.f)
                {
                    ImGui::SetScrollHereY((float)i / (float)RETRO_KEYS_SIZE);
                }
            }
        }

        ImGui::EndCombo();
    }

    ImGui::SameLine();
    _control_map->turbo ? ImGui::Text(TEXT(TURBO)) : ImGui::TextDisabled(TEXT(TURBO));

    ImGui::NextColumn();
}

void ItemControl::OnActive(Input *input)
{
    LogFunctionName;
    _actived = true;
    _old_retro = _control_map->retro;
    input->PushCallbacks();
    SetInputHooks(input);
}

void ItemControl::OnOption(Input *input)
{
    LogFunctionName;
    _control_map->turbo = !_control_map->turbo;
}

uint8_t ItemControl::_GetCurrentRetroIndex()
{
    for (uint8_t i = 0; i < RETRO_KEYS_SIZE; i++)
    {
        if (RetroKeys[i] == _control_map->retro)
        {
            return i;
        }
    }
    return 0;
}

void ItemControl::_OnKeyUp(Input *input)
{
    uint8_t current = _GetCurrentRetroIndex();
    if (current == 0)
    {
        _control_map->retro = RetroKeys[RETRO_KEYS_SIZE - 1];
    }
    else
    {
        _control_map->retro = RetroKeys[current - 1];
    }
}

void ItemControl::_OnKeyDown(Input *input)
{
    uint8_t current = _GetCurrentRetroIndex();
    if (current == RETRO_KEYS_SIZE - 1)
    {
        _control_map->retro = RetroKeys[0];
    }
    else
    {
        _control_map->retro = RetroKeys[current + 1];
    }
}

void ItemControl::_OnClick(Input *input)
{
    LogFunctionName;
    _actived = false;
    input->PopCallbacks();
}

void ItemControl::_OnCancel(Input *input)
{
    LogFunctionName;
    _actived = false;
    input->PopCallbacks();
    _control_map->retro = _old_retro;
}
