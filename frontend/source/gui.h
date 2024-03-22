#pragma once
#include <vita2d.h>
#include <imgui_vita2d/imgui.h>

class Gui
{
public:
    Gui();
    virtual ~Gui();
    void Run();

private:
    ImGuiContext *_imgui;
    SceGxmShaderPatcherId _vertexProgramId;
    SceGxmShaderPatcherId _fragmentProgramId;
};