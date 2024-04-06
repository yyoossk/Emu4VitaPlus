#include <imgui_vita2d/imgui_vita.h>
#include "my_imgui.h"
#include "ui.h"
#include "global.h"
#include "log.h"
#include "tab_system.h"
#include "tab_browser.h"
#include "tab_about.h"

enum
{
    TAB_ITEM_SYSTEM = 0,
    TAB_ITEM_BROWSER,
    TAB_ITEM_ABOUT,
    // TAB_ITEM_FAVORITE,
    TAB_ITEM_COUNT
};

Ui::Ui(const char *path)
    : _tab_index(1)
{
    LogFunctionName;
    _tabs[0] = new TabSystem();
    _tabs[1] = new TabBrowser(path);
    _tabs[2] = new TabAbout();

    _tabs[_tab_index]->SetInputHooks(&_input);

    _SetKeyHooks();
}

Ui::~Ui()
{
    LogFunctionName;
    for (auto &tab : _tabs)
    {
        delete tab;
    }
}

void Ui::_SetKeyHooks()
{
    _input.SetKeyUpCallback(SCE_CTRL_L1, std::bind(&Ui::_OnKeyL2, this));
    _input.SetKeyUpCallback(SCE_CTRL_R1, std::bind(&Ui::_OnKeyR2, this));
}

void Ui::_OnKeyL2()
{
    // LogFunctionName;
    _tabs[_tab_index]->UnsetInputHooks(&_input);

    do
    {
        _tab_index += TAB_ITEM_COUNT - 1;
        _tab_index %= TAB_ITEM_COUNT;
    } while (!_tabs[_tab_index]->Visable());

    _tabs[_tab_index]->SetInputHooks(&_input);
}

void Ui::_OnKeyR2()
{
    // LogFunctionName;
    _tabs[_tab_index]->UnsetInputHooks(&_input);

    do
    {
        _tab_index++;
        _tab_index %= TAB_ITEM_COUNT;
    } while (!_tabs[_tab_index]->Visable());

    _tabs[_tab_index]->SetInputHooks(&_input);
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
    ImGui::Begin("Emu4Vita++ (" APP_DIR_NAME ")", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

    if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None))
    {
        for (size_t i = 0; i < _tabs.size(); i++)
        {
            if (_tabs[i]->Visable())
            {
                _tabs[i]->Show(_tab_index == i);
            }
        }
        // if (ImGui::BeginTabItem("Browser", NULL, _tab_index == 0 ? ImGuiTabItemFlags_SetSelected : 0))
        // {
        //     ImGui::Text(_directory->GetCurrentPath().c_str());
        //     auto size = ImGui::GetContentRegionAvail();
        //     ImGui::ListBoxHeader("", {size.x * 0.5f, size.y});
        //     for (size_t i = 0; i < _directory->GetSize(); i++)
        //     {
        //         const auto item = _directory->GetItem(i);

        //         if (item.isDir)
        //         {
        //             ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        //         }

        //         ImGui::Selectable(item.name.c_str(), i == _browser_index);
        //         if (item.isDir)
        //         {
        //             ImGui::PopStyleColor();
        //         }

        //         if (i == _browser_index && ImGui::GetScrollMaxY() > 0.f)
        //         {
        //             ImGui::SetScrollHereY((float)_browser_index / (float)_directory->GetSize());
        //         }
        //     }

        //     // LogDebug("GetScrollY %f %f", ImGui::GetScrollY(), ImGui::GetScrollMaxY());
        //     ImGui::ListBoxFooter();
        //     ImGui::EndTabItem();
        // }
        // if (ImGui::BeginTabItem("Favorite", NULL, _tab_index == 1 ? ImGuiTabItemFlags_SetSelected : 0))
        // {
        //     ImGui::Text("This is the Favorite tab!\nblah blah blah blah blah");
        //     ImGui::EndTabItem();
        // }
        ImGui::EndTabBar();
    }

    ImGui::End();
    ImGui::Render();
    My_ImGui_ImplVita2D_RenderDrawData(ImGui::GetDrawData());

    return;
}