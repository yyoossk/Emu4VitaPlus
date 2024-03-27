#pragma once
#include "global.h"
#include "emulator.h"
#include "input.h"
#include "directory.h"
#include "global.h"

class Browser
{
public:
    Browser(const char *path);
    virtual ~Browser();
    void Show();

private:
    void _SetKeyHooks();
    void _OnKeyL2();
    void _OnKeyR2();
    void _OnKeyUp();
    void _OnKeyDown();
    void _OnKeyCircle();
    void _OnKeyCross();

    Input _input;
    Directory *_directory;
    int _tabIndex;
    int _browserIndex;
    int _favoriteIndex;
};
