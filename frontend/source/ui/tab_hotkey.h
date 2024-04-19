#pragma once
#include "global.h"
#include "tab_base.h"

class TabHotkey : virtual public TabBase
{
public:
    TabHotkey();
    virtual ~TabHotkey();
    void Show(bool selected);

private:
    size_t _GetItemCount() { return 0; };
};