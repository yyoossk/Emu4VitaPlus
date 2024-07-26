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
#include "utils.h"
#include "overlay.h"
#include "shader.h"
#include "defines.h"

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

static void ExitGame()
{
    LogFunctionName;
    gEmulator->UnloadGame();
    gStatus = APP_STATUS_SHOW_UI;
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
    My_Imgui_Create_Font(gConfig->language, CACHE_DIR);
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
    My_ImGui_ImplVita2D_Init(LANGUAGE::LANGUAGE_ENGLISH, CACHE_DIR);
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

Ui::Ui() : _tab_index(TAB_INDEX_BROWSER), _tabs{nullptr}
{
    LogFunctionName;
    _title = std::string("Emu4Vita++ (") + CORE_FULL_NAME + ") v" + APP_VER_STR;
    _InitImgui();
}

Ui::~Ui()
{
    LogFunctionName;
    _ClearTabs();
    _DeinitImgui();
}

void Ui::_ClearTabs()
{
    for (size_t i = 0; i < TAB_INDEX_COUNT; i++)
    {
        if (_tabs[i] != nullptr)
        {
            delete _tabs[i];
        }
        _tabs[i] = nullptr;
    }
}

void Ui::CreateTables(const char *path)
{
    LogFunctionName;
    LogDebug("  path: %s", path);

    if (gEmulator == nullptr)
    {
        LogError("gEmulator has not been initialized yet.");
        return;
    }

    _ClearTabs();

    _tabs[TAB_INDEX_SYSTEM] = new TabSeletable(TAB_SYSTEM,
                                               {new ItemBase(SYSTEM_RESUME_GAME, "", ResumeGame, NULL, false),
                                                new ItemBase(SYSTEM_RESET_GAME, "", ResetGame, NULL, false),
                                                new ItemBase(SYSTEM_EXIT_GAME, "", ExitGame, NULL, false),
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
                                                                 CONFIG_DISPLAY_SIZE_COUNT,
                                                                 std::bind(&Emulator::ChangeGraphicsConfig, gEmulator)),
                                                  new ItemConfig(GRAPHICS_MENU_ASPECT_RATIO,
                                                                 "",
                                                                 &gConfig->graphics[DISPLAY_RATIO],
                                                                 ASPECT_RATIO_BY_GAME_RESOLUTION,
                                                                 CONFIG_DISPLAY_RATIO_COUNT,
                                                                 std::bind(&Emulator::ChangeGraphicsConfig, gEmulator)),
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
                                                                 gShaders->GetConfigs(),
                                                                 std::bind(&Emulator::ChangeGraphicsConfig, gEmulator)),
                                                  new ItemConfig(GRAPHICS_MENU_GRAPHICS_SMOOTH,
                                                                 "",
                                                                 &gConfig->graphics[GRAPHICS_SMOOTH],
                                                                 NO,
                                                                 CONFIG_GRAPHICS_SMOOTHER_COUNT,
                                                                 std::bind(&Emulator::ChangeGraphicsConfig, gEmulator)),
                                                  new ItemConfig(GRAPHICS_MENU_OVERLAY_MODE,
                                                                 "",
                                                                 &gConfig->graphics[GRAPHICS_OVERLAY_MODE],
                                                                 OVERLAY_MODE_OVERLAY,
                                                                 CONFIG_GRAPHICS_OVERLAY_MODE_COUNT,
                                                                 std::bind(&Emulator::ChangeGraphicsConfig, gEmulator)),
                                                  new ItemConfig(GRAPHICS_MENU_OVERLAY_SELECT,
                                                                 "",
                                                                 &gConfig->graphics[GRAPHICS_OVERLAY],
                                                                 gOverlays->GetConfigs(),
                                                                 std::bind(&Emulator::ChangeGraphicsConfig, gEmulator)),
                                                  new ItemBase(RESET_CONFIGS,
                                                               "",
                                                               ResetGraphics,
                                                               std::bind(&Emulator::ChangeGraphicsConfig, gEmulator))});
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

    _tabs[TAB_INDEX_OPTIONS] = new TabSeletable(TAB_OPTIONS, {new ItemConfig(OPTIONS_MENU_LANGUAGE,
                                                                             "",
                                                                             (uint32_t *)&gConfig->language,
                                                                             {LanguageString(gLanguageNames[LANGUAGE_ENGLISH]),
                                                                              LanguageString(gLanguageNames[LANGUAGE_CHINESE])},
                                                                             ChangeLanguage),
                                                              new ItemConfig(OPTIONS_MENU_REWIND,
                                                                             "",
                                                                             &gConfig->rewind,
                                                                             {NO, YES},
                                                                             std::bind(&Emulator::ChangeRewindConfig, gEmulator)),
                                                              new ItemIntConfig(OPTIONS_MENU_REWIND_BUF_SIZE,
                                                                                "",
                                                                                &gConfig->rewind_buf_size,
                                                                                MIN_REWIND_BUF_SIZE,
                                                                                MAX_REWIND_BUF_SIZE,
                                                                                REWIND_BUF_SIZE_STEP)});

    _tabs[TAB_INDEX_ABOUT] = new TabAbout();

    SetInputHooks();
}

void Ui::SetInputHooks()
{
    _input.SetKeyUpCallback(SCE_CTRL_L1, std::bind(&Ui::_OnKeyL2, this, &_input));
    _input.SetKeyUpCallback(SCE_CTRL_R1, std::bind(&Ui::_OnKeyR2, this, &_input));

    while (!_tabs[_tab_index]->Visable())
    {
        _OnKeyL2(&_input);
    }
    _tabs[_tab_index]->SetInputHooks(&_input);
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

void Ui::_OnPsButton(Input *input)
{
    LogFunctionName;
    ResumeGame();
}

void Ui::Run()
{
    _input.Poll(true);
    static APP_STATUS last_status = APP_STATUS_SHOW_UI;
    if (gStatus != last_status)
    {
        LogDebug("  status change to: %d last status: %d", gStatus, last_status);
        LogDebug("  _tab_index: %d", _tab_index);
        gVideo->Lock();

        _tabs[TAB_INDEX_STATE]->SetVisable(gStatus == APP_STATUS_SHOW_UI_IN_GAME);
        _tabs[TAB_INDEX_BROWSER]->SetVisable(gStatus == APP_STATUS_SHOW_UI);

        TabSeletable *system_tab = (TabSeletable *)(_tabs[TAB_INDEX_SYSTEM]);
        system_tab->SetItemVisable(0, gStatus == APP_STATUS_SHOW_UI_IN_GAME);
        system_tab->SetItemVisable(1, gStatus == APP_STATUS_SHOW_UI_IN_GAME);
        system_tab->SetItemVisable(2, gStatus == APP_STATUS_SHOW_UI_IN_GAME);

        if (gStatus == APP_STATUS_SHOW_UI_IN_GAME)
        {
            system_tab->SetIndex(0);
            _input.SetKeyUpCallback(SCE_CTRL_PSBUTTON, std::bind(&Ui::_OnPsButton, this, &_input));
        }
        else
        {
            _tab_index = TAB_INDEX_BROWSER;
            _input.UnsetKeyUpCallback(SCE_CTRL_PSBUTTON);
        }

        SetInputHooks();

        gVideo->Unlock();

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

    for (const auto &log : _logs)
    {
        if (log == _logs.back())
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
        }
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

    if (_logs.size() > 0)
    {
        ImGui::PopStyleColor();
    }

    if (ImGui::GetScrollMaxY() > 0.f)
    {
        ImGui::SetScrollHereY(1.f);
    }
}

void Ui::_ShowNormal()
{
    if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None))
    {
        for (size_t i = 0; i < TAB_INDEX_COUNT; i++)
        {
            if (_tabs[i] && _tabs[i]->Visable())
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

    ImGui::Begin(_title.c_str(), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    ShowTimePower();

    gStatus == APP_STATUS_BOOT ? _ShowBoot() : _ShowNormal();

    ImGui::End();
    ImGui::Render();
    My_ImGui_ImplVita2D_RenderDrawData(ImGui::GetDrawData());

    return;
}

void Ui::AppendLog(const char *log)
{
    LogDebug("boot log: %s", log);

    gVideo->Lock();
    _logs.emplace_back(log);
    gVideo->Unlock();
}

void Ui::ClearLogs()
{
    LogFunctionName;
    _logs.clear();
}

void Ui::UpdateCoreOptions()
{
    std::vector<ItemBase *> options;
    for (auto &iter : gConfig->core_options)
    {
        options.emplace_back(new ItemCore(&iter.second));
    }

    gVideo->Lock();

    if (_tabs[TAB_INDEX_CORE] != nullptr)
    {
        delete _tabs[TAB_INDEX_CORE];
    }

    _tabs[TAB_INDEX_CORE] = new TabSeletable(TAB_CORE, options);

    gVideo->Unlock();
}