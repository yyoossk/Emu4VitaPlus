#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include <stdint.h>

#define IM_COL32_GREEN IM_COL32(0, 255, 0, 255)
#define IM_COL32_RED IM_COL32(255, 0, 0, 255)

IMGUI_API void My_ImGui_ImplVita2D_Init(uint32_t language, const char *cache_path = NULL);
IMGUI_API void My_ImGui_ImplVita2D_Shutdown();
IMGUI_API void My_ImGui_ImplVita2D_RenderDrawData(ImDrawData *draw_data);
IMGUI_API void My_Imgui_Create_Font(uint32_t language, const char *cache_path = NULL);
IMGUI_API void My_Imgui_Destroy_Font();

IMGUI_API bool My_Imgui_BeginCombo(const char *label, const char *preview_value, ImGuiComboFlags flags);
IMGUI_API void My_Imgui_CenteredText(const char *text, ...);