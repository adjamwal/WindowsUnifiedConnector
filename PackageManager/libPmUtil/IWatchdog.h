#pragma once

#include <functional>
#include <chrono>

class IWatchdog
{
public:
    IWatchdog() {}
    virtual ~IWatchdog() {}

    virtual void Start( std::function< std::chrono::milliseconds() >waitFunc, std::function<void()> timeoutFunc ) = 0;
    virtual void Stop() = 0;
    virtual void Kick() = 0;
};