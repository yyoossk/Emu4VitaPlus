#pragma once
#include <psp2/kernel/threadmgr.h>
#include <array>
#include <vector>
#include <string>
#include "emulator.h"
#include "input.h"
#include "tab_base.h"

enum
{
    TAB_ITEM_SYSTEM = 0,
    TAB_ITEM_BROWSER,
    // TAB_ITEM_FAVORITE,
    TAB_ITEM_GRAPHICS,
    TAB_ITME_CONTROL,
    TAB_ITEM_HOTKEY,
    TAB_ITEM_CORE,
    TAB_ITEM_ABOUT,
    TAB_ITEM_COUNT
};

class Ui
{
public:
    Ui(const char *path);
    virtual ~Ui();
    void Show();
    void Run();
    void AppendLog(const char *log) { _logs.emplace_back(log); };

private:
    void _InitImgui();
    void _DeinitImgui();

    void _SetKeyHooks();
    void _OnKeyL2(Input *input);
    void _OnKeyR2(Input *input);

    Input _input;
    size_t _tab_index;

    std::array<TabBase *, TAB_ITEM_COUNT> _tabs;
    std::vector<std::string> _logs;

    // SceUID _update_sema;
};
