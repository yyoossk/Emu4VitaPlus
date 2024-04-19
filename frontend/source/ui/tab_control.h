#pragma once
#include "global.h"
#include "tab_base.h"

class TabControl : virtual public TabBase
{
public:
    TabControl();
    virtual ~TabControl();
    void Show(bool selected);

private:
    size_t _GetItemCount() { return 0; };
};