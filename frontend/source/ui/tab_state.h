#pragma once
#include "vita2d.h"
#include "tab_selectable.h"
#include "state_manager.h"

class TabState : public TabSeletable
{
public:
    TabState();
    virtual ~TabState();
    void Show(bool selected);

private:
    size_t _GetItemCount() { return MAX_STATES; };
};