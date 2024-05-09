#pragma once
#include "item_base.h"
#include "input.h"

class ItemSelectable : public ItemBase
{
public:
    ItemSelectable(LanguageString text, LanguageString info = "");
    virtual ~ItemSelectable();

    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    virtual void Show(bool selected);
    virtual void OnActive(Input *input);

protected:
    virtual void _OnKeyUp(Input *input);
    virtual void _OnKeyDown(Input *input);
    virtual void _OnClick(Input *input);
    virtual void _OnCancel(Input *input);

    size_t _GetTotalCount() { return 0; };
    const char *_GetOptionString(size_t index) { return ""; };
    size_t _GetIndex() { return _index; };
    void _SetIndex(size_t index) { _index = index; };

    bool _actived;
    size_t _index;
    size_t _old_index;
};