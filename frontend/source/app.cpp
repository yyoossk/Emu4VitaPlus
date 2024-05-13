#include <psp2/ctrl.h>
#include <psp2/power.h>
#include <psp2/touch.h>
#include <psp2/apputil.h>
#include <psp2/display.h>
#include <vita2d.h>
#include "my_imgui.h"
#include "app.h"
#include "video.h"
#include "emulator.h"
#include "config.h"
#include "ui.h"
#include "state_manager.h"
// #include "file.h"
#include "log.h"
#include "defines.h"

APP_STATUS gStatus = APP_STATUS_BOOT;

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

    gConfig = new Config();

    gVideo = new Video();
    gUi = new Ui();
    gVideo->Start();

    gUi->AppendLog("Booting");
    gUi->AppendLog("Initialize video");
    gUi->AppendLog("Load config");
    if (!gConfig->Load())
    {
        gConfig->Save();
    }

    gUi->AppendLog("Initialize emulator");
    gEmulator = new Emulator();

    gUi->AppendLog("Initialize state manager");
    gStateManager = new StateManager();

    gUi->AppendLog("Create tables of UI");
    gUi->CreateTables("ux0:");

    if (gConfig->language != LANGUAGE::LANGUAGE_ENGLISH)
    {
        gUi->AppendLog("Load font");
        gVideo->Lock();
        My_Imgui_Destroy_Font();
        My_Imgui_Create_Font(gConfig->language);
        gVideo->Unlock();
    }

    gUi->ClearLogs();
}

App::~App()
{
    LogFunctionName;

    delete gVideo;
    delete gUi;
    delete gEmulator;
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