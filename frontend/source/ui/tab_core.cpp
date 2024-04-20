#include "tab_core.h"

TabCore::TabCore()
{
}

TabCore::~TabCore()
{
}

void TabCore::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(TAB_CORE), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::EndTabItem();
    }
}