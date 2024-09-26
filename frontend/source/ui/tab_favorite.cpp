#include <string>
#include "tab_favorite.h"
#include "file.h"
#include "config.h"

TabFavorite::TabFavorite() : TabSeletable(TAB_FAVORITE)
{
    std::string name = File::GetName(gConfig->last_rom.c_str());
    size_t count = 0;
    for (const auto &fav : *gFavorites)
    {
        if (name == fav.first)
        {
            _index = count;
            break;
        }
        count++;
    }
}

TabFavorite::~TabFavorite()
{
}

void TabFavorite::Show(bool selected)
{
    std::string title = std::string(TAB_ICONS[_title_id]) + TEXT(_title_id);
    if (ImGui::BeginTabItem(title.c_str(), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        if (_index >= gFavorites->size())
        {
            _index = 0;
        }

        ImGui::BeginChild(TEXT(_title_id));
        ImGui::Columns(2, NULL, false);

        auto iter = gFavorites->begin();
        std::advance(iter, _index);
        ImGui::Text(iter->second.path.c_str());

        ImGui::ListBoxHeader("", ImGui::GetContentRegionAvail());
        size_t count = 0;
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        for (const auto &fav : *gFavorites)
        {
            ImGui::Selectable(fav.second.item.name.c_str(), count == _index);
            count++;
        }
        ImGui::PopStyleColor();
        ImGui::ListBoxFooter();

        ImGui::NextColumn();
        ImGui::NextColumn();
        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

void TabFavorite::SetInputHooks(Input *input)
{
    TabSeletable::SetInputHooks(input);
    input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&TabFavorite::_OnKeyCross, this, input));
}

void TabFavorite::UnsetInputHooks(Input *input)
{
    TabSeletable::UnsetInputHooks(input);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}

void TabFavorite::_OnActive(Input *input)
{
}

void TabFavorite::_OnKeyCross(Input *input)
{
}