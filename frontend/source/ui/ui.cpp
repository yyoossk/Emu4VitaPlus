#include <imgui_vita2d/imgui_vita.h>
#include "my_imgui.h"
#include "ui.h"
#include "global.h"
#include "log.h"
#include "tab_system.h"
#include "tab_browser.h"
#include "tab_graphics.h"
#include "tab_about.h"

Ui::Ui(const char *path)
    : _tab_index(1)
{
    LogFunctionName;
    _tabs[TAB_ITEM_SYSTEM] = new TabSystem();
    _tabs[TAB_ITEM_BROWSER] = new TabBrowser(path);
    _tabs[TAB_ITEM_GRAPHICS] = new TabGraphics();
    _tabs[TAB_ITEM_ABOUT] = new TabAbout();

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
    _input.SetKeyUpCallback(SCE_CTRL_L1, std::bind(&Ui::_OnKeyL2, this, &_input));
    _input.SetKeyUpCallback(SCE_CTRL_R1, std::bind(&Ui::_OnKeyR2, this, &_input));
}

void Ui::_OnKeyL2(Input *input)
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

void Ui::_OnKeyR2(Input *input)
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
        ImGui::EndTabBar();
    }

    ImGui::End();
    ImGui::Render();
    My_ImGui_ImplVita2D_RenderDrawData(ImGui::GetDrawData());

    return;
}