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
    TAB_INDEX_SYSTEM = 0,
    TAB_INDEX_STATE,
    TAB_INDEX_BROWSER,
    // TAB_INDEX_FAVORITE,
    TAB_INDEX_GRAPHICS,
    TAB_INDEX_CONTROL,
    TAB_INDEX_HOTKEY,
    TAB_INDEX_CORE,
    TAB_INDEX_ABOUT,
    TAB_INDEX_COUNT
};

class Ui
{
    friend class App;

public:
    Ui();
    virtual ~Ui();

    void CreateTables(const char *path); // don't call it before gEmulator initialized
    void Show();
    void Run();
    void SetInputHooks();
    void AppendLog(const char *log);
    void ClearLogs();

private:
    void _InitImgui();
    void _DeinitImgui();

    void _OnKeyL2(Input *input);
    void _OnKeyR2(Input *input);

    void _ShowBoot();
    void _ShowNormal();

    Input _input;
    size_t _tab_index;

    std::array<TabBase *, TAB_INDEX_COUNT> _tabs;
    std::vector<std::string> _logs;
};

extern Ui *gUi;