#pragma once
#include <vita2d.h>
#include <imgui_vita2d/imgui_vita.h>
#include <string>
#include <vector>
#include "input.h"

class CoreButton
{
public:
    CoreButton(std::string name, std::vector<std::string> cores);
    virtual ~CoreButton();
    void Show(bool selected);
    void SetInputHooks(Input *input);
    void UnsetInputHooks(Input *input);

private:
    void _ShowPopup();
    void _OnKeyUp(Input *input);
    void _OnKeyDown(Input *input);
    void _OnClick(Input *input);
    void _OnCancel(Input *input);

    std::string _name;
    std::vector<std::string> _cores;
    vita2d_texture *_texture;
    bool _actived;
};