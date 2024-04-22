#pragma once
#include "array"
#include "global.h"
#include "tab_base.h"
#include "config_item.h"

#ifdef WANT_DISPLAY_ROTATE
#define GRAPHICS_MENU_COUNT 6
#else
#define GRAPHICS_MENU_COUNT 5
#endif

class TabGraphics : virtual public TabBase
{
public:
    TabGraphics();
    virtual ~TabGraphics();

    virtual void Show(bool selected);

private:
    size_t _GetItemCount() { return _items.size(); };
    void _OnClick(Input *input);

    std::array<ConfigItem *, GRAPHICS_MENU_COUNT> _items;
};