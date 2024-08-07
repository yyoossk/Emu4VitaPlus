#include <vita2d.h>
#include "tab_browser.h"
#include "emulator.h"
#include "video.h"
#include "app.h"
#include "file.h"
#include "log.h"

#define TEXTURE_MAX_WIDTH 446
#define TEXTURE_MAX_HEIGHT 442

TabBrowser::TabBrowser(const char *path)
    : TabSeletable(TAB_BROWSER),
      _texture(nullptr)
{
    LogFunctionName;

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
}

void TabBrowser::UnsetInputHooks(Input *input)
{
    TabSeletable::UnsetInputHooks(input);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}

void TabBrowser::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(_title_id), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::BeginChild(TEXT(_title_id));
        ImGui::Columns(2, NULL, false);

        ImGui::Text(_directory->GetCurrentPath().c_str());
        ImGui::ListBoxHeader("", ImGui::GetContentRegionAvail());
        for (size_t i = 0; i < _directory->GetSize(); i++)
        {
            const DirItem &item = _directory->GetItem(i);

            if (!item.is_dir)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            }

            ImGui::Selectable(item.name.c_str(), i == _index);

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
    }
    else
    {
        if (gEmulator->LoadGame((_directory->GetCurrentPath() + "/" + item.name).c_str(), item.entry_name.c_str(), item.crc32))
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
            LogDebug(path.c_str());
            path = path.substr(0, pos);
            LogDebug(path.c_str());
            _directory->SetCurrentPath(path);
        }
    }
}

void TabBrowser::_OnKeyUp(Input *input)
{
    TabSeletable::_OnKeyUp(input);
    _UpdateTexture();
}

void TabBrowser::_OnKeyDown(Input *input)
{
    TabSeletable::_OnKeyDown(input);
    _UpdateTexture();
}

void TabBrowser::_UpdateTexture()
{
    LogFunctionName;

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