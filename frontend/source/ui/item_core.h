#pragma once
#include <stdint.h>
#include "item_selectable.h"
#include "core_options.h"

class ItemCore : public ItemSelectable
{
public:
    ItemCore(CoreOption *option);
    virtual ~ItemCore();

private:
    size_t _GetTotalCount() { return _values.size(); };
    const char *_GetOptionString(size_t index) { return _values[_index].Get(); };

    CoreOption *_option;
    std::vector<LanguageString> _values;
};