#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include "input.h"

class TabBase
{
public:
    TabBase(Input *input, bool visable = true) : _input(input), _visable(visable)
    {
        SetInputHooks();
    };

    virtual ~TabBase()
    {
        UnsetInputHooks();
    };

    virtual void SetInputHooks() {};
    virtual void UnsetInputHooks() {};
    virtual void Show() = 0;

    void SetVisable(bool visable) { _visable = visable; };
    bool Visable() { return _visable; };

protected:
    Input *_input;
    bool _visable;
};