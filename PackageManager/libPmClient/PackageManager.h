#pragma once

#include "IPackageManager.h"
#include <chrono>
#include <mutex>
#include <string>
#include <vector>

class IPmConfig;
class IPmCloud;
class IPackageInventoryProvider;
class ICheckinFormatter;
class IUcidAdapter;
class ICertsAdapter;
class ICheckinManifestRetriever;
class IManifestProcessor;
class IWorkerThread;
class ICloudEventPublisher;
class ICloudEventStorage;
class IUcUpgradeEventHandler;

struct PmComponent;
struct PmEvent;
struct PmInstalledPackage;
struct PmDiscoveryComponent;

class PackageManager : public IPackageManager
{
public:
    PackageManager( IPmConfig& config,
        IPmCloud& cloud,
        IPackageInventoryProvider& packageInventoryProvider,
        ICheckinFormatter& checkinFormatter,
        IUcidAdapter& ucidAdapter, 
        ICertsAdapter& certsAdapter,
        ICheckinManifestRetriever& manifestRetriever,
        IManifestProcessor& manifestProcessor, 
        ICloudEventPublisher& cloudEventPublisher,
        ICloudEventStorage& cloudEventStorage,
        IUcUpgradeEventHandler& ucUpgradeEventHandler,
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
    IPackageInventoryProvider& m_packageInventoryProvider;
    ICheckinFormatter& m_checkinFormatter;
    IUcidAdapter& m_ucidAdapter;
    ICertsAdapter& m_certsAdapter;
    ICheckinManifestRetriever& m_manifestRetriever;
    IManifestProcessor& m_manifestProcessor;
    ICloudEventPublisher& m_cloudEventPublisher;
    ICloudEventStorage& m_cloudEventStorage;
    IUcUpgradeEventHandler& m_ucUpgradeEventHandler;
    IWorkerThread& m_thread;
    std::mutex m_mutex;
    std::string m_bsConfigFile;
    std::string m_pmConfigFile;

    IPmPlatformDependencies* m_dependencies;
    std::vector<PmInstalledPackage> m_packages;
    std::vector<PmDiscoveryComponent> m_discoveryList;

    void PmWorkflowThread();
    std::chrono::milliseconds PmThreadWait();
    bool LoadBsConfig();
    bool LoadPmConfig();
    bool PmSendEvent( const PmEvent& event );
    void SetupDiscoveryPackages();
};
