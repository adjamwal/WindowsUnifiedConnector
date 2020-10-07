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

    virtual int32_t Load( const std::string& filename ) = 0;
    virtual int32_t VerifyFileIntegrity( const std::string& filename ) = 0;
    virtual const std::string& GetCloudUri() = 0;
    virtual uint32_t GetCloudInterval() = 0;
    virtual const std::vector<PmComponent>& GetSupportedComponentList() = 0;
};