#pragma once
#include "video.h"

class App
{
public:
    App();
    virtual ~App();

    void Run();

private:
    void _InitImgui();
    void _DeinitImgui();

    Video *_video;
};