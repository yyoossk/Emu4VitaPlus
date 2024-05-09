#include <vita2d.h>
#include "video.h"
#include "app.h"
#include "ui.h"
#include "log.h"

Video *gVideo = nullptr;

Video::Video() : ThreadBase(_DrawThread)
{
    LogFunctionName;
    vita2d_init();
    // vita2d_set_vblank_wait(1);
    // vita2d_set_clear_color(0xFF362B00);
}

Video::~Video()
{
    LogFunctionName;
    if (IsRunning())
    {
        Stop();
    }

    vita2d_wait_rendering_done();
    vita2d_fini();
}

int Video::_DrawThread(SceSize args, void *argp)
{
    LogFunctionName;

    CLASS_POINTER(Video, video, argp);

    while (video->IsRunning())
    {
        video->Lock();
        vita2d_pool_reset();
        vita2d_start_drawing_advanced(NULL, 0);
        vita2d_clear_screen();

        switch (gStatus)
        {
        case APP_STATUS_BOOT:
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
        // vita2d_common_dialog_update();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
        video->Unlock();
    }

    LogDebug("_DrawThread exit");
    sceKernelExitThread(0);
    return 0;
}
