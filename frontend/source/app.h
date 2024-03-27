#pragma once
#include "browser.h"
#include "emulator.h"

class App
{
public:
    App();
    virtual ~App();

    void Run();

private:
    void _InitImgui();
    void _DeinitImgui();

    Browser *_browser;
};