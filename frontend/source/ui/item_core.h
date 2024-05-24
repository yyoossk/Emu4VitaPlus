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
        _index = _option->GetValueIndex();
    };

    virtual ~ItemCore(){};

private:
    size_t _GetTotalCount() override
    {
        return _values.size();
    };

    const char *_GetOptionString(size_t index) override
    {
        return _index < _values.size() ? _values[index].Get() : "Invalid";
    };

    void _SetIndex(size_t index) override
    {
        _index = index;
        _option->SetValueIndex(index);
    };

    void _OnClick(Input *input) override
    {
        ItemSelectable::_OnClick(input);
        gConfig->Save();
    };

    CoreOption *_option;
    std::vector<LanguageString> _values;
};