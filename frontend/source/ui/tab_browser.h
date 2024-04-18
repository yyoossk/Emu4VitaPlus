#pragma once
#include "global.h"
#include "tab_base.h"
#include "directory.h"

class TabBrowser : virtual public TabBase
{
public:
    TabBrowser(const char *path);
    virtual ~TabBrowser();
    void SetInputHooks(Input *input);
    void UnsetInputHooks(Input *input);
    void Show(bool selected);

private:
    size_t _GetItemCount() { return _directory->GetSize(); };
    void _OnKeyCircle();
    void _OnKeyCross();
    void _OnClick();

    Directory *_directory;
};