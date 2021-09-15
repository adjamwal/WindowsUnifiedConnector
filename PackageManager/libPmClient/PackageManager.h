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
class ICatalogJsonParser;
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
class IWatchdog;

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
        ICatalogJsonParser& catalogJsonParser,
        IUcidAdapter& ucidAdapter, 
        ICertsAdapter& certsAdapter,
        ICheckinManifestRetriever& manifestRetriever,
        IManifestProcessor& manifestProcessor, 
        ICloudEventPublisher& cloudEventPublisher,
        ICloudEventStorage& cloudEventStorage,
        IUcUpgradeEventHandler& ucUpgradeEventHandler,
        IRebootHandler& rebootHandler,
        IWorkerThread& thread,
        IWatchdog& watchdog );
    virtual ~PackageManager();

    int32_t Start( const char* pmConfigFile ) override;
    int32_t Stop() override;
    bool IsRunning() override;
    void SetPlatformDependencies( IPmPlatformDependencies* dependecies ) override;
    int32_t VerifyPmConfig( const char* pmConfigFile ) override;

private:
    IPmConfig& m_config;
    IPmCloud& m_cloud;
    IInstallerCacheManager& m_installerCacheMgr;
    IPackageDiscoveryManager& m_packageDiscoveryManager;
    ICheckinFormatter& m_checkinFormatter;
    ICatalogJsonParser& m_catalogJsonParser;
    IUcidAdapter& m_ucidAdapter;
    ICertsAdapter& m_certsAdapter;
    ICheckinManifestRetriever& m_manifestRetriever;
    IManifestProcessor& m_manifestProcessor;
    ICloudEventPublisher& m_cloudEventPublisher;
    ICloudEventStorage& m_cloudEventStorage;
    IUcUpgradeEventHandler& m_ucUpgradeEventHandler;
    IRebootHandler& m_rebootHandler;
    IWorkerThread& m_thread;
    IWatchdog& m_watchdog;

    std::mutex m_mutex;
    std::string m_pmConfigFile;

    bool m_useShorterInterval = false;

    IPmPlatformDependencies* m_dependencies;

    void PmWorkflowThread();
    std::chrono::milliseconds PmThreadWait();
    bool LoadPmConfig();
    std::chrono::milliseconds PmWatchdogWait();
    void PmWatchdogFired();
};
