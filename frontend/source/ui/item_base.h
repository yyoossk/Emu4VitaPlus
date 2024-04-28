#pragma once
#include "input.h"
#include "my_imgui.h"
#include "global.h"

typedef std::function<void()> CallbackFunc;

class ItemBase
{
public:
    ItemBase(size_t text_id, CallbackFunc active_callback = nullptr, CallbackFunc option_callback = nullptr, bool visable = true)
        : _text_id(text_id),
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

    const char *GetText() { return TEXT(_text_id); };
    void SetVisable(bool visable) { _visable = visable; };
    bool Visable() { return _visable; };

protected:
    size_t _text_id;
    CallbackFunc _active_callback;
    CallbackFunc _option_callback;

    bool _visable;
};