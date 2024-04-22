#pragma once
#include "input.h"
#include "my_imgui.h"
#include "global.h"

class ItemBase
{
public:
    ItemBase(size_t text_id, InputFunc active_callback = nullptr) : _text_id(text_id), _active_callback(active_callback){};
    virtual ~ItemBase(){};

    virtual void Show(bool selected)
    {
        ImGui::Selectable(TEXT(_text_id), selected);
        ImGui::NextColumn();
        ImGui::NextColumn();
    };

    virtual void OnActive(Input *input)
    {
        if (_active_callback != nullptr)
        {
            _active_callback(input);
        }
    };

protected:
    size_t _text_id;
    InputFunc _active_callback;
};