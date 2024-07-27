#pragma once

enum APP_STATUS
{
    APP_STATUS_BOOT,
    APP_STATUS_SHOW_UI,
    APP_STATUS_RUN_GAME,
    APP_STATUS_SHOW_UI_IN_GAME,
    APP_STATUS_RETURN_ARCH,
    APP_STATUS_EXIT
};

extern APP_STATUS gStatus;

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