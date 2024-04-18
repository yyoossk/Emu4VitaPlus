#include "menu_item.h"

MenuItem::MenuItem(size_t text_id, size_t *config, size_t sizeof_config, size_t config_text_start, size_t config_count)
    : _text_id(text_id),
      _config(config),
      _config_mask((1 << sizeof_config) - 1),
      _config_text_start(config_text_start),
      _config_count(config_count)
{
}

MenuItem::~MenuItem()
{
}

void MenuItem::Show(bool selected)
{
    ImGui::Selectable(TEXT(_text_id), selected);
    auto size = ImGui::GetContentRegionAvail();
    auto pos = ImGui::GetWindowPos();
    ImGui::SetNextWindowPos({VITA_WIDTH / 2, pos.y});
    // ImGui::SameLine(size.x / 2);
    if (ImGui::BeginCombo("", TEXT(_config_text_start + ((*_config) & _config_mask)), ImGuiComboFlags_NoArrowButton))
    {
        for (size_t i = 0; i < _config_count; i++)
        {
            ImGui::Selectable(TEXT(_config_text_start + i), *_config == i);
        }
        ImGui::EndCombo();
    }

    // auto size = ImGui::GetContentRegionAvail();
    // const char *text = TEXT(_config_text_start + ((*_config) & _config_mask));
    // ImGui::SetCursorPosX(size.x - ImGui::CalcTextSize(text).x);
    // ImGui::Text(text);
}