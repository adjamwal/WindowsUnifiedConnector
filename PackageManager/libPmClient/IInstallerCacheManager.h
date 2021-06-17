#pragma once
#pragma once

#include <string>
#include <filesystem>

class IPmPlatformDependencies;
struct PmComponent;

class IInstallerCacheManager
{
public:
    virtual ~IInstallerCacheManager() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual std::filesystem::path DownloadOrUpdateInstaller( const PmComponent& componentPackage ) = 0;
    virtual void DeleteInstaller( const std::filesystem::path& installerPath ) = 0;
    virtual void PruneInstallers( uint32_t ageInSeconds ) = 0;
};