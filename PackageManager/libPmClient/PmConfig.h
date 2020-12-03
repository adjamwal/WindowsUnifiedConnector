#pragma once

#include "IPmConfig.h"
#include <mutex>

#define PM_CONFIG_LOGLEVEL_DEFAULT 7
#define PM_CONFIG_INTERVAL_DEFAULT 300000

class IFileUtil;

struct PmConfigData
{
    std::string identifyUri;
    std::string checkinUri;
    uint32_t interval;
    uint32_t log_level;
};

class PmConfig : public IPmConfig
{
public:
    PmConfig( IFileUtil& fileUtil );
    ~PmConfig();

    int32_t LoadBsConfig( const std::string& bsConfig ) override;
    int32_t LoadPmConfig( const std::string& pmConfig ) override;
    int32_t VerifyBsFileIntegrity( const std::string& bsConfig ) override;
    int32_t VerifyPmFileIntegrity( const std::string& pmConfig ) override;
    const std::string& GetCloudIdentifyUri() override;
    const std::string& GetCloudCheckinUri() override;
    uint32_t GetCloudCheckinInterval() override;
    uint32_t GetLogLevel() override;
    const std::vector<PmComponent>& GetSupportedComponentList() override;

private:
    IFileUtil& m_fileUtil;

    PmConfigData m_configData;
    std::vector<PmComponent> m_ComponentList;
    std::mutex m_mutex;

    int32_t ParseBsConfig( const std::string& bsConfig );
    int32_t ParsePmConfig( const std::string& pmConfig );

    int32_t VerifyBsContents( const std::string& bsConfig );
    int32_t VerifyPmContents( const std::string& pmConfig );
};