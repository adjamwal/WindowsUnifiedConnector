#pragma once

#include "IPmConfig.h"
#include <filesystem>
#include <mutex>

#define PM_CONFIG_LOGLEVEL_DEFAULT 7
#define PM_CONFIG_INTERVAL_DEFAULT_MS ( 5 * 60 * 1000 )  // 5 minutes
#define PM_CONFIG_MAX_CACHE_AGE_DEFAULT_SECS ( 60 * 60 * 24 * 7) // One week
#define PM_CONFIG_REBOOT_THROTTLE_DEFAULT_SECS 3600 // One hour
#define PM_CONFIG_WATCHDOG_BUFFER_DEFAULT_MS ( 30 * 60 * 1000 ) // 30 minutes

class IFileSysUtil;

namespace Json
{
    class Value;
};

struct PmConfigData
{
    std::string identifyUri;
    std::string checkinUri;
    std::string eventUri;
    std::string catalogUri;
    uint32_t intervalMs;
    uint32_t maxDelayMs;
    uint32_t log_level;
    uint32_t maxFileCacheAge;
    bool allowPostInstallReboots;
    uint32_t rebootThrottleS;
    uint32_t watchdogTimeoutMs;
};

class PmConfig : public IPmConfig
{
public:
    PmConfig( IFileSysUtil& fileUtil );
    ~PmConfig();

    int32_t LoadBsConfig( const std::string& bsConfig ) override;
    int32_t LoadPmConfig( const std::string& pmConfig ) override;
    bool PmConfigFileChanged( const std::string& pmConfig ) override;
    int32_t VerifyBsFileIntegrity( const std::string& bsConfig ) override;
    int32_t VerifyPmFileIntegrity( const std::string& pmConfig ) override;
    const std::string& GetCloudIdentifyUri() override;
    const std::string& GetCloudCheckinUri() override;
    const std::string& GetCloudEventUri() override;
    const std::string& GetCloudCatalogUri() override;
    uint32_t GetCloudCheckinIntervalMs() override;
    uint32_t GetLogLevel() override;
    uint32_t GetMaxFileCacheAge() override;
    bool AllowPostInstallReboots() override;
    uint32_t GetRebootThrottleS() override;
    uint32_t GetWatchdogTimeoutMs() override;

private:
    IFileSysUtil& m_fileUtil;

    std::atomic<bool> m_isFirstCheckin;
    PmConfigData m_configData;
    std::mutex m_mutex;
    std::filesystem::file_time_type m_pmConfigFileTimestamp;

    int32_t ParseBsConfig( const std::string& bsConfig );
    int32_t ParsePmConfig( const std::string& pmConfig );

    int32_t VerifyBsContents( const std::string& bsConfig );
    int32_t VerifyPmContents( const std::string& pmConfig );
    bool VerifyPmLogLevel( const Json::Value& pmRoot );
    bool VerifyPmCheckinInterval( const Json::Value& pmRoot );
    bool VerifyPmMaxStartupDelay( const Json::Value& pmRoot );
    bool VerifyPmMaxFileCacheAge(const Json::Value& pmRoot);
    bool VerifyPmAllowPostInstallReboots( const Json::Value& pmRoot );
    bool VerifyPmRebootThrottle( const Json::Value& pmRoot );
    bool VerifyPmWatchdogBuffer( const Json::Value& pmRoot );
};
