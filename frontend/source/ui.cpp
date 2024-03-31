#include <imgui_vita2d/imgui_vita.h>
#include "my_imgui.h"
#include "ui.h"
#include "global.h"
#include "log.h"

enum
{
    TAB_ITEM_BROWSER = 0,
    TAB_ITEM_FAVORITE,
    TAB_ITEM_COUNT
};

Ui::Ui(const char *path)
    : _tab_index(0),
      _browser_index(0),
      _favorite_index(0)
{
    LogFunctionName;
    _directory = new Directory(path, gEmulator->GetValidExtensions());
    _SetKeyHooks();
}

Ui::~Ui()
{
    LogFunctionName;
    delete _directory;
}

void Ui::_SetKeyHooks()
{
    _input.SetKeyUpCallback(SCE_CTRL_L2, std::bind(&Ui::_OnKeyL2, this));
    _input.SetKeyUpCallback(SCE_CTRL_R2, std::bind(&Ui::_OnKeyR2, this));
    _input.SetKeyUpCallback(SCE_CTRL_UP, std::bind(&Ui::_OnKeyUp, this));
    _input.SetKeyUpCallback(SCE_CTRL_DOWN, std::bind(&Ui::_OnKeyDown, this));
    _input.SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&Ui::_OnKeyCircle, this));
    _input.SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&Ui::_OnKeyCross, this));
    _input.SetKeyTurbo(SCE_CTRL_UP | SCE_CTRL_DOWN);
}

void Ui::_OnKeyL2()
{
    // LogFunctionName;
    _tab_index += TAB_ITEM_COUNT - 1;
    _tab_index %= TAB_ITEM_COUNT;
}

void Ui::_OnKeyR2()
{
    // LogFunctionName;
    _tab_index++;
    _tab_index %= TAB_ITEM_COUNT;
}

void Ui::_OnKeyUp()
{
    // LogFunctionName;

    switch (_tab_index)
    {
    case TAB_ITEM_BROWSER:
        _browser_index += _directory->GetSize() - 1;
        _browser_index %= _directory->GetSize();
        break;

    case TAB_ITEM_FAVORITE:
        break;

    default:
        LogError("Wrong _tabIndex %d", _tab_index);
        break;
    }
}

void Ui::_OnKeyDown()
{
    // LogFunctionName;
    switch (_tab_index)
    {
    case TAB_ITEM_BROWSER:
        _browser_index++;
        _browser_index %= _directory->GetSize();
        break;

    case TAB_ITEM_FAVORITE:
        break;

    default:
        LogError("Wrong _tabIndex %d", _tab_index);
        break;
    }
}

void Ui::_OnKeyCircle()
{
    LogFunctionName;
    if (_tab_index == 0)
    {
        auto item = _directory->GetItem(_browser_index);

        if (item.isDir)
        {
            _directory->SetCurrentPath(_directory->GetCurrentPath() + "/" + item.name);
            _browser_index = 0;
        }
        else
        {
            if (gEmulator->LoadGame((_directory->GetCurrentPath() + "/" + item.name).c_str()))
            {
                gStatus = APP_STATUS_RUN_GAME;
            }
        }
    }
}

void Ui::_OnKeyCross()
{
    LogFunctionName;
    if (_tab_index == 0)
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
}

void Ui::Run()
{
    _input.Poll();
}

void Ui::Show()
{
    LogFunctionNameLimited;

    ImGui_ImplVita2D_NewFrame();
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
    ImGui::SetNextWindowPos({MAIN_WINDOW_PADDING, MAIN_WINDOW_PADDING});
    ImGui::SetNextWindowSize({VITA_WIDTH - MAIN_WINDOW_PADDING * 2, VITA_HEIGHT - MAIN_WINDOW_PADDING * 2});
    ImGui::Begin(APP_NAME_STR, NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Browser", NULL, _tab_index == 0 ? ImGuiTabItemFlags_SetSelected : 0))
        {
            ImGui::Text(_directory->GetCurrentPath().c_str());
            auto size = ImGui::GetContentRegionAvail();
            ImGui::ListBoxHeader("", {size.x * 0.5f, size.y});
            for (int i = 0; i < _directory->GetSize(); i++)
            {
                const auto item = _directory->GetItem(i);

                if (item.isDir)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                }

                ImGui::Selectable(item.name.c_str(), i == _browser_index);
                if (item.isDir)
                {
                    ImGui::PopStyleColor();
                }

                if (i == _browser_index && ImGui::GetScrollMaxY() > 0.f)
                {
                    ImGui::SetScrollHereY((float)_browser_index / (float)_directory->GetSize());
                }
            }

            // LogDebug("GetScrollY %f %f", ImGui::GetScrollY(), ImGui::GetScrollMaxY());
            ImGui::ListBoxFooter();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Favorite", NULL, _tab_index == 1 ? ImGuiTabItemFlags_SetSelected : 0))
        {
            ImGui::Text("This is the Favorite tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
    ImGui::Render();
    My_ImGui_ImplVita2D_RenderDrawData(ImGui::GetDrawData());

    return;
}