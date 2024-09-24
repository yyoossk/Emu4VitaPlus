#include "tab_favorite.h"

TabFavorite::TabFavorite() : TabSeletable(TAB_FAVORITE)
{
}

TabFavorite::~TabFavorite()
{
}

void TabFavorite::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(TAB_FAVORITE), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::EndTabItem();
    }
}