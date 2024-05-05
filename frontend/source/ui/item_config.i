#include <imgui_vita2d/imgui_vita.h>
#include "item_config.h"
#include "my_imgui.h"

template <typename T>
ItemConfig<T>::ItemConfig(size_t text_id, T *config,
                          std::vector<LanguageString> texts,
                          CallbackFunc active_callback, CallbackFunc option_callback)
    : ItemBase(text_id, active_callback, option_callback),
      _config(config),
      _config_texts(std::move(texts)),
      _actived(false)
{
}

template <typename T>
ItemConfig<T>::ItemConfig(size_t text_id, T *config, TEXT_ENUM start, size_t count)
    : ItemBase(text_id),
      _config(config),
      _actived(false)
{
    for (size_t i = 0; i < count; i++)
    {
        _config_texts.emplace_back(LanguageString(start + i));
    }
}

template <typename T>
ItemConfig<T>::~ItemConfig()
{
}

template <typename T>
void ItemConfig<T>::Show(bool selected)
{
    const char *text = GetText();
    ImGui::Selectable(text, selected);
    ImGui::NextColumn();

    bool is_popup = ImGui::IsPopupOpen(text);

    if (_actived && !is_popup)
    {
        ImGui::OpenPopup(text);
    }

    if (MyBeginCombo(text, _config_texts[GetConfig()].Get(), ImGuiComboFlags_NoArrowButton))
    {
        if (!_actived && is_popup)
        {
            ImGui::CloseCurrentPopup();
        }
        for (size_t i = 0; i < _config_texts.size(); i++)
        {
            ImGui::Selectable(_config_texts[i].Get(), GetConfig() == i);
        }
        ImGui::EndCombo();
    }

    ImGui::NextColumn();
}

template <typename T>
void ItemConfig<T>::OnActive(Input *input)
{
    LogFunctionName;
    _actived = true;
    _old_config = GetConfig();
    input->PushCallbacks();
    SetInputHooks(input);
}

template <typename T>
void ItemConfig<T>::_OnKeyUp(Input *input)
{
    // LogDebug("_OnKeyUp %d %d", GetConfig(), _config_count);
    T config = GetConfig();
    if (config == 0)
    {
        SetConfig(_config_texts.size() - 1);
    }
    else
    {
        SetConfig(config - 1);
    }
}

template <typename T>
void ItemConfig<T>::_OnKeyDown(Input *input)
{
    // LogDebug("_OnKeyDown %d %d", *_config, _config_count);
    T config = GetConfig();
    if (config == _config_texts.size() - 1)
    {
        SetConfig(0);
    }
    else
    {
        SetConfig(config + 1);
    }
}

template <typename T>
void ItemConfig<T>::_OnClick(Input *input)
{
    LogFunctionName;
    _actived = false;
    input->PopCallbacks();
    gConfig->Save();
    if (_active_callback != nullptr)
    {
        _active_callback();
    }
}

template <typename T>
void ItemConfig<T>::_OnCancel(Input *input)
{
    LogFunctionName;
    _actived = false;
    input->PopCallbacks();
    SetConfig(_old_config);
}

template <typename T>
void ItemConfig<T>::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_UP, std::bind(&ItemConfig::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_DOWN, std::bind(&ItemConfig::_OnKeyDown, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_UP, std::bind(&ItemConfig::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_DOWN, std::bind(&ItemConfig::_OnKeyDown, this, input), true);
    input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&ItemConfig::_OnClick, this, input));
    input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&ItemConfig::_OnCancel, this, input));
}

template <typename T>
void ItemConfig<T>::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_DOWN);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}