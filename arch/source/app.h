#pragma once
#include <vector>
#include "input.h"
#include "core_button.h"
#include "file.h"

extern bool gRunning;
extern char gCorePath[SCE_FIOS_PATH_MAX];

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
    void _OnClick(Input *input);
    void _OnKeyLeft(Input *input);
    void _OnKeyRight(Input *input);
    void _OnKeyUp(Input *input);
    void _OnKeyDown(Input *input);
    size_t _GetIndex();

    Input _input;
    std::vector<CoreButton *> _buttons;
    size_t _index_x;
    size_t _index_y;
};