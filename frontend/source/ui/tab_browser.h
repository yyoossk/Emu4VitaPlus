#pragma once
#include "global.h"
#include "tab_base.h"
#include "directory.h"

class TabBrowser : public TabBase
{
public:
    TabBrowser(const char *path);
    virtual ~TabBrowser();
    virtual void SetInputHooks(Input *input);
    virtual void UnsetInputHooks(Input *input);
    virtual void Show(bool selected);

private:
    void _OnKeyUp();
    void _OnKeyDown();
    void _OnKeyCircle();
    void _OnKeyCross();

    Directory *_directory;
    size_t _index;
};