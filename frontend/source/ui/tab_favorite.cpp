#include <string>
#include "tab_favorite.h"
#include "file.h"
#include "config.h"
#include "emulator.h"

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
    _UpdateStatus();
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

        ImVec2 size = {0.f, 0.f};
        ImVec2 s = ImGui::CalcTextSize(_status_text.c_str());
        size.y = -s.y * (s.x / ImGui::GetContentRegionAvailWidth() + 1);

        ImGui::BeginChild(TEXT(_title_id), size);
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

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0xcc, 0xcc, 0xcc, 255));
        ImGui::TextWrapped(_status_text.c_str());
        ImGui::PopStyleColor();

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
    LogFunctionName;

    auto iter = gFavorites->begin();
    std::advance(iter, _index);
    const Favorite &fav = iter->second;

    if (gEmulator->LoadRom((fav.path + "/" + fav.item.name).c_str(), fav.item.entry_name.c_str(), fav.item.crc32))
    {
        UnsetInputHooks(input);
    }
}

void TabFavorite::_OnKeyCross(Input *input)
{
    LogFunctionName;
}

void TabFavorite::_UpdateStatus()
{
    _status_text = TEXT(BUTTON_CIRCLE);
    _status_text += TEXT(BROWSER_ENTER_DIR);
    _status_text += "\t";
    _status_text += TEXT(BUTTON_CROSS);
    _status_text += TEXT(BROWSER_REMOVE_FAVORITE);
}