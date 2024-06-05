#include "defines.h"
#include "core_button.h"
#include "log.h"
#include "utils.h"
#include "file.h"
#include "app.h"

CoreButton::CoreButton(std::string name, std::vector<CoreName> cores)
    : _name(std::move(name)),
      _cores(std::move(cores)),
      _actived(false),
      _index(0)
{
    std::string icon = std::string(CORE_DATA_DIR) + "/" + _name + "/icon0.png";
    _texture = vita2d_load_PNG_file(icon.c_str());
}

CoreButton::~CoreButton()
{
    if (_texture)
    {
        vita2d_free_texture(_texture);
    }
}

void CoreButton::Show(bool selected)
{
    ImVec4 color = selected ? ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered) : ImVec4{0, 0, 0, 0};
    ImGui::ImageButton(_texture, {BUTTON_SIZE, BUTTON_SIZE}, {0.f, 0.f}, {1.f, 1.f}, 0, color);

    ImVec2 pos = ImGui::GetItemRectMin();
    ImVec2 size = ImGui::CalcTextSize(_name.c_str());

    pos.x += (BUTTON_SIZE - size.x) / 2;
    pos.y += BUTTON_SIZE - size.y - 5;
    ImDrawList *draw_list = ImGui::GetWindowDrawList();
    draw_list->AddText(pos, IM_COL32_WHITE, _name.c_str());

    if (selected)
    {
        _ShowPopup();
    }
}

void CoreButton::_ShowPopup()
{
    bool is_popup = ImGui::IsPopupOpen("popup_menu");

    if (_actived && !is_popup)
    {
        ImGui::OpenPopup("popup_menu");
    }

    if (ImGui::BeginPopupModal("popup_menu", NULL, ImGuiWindowFlags_NoTitleBar))
    {
        if (!_actived && is_popup)
        {
            ImGui::CloseCurrentPopup();
        }
        size_t count = 0;
        for (const auto &core : _cores)
        {
            ImGui::Selectable(core.name.c_str(), count == _index);
            count++;
        }
        ImGui::EndPopup();
    }
}

void CoreButton::OnActive(Input *input)
{
    _actived = true;
    input->PushCallbacks();
    SetInputHooks(input);
}

void CoreButton::_OnKeyUp(Input *input)
{
    LOOP_MINUS_ONE(_index, _cores.size());
}

void CoreButton::_OnKeyDown(Input *input)
{
    LOOP_PLUS_ONE(_index, _cores.size());
}

void CoreButton::_OnClick(Input *input)
{
    _actived = false;
    input->PopCallbacks();
    _BootCore();
}

void CoreButton::_OnCancel(Input *input)
{
    _actived = false;
    input->PopCallbacks();
}

void CoreButton::_BootCore()
{
    LogFunctionName;
    if (_cores.size() == 0)
    {
        return;
    }

    snprintf(gCorePath, SCE_FIOS_PATH_MAX, "app0:eboot_%s.self", _cores[_index].boot_name.c_str());
    gRunning = false;
}

void CoreButton::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_UP, std::bind(&CoreButton::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_DOWN, std::bind(&CoreButton::_OnKeyDown, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_UP, std::bind(&CoreButton::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_DOWN, std::bind(&CoreButton::_OnKeyDown, this, input), true);
    input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&CoreButton::_OnClick, this, input));
    input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&CoreButton::_OnCancel, this, input));
}

void CoreButton::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_DOWN);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}