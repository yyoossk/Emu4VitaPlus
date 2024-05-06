#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include "language_define.h"

IMGUI_API bool MyBeginCombo(const char *label, const char *preview_value, ImGuiComboFlags flags);
IMGUI_API void My_Imgui_Create_Font(LANGUAGE language);
IMGUI_API void My_Imgui_Destroy_Font();