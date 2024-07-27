#pragma once
#include <vector>
#include "tab_base.h"
#include "item_base.h"

class TabSeletable : public TabBase
{
public:
    TabSeletable(TEXT_ENUM title_id, std::vector<ItemBase *> items, int columns = 2);
    TabSeletable(TEXT_ENUM title_id, int columns = 2);
    virtual ~TabSeletable();

    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    virtual void Show(bool selected);
    void SetStatusText(std::string &text);
    void SetItemVisable(size_t index, bool visable);
    bool ItemVisable(size_t index);
    void SetColumns(int columns);
    void SetIndex(size_t index);

protected:
    virtual void _OnKeyUp(Input *input);
    virtual void _OnKeyDown(Input *input);
    virtual size_t _GetItemCount();
    virtual void _ShowItem(size_t index, bool selected);
    virtual void _OnActive(Input *input);
    virtual void _OnOption(Input *input);
    virtual bool _ItemVisable(size_t index);

    std::vector<ItemBase *> _items;
    size_t _index;
    std::string _status_text;
    int _columns;
};