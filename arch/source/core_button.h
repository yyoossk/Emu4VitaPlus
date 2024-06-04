#pragma once
#include <vita2d.h>
#include <imgui_vita2d/imgui_vita.h>
#include <string>
#include <vector>
#include "input.h"

class CoreButton
{
public:
    CoreButton(std::string name, std::vector<std::string> cores);
    virtual ~CoreButton();
    void Show(bool selected);

private:
    std::string _name;
    std::vector<std::string> _cores;
    vita2d_texture *_texture;
    ImVec2 _uv0;
    ImVec2 _uv1;
};