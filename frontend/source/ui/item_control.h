#pragma once
#include "item_base.h"
#include "global.h"

class ItemControl : public virtual ItemBase
{
public:
    ItemControl(ControlMapConfig *control_map);
    virtual ~ItemControl();

    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    void Show(bool selected);
    virtual void OnActive(Input *input);

private:
    void _OnKeyUp(Input *input);
    void _OnKeyDown(Input *input);
    void _OnClick(Input *input);
    void _OnCancel(Input *input);

    const char *_GetText();

    ControlMapConfig *_control_map;
    bool _actived;
    size_t _index;
};