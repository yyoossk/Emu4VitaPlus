#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include "input.h"
#include "language_define.h"

class TabBase
{
public:
    TabBase(TEXT_ENUM title_id, bool visable = true);

    virtual ~TabBase();

    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    virtual void Show(bool selected);
    void SetVisable(bool visable) { _visable = visable; };
    bool Visable() { return _visable; };

protected:
    virtual void _ShowItem(size_t index, bool selected){};
    virtual size_t _GetItemCount() = 0;
    virtual void _ActiveItem(Input *input, size_t index){};

    virtual void _OnKeyUp(Input *input);
    virtual void _OnKeyDown(Input *input);
    virtual void _OnClick(Input *input) { _ActiveItem(input, _index); };

    TEXT_ENUM _title_id;
    size_t _index;
    bool _visable;
};