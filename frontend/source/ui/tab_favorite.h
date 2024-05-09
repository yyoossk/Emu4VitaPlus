#pragma once
#include "tab_base.h"

class TabFavorite : virtual public TabBase
{
public:
    TabFavorite();
    virtual ~TabFavorite();
    void Show(bool selected);

private:
    size_t _GetItemCount() { return 0; };
};