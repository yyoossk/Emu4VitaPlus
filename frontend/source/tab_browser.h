#pragma once
#include "tab_base.h"
#include "directory.h"

class TabBrowser : public TabBase
{
public:
    TabBrowser(const char *path);
    virtual ~TabBrowser();
    void SetInputHooks(Input *input);
    void UnsetInputHooks(Input *input);
    void Show();

private:
    void _OnKeyUp();
    void _OnKeyDown();
    void _OnKeyCircle();
    void _OnKeyCross();

    Directory *_directory;
    size_t _index;
};