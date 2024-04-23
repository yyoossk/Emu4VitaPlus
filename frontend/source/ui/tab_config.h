#pragma once
#include <vector>
#include "tab_base.h"
#include "item_base.h"

class TabConfig : virtual public TabBase
{
public:
    TabConfig(TEXT_ENUM title_id, std::vector<ItemBase *> items)
        : TabBase(title_id),
          _items(std::move(items)){};

    virtual ~TabConfig()
    {
        for (auto &item : _items)
        {
            delete item;
        }
    };

private:
    size_t _GetItemCount()
    {
        return _items.size();
    };

    void _ShowItem(size_t index, bool selected)
    {
        if (index < _items.size())
        {
            _items[index]->Show(selected);
        }
    }

    void _ActiveItem(Input *input, size_t index)
    {
        if (index < _items.size())
        {
            _items[index]->OnActive(input);
        }
    }

    std::vector<ItemBase *> _items;
};