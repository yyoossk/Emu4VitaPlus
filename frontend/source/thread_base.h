#pragma once
#include <stdint.h>
#include <psp2/kernel/threadmgr.h>

#define CLASS_POINTER(CLASS, POINT, ARGP) CLASS *POINT = *(CLASS **)ARGP;

#define SCE_KERNEL_HIGHEST_PRIORITY_USER 64
#define SCE_KERNEL_LOWEST_PRIORITY_USER 191
#define SCE_KERNEL_DEFAULT_PRIORITY_USER 0x10000100

#if LOG_LEVEL <= LOG_LEVEL_DEBUG
#define LOG_CPU_ID_INTERVAL 5000000
#define LogCpu(CLASS, NAME) (CLASS)->_LogCpu(NAME);
#define BeginBlock(CLASS) (CLASS)->_BeginBlock();
#define EndBlock(CLASS) (CLASS)->_EndBlock();
#else
#define LogCpu(CLASS, NAME)
#define BeginBlock(CLASS)
#define EndBlock(CLASS)
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

protected:
#if LOG_LEVEL <= LOG_LEVEL_DEBUG
    void _LogCpu(const char *str);
    void _BeginBlock();
    void _EndBlock();

    uint64_t _block_time;
    uint64_t _start_time;
    uint64_t _next_log_time;
#endif

    SceKernelThreadEntry _entry;
    int _priority;
    SceSize _stack_size;
    bool _keep_running;
    SceUID _thread_id;
    SceKernelLwMutexWork _mutex;
    SceUID _semaid;
};