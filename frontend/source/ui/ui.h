#pragma once
#include <array>
#include "emulator.h"
#include "input.h"
#include "tab_base.h"

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

    Input _input;
    size_t _tab_index;

    std::array<TabBase *, 3> _tabs;
};
