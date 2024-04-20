#pragma once
#include "global.h"
#include "tab_base.h"

class TabCore : virtual public TabBase
{
public:
    TabCore();
    virtual ~TabCore();
    void Show(bool selected);

private:
    size_t _GetItemCount() { return 0; };
};