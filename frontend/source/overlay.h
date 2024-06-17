#pragma once
#include <string>
#include <vector>
#include "language_string.h"

struct Overlay
{
    std::string name;
    std::string image_name;
    int viewport_rotate;
    int viewport_width;
    int viewport_height;
    int viewport_x;
    int viewport_y;
};

class Overlays : public std::vector<Overlay>
{
public:
    Overlays();
    virtual ~Overlays();
    bool Load(const char *path);
    std::vector<LanguageString> GetConfigs();
};

extern Overlays *gOverlays;