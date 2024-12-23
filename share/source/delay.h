#pragma once
#include <stdint.h>

class Delay
{
public:
    Delay();
    Delay(uint64_t interval_ms);
    virtual ~Delay();
    void SetInterval(uint64_t interval_ms);
    void SetDelay(uint64_t ms);
    uint64_t GetInterval();
    bool Wait();
    bool TimeUp();

private:
    uint64_t _interval_ms;
    uint64_t _next_ms;
};