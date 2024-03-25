#include <psp2/ctrl.h>
#include <psp2/power.h>
#include <psp2/touch.h>
#include <psp2/apputil.h>
#include <psp2/display.h>
#include <vita2d.h>
#include <imgui_vita2d/imgui_vita.h>
#include "app.h"
#include "global.h"
#include "file.h"
#include "log.h"

App::App() : _running(true)
{
    LogFunctionName;

    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);

    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);

    vita2d_init();
    _InitImgui();
}

App::~App()
{
    LogFunctionName;

    _DeinitImgui();
    vita2d_fini();
    delete gLog;
    sceAppUtilShutdown();
}

void App::_InitImgui()
{
    LogFunctionName;

    ImGui::CreateContext();
    ImGui_ImplVita2D_Init();

    ImGui_ImplVita2D_TouchUsage(false);
    ImGui_ImplVita2D_UseIndirectFrontTouch(false);
    ImGui_ImplVita2D_UseRearTouch(false);
    ImGui_ImplVita2D_GamepadUsage(false);
}

void App::_DeinitImgui()
{
    LogFunctionName;

    ImGui_ImplVita2D_Shutdown();
    ImGui::DestroyContext();
}

void App::Run()
{
    LogFunctionName;

    while (_running)
    {
        vita2d_start_drawing();
        vita2d_clear_screen();
        _browser.Show();
        vita2d_end_drawing();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
    }
}