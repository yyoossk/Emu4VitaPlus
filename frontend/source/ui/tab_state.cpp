#include "tab_state.h"

TabState::TabState() : TabBase(TAB_STATE, false)
{
    _empty_texture = vita2d_create_empty_texture(100, 100);
}

TabState::~TabState()
{
    vita2d_free_texture(_empty_texture);
}

void TabState::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(_title_id), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::BeginChild(TEXT(_title_id));
        for (size_t i = 0; i < 5; i++)
        {
            ImGui::Image(_empty_texture, {100, 100});
            ImGui::SameLine();
            ImGui::Text("Auto Save");
        }

        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}