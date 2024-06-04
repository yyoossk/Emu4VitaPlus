#pragma once
#include <vector>
#include "input.h"
#include "core_button.h"

class App
{
public:
    App();
    virtual ~App();
    void Run();
    void SetInputHooks(Input *input);
    void UnsetInputHooks(Input *input);

private:
    void _Show();
    void _OnKeyLeft(Input *input);
    void _OnKeyRight(Input *input);

    Input _input;
    std::vector<CoreButton *> _buttons;
    size_t _index;
};