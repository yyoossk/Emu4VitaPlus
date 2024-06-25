#include "SimpleIni.h"
#include "overlay.h"
#include "log.h"
#include "video.h"

#define OVERLAY_PATH0 "app0:assets/overlays/"
#define OVERLAY_PATH1 "app0:data/" CONSOLE "/overlays/"

Overlays *gOverlays;

Overlay::Overlay() : _texture(nullptr)
{
}

Overlay::~Overlay()
{
    LogFunctionName;
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
        for (auto const &path : {OVERLAY_PATH0, OVERLAY_PATH1})
        {
            _texture = vita2d_load_PNG_file((path + image_name).c_str());
            if (_texture)
                break;
            _texture = vita2d_load_JPEG_file((path + image_name).c_str());
            if (_texture)
                break;
        }
    }
    return _texture;
}

Overlays::Overlays()
{
    if (!Load(OVERLAY_PATH0))
    {
        Load(OVERLAY_PATH1);
    }
}

Overlays::~Overlays()
{
    LogFunctionName;
}

bool Overlays::Load(const char *path)
{
    LogFunctionName;

    CSimpleIniA ini;
    if (ini.LoadFile((std::string(path) + "overlays.ini").c_str()) != SI_OK)
    {
        LogError("failed to load overlays.ini from %s", path);
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