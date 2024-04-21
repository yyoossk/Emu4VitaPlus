#include "tab_control.h"

TabControl::TabControl() : TabBase(TAB_CONTROL)
{
}

TabControl::~TabControl()
{
}

void TabControl::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(TAB_CONTROL), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::EndTabItem();
    }
}