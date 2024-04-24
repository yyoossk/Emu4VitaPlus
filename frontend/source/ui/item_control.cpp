#include <psp2/ctrl.h>
#include <unordered_map>
#include <stdint.h>
#include "item_control.h"

static std::unordered_map<uint32_t, TEXT_ENUM> ControlTextMap = {
    {SCE_CTRL_CROSS, BUTTON_CROSS},
    {SCE_CTRL_TRIANGLE, BUTTON_TRIANGLE},
    {SCE_CTRL_CIRCLE, BUTTON_CIRCLE},
    {SCE_CTRL_SQUARE, BUTTON_SQUARE},
    {SCE_CTRL_SELECT, BUTTON_SELECT},
    {SCE_CTRL_START, BUTTON_START},
    {SCE_CTRL_UP, BUTTON_UP},
    {SCE_CTRL_DOWN, BUTTON_DOWN},
    {SCE_CTRL_LEFT, BUTTON_LEFT},
    {SCE_CTRL_RIGHT, BUTTON_RIGHT},
    {SCE_CTRL_L1, BUTTON_L1},
    {SCE_CTRL_R1, BUTTON_R1},
    {SCE_CTRL_L2, BUTTON_L2},
    {SCE_CTRL_R2, BUTTON_R2},
    {SCE_CTRL_L3, BUTTON_L3},
    {SCE_CTRL_R3, BUTTON_R3},
    {SCE_CTRL_LSTICK_UP, BUTTON_LEFT_ANALOG_UP},
    {SCE_CTRL_LSTICK_DOWN, BUTTON_LEFT_ANALOG_DOWN},
    {SCE_CTRL_LSTICK_LEFT, BUTTON_LEFT_ANALOG_LEFT},
    {SCE_CTRL_LSTICK_RIGHT, BUTTON_LEFT_ANALOG_RIGHT},
    {SCE_CTRL_RSTICK_UP, BUTTON_RIGHT_ANALOG_UP},
    {SCE_CTRL_RSTICK_DOWN, BUTTON_RIGHT_ANALOG_DOWN},
    {SCE_CTRL_RSTICK_LEFT, BUTTON_RIGHT_ANALOG_LEFT},
    {SCE_CTRL_RSTICK_RIGHT, BUTTON_RIGHT_ANALOG_RIGHT},
    {SCE_CTRL_PSBUTTON, BUTTON_HOME},
};

static std::unordered_map<uint8_t, TEXT_ENUM> RetroTextMap = {
    {RETRO_DEVICE_ID_JOYPAD_B, BUTTON_B},
    {RETRO_DEVICE_ID_JOYPAD_Y, BUTTON_Y},
    {RETRO_DEVICE_ID_JOYPAD_SELECT, BUTTON_SELECT},
    {RETRO_DEVICE_ID_JOYPAD_START, BUTTON_START},
    {RETRO_DEVICE_ID_JOYPAD_UP, BUTTON_UP},
    {RETRO_DEVICE_ID_JOYPAD_DOWN, BUTTON_DOWN},
    {RETRO_DEVICE_ID_JOYPAD_LEFT, BUTTON_LEFT},
    {RETRO_DEVICE_ID_JOYPAD_RIGHT, BUTTON_RIGHT},
    {RETRO_DEVICE_ID_JOYPAD_A, BUTTON_A},
    {RETRO_DEVICE_ID_JOYPAD_X, BUTTON_X},
    {RETRO_DEVICE_ID_JOYPAD_L, BUTTON_L},
    {RETRO_DEVICE_ID_JOYPAD_R, BUTTON_R},
    {RETRO_DEVICE_ID_JOYPAD_L2, BUTTON_L2},
    {RETRO_DEVICE_ID_JOYPAD_R2, BUTTON_R2},
    {RETRO_DEVICE_ID_JOYPAD_L3, BUTTON_L3},
    {RETRO_DEVICE_ID_JOYPAD_R3, BUTTON_R3},
};

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
    ImGui::Selectable(_GetText(), selected);
    ImGui::NextColumn();

    bool is_popup = ImGui::IsPopupOpen(_GetText());

    if (_actived && !is_popup)
    {
        ImGui::OpenPopup(_GetText());
    }

    // if (MyBeginCombo(_GetText(), _config_texts[GetConfig()].Get(), ImGuiComboFlags_NoArrowButton))
    // {
    //     if (!_actived && is_popup)
    //     {
    //         ImGui::CloseCurrentPopup();
    //     }
    //     for (size_t i = 0; i < _config_texts.size(); i++)
    //     {
    //         ImGui::Selectable(_config_texts[i].Get(), GetConfig() == i);
    //         if (GetConfig() == i)
    //         {
    //             ImGui::SetItemDefaultFocus();
    //         }
    //     }
    //     ImGui::EndCombo();
    // }

    ImGui::NextColumn();
}

void ItemControl::_OnKeyUp(Input *input)
{
}

void ItemControl::_OnKeyDown(Input *input)
{
}

void ItemControl::_OnClick(Input *input)
{
}

void ItemControl::_OnCancel(Input *input)
{
}

const char *ItemControl::_GetText()
{
    return TEXT(ControlTextMap[_control_map->psv]);
};