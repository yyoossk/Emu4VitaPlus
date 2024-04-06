#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include "input.h"

class TabBase
{
public:
    TabBase(bool visable = true) : _visable(visable){};

    virtual ~TabBase(){};

    virtual void SetInputHooks(Input *input){};
    virtual void UnsetInputHooks(Input *input){};
    virtual void Show(bool selected) = 0;

    void SetVisable(bool visable) { _visable = visable; };
    bool Visable() { return _visable; };

protected:
    bool _visable;
};