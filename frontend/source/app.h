#pragma once
#include "gui.h"

class App
{
public:
    App();
    virtual ~App();

    void Run();

private:
    Gui *_gui;
    bool _running;
};