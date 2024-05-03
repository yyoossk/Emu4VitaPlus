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

App::App()
{
    LogFunctionName;

    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);

    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);

    gConfig = new Config();
    gVideo = new Video();
    gEmulator = new Emulator();
    gUi = new Ui("ux0:");
}

App::~App()
{
    LogFunctionName;

    delete gUi;
    delete gEmulator;
    delete gVideo;
    delete gConfig;

    sceAppUtilShutdown();
}

void App::Run()
{
    LogFunctionName;

    gVideo->Start();

    while (gStatus != APP_STATUS_EXIT)
    {
        switch (gStatus)
        {
        case APP_STATUS_SHOW_UI:
        case APP_STATUS_SHOW_UI_IN_GAME:
            gUi->Run();
            break;
        case APP_STATUS_RUN_GAME:
            gEmulator->Run();
            break;
        default:
            break;
        }
    }
}