#include <vita2d.h>
#include <vita2d_ext.h>
#include "video.h"
#include "app.h"
#include "ui.h"
#include "log.h"
#include "profiler.h"

Video *gVideo = nullptr;

Video::Video() : ThreadBase(_DrawThread, SCE_KERNEL_DEFAULT_PRIORITY_USER, SCE_KERNEL_CPU_MASK_USER_1)
{
    LogFunctionName;
    vita2d_init();
    vita2d_ext_init(vita2d_get_context(), vita2d_get_shader_patcher());
    vita2d_set_vblank_wait(1);
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
    vita2d_ext_fini();
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

        switch (gStatus.Get())
        {
        case APP_STATUS_BOOT:
        case APP_STATUS_SHOW_UI:
            gUi->Show();
            break;

        case APP_STATUS_RUN_GAME:
        case APP_STATUS_REWIND_GAME:
            BeginProfile("Video");
            gEmulator->Show();
            EndProfile("Video");
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
        video->Unlock();
    }

    LogDebug("_DrawThread exit");
    sceKernelExitThread(0);
    return 0;
}
