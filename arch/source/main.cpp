#include <psp2/kernel/processmgr.h>
#include <stdlib.h>
#include "file.h"
#include "log.h"
#include "app.h"
#include "defines.h"

int main(int argc, char *const argv[])
{
    File::MakeDirs(APP_DATA_DIR);
    gLog = new Log(APP_LOG_PATH);
    LogInfo("updated on " __DATE__ " " __TIME__);
    LogDebug("Start");
    {
        App app;
        app.Run();
    }

    LogDebug("Exit");

    delete gLog;

    if (*gCorePath)
    {
        char *const argv[] = {"arch", NULL};
        sceAppMgrLoadExec(gCorePath, argv, NULL);
    }

    sceKernelExitProcess(0);
    return 0;
}
