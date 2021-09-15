#pragma once

#include <string>
#include <vector>

#define EVENT_URL_VERSION "/1"

class IPmConfig
{
public:
    IPmConfig() {}
    virtual  ~IPmConfig() {}

    virtual int32_t LoadPmConfig( const std::string& pmConfig ) = 0;
    virtual bool PmConfigFileChanged( const std::string& pmConfig ) = 0;
    virtual int32_t VerifyPmFileIntegrity( const std::string& pmConfig ) = 0;
    virtual std::string GetCloudCheckinUri() = 0;
    virtual std::string GetCloudEventUri() = 0;
    virtual std::string GetCloudCatalogUri() = 0;
    virtual uint32_t GetCloudCheckinIntervalMs() = 0;
    virtual uint32_t GetLogLevel() = 0;
    virtual uint32_t GetMaxFileCacheAgeS() = 0;
    virtual uint32_t GetMaxEventTtlS() = 0;
    virtual bool AllowPostInstallReboots() = 0;
    virtual uint32_t GetRebootThrottleS() = 0;
    virtual uint32_t GetWatchdogTimeoutMs() = 0;
    virtual uint32_t GetNetworkFailureRetryInterval() = 0;
};
