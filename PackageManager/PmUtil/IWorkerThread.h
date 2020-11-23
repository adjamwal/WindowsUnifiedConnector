#pragma once

#include <functional>
#include <chrono>

class IWorkerThread
{
public:
    IWorkerThread() {}
    virtual ~IWorkerThread() {}

    virtual void Start( std::function< std::chrono::milliseconds() >waitFunc, std::function<void()> workFunc ) = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() = 0;
};