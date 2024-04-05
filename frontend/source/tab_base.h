#pragma once
#include "input.h"

class TabBase
{
public:
    TabBase(bool visable = true) : _visable(visable){};
    virtual ~TabBase(){};

    virtual void SetInputHooks(Input *input) = 0;
    virtual void UnsetInputHooks(Input *input) = 0;
    virtual void Show() = 0;

    void SetVisable(bool visable) { _visable = visable; };
    bool Visable() { return _visable; };

private:
    bool _visable;
};