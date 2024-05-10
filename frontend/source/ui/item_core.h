#pragma once
#include "item_selectable.h"
#include "core_options.h"
#include "log.h"

class ItemCore : public ItemSelectable
{
public:
    ItemCore(CoreOption *option)
        : ItemSelectable(option->desc, option->info),
          _option(option)
    {
        _values = _option->GetValues();
    };

    virtual ~ItemCore(){};

private:
    size_t _GetTotalCount() { return _values.size(); };
    const char *_GetOptionString(size_t index) { return _index < _values.size() ? _values[index].Get() : "Invalid"; };

    CoreOption *_option;
    std::vector<LanguageString> _values;
};