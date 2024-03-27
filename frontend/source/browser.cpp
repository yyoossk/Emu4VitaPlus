#include <imgui_vita2d/imgui_vita.h>
#include "browser.h"
#include "log.h"

enum
{
    TAB_ITEM_BROWSER = 0,
    TAB_ITEM_FAVORITE,
    TAB_ITEM_COUNT
};
Browser::Browser(const char *path)
    : _tabIndex(0),
      _browserIndex(0),
      _favoriteIndex(0)
{
    LogFunctionName;
    _directory = new Directory(path, gEmulator->GetValidExtensions());
    _SetKeyHooks();
}

Browser::~Browser()
{
    LogFunctionName;
    delete _directory;
}

void Browser::_SetKeyHooks()
{
    _input.SetKeyUpCallback(SCE_CTRL_L2, std::bind(&Browser::_OnKeyL2, this));
    _input.SetKeyUpCallback(SCE_CTRL_R2, std::bind(&Browser::_OnKeyR2, this));
    _input.SetKeyUpCallback(SCE_CTRL_UP, std::bind(&Browser::_OnKeyUp, this));
    _input.SetKeyUpCallback(SCE_CTRL_DOWN, std::bind(&Browser::_OnKeyDown, this));
    _input.SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&Browser::_OnKeyCircle, this));
    _input.SetKeyUpCallback(SCE_CTRL_CROSS, std::bind(&Browser::_OnKeyCross, this));
}

void Browser::_OnKeyL2()
{
    LogFunctionName;
    _tabIndex += TAB_ITEM_COUNT - 1;
    _tabIndex %= TAB_ITEM_COUNT;
}

void Browser::_OnKeyR2()
{
    LogFunctionName;
    _tabIndex++;
    _tabIndex %= TAB_ITEM_COUNT;
}

void Browser::_OnKeyUp()
{
    LogFunctionName;

    switch (_tabIndex)
    {
    case TAB_ITEM_BROWSER:
        _browserIndex += _directory->GetSize() - 1;
        _browserIndex %= _directory->GetSize();
        break;

    case TAB_ITEM_FAVORITE:
        break;

    default:
        LogError("Wrong _tabIndex %d", _tabIndex);
        break;
    }
}

void Browser::_OnKeyDown()
{
    LogFunctionName;
    switch (_tabIndex)
    {
    case TAB_ITEM_BROWSER:
        _browserIndex++;
        _browserIndex %= _directory->GetSize();
        break;

    case TAB_ITEM_FAVORITE:
        break;

    default:
        LogError("Wrong _tabIndex %d", _tabIndex);
        break;
    }
}

void Browser::_OnKeyCircle()
{
    LogFunctionName;
    if (_tabIndex == 0)
    {
        auto item = _directory->GetItem(_browserIndex);

        if (item.isDir)
        {
            _directory->SetCurrentPath(_directory->GetCurrentPath() + "/" + item.name);
        }
        else
        {
            LogDebug("%d", gEmulator->LoadGame((_directory->GetCurrentPath() + "/" + item.name).c_str()));
            gStatus = APP_STATUS_RUN_GAME;
        }
    }
}

void Browser::_OnKeyCross()
{
    LogFunctionName;
    if (_tabIndex == 0)
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

static bool GetDirectoryItem(void *data, int idx, const char **out_text)
{
    *out_text = ((Directory *)data)->GetItem(idx).name.c_str();
    return *out_text != nullptr;
}

void Browser::Show()
{
    LogFunctionNameLimited;

    _input.Poll();

    ImGui_ImplVita2D_NewFrame();
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
    ImGui::SetNextWindowPos({MAIN_WINDOW_PADDING, MAIN_WINDOW_PADDING});
    ImGui::SetNextWindowSize({VITA_WIDTH - MAIN_WINDOW_PADDING * 2, VITA_HEIGHT - MAIN_WINDOW_PADDING * 2});
    ImGui::Begin(APP_NAME_STR,
                 NULL,
                 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
    // ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoDecoration);
    if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Browser", NULL, _tabIndex == 0 ? ImGuiTabItemFlags_SetSelected : 0))
        {
            ImGui::BeginGroup();
            ImGui::Text(_directory->GetCurrentPath().c_str());
            auto size = ImGui::GetWindowContentRegionMax();
            auto min_size = ImGui::GetWindowContentRegionMin();
            ImGui::SetNextItemWidth(size.x * 0.5f);
            ImGui::ListBox("", &_browserIndex, GetDirectoryItem, _directory, _directory->GetSize(), size.y - min_size.y * 4);
            ImGui::EndGroup();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Favorite", NULL, _tabIndex == 1 ? ImGuiTabItemFlags_SetSelected : 0))
        {
            ImGui::Text("This is the Favorite tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplVita2D_RenderDrawData(ImGui::GetDrawData());

    return;
}