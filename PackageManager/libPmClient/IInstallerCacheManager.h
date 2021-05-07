#pragma once
#pragma once

#include <string>

class IPmPlatformDependencies;
struct PmComponent;

class IInstallerCacheManager
{
public:
    virtual ~IInstallerCacheManager() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual std::string DownloadOrUpdateInstaller( const PmComponent& componentPackage ) = 0;
    virtual void DeleteInstaller( const std::string& installerPath ) = 0;
    virtual void PruneInstallers( uint32_t ageInSeconds ) = 0;
};