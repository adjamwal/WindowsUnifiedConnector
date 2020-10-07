#pragma once

#include "IPmConfig.h"
#include <mutex>

class IFileUtil;

struct PmConfigData
{
    std::string cloudUri;
    uint32_t interval;
};

class PmConfig : public IPmConfig
{
public:
    PmConfig( IFileUtil& fileUtil );
    ~PmConfig();

    int32_t Load( const std::string& filename ) override;
    int32_t VerifyFileIntegrity( const std::string& filename ) override;
    const std::string& GetCloudUri() override;
    uint32_t GetCloudInterval() override;
    const std::vector<PmComponent>& GetSupportedComponentList() override;

private:
    IFileUtil& m_fileUtil;

    PmConfigData m_configData;
    std::vector<PmComponent> m_ComponentList;
    std::mutex m_mutex;

    int32_t ParseConfig( const std::string& config );
    int32_t VerifyContents( const std::string& config );
};