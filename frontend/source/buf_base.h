#pragma once
#include <array>
#include <psp2/kernel/threadmgr.h>

template <typename T, size_t S>
class BufBase
{
public:
    BufBase() : _index(0)
    {
        for (size_t i = 0; i < S; i++)
        {
            sceKernelCreateLwMutex(&_mutex[i], "buf_mutex", 0, 1, nullptr);
        }
    };

    virtual ~BufBase()
    {
        for (size_t i = 0; i < S; i++)
        {
            sceKernelDeleteLwMutex(&_mutex[i]);
        }
    };

    T Next()
    {
        _index++;
        _index %= S;
        return _buf[_index];
    };

    T Current()
    {
        return _buf[_index];
    };

    void Lock() { sceKernelLockLwMutex(&_mutex[_index], 1, nullptr); };
    void Unlock() { sceKernelUnlockLwMutex(&_mutex[_index], 1); };

protected:
    std::array<T, S> _buf;
    std::array<SceKernelLwMutexWork, S> _mutex;
    size_t _index;
};