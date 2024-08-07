#pragma once
#include <psp2/kernel/threadmgr.h>

#define CLASS_POINTER(CLASS, POINT, ARGP) CLASS *POINT = *(CLASS **)ARGP;

#define SCE_KERNEL_HIGHEST_PRIORITY_USER 64
#define SCE_KERNEL_LOWEST_PRIORITY_USER 191
#define SCE_KERNEL_DEFAULT_PRIORITY_USER 0x10000100

extern void LogCpuID(SceUID thread, const char *message);

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_CPU_ID_INTERVAL 5000000
#define LOG_CPU_ID
#endif

class ThreadBase
{
public:
    ThreadBase(SceKernelThreadEntry entry, int priority = SCE_KERNEL_DEFAULT_PRIORITY_USER, int stack_size = 0x10000);
    virtual ~ThreadBase();
    bool Start(); // the point of class will be sent
    bool Start(void *data, SceSize size);
    void Stop(bool force = false);
    bool IsRunning() { return _keep_running; };
    void Lock();
    void Unlock();
    void Wait();
    void Signal();

    void LogCpuId(const char *str);

protected:
    SceKernelThreadEntry _entry;
    int _priority;
    SceSize _stack_size;
    bool _keep_running;
    SceUID _thread_id;
    SceKernelLwMutexWork _mutex;
    SceKernelLwCondWork _cond;
};