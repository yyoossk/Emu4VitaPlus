#include "tab_system.h"

TabSystem::TabSystem()
{
}

TabSystem::~TabSystem()
{
}

void TabSystem::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(TAB_SYSTEM), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        auto size = ImGui::GetContentRegionAvail();
        ImGui::ListBoxHeader("", {size.x, size.y});
        for (size_t i = 0; i < _menu.size(); i++)
        {
            ImGui::Selectable(TEXT(_menu[i].text_id), i == _index);
        }
        ImGui::ListBoxFooter();
        ImGui::EndTabItem();
    }
}

void TabSystem::_OnClick()
{
    (*(this).*_menu[_index].function)();
}

void TabSystem::_ExitApp()
{
    LogFunctionName;
    gStatus = APP_STATUS_EXIT;
}