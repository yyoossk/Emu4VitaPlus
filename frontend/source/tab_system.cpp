#include "tab_system.h"

void TabSystem::Show(bool selected)
{
    if (ImGui::BeginTabItem("System", NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::EndTabItem();
    }
}

void TabSystem::SetInputHooks(Input *input)
{
}

void TabSystem::UnsetInputHooks(Input *input)
{
}