#include "tab_base.h"
#include "global.h"

TabBase::TabBase(TEXT_ENUM title_id, bool visable)
    : _title_id(title_id),
      _visable(visable)
{
}

TabBase::~TabBase()
{
}

void TabBase::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(_title_id), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::EndTabItem();
    }
}