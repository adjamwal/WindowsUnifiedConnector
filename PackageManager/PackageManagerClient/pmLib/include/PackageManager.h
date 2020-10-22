#pragma once

#include "IPackageManager.h"
#include <chrono>
#include <mutex>
#include <string>
#include <vector>

class IPmConfig;
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

class PackageManager : public IPackageManager
{
public:
    PackageManager( IPmConfig& config, 
        IPackageInventoryProvider& packageInventoryProvider,
        ICheckinFormatter& checkinFormatter,
        ITokenAdapter& tokenAdapter, 
        ICertsAdapter& certsAdapter,
        ICheckinManifestRetriever& manifestRetriever,
        IManifestProcessor& manifestProcessor, 
        IWorkerThread& thread );
    virtual ~PackageManager();

    int32_t Start( const char* configFile ) override;
    int32_t Stop() override;

    void SetPlatformDependencies( IPmPlatformDependencies* dependecies ) override;
    int32_t VerifyPacManConfig( const char* configFile ) override;

private:
    IPmConfig& m_config;
    IPackageInventoryProvider& m_packageInventoryProvider;
    ICheckinFormatter& m_checkinFormatter;
    ITokenAdapter& m_tokenAdapter;
    ICertsAdapter& m_certsAdapter;
    ICheckinManifestRetriever& m_manifestRetriever;
    IManifestProcessor& m_manifestProcessor;
    IWorkerThread& m_thread;
    std::mutex m_mutex;
    std::string m_configFilename;

    IPmPlatformDependencies* m_dependencies;
    std::vector<PmInstalledPackage> m_packages;

    void PmWorkflowThread();
    std::chrono::milliseconds PmThreadWait();
    bool PmLoadConfig();
    bool PmSendEvent( const PmEvent& event );
};
