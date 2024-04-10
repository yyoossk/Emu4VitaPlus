#pragma once
#include <array>
#include "emulator.h"
#include "input.h"
#include "tab_base.h"

enum
{
    TAB_ITEM_SYSTEM = 0,
    TAB_ITEM_BROWSER,
    TAB_ITEM_ABOUT,
    // TAB_ITEM_FAVORITE,
    TAB_ITEM_COUNT
};

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

    std::array<TabBase *, TAB_ITEM_COUNT> _tabs;
};
