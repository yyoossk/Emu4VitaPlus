#include "tab_base.h"
#include "global.h"

TabBase::TabBase(TEXT_ENUM title_id, bool visable)
    : _title_id(title_id),
      _index(0),
      _visable(visable)
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

void TabBase::Show(bool selected)
{
    LogFunctionName;
    if (ImGui::BeginTabItem(TEXT(_title_id), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::Columns(2, NULL, false);
        for (size_t i = 0; i < _GetItemCount(); i++)
        {
            _ShowItem(i, i == _index);
        }
        ImGui::Columns(1);
        ImGui::EndTabItem();
    }
}