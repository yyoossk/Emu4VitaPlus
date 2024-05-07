#include <psp2/ctrl.h>
#include <psp2/power.h>
#include <psp2/touch.h>
#include <psp2/apputil.h>
#include <psp2/display.h>
#include <vita2d.h>
#include "my_imgui.h"
#include "app.h"
#include "global.h"
#include "file.h"
#include "log.h"

static void ChangeFont()
{
    LogFunctionName;
    gVideo->Lock();
    ImGui_ImplVita2D_InvalidateDeviceObjects();
    ImGui::GetIO().Fonts->Clear();
    My_Imgui_Create_Font(gConfig->language);
    gVideo->Unlock();
}

App::App()
{
    LogFunctionName;

    scePowerSetArmClockFrequency(444);
    scePowerSetBusClockFrequency(222);
    scePowerSetGpuClockFrequency(222);
    scePowerSetGpuXbarClockFrequency(166);

    sceShellUtilInitEvents(0);

    SceAppUtilInitParam init_param{0};
    SceAppUtilBootParam boot_param{0};
    sceAppUtilInit(&init_param, &boot_param);

    SceCommonDialogConfigParam config;
    sceCommonDialogConfigParamInit(&config);
    sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_LANG, (int *)&config.language);
    sceAppUtilSystemParamGetInt(SCE_SYSTEM_PARAM_ID_ENTER_BUTTON, (int *)&config.enterButtonAssign);
    sceCommonDialogSetConfigParam(&config);

    sceCtrlSetSamplingModeExt(SCE_CTRL_MODE_ANALOG);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);

    _IsSaveMode();
    gVideo = new Video();

    gConfig = new Config();
    gEmulator = new Emulator();
    gUi = new Ui("ux0:");
    gVideo->Start();

    gUi->AppendLog("Booting");
    gUi->AppendLog("Load config");
    if (!gConfig->Load())
    {
        gConfig->Save();
    }

    gUi->AppendLog("Load font");
    ChangeFont();

    // sceKernelDelayThread(3000000);
}

App::~App()
{
    LogFunctionName;

    gVideo->Stop();
    delete gEmulator;
    gVideo->Lock();
    delete gUi;
    gVideo->Unlock();
    delete gVideo;
    delete gConfig;

    sceAppUtilShutdown();
}

void App::Run()
{
    LogFunctionName;
    gStatus = APP_STATUS_SHOW_UI;
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

        case APP_STATUS_BOOT:
        default:
            break;
        }
    }
}

bool App::_IsSaveMode()
{
    return sceIoDevctl("ux0:", 0x3001, NULL, 0, NULL, 0) == 0x80010030;
}