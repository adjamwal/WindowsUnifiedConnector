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
class ITokenAdapter;
class ICertsAdapter;
class ICheckinManifestRetriever;
class IManifestProcessor;
class IWorkerThread;

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
        ITokenAdapter& tokenAdapter, 
        ICertsAdapter& certsAdapter,
        ICheckinManifestRetriever& manifestRetriever,
        IManifestProcessor& manifestProcessor, 
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
    ITokenAdapter& m_tokenAdapter;
    ICertsAdapter& m_certsAdapter;
    ICheckinManifestRetriever& m_manifestRetriever;
    IManifestProcessor& m_manifestProcessor;
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
