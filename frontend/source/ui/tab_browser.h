#pragma once
#include "tab_selectable.h"
#include "directory.h"
#include "global.h"

class TabBrowser : public TabSeletable
{
public:
    TabBrowser(const char *path);
    virtual ~TabBrowser();
    void SetInputHooks(Input *input);
    void UnsetInputHooks(Input *input);
    void Show(bool selected);

private:
    size_t _GetItemCount() { return _directory->GetSize(); };
    void _OnKeyCross(Input *input);
    void _OnActive(Input *input);
    bool _ItemVisable(size_t index) { return true; };

    Directory *_directory;
};