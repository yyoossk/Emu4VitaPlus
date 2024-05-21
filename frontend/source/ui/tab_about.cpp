#include "tab_about.h"
#include "language_string.h"
#include "my_imgui.h"

void TabAbout::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(TAB_ABOUT), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        My_Imgui_CenteredText(APP_TITLE_NAME);
        My_Imgui_CenteredText("%s " __DATE__ " " __TIME__, TEXT(COMPILED_ON));
        My_Imgui_CenteredText("%s: noword   %s: yizhigai", TEXT(AUTHOR), TEXT(SPECIAL_THANKS));
        My_Imgui_CenteredText("%s: https://github.com/noword/Emu4VitaPlus", TEXT(SITE));
        ImGui::EndTabItem();
    }
}