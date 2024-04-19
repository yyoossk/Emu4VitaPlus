#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include "input.h"

class TabBase
{
public:
    TabBase(bool visable = true);

    virtual ~TabBase();

    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    virtual void Show(bool selected) = 0;
    void SetVisable(bool visable) { _visable = visable; };
    bool Visable() { return _visable; };

protected:
    virtual size_t _GetItemCount() = 0;
    virtual void _OnKeyUp(Input *input);
    virtual void _OnKeyDown(Input *input);
    virtual void _OnClick(Input *input){};

    size_t _index;
    bool _visable;
};