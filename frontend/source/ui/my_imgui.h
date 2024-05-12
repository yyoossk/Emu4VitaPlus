#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include <stdint.h>
#include "language_define.h"

IMGUI_API void My_ImGui_ImplVita2D_Init(uint32_t language);
IMGUI_API void My_ImGui_ImplVita2D_Shutdown();
IMGUI_API void My_ImGui_ImplVita2D_RenderDrawData(ImDrawData *draw_data);
IMGUI_API void My_Imgui_Create_Font(uint32_t language);
IMGUI_API void My_Imgui_Destroy_Font();

IMGUI_API bool My_Imgui_BeginCombo(const char *label, const char *preview_value, ImGuiComboFlags flags);
IMGUI_API void My_Imgui_CenteredText(const char *text, ...);