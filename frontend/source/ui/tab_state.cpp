#include "tab_state.h"

TabState::TabState() : TabBase(TAB_STATE, false)
{
}

TabState::~TabState()
{
}

void TabState::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(TAB_CORE), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::EndTabItem();
    }
}