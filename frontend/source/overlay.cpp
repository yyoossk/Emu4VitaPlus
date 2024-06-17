#include "SimpleIni.h"
#include "overlay.h"
#include "log.h"

Overlays *gOverlays;

Overlays::Overlays()
{
    Load("overlays/overlays.cfg");
}

Overlays::~Overlays()
{
}

bool Overlays::Load(const char *path)
{
    LogFunctionName;

    CSimpleIniA ini;
    if (ini.LoadFile(path) != SI_OK)
    {
        return false;
    }

    CSimpleIniA::TNamesDepend sections;
    ini.GetAllSections(sections);

    for (const auto &section : sections)
    {
        Overlay overlay;
        overlay.name = section.pItem;
        overlay.image_name = ini.GetValue(section.pItem, "image_name", "NULL");
        overlay.viewport_width = ini.GetLongValue(section.pItem, "viewport_width");
        overlay.viewport_height = ini.GetLongValue(section.pItem, "viewport_height");
        overlay.viewport_rotate = ini.GetLongValue(section.pItem, "viewport_rotate");
        overlay.viewport_x = ini.GetLongValue(section.pItem, "viewport_x");
        overlay.viewport_y = ini.GetLongValue(section.pItem, "viewport_y");
        this->emplace_back(overlay);
    }

    return true;
}

std::vector<LanguageString> Overlays::GetConfigs()
{
    std::vector<LanguageString> configs;
    for (const auto &overlay : *this)
    {
        configs.emplace_back(overlay.name);
    }
    return configs;
}