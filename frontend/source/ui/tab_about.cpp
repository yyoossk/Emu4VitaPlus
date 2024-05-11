#include "tab_about.h"
#include "language_string.h"
#include "my_imgui.h"

static const char *AboutTexts[] = {
    "Emu4Vita++ (" APP_DIR_NAME ")",
    "Compiled on " __DATE__ " " __TIME__,
    "Author: noword   Special Thanks: yizhigai",
    "Site: https://github.com/noword/Emu4VitaPlus"};

void TabAbout::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(TAB_ABOUT), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        for (size_t i = 0; i < sizeof(AboutTexts) / sizeof(*AboutTexts); i++)
        {
            MyCenteredText(AboutTexts[i]);
        }

        ImGui::EndTabItem();
    }
}