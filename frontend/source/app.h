#pragma once
#include "video.h"
#include "audio.h"

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
    Audio *_audio;
};