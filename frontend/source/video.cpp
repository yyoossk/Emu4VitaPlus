#include "global.h"
#include "video.h"
#include "log.h"
#include "my_imgui.h"

Video::Video() : ThreadBase(_DrawThread)
{
    vita2d_init();
    vita2d_set_vblank_wait(1);
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

    CLASS_POINTER(Video, video, argp);
    while (video->IsRunning())
    {
        // video->Lock();
        // LogDebug("video %08x", video);
        vita2d_pool_reset();
        vita2d_start_drawing_advanced(NULL, 0);
        vita2d_clear_screen();

        switch (gStatus)
        {
        case APP_STATUS_SHOW_UI:
            gUi->Show();
            break;

        case APP_STATUS_RUN_GAME:
            gEmulator->Show();
            break;

        case APP_STATUS_SHOW_UI_IN_GAME:
            gEmulator->Show();
            gUi->Show();
            break;

        default:
            break;
        }

        vita2d_end_drawing();
        vita2d_swap_buffers();
        // video->Unlock();
        // sceDisplayWaitVblankStart();
        // LogDebug("Unlock %08x", video);
    }

    LogDebug("_DrawThread exit");
    sceKernelExitThread(0);
    return 0;
}

void Video::_InitImgui()
{
    LogFunctionName;

    ImGui::CreateContext();
    My_ImGui_ImplVita2D_Init(gConfig->language);
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