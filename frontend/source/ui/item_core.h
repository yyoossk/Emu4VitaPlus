#pragma once
#include <stdint.h>
#include "item_base.h"
#include "core_options.h"

class ItemCore : public virtual ItemBase
{
public:
    ItemCore(CoreOption *option);
    virtual ~ItemCore();

    virtual void Show(bool selected);

private:
    CoreOption *_option;
};