#include <psp2/ctrl.h>
#include <psp2/power.h>
#include <psp2/touch.h>
#include <psp2/apputil.h>
#include <vita2d.h>
#include "app.h"
#include "log.h"
#include "file.h"
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

    gLog = new cLog(APP_LOG_PATH);
    File::MakeDirs(APP_DATA_DIR);

    vita2d_init();

    _gui = new Gui();
}

App::~App()
{
    LogFunctionName;

    delete _gui;
    delete gLog;
    vita2d_fini();
    sceAppUtilShutdown();
}

void App::Run()
{
    LogFunctionName;

    while (_running)
    {
        vita2d_start_drawing();
        vita2d_clear_screen();
        _gui->Run();
        vita2d_end_drawing();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
    }
}