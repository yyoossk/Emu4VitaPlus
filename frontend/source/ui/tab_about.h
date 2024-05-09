#pragma once
#include "tab_base.h"

class TabAbout : public virtual TabBase
{
public:
    TabAbout() : TabBase(TAB_ABOUT){};
    virtual ~TabAbout(){};
    virtual void Show(bool selected);

private:
    size_t _GetItemCount() { return 0; };
};