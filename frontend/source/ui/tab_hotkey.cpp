#include "tab_hotkey.h"

TabHotkey::TabHotkey() : TabBase(TAB_HOTKEY)
{
}

TabHotkey::~TabHotkey()
{
}

void TabHotkey::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(TAB_HOTKEY), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::EndTabItem();
    }
}