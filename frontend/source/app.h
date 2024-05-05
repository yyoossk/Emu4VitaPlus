#pragma once
#include "video.h"

class App
{
public:
    App();
    virtual ~App();

    void Run();

private:
    bool _IsSaveMode();
};