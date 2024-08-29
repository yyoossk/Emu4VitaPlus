#pragma once

#include "video.h"

enum APP_STATUS
{
    APP_STATUS_BOOT,
    APP_STATUS_SHOW_UI,
    APP_STATUS_RUN_GAME,
    APP_STATUS_SHOW_UI_IN_GAME,
    APP_STATUS_RETURN_ARCH,
    APP_STATUS_EXIT
};

class AppStatus
{
public:
    AppStatus() : _status(APP_STATUS_BOOT) {};
    virtual ~AppStatus() {};

    APP_STATUS Get() { return _status; };
    void Set(APP_STATUS status)
    {
        gVideo->Lock();
        _status = status;
        gVideo->Unlock();
    };

private:
    APP_STATUS _status;
};

extern AppStatus gStatus;

class App
{
public:
    App(int argc, char *const argv[]);
    virtual ~App();

    void Run();

private:
    bool _IsSaveMode();
    void _ParseParams(int argc, char *const argv[]);
};