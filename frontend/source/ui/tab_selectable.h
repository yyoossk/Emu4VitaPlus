#pragma once
#include <vector>
#include "tab_base.h"
#include "item_base.h"

class TabSeletable : virtual public TabBase
{
public:
    TabSeletable(TEXT_ENUM title_id, std::vector<ItemBase *> items)
        : TabBase(title_id),
          _items(std::move(items)){};

    virtual ~TabSeletable()
    {
        for (auto &item : _items)
        {
            delete item;
        }
    };

private:
    size_t _GetItemCount() override
    {
        return _items.size();
    };

    void _ShowItem(size_t index, bool selected) override
    {
        if (index < _items.size())
        {
            _items[index]->Show(selected);
        }
    };

    void _OnActive(Input *input) override
    {
        LogFunctionName;
        if (_index < _items.size())
        {
            _items[_index]->OnActive(input);
        }
    };

    void _OnOption(Input *input) override
    {
        LogFunctionName;
        if (_index < _items.size())
        {
            _items[_index]->OnOption(input);
        }
    };

    std::vector<ItemBase *> _items;
};