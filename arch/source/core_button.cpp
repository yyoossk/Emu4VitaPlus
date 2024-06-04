#include "defines.h"
#include "core_button.h"
#include "log.h"

CoreButton::CoreButton(std::string name, std::vector<std::string> cores)
    : _name(std::move(name)),
      _cores(std::move(cores)),
      _actived(false)
{
    std::string icon = std::string(CORE_DATA_DIR) + "/" + _name + "/icon0.png";
    _texture = vita2d_load_PNG_file(icon.c_str());
    if (_texture)
    {
        float width = vita2d_texture_get_width(_texture);
        float height = (float)vita2d_texture_get_height(_texture);
        _uv0.x = (BUTTON_SIZE - width) / 2 / BUTTON_SIZE;
        _uv0.y = (BUTTON_SIZE - height) / 2 / BUTTON_SIZE;
        _uv1.x = _uv0.x + width / BUTTON_SIZE;
        _uv1.y = _uv0.y + height / BUTTON_SIZE;
    }
    else
    {
        LogError("failed to load icon: %s", icon.c_str());
    }
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
}