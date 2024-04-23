#pragma once
#include "item_base.h"
#include "global.h"

class ItemControl : public virtual ItemBase
{
public:
    ItemControl(ControlMapConfig *control_map);
    virtual ~ItemControl();

private:
    ControlMapConfig *_control_map;
};