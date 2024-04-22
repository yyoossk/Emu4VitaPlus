#pragma once
#include <array>
#include "global.h"
#include "tab_base.h"
#include "item_base.h"

class TabSystem : public virtual TabBase
{
public:
    TabSystem();
    virtual ~TabSystem();

private:
    size_t _GetItemCount() { return _items.size(); };
    void _ShowItem(size_t index, bool selected);
    void _ActiveItem(Input *input, size_t index);

    void _ExitApp();

    std::array<ItemBase *, 2> _items;
};