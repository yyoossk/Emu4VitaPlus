#include "global.h"
#include "drawer.h"
#include "log.h"
#include "my_imgui.h"

Drawer::Drawer() : _thread_id(-1)
{
    vita2d_init();
    _InitImgui();
}

Drawer::~Drawer()
{
    if (_thread_id >= 0)
    {
        Stop();
    }
    _DeinitImgui();
    vita2d_fini();
}

bool Drawer::Start()
{
    LogFunctionName;

    _thread_id = sceKernelCreateThread("draw_thread", Drawer::_DrawThread, 64, 0x10000, 0, 0, NULL);
    if (_thread_id < 0)
    {
        LogError("failed to create drawer thread");
        return false;
    }

    _keep_running = true;
    int result = sceKernelStartThread(_thread_id, sizeof(this), this);
    if (result != SCE_OK)
    {
        LogError("failed to start drawer thread: %d", result);
        sceKernelDeleteThread(_thread_id);
        _thread_id = -1;
        return false;
    }

    return true;
}

void Drawer::Stop()
{
    LogFunctionName;
    _keep_running = false;
    sceKernelWaitThreadEnd(_thread_id, NULL, NULL);
    sceKernelDeleteThread(_thread_id);
    _thread_id = -1;
}

int Drawer::_DrawThread(SceSize args, void *argp)
{
    Drawer *drawer = (Drawer *)argp;

    while (drawer->_keep_running)
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

    sceKernelExitThread(0);
    LogDebug("_DrawThread exit");
    return 0;
}

void Drawer::_InitImgui()
{
    LogFunctionName;

    ImGui::CreateContext();
    My_ImGui_ImplVita2D_Init();
    ImGui_ImplVita2D_TouchUsage(false);
    ImGui_ImplVita2D_UseIndirectFrontTouch(false);
    ImGui_ImplVita2D_UseRearTouch(false);
    ImGui_ImplVita2D_GamepadUsage(false);
}

void Drawer::_DeinitImgui()
{
    LogFunctionName;

    My_ImGui_ImplVita2D_Shutdown();
    ImGui::DestroyContext();
}