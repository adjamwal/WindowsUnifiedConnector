#pragma once

#include "IRebootHandler.h"
#include <time.h>
#include <mutex>

class IPmConfig;

class RebootHandler : public IRebootHandler
{
public:
    RebootHandler( IPmConfig& config );
    ~RebootHandler();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool HandleReboot( bool rebootRequired ) override;

private:
    IPmConfig& m_config;
    IPmPlatformDependencies* m_dependencies;
    std::mutex m_mutex;

    bool m_rebootRequired;
    time_t m_lastRebootNotification;

    bool IsRebootThrottled();
};