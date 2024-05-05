#ifndef TOML_EXCEPTIONS
#define TOML_EXCEPTIONS 0
#endif
#include <toml++/toml.hpp>

#include "overlay.h"

Overlays::Overlays()
{
    Load("overlays/overlays.cfg");
}

Overlays::~Overlays()
{
}

bool Overlays::Load(const char *path)
{
    toml::parse_result result = toml::parse_file(path);
    if (!result)
    {
        return false;
    }

    const toml::table tbl(std::move(result.table()));
    for (auto const &overlay : tbl)
    {
        const auto t = overlay.second.as_table();
        if (t && t->contains("image_name"))
        {
            _overlays.emplace_back(Overlay{
                std::string{overlay.first.str()},
                (*t)["image_name"].value<std::string>().value(),
                (*t)["viewport_rotate"].value_or(-1),
                (*t)["viewport_width"].value_or(-1),
                (*t)["viewport_height"].value_or(-1),
                (*t)["viewport_x"].value_or(-1),
                (*t)["viewport_y"].value_or(-1),
            });
        }
    }

    return true;
}