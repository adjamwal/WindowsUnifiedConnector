#pragma once
#include "IPmConfig.h"
#include "PmTypes.h"

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
    const std::vector<PmComponent>& GetSupportedComponentList() override;
    uint32_t GetMaxFileCacheAge() override;
    bool AllowPostInstallReboots() override;
    uint32_t GetRebootThrottleS() override;

    void SetCloudEventUri( std::string& uri );
private:
    InstallerConfigData m_configData;
    std::vector<PmComponent> m_ComponentList;
};