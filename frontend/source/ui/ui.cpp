#include <imgui_vita2d/imgui_vita.h>
#include "my_imgui.h"
#include "ui.h"
#include "log.h"
#include "tab_selectable.h"
#include "item_config.h"
#include "item_control.h"
#include "item_hotkey.h"
#include "tab_browser.h"
#include "tab_core.h"
#include "tab_about.h"
#include "global.h"

static void ResumeGame()
{
    LogFunctionName;
}

static void ExitApp()
{
    LogFunctionName;
    gStatus = APP_STATUS_EXIT;
}

static void ChangeLanguage()
{
    LogFunctionName;
    gVideo->Lock();
    My_Imgui_Destroy_Font();
    My_Imgui_Create_Font(gConfig->language);
    gVideo->Unlock();
    gConfig->Save();
}

static void ResetGraphics()
{
    LogFunctionName;
    gConfig->DefaultGraphics();
    gConfig->Save();
}

static void ResetControl()
{
    LogFunctionName;
    gConfig->DefaultControlMap();
    gConfig->Save();
}

static void ResetHotkey()
{
    LogFunctionName;
    gConfig->DefaultHotKey();
    gConfig->Save();
}

void Ui::_InitImgui()
{
    LogFunctionName;

    ImGui::CreateContext();
    ImGui_ImplVita2D_Init();
    ImGui_ImplVita2D_TouchUsage(false);
    ImGui_ImplVita2D_UseIndirectFrontTouch(false);
    ImGui_ImplVita2D_UseRearTouch(false);
    ImGui_ImplVita2D_GamepadUsage(false);

    ImGuiStyle *style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_TitleBg] = style->Colors[ImGuiCol_TitleBgActive];
}

void Ui::_DeinitImgui()
{
    LogFunctionName;

    ImGui_ImplVita2D_Shutdown();
    ImGui::DestroyContext();
}

Ui::Ui(const char *path) : _tab_index(TAB_ITEM_BROWSER)
{
    LogFunctionName;
    _InitImgui();

    _tabs[TAB_ITEM_SYSTEM] = new TabSeletable(TAB_SYSTEM,
                                              {new ItemConfig(SYSTEM_MENU_LANGUAGE,
                                                              &gConfig->language,
                                                              {LanguageString(gLanguageNames[LANGUAGE_ENGLISH]),
                                                               LanguageString(gLanguageNames[LANGUAGE_CHINESE])},
                                                              ChangeLanguage),
                                               new ItemBase(SYSTEM_MENU_EXIT, ExitApp)});

    _tabs[TAB_ITEM_BROWSER] = new TabBrowser(path);

    _tabs[TAB_ITEM_GRAPHICS] = new TabSeletable(TAB_GRAPHICS,
                                                {new ItemConfig(GRAPHICS_MENU_DISPLAY_SIZE,
                                                                &gConfig->graphics_config.size,
                                                                DISPLAY_SIZE_1X,
                                                                CONFIG_DISPLAY_SIZE_COUNT),
                                                 new ItemConfig(GRAPHICS_MENU_ASPECT_RATIO,
                                                                &gConfig->graphics_config.ratio,
                                                                ASPECT_RATIO_BY_GAME_RESOLUTION,
                                                                CONFIG_DISPLAY_RATIO_COUNT),
#ifdef WANT_DISPLAY_ROTATE
                                                 new ItemConfig(GRAPHICS_MENU_DISPLAY_ROTATE, (size_t *)&gConfig->graphics_config.rotate,
                                                                sizeof(gConfig->graphics_config.rotate),
                                                                DISPLAY_ROTATE_DISABLE,
                                                                CONFIG_DISPLAY_ROTATE_COUNT),
#endif
                                                 new ItemConfig(GRAPHICS_MENU_GRAPHICS_SHADER,
                                                                &gConfig->graphics_config.shader,
                                                                SHADER_DEFAULT,
                                                                CONFIG_GRAPHICS_SHADER_COUNT),
                                                 new ItemConfig(GRAPHICS_MENU_GRAPHICS_SMOOTH,
                                                                &gConfig->graphics_config.smooth,
                                                                NO,
                                                                CONFIG_GRAPHICS_SMOOTHER_COUNT),
                                                 new ItemConfig(GRAPHICS_MENU_OVERLAY_MODE,
                                                                &gConfig->graphics_config.overlay_mode,
                                                                OVERLAY_MODE_OVERLAY,
                                                                CONFIG_GRAPHICS_OVERLAY_MODE_COUNT),
                                                 new ItemBase(RESET_CONFIGS, ResetGraphics)});
    std::vector<ItemBase *> controls;
    for (ControlMapConfig &cmap : gConfig->control_maps)
    {
        controls.emplace_back(new ItemControl(&cmap));
    }
    controls.emplace_back(new ItemBase(RESET_CONFIGS, ResetControl));
    _tabs[TAB_ITME_CONTROL] = new TabSeletable(TAB_CONTROL, controls);

    std::vector<ItemBase *> hotkeys;
    for (size_t i = 0; i < HOT_KEY_COUNT; i++)
    {
        hotkeys.emplace_back(new ItemHotkey((HotKeyConfig)i, &gConfig->hotkeys[i]));
    }
    hotkeys.emplace_back(new ItemBase(RESET_CONFIGS, ResetHotkey));
    _tabs[TAB_ITEM_HOTKEY] = new TabSeletable(TAB_HOTKEY, hotkeys);

    _tabs[TAB_ITEM_CORE] = new TabCore();

    _tabs[TAB_ITEM_ABOUT] = new TabAbout();

    _tabs[_tab_index]->SetInputHooks(&_input);

    _SetKeyHooks();

    // _update_sema = sceKernelCreateSema("ui", 0, 1, 1, NULL);
}

Ui::~Ui()
{
    LogFunctionName;
    for (auto &tab : _tabs)
    {
        delete tab;
    }

    _DeinitImgui();
    // sceKernelDeleteSema(_update_sema);
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
        _tab_index = (_tab_index == 0 ? TAB_ITEM_COUNT - 1 : _tab_index - 1);
    } while (!_tabs[_tab_index]->Visable());

    _tabs[_tab_index]->SetInputHooks(&_input);
}

void Ui::_OnKeyR2(Input *input)
{
    // LogFunctionName;
    _tabs[_tab_index]->UnsetInputHooks(&_input);

    do
    {
        _tab_index = (_tab_index + 1 == TAB_ITEM_COUNT ? 0 : _tab_index + 1);
    } while (!_tabs[_tab_index]->Visable());

    _tabs[_tab_index]->SetInputHooks(&_input);
}

void Ui::Run()
{
    // LogFunctionName;

    _input.Poll();
    // sceKernelSignalSema(_update_sema, 1);
}

void Ui::Show()
{
    LogFunctionNameLimited;
    vita2d_set_clip_rectangle(0, 0, VITA_WIDTH, VITA_HEIGHT);
    // sceKernelWaitSema(_update_sema, 1, NULL);

    ImGui_ImplVita2D_NewFrame();
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
    ImGui::SetNextWindowPos({MAIN_WINDOW_PADDING, MAIN_WINDOW_PADDING});
    ImGui::SetNextWindowSize({VITA_WIDTH - MAIN_WINDOW_PADDING * 2, VITA_HEIGHT - MAIN_WINDOW_PADDING * 2});

    ImGui::Begin("Emu4Vita++ (" APP_DIR_NAME ")", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    if (gStatus == APP_STATUS_BOOT)
    {
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        for (const auto &log : _logs)
        {
            ImGui::Text(log.c_str());
        }
        ImGui::PopStyleColor();
    }
    else if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None)) // APP_STATUS_SHOW_UI or APP_STATUS_SHOW_UI_IN_GAME
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
    ImGui_ImplVita2D_RenderDrawData(ImGui::GetDrawData());

    return;
}