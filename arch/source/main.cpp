#include <psp2/kernel/processmgr.h>
#include <stdlib.h>
#include "file.h"
#include "log.h"
#include "app.h"
#include "defines.h"
#include "config.h"

static char *const _argv[] = {"arch", NULL};

int main(int argc, char *const argv[])
{
    File::MakeDirs(APP_DATA_DIR);
    gLog = new Log(APP_LOG_PATH);
    gConfig = new Config();
    LogInfo("updated on " __DATE__ " " __TIME__);
    LogDebug("Start");
    {
        App app;
        app.Run();
    }

    LogDebug("Exit");
    LogInfo("gCorePath: %s", gCorePath);

    delete gConfig;
    delete gLog;

    if (*gCorePath)
    {
        sceAppMgrLoadExec(gCorePath, _argv, NULL);
    }

    sceAppUtilShutdown();
    sceKernelExitProcess(0);
    return 0;
}
