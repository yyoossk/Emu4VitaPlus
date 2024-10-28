#pragma once
#include <psp2/kernel/threadmgr.h>
#include <vector>
#include <string>
#include <language_string.h>
#include "emulator.h"
#include "input.h"
#include "tab_base.h"
#include "dialog.h"
#include "app.h"

class Ui
{
    friend class App;

public:
    Ui();
    virtual ~Ui();

    void CreateTables(); // don't call it before gEmulator initialized
    void Show();
    void Run();
    void SetInputHooks();
    void AppendLog(const char *log);
    void ClearLogs();
    void UpdateCoreOptions();
    void ChangeLanguage();
    void UpdateCheatOptions();
    void OnStatusChanged(APP_STATUS status);
    void SetHint(const char *s, int frame_count = 120);

private:
    void
    _InitImgui();
    void _DeinitImgui();

    void _OnKeyL2(Input *input);
    void _OnKeyR2(Input *input);
    void _OnPsButton(Input *input);
    void _OnDialog(Input *input, int index);
    void _OnCleanCache(Input *input);

    void _ShowBoot();
    void _ShowNormal();
    void _ShowHint();

    void _ClearTabs();

    std::string _title;

    Input _input;
    size_t _tab_index;

    TabBase *_tabs[TAB_INDEX_COUNT];
    std::vector<std::string> _logs;
    Dialog *_dialog;
    int _current_dialog;

    LanguageString _hint;
    int _hint_count;
};

extern Ui *gUi;