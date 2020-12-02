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
    virtual const std::string& GetCloudUri() = 0;
    virtual uint32_t GetCloudInterval() = 0;
    virtual uint32_t GetLogLevel() = 0;
    virtual const std::vector<PmComponent>& GetSupportedComponentList() = 0;
};