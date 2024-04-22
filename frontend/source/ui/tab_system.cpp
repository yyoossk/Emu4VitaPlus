#include "tab_system.h"

TabSystem::TabSystem() : TabBase(TAB_SYSTEM)
{
    _items[0] = new ItemBase(SYSTEM_MENU_EXIT);
}

TabSystem::~TabSystem()
{
    for (auto &item : _items)
    {
        delete item;
    }
}

// void TabSystem::Show(bool selected)
// {
//     if (ImGui::BeginTabItem(TEXT(TAB_SYSTEM), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
//     {
//         auto size = ImGui::GetContentRegionAvail();
//         ImGui::ListBoxHeader("", {size.x, size.y});
//         for (size_t i = 0; i < _menu.size(); i++)
//         {
//             ImGui::Selectable(TEXT(_menu[i].text_id), i == _index);
//         }
//         ImGui::ListBoxFooter();
//         ImGui::EndTabItem();
//     }
// }

// void TabSystem::_OnClick()
// {
//     (*(this).*_menu[_index].function)();
// }

void TabSystem::_ExitApp()
{
    LogFunctionName;
    gStatus = APP_STATUS_EXIT;
}

void TabSystem::_ShowItem(size_t index, bool selected)
{
    LogFunctionName;
    _items[index]->Show(selected);
}