#include "defines.h"
#include "core_spec.h"

extern "C" void _init_vita_heap(void);

void InitCoreSpec()
{
#if defined(PS_BUILD)
    _init_vita_heap();
#endif
}