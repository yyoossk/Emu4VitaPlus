#include "tab_about.h"
#include "language_string.h"
#include "my_imgui.h"
#include "defines.h"
#include "utils.h"
#include "config.h"

TabAbout::TabAbout() : TabBase(TAB_ABOUT),
                       _index(0)
{
    _InitTexts();
}

TabAbout::~TabAbout()
{
}

void TabAbout::Show(bool selected)
{
    if (_last_lang != gConfig->language)
    {
        _InitTexts();
    }

    if (ImGui::BeginTabItem(TEXT(TAB_ABOUT), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
        ImGui::BeginChild("ChildAbout", {0, 0}, false, window_flags);

        for (size_t i = 0; i < _texts.size(); i++)
        {
            My_Imgui_CenteredText(_texts[i].c_str());
            if (i == _index && ImGui::GetScrollMaxY() > 0.f)
            {
                ImGui::SetScrollHereY((float)_index / (float)_texts.size());
            }
        }

        ImGui::EndChild();
        ImGui::EndTabItem();
    }
}

void TabAbout::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_UP, std::bind(&TabAbout::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_DOWN, std::bind(&TabAbout::_OnKeyDown, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_UP, std::bind(&TabAbout::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_DOWN, std::bind(&TabAbout::_OnKeyDown, this, input), true);
}

void TabAbout::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_DOWN);
}

void TabAbout::_InitTexts()
{
    _last_lang = gConfig->language;
    _texts.clear();
    _texts.emplace_back(APP_TITLE_NAME);
    _texts.emplace_back(std::string(TEXT(COMPILED_ON)) + " " + __DATE__ + " " + __TIME__);
    _texts.emplace_back(std::string(TEXT(AUTHOR)) + ": noword   " + TEXT(SPECIAL_THANKS) + ": yizhigai");
    _texts.emplace_back(std::string(TEXT(SITE)) + ": https://github.com/noword/Emu4VitaPlus");
    _texts.emplace_back("");
    _texts.emplace_back(TEXT(EXTERNAL_COMPONENTS));
    _texts.emplace_back("7-Zip (GPL v2.1) https://github.com/mcmilk/7-Zip.git");
    _texts.emplace_back("minizip-ng (zlib) https://github.com/zlib-ng/minizip-ng");
    _texts.emplace_back("simpleini (MIT) https://github.com/brofield/simpleini");
    _texts.emplace_back("libretro-common (?) https://github.com/libretro/libretro-common");
    _texts.emplace_back("libvita2d (MIT) https://github.com/xerpi/libvita2d");
    _texts.emplace_back("libvita2d_ext (MIT) https://github.com/frangarcj/libvita2d_ext");
    _texts.emplace_back("");
    _texts.emplace_back(TEXT(TAB_CORE));
    _texts.emplace_back("https://gitee.com/yizhigai/libretro-fba-lite");
    _texts.emplace_back("https://github.com/libretro/gpsp");
    _texts.emplace_back("https://github.com/libretro/libretro-fceumm");
    _texts.emplace_back("https://github.com/libretro/gambatte-libretro");
    _texts.emplace_back("https://github.com/libretro/beetle-ngp-libretro");
    _texts.emplace_back("https://github.com/libretro/beetle-pce-fast-libretro");
    _texts.emplace_back("https://github.com/libretro/Genesis-Plus-GX");
    _texts.emplace_back("https://github.com/libretro/beetle-supergrafx-libretro");
    _texts.emplace_back("https://github.com/libretro/beetle-wswan-libretro");
    _texts.emplace_back("https://github.com/libretro/nestopia");
    _texts.emplace_back("https://github.com/libretro/pcsx_rearmed");
    _texts.emplace_back("https://github.com/libretro/picodrive");
    _texts.emplace_back("https://github.com/libretro/snes9x2002");
    _texts.emplace_back("https://github.com/libretro/snes9x2005");
    _texts.emplace_back("https://github.com/libretro/snes9x2010");
    _texts.emplace_back("https://github.com/libretro/vba-next");
    _texts.emplace_back("https://github.com/libretro/FBNeo.git");
    _texts.emplace_back("https://github.com/libretro/fbalpha2012.git");
    _texts.emplace_back("https://github.com/libretro/mgba.git");
    _texts.emplace_back("https://github.com/libretro/Genesis-Plus-GX-Wide.git");
}

void TabAbout::_OnKeyUp(Input *input)
{
    LOOP_MINUS_ONE(_index, _texts.size());
}

void TabAbout::_OnKeyDown(Input *input)
{
    LOOP_PLUS_ONE(_index, _texts.size());
}