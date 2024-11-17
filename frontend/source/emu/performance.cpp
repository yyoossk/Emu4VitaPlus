#include <vector>
#include <string.h>
#include <psp2/kernel/processmgr.h>
#include "performance.h"
#include "log.h"

std::vector<retro_perf_counter *> gPerfCounters;

int64_t GetTimeUsec()
{
    return sceKernelGetSystemTimeWide();
}

uint64_t GetCpuFeatures()
{
    return RETRO_SIMD_NEON | RETRO_SIMD_VFPV3;
}

uint64_t GetPerfCounter()
{
    return sceKernelGetSystemTimeWide();
}

void RegisterPerfCounter(retro_perf_counter *perf)
{
    if (!perf->registered)
    {
        memset(perf, 0, sizeof(retro_perf_counter));
        perf->registered = true;
        gPerfCounters.push_back(perf);
    }
}

void PerfCounterStart(retro_perf_counter *perf)
{
    perf->call_cnt++;
    perf->start = sceKernelGetSystemTimeWide();
}

void PerfCounterStop(retro_perf_counter *perf)
{
    perf->total += sceKernelGetSystemTimeWide() - perf->start;
}

void PerfLog()
{
    for (const auto &perf : gPerfCounters)
    {
        LogDebug("%s  %llu ticks, %llu runs", perf->ident, perf->total / perf->call_cnt, perf->call_cnt);
    }
}