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
    void _OnActive(Input *input) override;

    void _ShowPopup();
    void _GetButtonText(int index, char *text, size_t length);
    void _OnKeyLeft(Input *input);
    void _OnKeyRight(Input *input);
    void _OnClick(Input *input);
    void _OnCancel(Input *input);
    void _SetInputHooks(Input *input);
    void _UnsetInputHooks(Input *input);

    bool _actived;
    size_t _popup_index;
};