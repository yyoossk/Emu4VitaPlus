#pragma once
#include <array>
#include <psp2/kernel/threadmgr.h>
#include "defines.h"

template <typename T, size_t S>
class BufBase
{
public:
    BufBase() : _index(0)
    {
        sceKernelCreateLwMutex(&_mutex, "buf_mutex", 0, 1, nullptr);
    };

    virtual ~BufBase()
    {
        sceKernelDeleteLwMutex(&_mutex);
    };

    T Next()
    {
        LOOP_PLUS_ONE(_index, S);
        return _buf[_index];
    };

    T Current()
    {
        return _buf[_index];
    };

    void Lock() { sceKernelLockLwMutex(&_mutex, 1, nullptr); };
    void Unlock() { sceKernelUnlockLwMutex(&_mutex, 1); };

protected:
    std::array<T, S> _buf;
    SceKernelLwMutexWork _mutex;
    size_t _index;
};