#pragma once
#include "tab_base.h"

class TabSystem : public TabBase
{
public:
    virtual void SetInputHooks();
    virtual void UnsetInputHooks();
    virtual void Show(bool selected);

private:
    void _OnKeyUp();
    void _OnKeyDown();
    void _OnKeyCircle();
    void _OnKeyCross();

    size_t _index;
}
