#include "global.h"
#include "video.h"
#include "log.h"
#include "my_imgui.h"

Video::Video() : ThreadBase(_DrawThread)
{
    vita2d_init();
    _InitImgui();
}

Video::~Video()
{
    if (IsRunning())
    {
        Stop();
    }

    vita2d_wait_rendering_done();
    _DeinitImgui();
    vita2d_fini();
}

int Video::_DrawThread(SceSize args, void *argp)
{
    LogFunctionName;

    CLASS_POINT(Video, video, argp);
    while (video->IsRunning())
    {
        vita2d_pool_reset();
        vita2d_start_drawing_advanced(NULL, 0);
        vita2d_clear_screen();

        switch (gStatus)
        {
        case APP_STATUS_SHOW_BROWSER:
            gUi->Show();
            break;
        case APP_STATUS_RUN_GAME:
            gEmulator->Show();
            break;
        default:
            break;
        }

        vita2d_end_drawing();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
    }

    LogDebug("_DrawThread exit");
    sceKernelExitThread(0);
    return 0;
}

void Video::_InitImgui()
{
    LogFunctionName;

    ImGui::CreateContext();
    My_ImGui_ImplVita2D_Init(gLanguage->Get());
    ImGui_ImplVita2D_TouchUsage(false);
    ImGui_ImplVita2D_UseIndirectFrontTouch(false);
    ImGui_ImplVita2D_UseRearTouch(false);
    ImGui_ImplVita2D_GamepadUsage(false);
}

void Video::_DeinitImgui()
{
    LogFunctionName;

    My_ImGui_ImplVita2D_Shutdown();
    ImGui::DestroyContext();
}