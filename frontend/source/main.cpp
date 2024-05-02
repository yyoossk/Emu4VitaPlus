#include "file.h"
#include "global.h"
#include "app.h"
#include "log.h"

int main(int argc, char *const argv[])
{
    File::MakeDirs(APP_DATA_DIR);
    gLog = new Log(APP_LOG_PATH);
    LogDebug("Start");
    {
        App app;
        app.Run();
    }

    LogDebug("Exit now");

    delete gLog;
    return 0;
}
