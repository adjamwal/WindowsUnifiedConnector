#pragma once

#include <string>
#include <vector>

class IPmPlatformConfiguration;
struct PmComponent;

class IPmConfig
{
public:
    IPmConfig() {}
    virtual  ~IPmConfig() {}

    virtual int32_t LoadBsConfig( const std::string& bsConfig ) = 0;
    virtual int32_t LoadPmConfig( const std::string& pmConfig ) = 0;
    virtual int32_t VerifyBsFileIntegrity( const std::string& bsConfig ) = 0;
    virtual int32_t VerifyPmFileIntegrity( const std::string& pmConfig ) = 0;
    virtual const std::string& GetCloudIdentifyUri() = 0;
    virtual const std::string& GetCloudCheckinUri() = 0;
    virtual const std::string& GetCloudEventUri() = 0;
    virtual const std::string& GetCloudCatalogUri() = 0;
    virtual uint32_t GetCloudCheckinInterval() = 0;
    virtual uint32_t GetLogLevel() = 0;
    virtual const std::vector<PmComponent>& GetSupportedComponentList() = 0;
    virtual uint32_t GetMaxFileCacheAge() = 0;
};