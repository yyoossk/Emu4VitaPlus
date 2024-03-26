#pragma once
#include "global.h"
#include "emulator.h"
#include "input.h"
#include "directory.h"

class Browser
{
public:
    Browser(const char *path, const Emulator *emulator);
    virtual ~Browser();
    SCREEN_STATUS Show();

private:
    void _OnKeyL2();
    void _OnKeyR2();
    void _OnKeyUp();
    void _OnKeyDown();

    Input _input;
    const Emulator *_emulator;
    Directory *_directory;
    int _tabIndex;
    int _browserIndex;
    int _favoriteIndex;
};
