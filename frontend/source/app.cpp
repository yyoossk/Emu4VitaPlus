#include <psp2/ctrl.h>
#include <psp2/power.h>
#include <psp2/touch.h>
#include "app.h"

App::App()
{
    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);

    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);

    vita2d_init();
}

App::~App()
{
    vita2d_fini();
}