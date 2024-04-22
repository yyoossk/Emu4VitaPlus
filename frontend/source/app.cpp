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
    gEmulator = new Emulator();
    gUi = new Ui("ux0:");
    _video = new Video();
}

App::~App()
{
    LogFunctionName;

    delete _video;
    delete gUi;
    delete gEmulator;
    delete gConfig;

    sceAppUtilShutdown();
}

void App::Run()
{
    LogFunctionName;

    _video->Start();

    while (gStatus != APP_STATUS_EXIT)
    {
        switch (gStatus)
        {
        case APP_STATUS_SHOW_BROWSER:
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