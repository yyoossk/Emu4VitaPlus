#pragma once
#include "array"
#include "global.h"
#include "tab_base.h"
#include "menu_item.h"

class TabGraphics : virtual public TabBase
{
public:
    TabGraphics();
    virtual ~TabGraphics();

    virtual void Show(bool selected);

private:
    size_t _GetItemCount() { return _items.size(); };
    void _OnClick(Input *input);

    std::array<MenuItem *, 2> _items;
};