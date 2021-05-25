#pragma once

#include "IInstallerCacheManager.h"
#include <mutex>
#include <filesystem>

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
    std::string DownloadOrUpdateInstaller( const PmComponent& componentPackage ) override;
    void DeleteInstaller( const std::string& installerPath ) override;
    void PruneInstallers( uint32_t ageInSeconds ) override;

private:
    bool ValidateInstaller( const PmComponent& componentPackage, const std::string& installerPath );
    std::string SanitizeComponentProductAndVersion( const std::string& productAndVersion );

    IPmCloud& m_pmCloud;
    IFileSysUtil& m_fileUtil;
    ISslUtil& m_sslUtil;
    std::mutex m_mutex;
    IPmPlatformComponentManager* m_componentMgr;

    const std::filesystem::path m_downloadPath;
};