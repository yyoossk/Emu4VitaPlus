#pragma once
#include "emulator.h"
#include "input.h"
#include "directory.h"

class Ui
{
public:
    Ui(const char *path);
    virtual ~Ui();
    void Show();
    void Run();

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
    int _tab_index;
    int _browser_index;
    int _favorite_index;
};
