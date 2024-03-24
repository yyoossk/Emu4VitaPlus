#pragma once
#include <vita2d.h>
#include <imgui_vita2d/imgui_vita.h>
#include "input.h"

class Gui
{
public:
    Gui();
    virtual ~Gui();
    bool Run();

private:
    void _OnCrossPress();

    Input _input;
    bool _running;
};