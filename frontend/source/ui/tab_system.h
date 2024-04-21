#pragma once
#include <array>
#include "global.h"
#include "tab_base.h"

class TabSystem : public virtual TabBase
{
public:
    struct MenuItem
    {
        TEXT_ENUM text_id;
        bool visable;
        void (TabSystem::*function)(void);
    };

    TabSystem();
    virtual ~TabSystem();
    virtual void Show(bool selected);

private:
    size_t _GetItemCount() { return _menu.size(); };
    void _OnClick();

    void _ExitApp();

    std::array<MenuItem, 1> _menu{{SYSTEM_MENU_EXIT, true, &TabSystem::_ExitApp}};
};