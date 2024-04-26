#pragma once
#include <stdint.h>
#include <string>
#include "item_base.h"

class ItemHotkey : public virtual ItemBase
{
public:
    ItemHotkey(HotKeyConfig index, uint32_t *hotkey);
    virtual ~ItemHotkey();

    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    void Show(bool selected);
    virtual void OnActive(Input *input);

private:
    const char *_GetPreviewText();
    void _OnKeyUp(Input *input);
    void _OnKeyDown(Input *input);
    void _OnClick(Input *input);
    void _OnQuit(Input *input);

    uint32_t *_hotkey;
    bool _actived;
    size_t _index;
};