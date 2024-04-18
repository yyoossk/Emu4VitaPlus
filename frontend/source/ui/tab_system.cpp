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

void TabSystem::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_UP, std::bind(&TabSystem::_OnKeyUp, this), true);
    input->SetKeyDownCallback(SCE_CTRL_DOWN, std::bind(&TabSystem::_OnKeyDown, this), true);
    input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&TabSystem::_OnKeyCircle, this));
}

void TabSystem::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
}

void TabSystem::_OnKeyCircle()
{
    (*(this).*_menu[_index].function)();
}

void TabSystem::_ExitApp()
{
    LogFunctionName;
    gStatus = APP_STATUS_EXIT;
}