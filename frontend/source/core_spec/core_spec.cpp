#include "defines.h"
#include "core_spec.h"
#include "log.h"

extern "C"
{
    extern int _newlib_heap_memblock;
    extern int _newlib_vm_memblock;
    extern int _newlib_heap_size_user __attribute__((weak));
    extern int _newlib_vm_size_user __attribute__((weak));
    void _init_vita_heap(void);
}

void InitCoreSpec()
{
#if defined(PCSX_REARMED_BUILD) || defined(DOSBOX_PURE_BUILD) || defined(GPSP_BUILD) || defined(PICODRIVE_BUILD)
    // LogFunctionName;
    // LogDebug("  _newlib_vm_size_user:   %08x", _newlib_vm_size_user);
    // LogDebug("  _newlib_heap_size_user: %08x", _newlib_heap_size_user);
    // _init_vita_heap();

#endif
}