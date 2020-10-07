#pragma once

#include "IWorkerThread.h"
#include <thread>
#include <atomic>
#include <mutex>

#if defined( _WIN32 )
#include <Windows.h>
#else
#include <condition_variable>
#endif

class WorkerThread : public IWorkerThread
{
public:
    WorkerThread();
    ~WorkerThread();

    void Start( std::function< std::chrono::milliseconds() >waitFunc, std::function<void()> workFunc ) override;
    void Stop() override;
    bool IsRunning() override;

private:
    std::atomic_bool m_enabled;
#if defined( _WIN32 )
    CRITICAL_SECTION m_waitLock;
    CONDITION_VARIABLE m_threadCond;
#else
    std::mutex m_waitLock;
    std::condition_variable m_threadCond;
#endif
    std::thread m_thread;
    std::mutex m_mutex;
};
