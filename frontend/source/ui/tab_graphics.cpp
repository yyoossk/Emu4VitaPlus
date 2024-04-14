#include "tab_graphics.h"

TabGraphics::TabGraphics() : _index(0)
{
}

TabGraphics::~TabGraphics()
{
}

void TabGraphics::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(TAB_GRAPHICS), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        auto size = ImGui::GetContentRegionAvail();
        ImGui::ListBoxHeader("", {size.x, size.y});
        // for (size_t i = 0; i < _menu.size(); i++)
        // {
        //     ImGui::Selectable(TEXT(_menu[i].text_id), i == _index);
        // }
        ImGui::ListBoxFooter();
        ImGui::EndTabItem();
    }
}

void TabGraphics::SetInputHooks(Input *input)
{
}

void TabGraphics::UnsetInputHooks(Input *input)
{
}
