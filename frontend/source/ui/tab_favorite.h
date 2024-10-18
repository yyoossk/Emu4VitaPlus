#pragma once
#include <vita2d.h>
#include "tab_selectable.h"
#include "favorite.h"
#include "dialog.h"

class TabFavorite : virtual public TabSeletable
{
public:
    TabFavorite();
    virtual ~TabFavorite();
    void Show(bool selected);
    void SetInputHooks(Input *input);
    void UnsetInputHooks(Input *input);
    virtual void ChangeLanguage(uint32_t language) override;

private:
    size_t _GetItemCount() override { return gFavorites->size(); };
    bool _ItemVisable(size_t index) override { return true; };
    void _OnActive(Input *input) override;
    void _OnKeyCross(Input *input);
    void _OnKeyUp(Input *input) override;
    void _OnKeyDown(Input *input) override;
    void _OnRemove(Input *input, int index);
    void _UpdateStatus();
    void _UpdateTexture();

    Dialog *_dialog;
    vita2d_texture *_texture;
    float _texture_width;
    float _texture_height;
};