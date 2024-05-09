#include "tab_about.h"
#include "language_string.h"

void TabAbout::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(TAB_ABOUT), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::EndTabItem();
    }
}