#include <psp2/ctrl.h>
#include <psp2/power.h>
#include <psp2/touch.h>
#include <psp2/apputil.h>
#include "app.h"
#include "log.h"

#define APP_DATA_DIR "ux0:data/EMU4VITAPLUS/" APP_DIR_NAME
#define APP_LOG_PATH APP_DATA_DIR "/app_log.txt"

App::App() : _running(true)
{
    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);

    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);

    _gui = new Gui();
}

App::~App()
{
    delete _gui;
    sceAppUtilShutdown();
}

void App::Run()
{
    SceCtrlData ctrl_data;
    int port = 0;
    while (_running)
    {
        sceCtrlPeekBufferPositiveExt2(port, &ctrl_data, 1);
        _gui->Run();
    }
}