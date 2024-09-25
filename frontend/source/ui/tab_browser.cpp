#include <vita2d.h>
#include "tab_browser.h"
#include "emulator.h"
#include "video.h"
#include "app.h"
#include "file.h"
#include "log.h"
#include "favorite.h"
#include "config.h"

#define TEXTURE_MAX_WIDTH 446
#define TEXTURE_MAX_HEIGHT 442

TabBrowser::TabBrowser() : TabSeletable(TAB_BROWSER),
                           _texture(nullptr)
{
    LogFunctionName;

    const char *path = gConfig->last_rom.c_str();
    _directory = new Directory(File::GetDir(path).c_str(), gEmulator->GetValidExtensions());
    std::string name = File::GetName(path);
    for (size_t i = 0; i < _directory->GetSize(); i++)
    {
        if (_directory->GetItemName(i) == name)
        {
            _index = i;
            break;
        }
    }
    _UpdateStatus();
}

TabBrowser::~TabBrowser()
{
    LogFunctionName;
    delete _directory;
}

void TabBrowser::SetInputHooks(Input *input)
{
    TabSeletable::SetInputHooks(input);
    input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&TabBrowser::_OnKeyCross, this, input));
    input->SetKeyUpCallback(SCE_CTRL_START, std::bind(&TabBrowser::_OnKeyStart, this, input));
}

void TabBrowser::UnsetInputHooks(Input *input)
{
    TabSeletable::UnsetInputHooks(input);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
    input->UnsetKeyUpCallback(SCE_CTRL_START);
}

void TabBrowser::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(_title_id), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImVec2 size = {0.f, 0.f};
        if (_status_text.size() > 0)
        {
            ImVec2 s = ImGui::CalcTextSize(_status_text.c_str());
            size.y = -s.y * (s.x / ImGui::GetContentRegionAvailWidth() + 1);
        }

        ImGui::BeginChild(TEXT(_title_id), size);
        ImGui::Columns(2, NULL, false);

        ImGui::Text(_directory->GetCurrentPath().c_str());
        ImGui::ListBoxHeader("", ImGui::GetContentRegionAvail());
        for (size_t i = 0; i < _directory->GetSize(); i++)
        {
            const DirItem &item = _directory->GetItem(i);

            std::string name(item.name);
            if (!item.is_dir)
            {

                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            }

            if (gFavorites->find(name) != gFavorites->end())
            {
                name.insert(0, "☆ ");
            }

            ImGui::Selectable(name.c_str(), i == _index);

            if (!item.is_dir)
            {
                ImGui::PopStyleColor();
            }

            if (i == _index && ImGui::GetScrollMaxY() > 0.f)
            {
                ImGui::SetScrollHereY((float)_index / (float)_directory->GetSize());
            }
        }

        ImGui::ListBoxFooter();
        ImGui::NextColumn();

        if (_texture != nullptr)
        {
            ImGui::Image(_texture, {_texture_width, _texture_height});
        }

        ImGui::NextColumn();

        ImGui::Columns(1);
        if (_status_text.size() > 0)
        {
            ImGui::Text(_status_text.c_str());
        }
        ImGui::EndChild();

        if (_status_text.size() > 0)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0xcc, 0xcc, 0xcc, 255));
            ImGui::TextWrapped(_status_text.c_str());
            ImGui::PopStyleColor();
        }

        ImGui::EndTabItem();
    }
}

void TabBrowser::_OnActive(Input *input)
{
    LogFunctionName;
    auto item = _directory->GetItem(_index);

    if (item.is_dir)
    {
        if (_directory->GetCurrentPath().size() == 0)
        {
            _directory->SetCurrentPath(item.name);
        }
        else
        {
            _directory->SetCurrentPath(_directory->GetCurrentPath() + "/" + item.name);
        }
        _index = 0;
        _UpdateStatus();
    }
    else
    {
        if (gEmulator->LoadRom((_directory->GetCurrentPath() + "/" + item.name).c_str(), item.entry_name.c_str(), item.crc32))
        {
            UnsetInputHooks(input);
        }
    }
}

void TabBrowser::_OnKeyCross(Input *input)
{
    auto path = _directory->GetCurrentPath();
    if (path.size() <= 5)
    {
        _directory->SetCurrentPath("");
    }
    else
    {
        size_t pos = path.rfind('/');
        if (pos != std::string::npos)
        {
            _directory->SetCurrentPath(path.substr(0, pos));
        }
    }

    _index = 0;
    _UpdateStatus();
}

void TabBrowser::_OnKeyStart(Input *input)
{
    DirItem item = _directory->GetItem(_index);
    if (item.is_dir)
    {
        return;
    }

    const auto &iter = gFavorites->find(item.name);
    if (iter == gFavorites->end())
    {
        gFavorites->emplace(item.name, Favorite{item, _directory->GetCurrentPath()});
    }
    else
    {
        gFavorites->erase(item.name);
    }

    gFavorites->Save();
    _UpdateStatus();
}

void TabBrowser::_OnKeyUp(Input *input)
{
    TabSeletable::_OnKeyUp(input);
    _UpdateStatus();
    _UpdateTexture();
}

void TabBrowser::_OnKeyDown(Input *input)
{
    TabSeletable::_OnKeyDown(input);
    _UpdateStatus();
    _UpdateTexture();
}

void TabBrowser::_UpdateTexture()
{
    // LogFunctionName;

    if (_texture != nullptr)
    {
        gVideo->Lock();
        vita2d_wait_rendering_done();
        vita2d_free_texture(_texture);
        _texture = nullptr;
        gVideo->Unlock();
    }

    const DirItem &item = _directory->GetItem(_index);
    if (item.is_dir)
    {
        return;
    }

    size_t node_pos = item.name.rfind('.');
    if (node_pos == std::string::npos)
    {
        return;
    }

    std::string path = _directory->GetCurrentPath() + "/" PREVIEW_DIR_NAME "/" + item.name.substr(0, node_pos);
    _texture = vita2d_load_PNG_file((path + ".png").c_str());
    if (_texture == nullptr)
    {
        _texture = vita2d_load_JPEG_file((path + ".jpg").c_str());
    }

    if (_texture)
    {
        float width = vita2d_texture_get_width(_texture);
        float height = vita2d_texture_get_height(_texture);
        float zoom = 1.0;

        if (width > TEXTURE_MAX_WIDTH)
        {
            zoom = TEXTURE_MAX_WIDTH / width;
        }

        if (height * zoom > TEXTURE_MAX_HEIGHT)
        {
            zoom = TEXTURE_MAX_HEIGHT / height;
        }

        _texture_width = width * zoom;
        _texture_height = height * zoom;
    }
}

void TabBrowser::_UpdateStatus()
{
    if (_index >= _directory->GetSize())
    {
        _status_text.clear();
        return;
    }

    const DirItem &item = _directory->GetItem(_index);

    _status_text = TEXT(BUTTON_CIRCLE);
    _status_text += item.is_dir ? TEXT(BROWSER_ENTER_DIR) : TEXT(BROWSER_START_GAME);
    _status_text += "\t";
    _status_text += TEXT(BUTTON_CROSS);
    _status_text += TEXT(BROWSER_BACK_DIR);
    _status_text += "\t";
    if (!item.is_dir)
    {
        _status_text += TEXT(BUTTON_START);
    }

    if (gFavorites->find(item.name) == gFavorites->end())
    {
        _status_text += TEXT(BROWSER_ADD_FAVORITE);
    }
    else
    {
        _status_text += TEXT(BROWSER_REMOVE_FAVORITE);
    }
}