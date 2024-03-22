
#include "gui.h"
#include "log.h"

extern SceGxmProgram _binary_assets_imgui_v_cg_gxp_start;
extern SceGxmProgram _binary_assets_imgui_f_cg_gxp_start;

Gui::Gui()
{
    LogFunctionName;

    ImGui::CreateContext();
    ImGui_ImplVita2D_Init();

    ImGui_ImplVita2D_TouchUsage(true);
    ImGui_ImplVita2D_UseIndirectFrontTouch(false);
    ImGui_ImplVita2D_UseRearTouch(true);
    ImGui_ImplVita2D_GamepadUsage(true);

    LogDebug("Gui init end");
}

Gui::~Gui()
{
    LogFunctionName;
    ImGui_ImplVita2D_Shutdown();
    ImGui::DestroyContext();
}

void Gui::Run()
{
    LogFunctionNameLimited;

    ImGui_ImplVita2D_NewFrame();

    ImGui::Render();
    ImGui_ImplVita2D_RenderDrawData(ImGui::GetDrawData());
}