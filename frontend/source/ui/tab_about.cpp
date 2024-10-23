#include "tab_about.h"
#include "language_string.h"
#include "my_imgui.h"
#include "defines.h"
#include "utils.h"
#include "config.h"

static const char *AppTitle[] = {
    // " _____                      ___  _   _  _  _                        ",
    // "|  ___|                    /   || | | |(_)| |            _      _   ",
    // "| |__   _ __ ___   _   _  / /| || | | | _ | |_   __ _  _| |_  _| |_ ",
    // "|  __| | '_ ` _ \\ | | | |/ /_| || | | || || __| / _` ||_   _||_   _|",
    // "| |___ | | | | | || |_| |\\___  |\\ \\_/ /| || |_ | (_| |  |_|    |_|  ",
    // "\\____/ |_| |_| |_| \\__,_|    |_/ \\___/ |_| \\__| \\__,_|              ",
};

static const size_t AppTitleSize = sizeof(AppTitle) / sizeof(*AppTitle);

TabAbout::TabAbout() : TabBase(TAB_ABOUT),
                       _index(0)
{
    _InitTexts();
}

TabAbout::~TabAbout()
{
}

size_t TabAbout::_GetItemCount()
{
    return _texts.size() + AppTitleSize;
}

void TabAbout::Show(bool selected)
{
    if (_last_lang != gConfig->language)
    {
        _InitTexts();
    }

    std::string title = std::string(TAB_ICONS[_title_id]) + TEXT(_title_id);
    if (ImGui::BeginTabItem(title.c_str(), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
        ImGui::BeginChild("ChildAbout", {0, 0}, false, window_flags);

        for (size_t i = 0; i < AppTitleSize; i++)
        {
            ImGui::TextUnformatted(AppTitle[i]);
            if (i == _index && ImGui::GetScrollMaxY() > 0.f)
            {
                ImGui::SetScrollHereY((float)_index / (float)_GetItemCount());
            }
        }

        for (size_t i = 0; i < _texts.size(); i++)
        {
            My_Imgui_CenteredText(_texts[i].c_str());
            if (i + AppTitleSize == _index && ImGui::GetScrollMaxY() > 0.f)
            {
                ImGui::SetScrollHereY((float)_index / (float)_GetItemCount());
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
    _texts = {
        APP_TITLE_NAME,
        std::string(TEXT(COMPILED_ON)) + " " + __DATE__ + " " + __TIME__,
        std::string(TEXT(AUTHOR)) + ": noword   " + TEXT(SPECIAL_THANKS) + ": yizhigai, KyleBing",
        std::string(TEXT(SITE)) + ": https://github.com/noword/Emu4VitaPlus",
        "",
        TEXT(EXTERNAL_COMPONENTS),
        "7-Zip (GPL v2.1) https://github.com/mcmilk/7-Zip.git",
        "minizip-ng (zlib) https://github.com/zlib-ng/minizip-ng",
        "simpleini (MIT) https://github.com/brofield/simpleini",
        "libretro-common (?) https://github.com/libretro/libretro-common",
        "libvita2d (MIT) https://github.com/xerpi/libvita2d",
        "libvita2d_ext (MIT) https://github.com/frangarcj/libvita2d_ext",
        "",
        TEXT(TAB_CORE),
        "https://gitee.com/yizhigai/libretro-fba-lite",
        "https://github.com/libretro/FBNeo.git",
        "https://github.com/libretro/fbalpha2012.git",
        "https://github.com/libretro/gpsp",
        "https://github.com/libretro/libretro-fceumm",
        "https://github.com/libretro/gambatte-libretro",
        "https://github.com/libretro/beetle-ngp-libretro",
        "https://github.com/libretro/beetle-pce-fast-libretro",
        "https://github.com/libretro/Genesis-Plus-GX",
        "https://github.com/libretro/Genesis-Plus-GX-Wide.git",
        "https://github.com/libretro/beetle-supergrafx-libretro",
        "https://github.com/libretro/beetle-wswan-libretro",
        "https://github.com/libretro/nestopia",
        "https://github.com/libretro/pcsx_rearmed",
        "https://github.com/libretro/picodrive",
        "https://github.com/libretro/snes9x2002",
        "https://github.com/libretro/snes9x2005",
        "https://github.com/libretro/snes9x2010",
        "https://github.com/libretro/vba-next",
        "https://github.com/libretro/mgba.git",
    };
}

void TabAbout::_OnKeyUp(Input *input)
{
    LOOP_MINUS_ONE(_index, _texts.size() + AppTitleSize);
}

void TabAbout::_OnKeyDown(Input *input)
{
    LOOP_PLUS_ONE(_index, _texts.size() + AppTitleSize);
}