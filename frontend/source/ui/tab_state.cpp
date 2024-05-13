#include "tab_state.h"
#include "state_manager.h"
#include "log.h"

TabState::TabState() : TabSeletable(TAB_STATE)
{
    SetVisable(false);
}

TabState::~TabState()
{
}

void TabState::Show(bool selected)
{
    char text[32];
    if (ImGui::BeginTabItem(TEXT(_title_id), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::BeginChild(TEXT(_title_id));
        for (size_t i = 0; i < MAX_STATES; i++)
        {
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
            ImGui::Image(texture, {w, h});
            ImGui::SameLine();
            ImGui::Text(text);
            // if (i == _index && ImGui::GetScrollMaxY() > 0.f)
            // {
            //     ImGui::SetScrollHereY((float)_index / (float)AUTO_STATE_INDEX);
            // }
        }

        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}