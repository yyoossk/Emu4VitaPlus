
#include "gui.h"
#include "log.h"

extern SceGxmProgram _binary_assets_imgui_v_cg_gxp_start;
extern SceGxmProgram _binary_assets_imgui_f_cg_gxp_start;

Gui::Gui()
{
    LogFunctionName;
    if (sceGxmProgramCheck(&_binary_assets_imgui_v_cg_gxp_start) != SCE_OK)
    {
        LogError("sceGxmProgramCheck _binary_assets_imgui_v_cg_gxp_start error");
        return;
    }

    if (sceGxmProgramCheck(&_binary_assets_imgui_f_cg_gxp_start) != SCE_OK)
    {
        LogError("sceGxmProgramCheck _binary_assets_imgui_f_cg_gxp_start error");
        return;
    }

    if (sceGxmShaderPatcherRegisterProgram(vita2d_get_shader_patcher(),
                                           &_binary_assets_imgui_v_cg_gxp_start,
                                           &_vertexProgramId))
    {
        LogError("sceGxmShaderPatcherRegisterProgram error");
        return;
    }

    if (sceGxmShaderPatcherRegisterProgram(vita2d_get_shader_patcher(),
                                           &_binary_assets_imgui_f_cg_gxp_start,
                                           &_fragmentProgramId))
    {
        LogError("sceGxmShaderPatcherRegisterProgram error");
        return;
    }

    _imgui = ImGui::CreateContext();
}

Gui::~Gui()
{
    LogFunctionName;
    ImGui::DestroyContext(_imgui);
}

void Gui::Run()
{
    LogFunctionNameLimited;
    ImGui::NewFrame();
    ImGuiIO &io = ImGui::GetIO();
    ImGui::End();
}