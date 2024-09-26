#pragma once
#include "tab_selectable.h"
#include "favorite.h"

class TabFavorite : virtual public TabSeletable
{
public:
    TabFavorite();
    virtual ~TabFavorite();
    void Show(bool selected);
    void SetInputHooks(Input *input);
    void UnsetInputHooks(Input *input);

private:
    size_t _GetItemCount() { return gFavorites->size(); };
    bool _ItemVisable(size_t index) override { return true; };
    void _OnActive(Input *input) override;
    void _OnKeyCross(Input *input);
    void _UpdateStatus();
};