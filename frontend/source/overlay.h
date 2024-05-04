#pragma once
#include <string>
#include <vector>

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

class Overlays
{
public:
    Overlays();
    virtual ~Overlays();

    bool Load(const char *path);
    const std::vector<Overlay> &Get() { return _overlays; };

private:
    std::vector<Overlay> _overlays;
};