#include <imgui_vita2d/imgui_vita.h>
#include "my_imgui.h"
#include "ui.h"
#include "app.h"
#include "video.h"
#include "log.h"
#include "state_manager.h"
#include "tab_selectable.h"
#include "item_config.h"
#include "item_control.h"
#include "item_hotkey.h"
#include "item_core.h"
#include "item_state.h"
#include "tab_browser.h"
#include "tab_about.h"

#define MAIN_WINDOW_PADDING 10

Ui *gUi = nullptr;

static void ResumeGame()
{
    LogFunctionName;
    gStatus = APP_STATUS_RUN_GAME;
}

static void ResetGame()
{
    LogFunctionName;
    gEmulator->Reset();
    gStatus = APP_STATUS_RUN_GAME;
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
    My_ImGui_ImplVita2D_Init(LANGUAGE::LANGUAGE_ENGLISH);
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
    My_ImGui_ImplVita2D_Shutdown();
    ImGui::DestroyContext();
}

Ui::Ui() : _tab_index(TAB_INDEX_BROWSER)
{
    LogFunctionName;
    _InitImgui();
}

Ui::~Ui()
{
    LogFunctionName;
    for (auto &tab : _tabs)
    {
        if (tab != nullptr)
        {
            delete tab;
        }
    }

    _DeinitImgui();
}

void Ui::CreateTables(const char *path)
{
    if (gEmulator == nullptr)
    {
        LogError("gEmulator has not been initialized yet.");
        return;
    }

    _tabs[TAB_INDEX_SYSTEM] = new TabSeletable(TAB_SYSTEM,
                                               {new ItemBase(SYSTEM_RESUME_GAME, "", ResumeGame, NULL, false),
                                                new ItemBase(SYSTEM_RESET_GAME, "", ResetGame, NULL, false),
                                                new ItemConfig(SYSTEM_MENU_LANGUAGE,
                                                               "",
                                                               (uint32_t *)&gConfig->language,
                                                               {LanguageString(gLanguageNames[LANGUAGE_ENGLISH]),
                                                                LanguageString(gLanguageNames[LANGUAGE_CHINESE])},
                                                               ChangeLanguage),

                                                new ItemBase(SYSTEM_MENU_EXIT, "", ExitApp)});

    std::vector<ItemBase *> states;
    for (size_t i = 0; i < MAX_STATES; i++)
    {
        states.emplace_back(new ItemState(gStateManager->states[i]));
    }
    _tabs[TAB_INDEX_STATE] = new TabSeletable(TAB_STATE, states, 1);
    _tabs[TAB_INDEX_STATE]->SetVisable(false);

    _tabs[TAB_INDEX_BROWSER] = new TabBrowser(path);

    _tabs[TAB_INDEX_GRAPHICS] = new TabSeletable(TAB_GRAPHICS,
                                                 {new ItemConfig(GRAPHICS_MENU_DISPLAY_SIZE,
                                                                 "",
                                                                 &gConfig->graphics[DISPLAY_SIZE],
                                                                 DISPLAY_SIZE_1X,
                                                                 CONFIG_DISPLAY_SIZE_COUNT),
                                                  new ItemConfig(GRAPHICS_MENU_ASPECT_RATIO,
                                                                 "",
                                                                 &gConfig->graphics[DISPLAY_RATIO],
                                                                 ASPECT_RATIO_BY_GAME_RESOLUTION,
                                                                 CONFIG_DISPLAY_RATIO_COUNT),
#ifdef WANT_DISPLAY_ROTATE
                                                  new ItemConfig(GRAPHICS_MENU_DISPLAY_ROTATE,
                                                                 "",
                                                                 &gConfig->graphics[DISPLAY_ROTATE],
                                                                 DISPLAY_ROTATE_DISABLE,
                                                                 CONFIG_DISPLAY_ROTATE_COUNT),
#endif
                                                  new ItemConfig(GRAPHICS_MENU_GRAPHICS_SHADER,
                                                                 "",
                                                                 &gConfig->graphics[GRAPHICS_SHADER],
                                                                 SHADER_DEFAULT,
                                                                 CONFIG_GRAPHICS_SHADER_COUNT),
                                                  new ItemConfig(GRAPHICS_MENU_GRAPHICS_SMOOTH,
                                                                 "",
                                                                 &gConfig->graphics[GRAPHICS_SMOOTH],
                                                                 NO,
                                                                 CONFIG_GRAPHICS_SMOOTHER_COUNT),
                                                  new ItemConfig(GRAPHICS_MENU_OVERLAY_MODE,
                                                                 "",
                                                                 &gConfig->graphics[GRAPHICS_OVERLAY],
                                                                 OVERLAY_MODE_OVERLAY,
                                                                 CONFIG_GRAPHICS_OVERLAY_MODE_COUNT),
                                                  new ItemBase(RESET_CONFIGS, "", ResetGraphics)});
    std::vector<ItemBase *> controls;
    for (ControlMapConfig &cmap : gConfig->control_maps)
    {
        controls.emplace_back(new ItemControl(&cmap));
    }
    controls.emplace_back(new ItemBase(RESET_CONFIGS, "", ResetControl));
    _tabs[TAB_INDEX_CONTROL] = new TabSeletable(TAB_CONTROL, controls);

    std::vector<ItemBase *> hotkeys;
    for (size_t i = 0; i < HOT_KEY_COUNT; i++)
    {
        hotkeys.emplace_back(new ItemHotkey((HotKeyConfig)i, &gConfig->hotkeys[i]));
    }
    hotkeys.emplace_back(new ItemBase(RESET_CONFIGS, "", ResetHotkey));
    _tabs[TAB_INDEX_HOTKEY] = new TabSeletable(TAB_HOTKEY, hotkeys);

    std::vector<ItemBase *> options;
    for (auto &iter : gConfig->core_options)
    {
        options.emplace_back(new ItemCore(&iter.second));
    }
    _tabs[TAB_INDEX_CORE] = new TabSeletable(TAB_CORE, options);

    _tabs[TAB_INDEX_ABOUT] = new TabAbout();

    SetInputHooks();
}

void Ui::SetInputHooks()
{
    _input.SetKeyUpCallback(SCE_CTRL_L1, std::bind(&Ui::_OnKeyL2, this, &_input));
    _input.SetKeyUpCallback(SCE_CTRL_R1, std::bind(&Ui::_OnKeyR2, this, &_input));

    _tabs[_tab_index]->SetInputHooks(&_input);
    while (!_tabs[_tab_index]->Visable())
    {
        _OnKeyL2(&_input);
    }
}

void Ui::_OnKeyL2(Input *input)
{
    // LogFunctionName;
    _tabs[_tab_index]->UnsetInputHooks(&_input);

    do
    {
        LOOP_MINUS_ONE(_tab_index, TAB_INDEX_COUNT);
    } while (!_tabs[_tab_index]->Visable());

    _tabs[_tab_index]->SetInputHooks(&_input);
}

void Ui::_OnKeyR2(Input *input)
{
    // LogFunctionName;
    _tabs[_tab_index]->UnsetInputHooks(&_input);

    do
    {
        LOOP_PLUS_ONE(_tab_index, TAB_INDEX_COUNT);
    } while (!_tabs[_tab_index]->Visable());

    _tabs[_tab_index]->SetInputHooks(&_input);
}

void Ui::Run()
{
    _input.Poll();
    static APP_STATUS last_status = APP_STATUS_SHOW_UI;
    if (gStatus != last_status)
    {
        gVideo->Lock();

        _tabs[TAB_INDEX_STATE]->SetVisable(gStatus == APP_STATUS_SHOW_UI_IN_GAME);
        _tabs[TAB_INDEX_BROWSER]->SetVisable(gStatus == APP_STATUS_SHOW_UI);

        TabSeletable *system_tab = (TabSeletable *)(_tabs[TAB_INDEX_SYSTEM]);
        system_tab->SetItemVisable(0, gStatus == APP_STATUS_SHOW_UI_IN_GAME);
        system_tab->SetItemVisable(1, gStatus == APP_STATUS_SHOW_UI_IN_GAME);

        gVideo->Unlock();

        SetInputHooks();

        last_status = gStatus;
    }

    // sceKernelSignalSema(_update_sema, 1);
}

void Ui::_ShowBoot()
{
    static const char *frames[] = {"-",
                                   "\\",
                                   "|",
                                   "/"};
    static size_t count = 0;
    static uint64_t next_ms = 0;

    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
    for (const auto &log : _logs)
    {
        ImGui::Text(log.c_str());
    }

    if (_logs.size() > 0 && _logs.back() != "Done")
    {
        uint64_t current_ms = sceKernelGetProcessTimeWide();
        if (next_ms <= current_ms)
        {
            count++;
            if (count >= sizeof(frames) / sizeof(*frames))
            {
                count = 0;
            }
            next_ms = current_ms + 200000;
        }
        ImGui::Text(frames[count]);
    }
    ImGui::PopStyleColor();
}

void Ui::_ShowNormal()
{
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
}

void Ui::Show()
{
    LogFunctionNameLimited;
    // vita2d_set_clip_rectangle(0, 0, VITA_WIDTH, VITA_HEIGHT);

    ImGui_ImplVita2D_NewFrame();
    ImGui::SetMouseCursor(ImGuiMouseCursor_None);
    ImGui::SetNextWindowPos({MAIN_WINDOW_PADDING, MAIN_WINDOW_PADDING});
    ImGui::SetNextWindowSize({VITA_WIDTH - MAIN_WINDOW_PADDING * 2, VITA_HEIGHT - MAIN_WINDOW_PADDING * 2});

    ImGui::Begin("Emu4Vita++ (" APP_DIR_NAME ")", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    gStatus == APP_STATUS_BOOT ? _ShowBoot() : _ShowNormal();

    ImGui::End();
    ImGui::Render();
    My_ImGui_ImplVita2D_RenderDrawData(ImGui::GetDrawData());

    return;
}

void Ui::AppendLog(const char *log)
{
    LogDebug("boot log: %s", log);
    _logs.emplace_back(log);
}

void Ui::ClearLogs()
{
    LogFunctionName;
    _logs.clear();
}