#include "tab_base.h"

TabBase::TabBase(bool visable) : _index(0), _visable(visable)
{
}

TabBase::~TabBase()
{
}

void TabBase::_OnKeyUp(Input *input)
{
    if (_GetItemCount() == 0)
    {
        return;
    }

    if (_index == 0)
    {
        _index = _GetItemCount() - 1;
    }
    else
    {
        _index--;
    };
}

void TabBase::_OnKeyDown(Input *input)
{
    if (_GetItemCount() == 0)
    {
        return;
    }

    if (_index == _GetItemCount() - 1)
    {
        _index = 0;
    }
    else
    {
        _index++;
    }
}

void TabBase::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_UP, std::bind(&TabBase::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_DOWN, std::bind(&TabBase::_OnKeyDown, this, input), true);
    input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&TabBase::_OnClick, this, input));
}

void TabBase::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
}