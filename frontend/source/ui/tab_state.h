#pragma once
#include "vita2d.h"
#include "tab_base.h"
#include "state_manager.h"

class TabState : virtual public TabBase
{
public:
    TabState();
    virtual ~TabState();
    void Show(bool selected);

private:
    size_t _GetItemCount() { return 0; };

    vita2d_texture *_empty_texture;
};