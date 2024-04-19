#include "menu_item.h"
#include "my_imgui.h"

MenuItem::MenuItem(size_t text_id, size_t *config, size_t sizeof_config, size_t config_text_start, size_t config_count)
    : _text_id(text_id),
      _config(config),
      _config_mask((1 << (sizeof_config * 8)) - 1),
      _config_text_start(config_text_start),
      _config_count(config_count),
      _actived(false)
{
}

MenuItem::~MenuItem()
{
}

void MenuItem::Show(bool selected)
{
    ImGui::Selectable(TEXT(_text_id), selected);
    ImGui::NextColumn();

    if (_actived)
    {
        if (!ImGui::IsPopupOpen(TEXT(_text_id)))
        {
            ImGui::OpenPopup(TEXT(_text_id));
        }
    }
    else
    {
        if (ImGui::IsPopupOpen(TEXT(_text_id)))
        {
            ImGui::CloseCurrentPopup();
        }
    }

    if (MyBeginCombo(TEXT(_text_id), TEXT(_config_text_start + GetConfig()), ImGuiComboFlags_NoArrowButton))
    {
        for (size_t i = 0; i < _config_count; i++)
        {
            ImGui::Selectable(TEXT(_config_text_start + i), GetConfig() == i);
            if (GetConfig() == i)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::NextColumn();
}

void MenuItem::OnActive(Input *input)
{
    _actived = true;
    _old_config = GetConfig();
    input->PushCallbacks();
    SetInputHooks(input);
}

void MenuItem::_OnKeyUp(Input *input)
{
    // LogDebug("_OnKeyUp %d %d", GetConfig(), _config_count);
    size_t config = GetConfig();
    if (config == 0)
    {
        SetConfig(_config_count - 1);
    }
    else
    {
        SetConfig(config - 1);
    }
}

void MenuItem::_OnKeyDown(Input *input)
{
    // LogDebug("_OnKeyDown %d %d", *_config, _config_count);
    size_t config = GetConfig();
    if (config == _config_count - 1)
    {
        SetConfig(0);
    }
    else
    {
        SetConfig(config + 1);
    }
}

void MenuItem::_OnClick(Input *input)
{
    LogFunctionName;
    _actived = false;
    input->PopCallbacks();
}

void MenuItem::_OnCancel(Input *input)
{
    LogFunctionName;
    _actived = false;
    SetConfig(_old_config);
    input->PopCallbacks();
}

void MenuItem::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_UP, std::bind(&MenuItem::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_DOWN, std::bind(&MenuItem::_OnKeyDown, this, input), true);
    input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&MenuItem::_OnClick, this, input));
    input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&MenuItem::_OnCancel, this, input));
}

void MenuItem::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}