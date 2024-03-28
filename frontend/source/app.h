#pragma once
#include "drawer.h"

class App
{
public:
    App();
    virtual ~App();

    void Run();

private:
    void _InitImgui();
    void _DeinitImgui();

    Drawer *_drawer;
};