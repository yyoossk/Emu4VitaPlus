#pragma once
#include <stdint.h>
#include <libretro.h>

#ifdef __cplusplus
extern "C"
{
#endif

    extern int64_t GetTimeUsec();
    extern uint64_t GetCpuFeatures();
    extern uint64_t GetPerfCounter();
    extern void RegisterPerfCounter(retro_perf_counter *perf);
    extern void PerfCounterStart(retro_perf_counter *perf);
    extern void PerfCounterStop(retro_perf_counter *perf);
    extern void PerfLog();

#ifdef __cplusplus
}
#endif