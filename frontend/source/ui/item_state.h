#pragma once
#include "item_selectable.h"
#include "state_manager.h"

enum
{
    POPUP_SAVE = 0,
    POPUP_LOAD,
    POPUP_DELETE,
    POPUP_CANCEL,
    POPUP_COUNT,
};

class ItemState : public ItemSelectable
{
public:
    ItemState(State *state);
    virtual ~ItemState();
    void Show(bool selected) override;

private:
    void _ShowPopup();

    size_t _GetTotalCount() override { return POPUP_COUNT; };
    void _OnClick(Input *input) override;
    void _OnCancel(Input *input) override;

    State *_state;
    bool _dialog_actived;
};