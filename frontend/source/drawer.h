#pragma once
#include <psp2/kernel/threadmgr.h>

class Drawer
{
public:
    Drawer();
    virtual ~Drawer();

    bool Start();
    void Stop();

private:
    static int _DrawThread(SceSize args, void *argp);
    void _InitImgui();
    void _DeinitImgui();

    bool _keep_running;
    SceUID _thread_id;
};