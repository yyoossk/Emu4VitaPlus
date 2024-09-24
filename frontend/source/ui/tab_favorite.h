#pragma once
#include "tab_selectable.h"

class TabFavorite : virtual public TabSeletable
{
public:
    TabFavorite();
    virtual ~TabFavorite();
    void Show(bool selected);

private:
    size_t _GetItemCount() { return 0; };
};