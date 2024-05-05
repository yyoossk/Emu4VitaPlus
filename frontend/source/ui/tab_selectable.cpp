#include "tab_selectable.h"

TabSeletable::TabSeletable(TEXT_ENUM title_id, std::vector<ItemBase *> items)
    : TabBase(title_id),
      _items(std::move(items)),
      _index(0)
{
    LogFunctionName;
}

TabSeletable::TabSeletable(TEXT_ENUM title_id)
    : TabBase(title_id),
      _index(0)
{
    LogFunctionName;
}

TabSeletable::~TabSeletable()
{
    LogFunctionName;
    for (auto &item : _items)
    {
        delete item;
    }
}

void TabSeletable::SetInputHooks(Input *input)
{
    input->SetKeyDownCallback(SCE_CTRL_UP, std::bind(&TabSeletable::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_DOWN, std::bind(&TabSeletable::_OnKeyDown, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_UP, std::bind(&TabSeletable::_OnKeyUp, this, input), true);
    input->SetKeyDownCallback(SCE_CTRL_LSTICK_DOWN, std::bind(&TabSeletable::_OnKeyDown, this, input), true);
    input->SetKeyUpCallback(SCE_CTRL_CIRCLE, std::bind(&TabSeletable::_OnActive, this, input));
    input->SetKeyUpCallback(SCE_CTRL_TRIANGLE, std::bind(&TabSeletable::_OnOption, this, input));
}

void TabSeletable::UnsetInputHooks(Input *input)
{
    input->UnsetKeyDownCallback(SCE_CTRL_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_DOWN);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_UP);
    input->UnsetKeyDownCallback(SCE_CTRL_LSTICK_DOWN);
    input->UnsetKeyUpCallback(SCE_CTRL_CIRCLE);
    input->UnsetKeyUpCallback(SCE_CTRL_TRIANGLE);
}

void TabSeletable::Show(bool selected)
{
    if (ImGui::BeginTabItem(TEXT(_title_id), NULL, selected ? ImGuiTabItemFlags_SetSelected : 0))
    {
        ImGui::BeginChild(TEXT(_title_id));
        ImGui::Columns(2, NULL, false);
        for (size_t i = 0; i < _GetItemCount(); i++)
        {
            _ShowItem(i, i == _index);
            if (i == _index && ImGui::GetScrollMaxY() > 0.f)
            {
                ImGui::SetScrollHereY((float)_index / (float)_GetItemCount());
            }
        }
        ImGui::Columns(1);
        if (_status_text.size() > 0)
        {
            ImGui::Text(_status_text.c_str());
        }
        ImGui::EndChild();

        ImGui::EndTabItem();
    }
}

void TabSeletable::_OnKeyUp(Input *input)
{
    LogFunctionName;
    if (_GetItemCount() == 0)
    {
        return;
    }

    do
    {
        _index = ((_index == 0 ? (_GetItemCount() - 1) : (_index - 1)));
        LogDebug("_ItemVisable(_index) %d", _ItemVisable(_index));
    } while (!_ItemVisable(_index));
}

void TabSeletable::_OnKeyDown(Input *input)
{
    LogFunctionName;
    if (_GetItemCount() == 0)
    {
        return;
    }

    do
    {
        _index = (((_index + 1) == _GetItemCount() ? 0 : (_index + 1)));
        LogDebug("_ItemVisable(_index) %d %d", _index, _ItemVisable(_index));
    } while (!_ItemVisable(_index));
}

size_t TabSeletable::_GetItemCount()
{
    return _items.size();
}

void TabSeletable::_ShowItem(size_t index, bool selected)
{
    if (index < _items.size())
    {
        _items[index]->Show(selected);
    }
}

void TabSeletable::_OnActive(Input *input)
{
    LogFunctionName;
    if (_index < _items.size())
    {
        _items[_index]->OnActive(input);
    }
}

void TabSeletable::_OnOption(Input *input)
{
    LogFunctionName;
    if (_index < _items.size())
    {
        _items[_index]->OnOption(input);
    }
}

bool TabSeletable::_ItemVisable(size_t index)
{
    return _items[_index]->Visable();
}

void TabSeletable::SetStatusText(std::string &text)
{
    _status_text = text;
}