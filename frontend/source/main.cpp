#include "file.h"
#include "global.h"
#include "app.h"
#include "log.h"

int main(int argc, char *const argv[])
{
    File::MakeDirs(APP_DATA_DIR);
    gLog = new Log(APP_LOG_PATH);

    App app;
    app.Run();

    delete gLog;

    return 0;
}
