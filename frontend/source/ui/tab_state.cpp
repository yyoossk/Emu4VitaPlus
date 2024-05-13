#include "tab_state.h"
#include "state_manager.h"

TabState::TabState()
    : TabSeletable(TAB_STATE)
{
}

TabState::~TabState()
{
}

void TabState::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(_title_id), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::BeginChild(TEXT(_title_id));
        for (size_t i = 0; i < MAX_STATES; i++)
        {
            char text[32];
            if (i == AUTO_STATE_INDEX)
            {
                snprintf(text, 32, "%16s", "Auto Save");
            }
            else
            {
                snprintf(text, 32, "%16.2d", i);
            }
            ImGui::Selectable("", i == _index);
            ImGui::SameLine();
            vita2d_texture *texture = gStateManager->Texture(i);
            float w = vita2d_texture_get_width(texture);
            float h = vita2d_texture_get_height(texture);
            ImGui::Image(gStateManager->Texture(i), {w, h});
            ImGui::SameLine();
            ImGui::Text(text);
        }

        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}