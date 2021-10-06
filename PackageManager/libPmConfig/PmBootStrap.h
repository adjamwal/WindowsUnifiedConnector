#pragma once

#include "IPmBootstrap.h"
#include <mutex>

class IFileSysUtil;

class PmBootstrap : public IPmBootstrap
{
public:
    PmBootstrap( IFileSysUtil& fileUtil );
    ~PmBootstrap();

    int32_t LoadPmBootstrap( const std::string& filename ) override;
    std::string GetIdentifyUri() override;

private:
    IFileSysUtil& m_fileUtil;

    std::mutex m_mutex;
    std::string m_idenitfyUri;

    int32_t ParseBootstrapConfig( const std::string& bsConfig );
};
