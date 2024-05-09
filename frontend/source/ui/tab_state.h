#pragma once
#include "tab_base.h"

class TabState : virtual public TabBase
{
public:
    TabState();
    virtual ~TabState();
    void Show(bool selected);

private:
    size_t _GetItemCount() { return 0; };
};