#pragma once
#include <imgui_vita2d/imgui_vita.h>
#include "input.h"
#include "language_string.h"

typedef std::function<void()> CallbackFunc;

class ItemBase
{
public:
    ItemBase(LanguageString text, LanguageString info = "", CallbackFunc active_callback = nullptr, CallbackFunc option_callback = nullptr, bool visable = true)
        : _text(text),
          _info(info),
          _active_callback(active_callback),
          _option_callback(option_callback),
          _visable(visable){};

    virtual ~ItemBase(){};

    virtual void Show(bool selected)
    {
        ImGui::Selectable(GetText(), selected);
        ImGui::NextColumn();
        ImGui::NextColumn();
    };

    virtual void OnActive(Input *input)
    {
        if (_active_callback != nullptr)
        {
            _active_callback();
        }
    };

    virtual void OnOption(Input *input)
    {
        if (_option_callback != nullptr)
        {
            _option_callback();
        }
    }

    const char *GetText() { return _text.Get(); };
    const char *GetInfo() { return _info.Get(); };
    void SetVisable(bool visable) { _visable = visable; };
    bool Visable() { return _visable; };

protected:
    LanguageString _text;
    LanguageString _info;
    CallbackFunc _active_callback;
    CallbackFunc _option_callback;

    bool _visable;
};