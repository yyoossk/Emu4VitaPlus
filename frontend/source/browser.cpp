#include <imgui_vita2d/imgui_vita.h>
#include "browser.h"
#include "log.h"

#define TAB_ITEM_NUM 2

Browser::Browser(const char *path, const Emulator *emulator)
    : _emulator(emulator), _tabIndex(0), _browserIndex(0), _favoriteIndex(0)
{
    LogFunctionName;
    _directory = new Directory(path, _emulator->GetValidExtensions());
    _input.SetKeyUpCallback(SCE_CTRL_L2, std::bind(&Browser::_OnKeyL2, this));
    _input.SetKeyUpCallback(SCE_CTRL_R2, std::bind(&Browser::_OnKeyR2, this));
    _input.SetKeyUpCallback(SCE_CTRL_UP, std::bind(&Browser::_OnKeyUp, this));
    _input.SetKeyUpCallback(SCE_CTRL_DOWN, std::bind(&Browser::_OnKeyDown, this));
}

Browser::~Browser()
{
    LogFunctionName;
    delete _directory;
}

void Browser::_OnKeyL2()
{
    LogFunctionName;
    _tabIndex += TAB_ITEM_NUM - 1;
    _tabIndex %= TAB_ITEM_NUM;
}

void Browser::_OnKeyR2()
{
    LogFunctionName;
    _tabIndex++;
    _tabIndex %= TAB_ITEM_NUM;
}

void Browser::_OnKeyUp()
{
    if (_tabIndex == 0)
    {
        _browserIndex += _directory->GetSize() - 1;
        _browserIndex %= _directory->GetSize();
    }
}

void Browser::_OnKeyDown()
{
    if (_tabIndex == 0)
    {
        _browserIndex++;
        _browserIndex %= _directory->GetSize();
    }
}

static bool GetDirectoryItem(void *data, int idx, const char **out_text)
{
    *out_text = ((Directory *)data)->GetItem(idx);
    return *out_text != nullptr;
}

SCREEN_STATUS Browser::Show()
{
    LogFunctionNameLimited;

    _input.Poll();

    ImGui_ImplVita2D_NewFrame();
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
    ImGui::SetNextWindowPos({MAIN_WINDOW_PADDING, MAIN_WINDOW_PADDING});
    ImGui::SetNextWindowSize({SCREEN_WIDTH - MAIN_WINDOW_PADDING * 2, SCREEN_HEIGHT - MAIN_WINDOW_PADDING * 2});
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
    return SCREEN_BROWSER;
}