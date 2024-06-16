#pragma once
#include <stdint.h>

class Delay
{
public:
    Delay();
    Delay(uint64_t interval_ms);
    virtual ~Delay();
    void SetInterval(uint64_t interval_ms);
    uint64_t GetInterval();
    void Wait();
    bool TimeUp();

private:
    uint64_t _interval_ms;
    uint64_t _next_ms;
};