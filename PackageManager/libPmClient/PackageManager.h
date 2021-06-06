#pragma once

#include "IPackageManager.h"
#include <chrono>
#include <mutex>
#include <string>
#include <vector>

class IPmConfig;
class IPmCloud;
class IPackageDiscoveryManager;
class ICheckinFormatter;
class IUcidAdapter;
class ICertsAdapter;
class ICheckinManifestRetriever;
class IManifestProcessor;
class IWorkerThread;
class ICloudEventPublisher;
class ICloudEventStorage;
class IUcUpgradeEventHandler;
class IInstallerCacheManager;
class IRebootHandler;

struct PmComponent;
struct PmEvent;
struct PmInstalledPackage;
struct PmDiscoveryComponent;

class PackageManager : public IPackageManager
{
public:
    PackageManager( IPmConfig& config,
        IPmCloud& cloud,
        IInstallerCacheManager& installerCacheMgr,
        IPackageDiscoveryManager& packageDiscoveryManager,
        ICheckinFormatter& checkinFormatter,
        IUcidAdapter& ucidAdapter, 
        ICertsAdapter& certsAdapter,
        ICheckinManifestRetriever& manifestRetriever,
        IManifestProcessor& manifestProcessor, 
        ICloudEventPublisher& cloudEventPublisher,
        ICloudEventStorage& cloudEventStorage,
        IUcUpgradeEventHandler& ucUpgradeEventHandler,
        IRebootHandler& rebootHandler,
        IWorkerThread& thread );
    virtual ~PackageManager();

    int32_t Start( const char* bsConfigFile, const char* pmConfigFile ) override;
    int32_t Stop() override;
    bool IsRunning() override;
    void SetPlatformDependencies( IPmPlatformDependencies* dependecies ) override;
    int32_t VerifyBsConfig( const char* bsConfigFile ) override;
    int32_t VerifyPmConfig( const char* pmConfigFile ) override;

private:
    IPmConfig& m_config;
    IPmCloud& m_cloud;
    IInstallerCacheManager& m_installerCacheMgr;
    IPackageDiscoveryManager& m_packageDiscoveryManager;
    ICheckinFormatter& m_checkinFormatter;
    IUcidAdapter& m_ucidAdapter;
    ICertsAdapter& m_certsAdapter;
    ICheckinManifestRetriever& m_manifestRetriever;
    IManifestProcessor& m_manifestProcessor;
    ICloudEventPublisher& m_cloudEventPublisher;
    ICloudEventStorage& m_cloudEventStorage;
    IUcUpgradeEventHandler& m_ucUpgradeEventHandler;
    IRebootHandler& m_rebootHandler;
    IWorkerThread& m_thread;
    std::mutex m_mutex;
    std::string m_bsConfigFile;
    std::string m_pmConfigFile;

    IPmPlatformDependencies* m_dependencies;

    void PmWorkflowThread();
    std::chrono::milliseconds PmThreadWait();
    bool LoadBsConfig();
    bool LoadPmConfig();
    bool PmSendEvent( const PmEvent& event );
};
