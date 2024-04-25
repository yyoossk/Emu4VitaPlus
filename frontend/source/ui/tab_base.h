#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include "input.h"
#include "language_define.h"

class TabBase
{
public:
    TabBase(TEXT_ENUM title_id);

    virtual ~TabBase();

    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    virtual void Show(bool selected);

protected:
    virtual void _ShowItem(size_t index, bool selected){};
    virtual size_t _GetItemCount() = 0;

    virtual void _OnKeyUp(Input *input);
    virtual void _OnKeyDown(Input *input);
    virtual void _OnActive(Input *input){};
    virtual void _OnOption(Input *input){};

    TEXT_ENUM _title_id;
    size_t _index;
};