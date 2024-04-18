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

    if (_actived && !ImGui::IsPopupOpen(TEXT(_text_id)))
        ImGui::OpenPopup(TEXT(_text_id));
    if (MyBeginCombo(TEXT(_text_id), TEXT(_config_text_start + ((*_config) & _config_mask)), ImGuiComboFlags_NoArrowButton))
    {
        for (size_t i = 0; i < _config_count; i++)
        {
            ImGui::Selectable(TEXT(_config_text_start + i), *_config == i);
            if (*_config == i)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::NextColumn();
}