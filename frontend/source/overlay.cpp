#include "SimpleIni.h"
#include "overlay.h"
#include "log.h"
#include "video.h"

#define OVERLAY_PATH "app0:assets/overlays/"

Overlays *gOverlays;

Overlay::Overlay() : _texture(nullptr)
{
}

Overlay::~Overlay()
{
    if (_texture != nullptr)
    {
        gVideo->Lock();
        vita2d_wait_rendering_done();
        vita2d_free_texture(_texture);
        _texture = nullptr;
        gVideo->Unlock();
    }
}

vita2d_texture *Overlay::Get()
{
    if (_texture == nullptr && image_name.size() > 0)
    {
        _texture = vita2d_load_PNG_file((OVERLAY_PATH + image_name).c_str());
        if (_texture == nullptr)
        {
            _texture = vita2d_load_JPEG_file((OVERLAY_PATH + image_name).c_str());
        }
    }
    return _texture;
}

Overlays::Overlays()
{
    Load(OVERLAY_PATH "overlays.ini");
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
        if (overlay.viewport_width == 0)
        {
            continue;
        }
        overlay.viewport_height = ini.GetLongValue(section.pItem, "viewport_height");
        if (overlay.viewport_height == 0)
        {
            continue;
        }
        overlay.viewport_rotate = ini.GetLongValue(section.pItem, "viewport_rotate");
        overlay.viewport_x = ini.GetLongValue(section.pItem, "viewport_x");
        overlay.viewport_y = ini.GetLongValue(section.pItem, "viewport_y");
        this->emplace_back(overlay);
        LogDebug("  %s %s %d %d", section.pItem, overlay.image_name.c_str(), overlay.viewport_width, overlay.viewport_height);
    }

    return true;
}

std::vector<LanguageString> Overlays::GetConfigs()
{
    std::vector<LanguageString> configs;
    configs.emplace_back(LanguageString(NONE));
    for (const auto &overlay : *this)
    {
        configs.emplace_back(overlay.name);
    }
    return configs;
}