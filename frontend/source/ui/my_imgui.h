#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include "language_define.h"

extern ImFont *gGamePadFont;

IMGUI_API void My_ImGui_ImplVita2D_Init(LANGUAGE language);
IMGUI_API void My_ImGui_ImplVita2D_Shutdown();
IMGUI_API void My_ImGui_ImplVita2D_RenderDrawData(ImDrawData *draw_data);
IMGUI_API bool MyBeginCombo(const char *label, const char *preview_value, ImGuiComboFlags flags);