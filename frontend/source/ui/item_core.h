#pragma once
#include "ui.h"
#include "item_selectable.h"
#include "core_options.h"
#include "log.h"

void ItemCoreOnClick()
{
    gConfig->Save();
    gEmulator->CoreOptionUpdate();
    gUi->SetHint(TEXT(LANG_CORE_NOTICE));
}

class ItemCore : public ItemSelectable
{
public:
    ItemCore(CoreOption *option)
        : ItemSelectable(option->desc, option->info, ItemCoreOnClick),
          _option(option)
    {
        _values = _option->GetValues();
        _index = _option->GetValueIndex();
    };

    virtual ~ItemCore() {};

private:
    virtual size_t _GetTotalCount() override
    {
        return _values.size();
    };

    virtual const char *_GetOptionString(size_t index) override
    {
        return _index < _values.size() ? _values[index].Get() : "Invalid";
    };

    virtual void _SetIndex(size_t index) override
    {
        _index = index;
        _option->SetValueIndex(index);
    };

    CoreOption *_option;
    std::vector<LanguageString> _values;
};