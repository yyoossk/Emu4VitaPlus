#include <stdlib.h>
#include "file.h"
#include "app.h"
#include "log.h"
#include "profiler.h"
#include "defines.h"

#define SCE_LIBC_HEAP_SIZE_EXTENDED_ALLOC_NO_LIMIT (0xffffffffU)

unsigned int sceLibcHeapExtendedAlloc = 1;
unsigned int sceLibcHeapSize = SCE_LIBC_HEAP_SIZE_EXTENDED_ALLOC_NO_LIMIT;
// unsigned int sceUserMainThreadStackSize = 2 * 1024 * 1024;

int main(int argc, char *const argv[])
{

    sceKernelChangeThreadCpuAffinityMask(sceKernelGetThreadId(), SCE_KERNEL_CPU_MASK_USER_0);

    File::MakeDirs(APP_DATA_DIR);
    File::MakeDirs(CORE_SYSTEM_DIR);
    gLog = new Log(APP_LOG_PATH);
#if LOG_LEVEL <= LOG_LEVEL_DEBUG
    gProfiler = new Profiler();
#endif
    LogInfo("updated on " __DATE__ " " __TIME__);
    LogDebug("Start");
    {
        App app(argc, argv);
        app.Run();
    }

    LogDebug("Exit");

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
    delete gProfiler;
#endif
    delete gLog;

    sceKernelExitProcess(0);
    return 0;
}
