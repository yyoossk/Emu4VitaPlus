#include "thread_base.h"
#include "log.h"

ThreadBase::ThreadBase(SceKernelThreadEntry entry, int priority, int stack_size)
    : _entry(entry),
      _priority(priority),
      _stack_size(stack_size),
      _thread_id(-1)
{
    LogFunctionName;
    sceKernelCreateLwMutex(&_mutex, "thread_mutex", 0, 0, NULL);
    sceKernelCreateLwCond(&_cond, "thread_cond", 0, &_mutex, NULL);
}

ThreadBase::~ThreadBase()
{
    LogFunctionName;
    if (_thread_id >= 0)
    {
        Stop(true);
    }
    sceKernelDeleteLwMutex(&_mutex);
    sceKernelDeleteLwCond(&_cond);
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
    sceKernelWaitLwCond(&_cond, NULL);
}

void ThreadBase::Signal()
{
    sceKernelSignalLwCond(&_cond);
}