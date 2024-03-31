#include <psp2/kernel/processmgr.h>
#include "delay.h"
#include "log.h"

Delay::Delay() : Delay(0ULL)
{
}

Delay::Delay(uint64_t interval_ms) : _interval_ms(interval_ms)
{
    _next_ms = sceKernelGetProcessTimeWide();
}

Delay::~Delay()
{
}

void Delay::SetInterval(uint64_t interval_ms)
{
    _interval_ms = interval_ms;
    _next_ms = sceKernelGetProcessTimeWide() + _interval_ms;
}

void Delay::Wait()
{
    uint64_t current = sceKernelGetProcessTimeWide();
    if (current < _next_ms)
    {
        uint64_t delay_ms = _next_ms - current;
        sceKernelDelayThread(delay_ms);
        _next_ms = current + delay_ms;
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