#include <imgui_vita2d/imgui_vita.h>
#include "my_imgui.h"
#include "ui.h"
#include "log.h"
#include "tab_selectable.h"
#include "item_config.h"
#include "item_control.h"
#include "tab_browser.h"
#include "tab_control.h"
#include "tab_hotkey.h"
#include "tab_core.h"
#include "tab_about.h"
#include "global.h"

void ExitApp()
{
    LogFunctionName;
    gStatus = APP_STATUS_EXIT;
}

Ui::Ui(const char *path)
    : _tab_index(TAB_ITEM_BROWSER)
{
    LogFunctionName;

    _tabs[TAB_ITEM_SYSTEM] = new TabSeletable(TAB_SYSTEM, {new ItemConfig(SYSTEM_MENU_LANGUAGE,
                                                                          &gConfig->language,
                                                                          {LanguageString(gLanguageNames[LANGUAGE_ENGLISH]),
                                                                           LanguageString(gLanguageNames[LANGUAGE_CHINESE])}),
                                                           new ItemBase(SYSTEM_MENU_EXIT, ExitApp)});
    _tabs[TAB_ITEM_BROWSER] = new TabBrowser(path);
    _tabs[TAB_ITEM_GRAPHICS] = new TabSeletable(TAB_GRAPHICS, {new ItemConfig(GRAPHICS_MENU_DISPLAY_SIZE,
                                                                              &gConfig->graphics_config.size,
                                                                              DISPLAY_SIZE_1X,
                                                                              4),
                                                               new ItemConfig(GRAPHICS_MENU_ASPECT_RATIO,
                                                                              &gConfig->graphics_config.ratio,
                                                                              ASPECT_RATIO_BY_GAME_RESOLUTION,
                                                                              6),
#ifdef WANT_DISPLAY_ROTATE
                                                               new ItemConfig(GRAPHICS_MENU_DISPLAY_ROTATE, (size_t *)&gConfig->graphics_config.rotate,
                                                                              sizeof(gConfig->graphics_config.rotate),
                                                                              DISPLAY_ROTATE_DISABLE,
                                                                              5),
#endif
                                                               new ItemConfig(GRAPHICS_MENU_GRAPHICS_SHADER,
                                                                              &gConfig->graphics_config.shader,
                                                                              SHADER_DEFAULT,
                                                                              5),
                                                               new ItemConfig(GRAPHICS_MENU_GRAPHICS_SMOOTH,
                                                                              &gConfig->graphics_config.smooth,
                                                                              NO,
                                                                              2),
                                                               new ItemConfig(GRAPHICS_MENU_OVERLAY_MODE,
                                                                              &gConfig->graphics_config.overlay_mode,
                                                                              OVERLAY_MODE_OVERLAY,
                                                                              2)});
    std::vector<ItemBase *> controls;
    for (ControlMapConfig &cmap : gConfig->control_maps)
    {
        controls.emplace_back(new ItemControl(&cmap));
    }

    _tabs[TAB_ITME_CONTROL] = new TabSeletable(TAB_CONTROL, controls);
    _tabs[TAB_ITEM_HOTKEY] = new TabHotkey();
    _tabs[TAB_ITEM_CORE] = new TabCore();
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