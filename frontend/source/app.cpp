#include <psp2/ctrl.h>
#include <psp2/power.h>
#include <psp2/touch.h>
#include <psp2/apputil.h>
#include <psp2/display.h>
#include <vita2d.h>
#include <imgui_vita2d/imgui_vita.h>
#include "app.h"
#include "log.h"
#include "file.h"
#include "log.h"

#define APP_DATA_DIR "ux0:data/EMU4VITAPLUS/" APP_DIR_NAME
#define APP_LOG_PATH APP_DATA_DIR "/app_log.txt"

#if LOG_LEVEL != LOG_LEVEL_OFF
Log *gLog = NULL;
#endif

App::App() : _running(true)
{
    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);

    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);

    File::MakeDirs(APP_DATA_DIR);
    gLog = new Log(APP_LOG_PATH);

    vita2d_init();
}

App::~App()
{
    LogFunctionName;

    delete gLog;
    vita2d_fini();
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

        vita2d_end_drawing();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
    }
}