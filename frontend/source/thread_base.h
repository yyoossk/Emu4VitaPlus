#pragma once
#include <psp2/kernel/threadmgr.h>

class ThreadBase
{
public:
    ThreadBase(SceKernelThreadEntry entry, int priority = 64, int stack_size = 0x10000);
    virtual ~ThreadBase();
    bool Start(); // the point of class will be sent
    bool Start(void *data, SceSize size);
    void Stop();
    bool IsRunning() { return _keep_running; };
    void Lock();
    void Unlock();
    void Wait();
    void Signal();

private:
    SceKernelThreadEntry _entry;
    int _priority;
    SceSize _stack_size;
    bool _keep_running;
    SceUID _thread_id;
    SceKernelLwMutexWork _mutex;
    SceKernelLwCondWork _cond;
};