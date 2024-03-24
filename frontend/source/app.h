#pragma once

class App
{
public:
    App();
    virtual ~App();

    void Run();

private:
    void _InitImgui();
    void _DeinitImgui();

    bool _running;
};