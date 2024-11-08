#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include <stdint.h>
#include "delay.h"

#define IM_COL32_GREEN IM_COL32(0, 255, 0, 255)
#define IM_COL32_RED IM_COL32(255, 0, 0, 255)
#define DEFAULT_TEXT_MOVING_INTERVAL 15000
#define DEFAULT_TEXT_MOVING_START 800000

struct TextMovingStatus
{
    int pos = 0;
    int delta = -1;
    Delay delay{DEFAULT_TEXT_MOVING_INTERVAL};

    void Reset()
    {
        pos = 0;
        delta = -1;
        delay.SetDelay(DEFAULT_TEXT_MOVING_START);
    }
};

IMGUI_API void My_ImGui_ImplVita2D_Init(uint32_t language, const char *cache_path = NULL);
IMGUI_API void My_ImGui_ImplVita2D_Shutdown();
IMGUI_API void My_ImGui_ImplVita2D_RenderDrawData(ImDrawData *draw_data);
IMGUI_API void My_Imgui_Create_Font(uint32_t language, const char *cache_path = NULL);
IMGUI_API void My_Imgui_Destroy_Font();

IMGUI_API bool My_Imgui_BeginCombo(const char *label, const char *preview_value, ImGuiComboFlags flags);
IMGUI_API void My_Imgui_CenteredText(const char *text, ...);
IMGUI_API bool My_Imgui_Selectable(const char *label, bool selected, TextMovingStatus *status);
IMGUI_API void My_Imgui_ShowTimePower();