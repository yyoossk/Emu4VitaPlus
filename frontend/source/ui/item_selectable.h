#pragma once
#include "item_base.h"
#include "input.h"

class ItemSelectable : public ItemBase
{
public:
    ItemSelectable();
    virtual ~ItemSelectable();

    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    virtual void Show(bool selected);

protected:
    void _OnKeyUp(Input *input);
    void _OnKeyDown(Input *input);
    void _OnClick(Input *input);
    void _OnCancel(Input *input);
    virtual size_t _GetCount();

    bool _actived;
};