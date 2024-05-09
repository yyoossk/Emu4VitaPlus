#include "file.h"
#include "app.h"
#include "log.h"
#include "defines.h"

int main(int argc, char *const argv[])
{
    File::MakeDirs(APP_DATA_DIR);
    gLog = new Log(APP_LOG_PATH);
    LogDebug("updated on " __DATE__ " " __TIME__);
    LogDebug("Start");
    {
        App app;
        app.Run();
    }

    LogDebug("Exit");

    delete gLog;

    sceKernelExitProcess(0);
    return 0;
}
