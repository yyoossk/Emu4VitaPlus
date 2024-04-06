#include "tab_about.h"

void TabAbout::Show(bool selected)
{
    if (ImGui::BeginTabItem("About", NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::EndTabItem();
    }
}