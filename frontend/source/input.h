#pragma once
#include <stdint.h>
#include <functional>
#include <unordered_map>

typedef std::function<void()> InputFunc;
const uint64_t SCE_CTRL_LSTICK_UP = 0x0000000100000000;
const uint64_t SCE_CTRL_LSTICK_RIGHT = 0x0000000200000000;
const uint64_t SCE_CTRL_LSTICK_DOWN = 0x0000000400000000;
const uint64_t SCE_CTRL_LSTICK_LEFT = 0x0000000800000000;
const uint64_t SCE_CTRL_RSTICK_UP = 0x0000001000000000;
const uint64_t SCE_CTRL_RSTICK_RIGHT = 0x0000002000000000;
const uint64_t SCE_CTRL_RSTICK_DOWN = 0x0000004000000000;
const uint64_t SCE_CTRL_RSTICK_LEFT = 0x0000008000000000;

class Input
{
public:
    Input();
    virtual ~Input();
    void SetCallback(uint64_t key, InputFunc func);
    void Run();

private:
    std::unordered_map<uint64_t, InputFunc> _callbacks;

    uint64_t _old_key;
};