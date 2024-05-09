#pragma once
#include <stdint.h>
#include "item_base.h"

class ItemControl : public virtual ItemBase
{
public:
    ItemControl(ControlMapConfig *control_map);
    virtual ~ItemControl();

    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    void Show(bool selected);
    virtual void OnActive(Input *input);
    virtual void OnOption(Input *input);

private:
    void _OnKeyUp(Input *input);
    void _OnKeyDown(Input *input);
    void _OnClick(Input *input);
    void _OnCancel(Input *input);
    uint8_t _GetCurrentRetroIndex();

    ControlMapConfig *_control_map;
    uint8_t _old_retro;
    bool _actived;
};