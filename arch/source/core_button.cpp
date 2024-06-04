#include "defines.h"
#include "core_button.h"
#include "log.h"

CoreButton::CoreButton(std::string name, std::vector<std::string> cores)
    : _name(std::move(name)),
      _cores(std::move(cores))
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
    ImGui::ImageButton(_texture, {BUTTON_SIZE, BUTTON_SIZE}, _uv0, _uv1, 0, color);
}