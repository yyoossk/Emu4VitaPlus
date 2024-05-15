#include "tab_selectable.h"
#include "defines.h"

TabSeletable::TabSeletable(TEXT_ENUM title_id, std::vector<ItemBase *> items, int columns)
    : TabBase(title_id),
      _items(std::move(items)),
      _index(0),
      _columns(columns)
{
    LogFunctionName;
}

TabSeletable::TabSeletable(TEXT_ENUM title_id, int columns)
    : TabBase(title_id),
      _index(0),
      _columns(columns)
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
        ImVec2 size = {0.f, 0.f};
        if (ItemVisable(_index))
        {
            _status_text = _items[_index]->GetInfo();
            if (_status_text.size() > 0)
            {
                ImVec2 s = ImGui::CalcTextSize(_status_text.c_str());
                size.y = -s.y * (s.x / ImGui::GetContentRegionAvailWidth() + 1);
            }
        }

        ImGui::BeginChild(TEXT(_title_id), size);
        ImGui::Columns(_columns, NULL, false);
        size_t total = _GetItemCount();
        for (size_t i = 0; i < total; i++)
        {
            if (ItemVisable(i))
            {
                _ShowItem(i, i == _index);
                if (i == _index && ImGui::GetScrollMaxY() > 0.f)
                {
                    ImGui::SetScrollHereY((float)_index / (float)total);
                }
                ImGui::NextColumn();
            }
            else if (i == _index)
            {
                LOOP_PLUS_ONE(_index, total);
            }
        }
        ImGui::Columns(1);
        ImGui::EndChild();

        if (_status_text.size() > 0)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
            ImGui::TextWrapped(_status_text.c_str());
            ImGui::PopStyleColor();
        }

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
        LOOP_MINUS_ONE(_index, _GetItemCount());
        // LogDebug("_ItemVisable(_index) %d", _ItemVisable(_index));
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
        LOOP_PLUS_ONE(_index, _GetItemCount());
        // LogDebug("_ItemVisable(_index) %d %d", _index, _ItemVisable(_index));
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

void TabSeletable::SetItemVisable(size_t index, bool visable)
{
    if (index < _items.size())
    {
        _items[index]->SetVisable(visable);
    }
}

bool TabSeletable::ItemVisable(size_t index)
{
    if (index < _items.size())
    {
        return _items[index]->Visable();
    }
    else
    {
        return false;
    }
}

void TabSeletable::SetColumns(int columns)
{
    _columns = columns;
}