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
    // virtual void Show(bool selected);

private:
    size_t _GetItemCount() { return _items.size(); };
    void _ShowItem(size_t index, bool selected);
    // void _OnClick();

    void _ExitApp(Input *input);

    std::array<ItemBase *, 1> _items;
};