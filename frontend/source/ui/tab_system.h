#pragma once
#include <array>
#include "global.h"
#include "tab_base.h"

class TabSystem : public TabBase
{
public:
    struct MenuItem
    {
        TEXT text_id;
        bool visable;
        void (TabSystem::*function)(void);
    };

    TabSystem();
    virtual ~TabSystem();
    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    virtual void Show(bool selected);

private:
    void _OnKeyUp();
    void _OnKeyDown();
    void _OnKeyCircle();

    void _ExitApp();

    size_t _index;
    std::array<MenuItem, 1> _menu{{SYSTEM_MENU_EXIT, true, &TabSystem::_ExitApp}};
};