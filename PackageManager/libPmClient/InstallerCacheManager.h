#pragma once

#include "IInstallerCacheManager.h"
#include <mutex>

class IPmCloud;
class IFileSysUtil;
class ISslUtil;
class IPmPlatformComponentManager;

class InstallerCacheManager : public IInstallerCacheManager
{
public:
    InstallerCacheManager( IPmCloud& pmCloud, IFileSysUtil& fileUtil, ISslUtil& sslUtil );
    ~InstallerCacheManager();

    void Initialize( IPmPlatformDependencies* dep ) override;
    std::filesystem::path DownloadOrUpdateInstaller( const PmComponent& componentPackage ) override;
    void DeleteInstaller( const std::filesystem::path& installerPath ) override;
    void PruneInstallers( uint32_t ageInSeconds ) override;

private:
    bool ValidateInstaller( const PmComponent& componentPackage, const std::filesystem::path& installerPath );
    std::string SanitizeComponentProductAndVersion( const std::string& productAndVersion );

    IPmCloud& m_pmCloud;
    IFileSysUtil& m_fileUtil;
    ISslUtil& m_sslUtil;
    std::mutex m_mutex;
    IPmPlatformComponentManager* m_componentMgr;

    std::filesystem::path m_tempDownloadPath;
};