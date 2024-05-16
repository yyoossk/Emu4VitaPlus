#pragma once
#include <vector>
#include <imgui_vita2d/imgui_vita.h>
#include "language_string.h"
#include "input.h"

typedef std::function<void(Input *, int)> DialogCallbackFunc;

class Dialog
{
public:
    Dialog(LanguageString text,
           std::vector<LanguageString> options,
           DialogCallbackFunc callback);
    virtual ~Dialog();

    void Show();
    void SetInputHooks(Input *input);
    void UnsetInputHooks(Input *input);
    void OnActive(Input *input);
    void SetText(LanguageString text);

private:
    void _OnKeyLeft(Input *input);
    void _OnKeyRight(Input *input);
    void _OnClick(Input *input);
    void _OnCancel(Input *input);

    LanguageString _text;
    std::vector<LanguageString> _options;
    DialogCallbackFunc _callback;
    size_t _index;
    bool _actived;
};