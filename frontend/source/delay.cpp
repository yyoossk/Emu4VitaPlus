#include <psp2/kernel/processmgr.h>
#include "delay.h"
#include "log.h"

Delay::Delay() : Delay(0ULL)
{
    LogFunctionName;
}

Delay::Delay(uint64_t interval_ms)
{
    LogFunctionName;
    SetInterval(interval_ms);
}

Delay::~Delay()
{
    LogFunctionName;
}

void Delay::SetInterval(uint64_t interval_ms)
{
    LogFunctionName;
    _interval_ms = interval_ms;
    _next_ms = sceKernelGetProcessTimeWide() + _interval_ms;
    LogDebug("_interval_ms: %lld _next_ms:%lld", _interval_ms, _next_ms);
}

uint64_t Delay::GetInterval()
{
    return _interval_ms;
}

void Delay::Wait()
{
    uint64_t current = sceKernelGetProcessTimeWide();
    if (current < _next_ms)
    {
        uint64_t delay_ms = _next_ms - current;
        // LogDebug("delay %lld", delay_ms);
        sceKernelDelayThread(delay_ms);
        _next_ms += _interval_ms;
    }
    else
    {
        _next_ms = current + _interval_ms;
    }
}

bool Delay::TimeUp()
{
    uint64_t current = sceKernelGetProcessTimeWide();
    bool result = (current >= _next_ms);
    if (result)
    {
        _next_ms = current + _interval_ms;
    }
    return result;
}