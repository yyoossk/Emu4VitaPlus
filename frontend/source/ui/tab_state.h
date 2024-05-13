#pragma once
#include "vita2d.h"
#include "tab_selectable.h"
#include "state_manager.h"

class TabState : public TabSeletable
{
public:
    TabState();
    virtual ~TabState();
    void Show(bool selected) override;

private:
    size_t _GetItemCount() override { return MAX_STATES; };
    bool _ItemVisable(size_t index) override { return true; };
};