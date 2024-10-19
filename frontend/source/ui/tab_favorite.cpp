#include <string>
#include "tab_favorite.h"
#include "file.h"
#include "config.h"
#include "emulator.h"
#include "video.h"
#include "misc.h"

TabFavorite::TabFavorite() : TabSeletable(TAB_FAVORITE), _texture(nullptr)
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
    _UpdateTexture();

    _dialog = new Dialog{DIALOG_REMOVE_FAVORITE,
                         {DIALOG_OK, DIALOG_CANCEL},
                         std::bind(&TabFavorite::_OnRemove, this, std::placeholders::_1, std::placeholders::_2)};
}

TabFavorite::~TabFavorite()
{
    delete _dialog;
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
        if (_texture != nullptr)
        {
            ImVec2 avail_size = ImGui::GetContentRegionAvail();
            ImVec2 pos = ImGui::GetCursorScreenPos();
            pos.x += ceilf(fmax(0.0f, (avail_size.x - _texture_width) * 0.5f));
            pos.y += ceilf(fmax(0.0f, (avail_size.y - _texture_height) * 0.5f));
            ImGui::SetCursorScreenPos(pos);
            ImGui::Image(_texture, {_texture_width, _texture_height});
        }

        ImGui::NextColumn();
        ImGui::EndChild();

        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0xcc, 0xcc, 0xcc, 255));
        ImGui::TextWrapped(_status_text.c_str());
        ImGui::PopStyleColor();

        ImGui::EndTabItem();
    }

    if (_dialog->IsActived())
    {
        _dialog->Show();
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

void TabFavorite::_OnKeyUp(Input *input)
{
    TabSeletable::_OnKeyUp(input);
    _UpdateTexture();
}

void TabFavorite::_OnKeyDown(Input *input)
{
    TabSeletable::_OnKeyDown(input);
    _UpdateTexture();
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
    _dialog->OnActive(input);
}

void TabFavorite::_UpdateStatus()
{
    _status_text = TEXT(BUTTON_CIRCLE);
    _status_text += TEXT(BROWSER_START_GAME);
    _status_text += "\t";
    _status_text += TEXT(BUTTON_CROSS);
    _status_text += TEXT(BROWSER_REMOVE_FAVORITE);
}

void TabFavorite::_OnRemove(Input *input, int index)
{
    LogFunctionName;
    gVideo->Lock();
    auto iter = gFavorites->begin();
    std::advance(iter, _index);
    gFavorites->erase(iter);
    gFavorites->Save();
    gVideo->Unlock();
}

void TabFavorite::ChangeLanguage(uint32_t language)
{
    LogFunctionName;
    _UpdateStatus();
}

void TabFavorite::_UpdateTexture()
{
    if (_texture != nullptr)
    {
        gVideo->Lock();
        vita2d_wait_rendering_done();
        vita2d_free_texture(_texture);
        _texture = nullptr;
        gVideo->Unlock();
    }

    if (gFavorites->size() == 0)
    {
        return;
    }

    auto iter = gFavorites->begin();
    std::advance(iter, _index);
    const Favorite &fav = iter->second;

    _texture = GetRomPreviewImage(fav.path.c_str(), fav.item.name.c_str());

    if (_texture)
    {
        CalcFitSize(vita2d_texture_get_width(_texture),
                    vita2d_texture_get_height(_texture),
                    BROWSER_TEXTURE_MAX_WIDTH,
                    BROWSER_TEXTURE_MAX_HEIGHT,
                    &_texture_width,
                    &_texture_height);
    }
}