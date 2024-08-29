#include <imgui_vita2d/imgui_vita.h>
#include "item_config.h"
#include "my_imgui.h"
#include "config.h"
#include "log.h"
#include "utils.h"

ItemConfig::ItemConfig(LanguageString text,
                       LanguageString info,
                       uint32_t *config,
                       std::vector<LanguageString> config_texts,
                       CallbackFunc active_callback,
                       CallbackFunc option_callback)
    : ItemBase(text, info, active_callback, option_callback),
      _config(config),
      _config_texts(std::move(config_texts)),
      _actived(false)
{
    if (_config_texts.size() > 0 && *_config >= _config_texts.size())
    {
        *_config = 0;
    }
}

ItemConfig::ItemConfig(LanguageString text,
                       LanguageString info,
                       uint32_t *config,
                       TEXT_ENUM start,
                       size_t count,
                       CallbackFunc active_callback,
                       CallbackFunc option_callback)
    : ItemBase(text, info, active_callback, option_callback),
      _config(config),
      _actived(false)
{
    _config_texts.reserve(count);
    for (size_t i = 0; i < count; i++)
    {
        _config_texts.emplace_back(LanguageString(start + i));
    }

    if (*_config >= _config_texts.size())
    {
        *_config = 0;
    }
}

ItemConfig::~ItemConfig()
{
}

void ItemConfig::Show(bool selected)
{
    const char *text = GetText();

    ImGui::Selectable(text, selected);
    ImGui::NextColumn();

    bool is_popup = ImGui::IsPopupOpen(text);

    if (_actived && !is_popup)
    {
        ImGui::OpenPopup(text);
    }

    if (My_Imgui_BeginCombo(text, _config_texts[GetConfig()].Get(), ImGuiComboFlags_NoArrowButton))
    {
        if (!_actived && is_popup)
        {
            ImGui::CloseCurrentPopup();
        }
        for (size_t i = 0; i < _config_texts.size(); i++)
        {
            ImGui::Selectable(_config_texts[i].Get(), GetConfig() == i);
            if (GetConfig() == i && ImGui::GetScrollMaxY() > 0.f)
            {
                ImGui::SetScrollHereY((float)i / (float)_config_texts.size());
            }
        }

        ImGui::EndCombo();
    }
}

void ItemConfig::OnActive(Input *input)
{
    LogFunctionName;
    _actived = true;
    _old_config = GetConfig();
    input->PushCallbacks();
    SetInputHooks(input);
}

void ItemConfig::_OnKeyUp(Input *input)
{
    // LogDebug("_OnKeyUp %d %d", GetConfig(), _config_count);
    uint32_t config = GetConfig();
    LOOP_MINUS_ONE(config, _config_texts.size());
    SetConfig(config);
}

void ItemConfig::_OnKeyDown(Input *input)
{
    // LogDebug("_OnKeyDown %d %d", *_config, _config_count);
    uint32_t config = GetConfig();
    LOOP_PLUS_ONE(config, _config_texts.size());
    SetConfig(config);
}

void ItemConfig::_OnClick(Input *input)
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

void ItemConfig::_OnCancel(Input *input)
{
    LogFunctionName;
    _actived = false;
    input->PopCallbacks();
    SetConfig(_old_config);
}

void ItemConfig::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_UP, std::bind(&ItemConfig::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_DOWN, std::bind(&ItemConfig::_OnKeyDown, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_UP, std::bind(&ItemConfig::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_DOWN, std::bind(&ItemConfig::_OnKeyDown, this, input), true);
    input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&ItemConfig::_OnClick, this, input));
    input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&ItemConfig::_OnCancel, this, input));
}

void ItemConfig::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_DOWN);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}

ItemIntConfig::ItemIntConfig(LanguageString text,
                             LanguageString info,
                             uint32_t *value,
                             size_t start,
                             size_t end,
                             size_t step,
                             CallbackFunc active_callback,
                             CallbackFunc option_callback)
    : ItemBase(text, info, active_callback, option_callback),
      ItemConfig(text, info, &_index, {}, active_callback, option_callback),
      _value(value),
      _step(step)
{
    _config_texts.reserve(end - start + 1);
    for (size_t i = start; i <= end; i += step)
    {
        _config_texts.emplace_back(LanguageString(std::to_string(i)));
    }

    _index = (*value - start) / step;
}

void ItemIntConfig::SetConfig(uint32_t value)
{
    ItemConfig::SetConfig(value);
    *_value = std::stoi(_config_texts[value].Get());
}