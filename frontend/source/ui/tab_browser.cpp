#include "global.h"
#include "tab_browser.h"
#include "log.h"

TabBrowser::TabBrowser(const char *path) : TabBase(TAB_BROWSER)
{
    LogFunctionName;
    _directory = new Directory(path, gEmulator->GetValidExtensions());
}

TabBrowser::~TabBrowser()
{
    LogFunctionName;
    delete _directory;
}

void TabBrowser::SetInputHooks(Input *input)
{
    TabBase::SetInputHooks(input);
    input->SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&TabBrowser::_OnKeyCross, this, input));
}

void TabBrowser::UnsetInputHooks(Input *input)
{
    TabBase::UnsetInputHooks(input);
    input->UnsetKeyUpCallback(SCE_CTRL_CROSS);
}

void TabBrowser::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(TAB_BROWSER), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::Text(_directory->GetCurrentPath().c_str());
        auto size = ImGui::GetContentRegionAvail();
        ImGui::ListBoxHeader("", {size.x * 0.5f, size.y});

        for (size_t i = 0; i < _directory->GetSize(); i++)
        {
            const auto item = _directory->GetItem(i);

            if (item.isDir)
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            }

            ImGui::Selectable(item.name.c_str(), i == _index);
            if (item.isDir)
            {
                ImGui::PopStyleColor();
            }

            if (i == _index && ImGui::GetScrollMaxY() > 0.f)
            {
                ImGui::SetScrollHereY((float)_index / (float)_directory->GetSize());
            }
        }

        // LogDebug("GetScrollY %f %f", ImGui::GetScrollY(), ImGui::GetScrollMaxY());
        ImGui::ListBoxFooter();
        ImGui::EndTabItem();
    }
}

void TabBrowser::_OnActive(Input *input)
{
    auto item = _directory->GetItem(_index);

    if (item.isDir)
    {
        _directory->SetCurrentPath(_directory->GetCurrentPath() + "/" + item.name);
        _index = 0;
    }
    else
    {
        if (gEmulator->LoadGame((_directory->GetCurrentPath() + "/" + item.name).c_str()))
        {
            gStatus = APP_STATUS_RUN_GAME;
        }
    }
}

void TabBrowser::_OnKeyCross(Input *input)
{
    auto path = _directory->GetCurrentPath();
    if (path.size() <= 5)
    {
        return;
    }

    size_t pos = path.rfind('/');
    if (pos != std::string::npos)
    {
        LogDebug(path.c_str());
        path = path.substr(0, pos);
        LogDebug(path.c_str());
        _directory->SetCurrentPath(path);
    }
}