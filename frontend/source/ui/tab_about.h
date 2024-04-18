#pragma once
#include "global.h"
#include "tab_base.h"

class TabAbout : public virtual TabBase
{
public:
    virtual void Show(bool selected);

private:
    size_t _GetItemCount() { return 0; };
};