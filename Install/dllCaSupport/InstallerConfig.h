#pragma once
#include "IPmConfig.h"

struct InstallerConfigData
{
    std::string eventUri;
    uint32_t log_level;
};

class InstallerConfig : public IPmConfig
{
public:
    InstallerConfig();
    ~InstallerConfig();

    int32_t LoadPmConfig( const std::string& pmConfig ) override;
    bool PmConfigFileChanged( const std::string& pmConfig ) override;
    int32_t VerifyPmFileIntegrity( const std::string& pmConfig ) override;
    std::string GetCloudCheckinUri() override;
    std::string GetCloudEventUri() override;
    std::string GetCloudCatalogUri() override;
    uint32_t GetCloudCheckinIntervalMs() override;
    uint32_t GetLogLevel() override;
    uint32_t GetMaxFileCacheAgeS() override;
    uint32_t GetMaxEventTtlS() override;
    bool AllowPostInstallReboots() override;
    uint32_t GetRebootThrottleS() override;
    uint32_t GetWatchdogTimeoutMs() override;
    uint32_t GetNetworkFailureRetryInterval() override;

    void SetCloudEventUri( std::string& uri );
private:
    InstallerConfigData m_configData;
    std::string m_returnAsRef;
};