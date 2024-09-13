#include <stdint.h>
#include "thread_base.h"

ThreadBase::ThreadBase(SceKernelThreadEntry entry, int priority, int stack_size)
    : _entry(entry),
      _priority(priority),
      _stack_size(stack_size),
      _thread_id(-1)
{
    LogFunctionName;
    sceKernelCreateLwMutex(&_mutex, "thread_mutex", 0, 0, NULL);
    _semaid = sceKernelCreateSema("thread_sema", 0, 0, 1, NULL);
}

ThreadBase::~ThreadBase()
{
    LogFunctionName;
    if (_thread_id >= 0)
    {
        Stop(true);
    }
    sceKernelDeleteLwMutex(&_mutex);
    sceKernelDeleteSema(_semaid);
}

bool ThreadBase::Start()
{
    LogFunctionName;
    switch (sizeof(this))
    {
    case 4:
    {
        uint32_t p = (uint32_t)this;
        return Start(&p, 4);
    }
    case 8:
    {
        uint64_t p = (uint64_t)this;
        return Start(&p, 8);
    }
    default:
        LogError("not the expected size");
        return false;
    }
}

bool ThreadBase::Start(void *data, SceSize size)
{
    LogFunctionName;

    _thread_id = sceKernelCreateThread(__PRETTY_FUNCTION__, _entry, _priority, _stack_size, 0, 0, NULL);
    if (_thread_id < 0)
    {
        LogError("failed to create thread: %s", __PRETTY_FUNCTION__);
        return false;
    }

    _keep_running = true;
    int result = sceKernelStartThread(_thread_id, size, data);
    if (result != SCE_OK)
    {
        LogError("failed to start thread: %s / %d", __PRETTY_FUNCTION__, result);
        sceKernelDeleteThread(_thread_id);
        _thread_id = -1;
        _keep_running = false;
        return false;
    }

    return true;
}

void ThreadBase::Stop(bool force)
{
    LogFunctionName;
    if (_thread_id == -1)
    {
        return;
    }

    Signal();
    _keep_running = false;
    if (force)
    {
        sceKernelDelayThread(10000);
    }
    else
    {
        sceKernelWaitThreadEnd(_thread_id, NULL, NULL);
    }
    sceKernelDeleteThread(_thread_id);
    _thread_id = -1;
}

void ThreadBase::Lock()
{
    sceKernelLockLwMutex(&_mutex, 1, NULL);
}

void ThreadBase::Unlock()
{
    sceKernelUnlockLwMutex(&_mutex, 1);
}

void ThreadBase::Wait()
{
    sceKernelWaitSema(_semaid, 1, NULL);
}

void ThreadBase::Signal()
{
    sceKernelSignalSema(_semaid, 1);
}
