#pragma once

#include "IPackageManager.h"
#include <chrono>
#include <mutex>
#include <string>
#include <vector>

class IPmConfig;
class IPmCloud;
class IPmManifest;
class IWorkerThread;

struct PmComponent;
struct PmEvent;
struct PmInstalledPackage;

class PackageManager : public IPackageManager
{
public:
    PackageManager( IPmConfig& config, IPmCloud& cloud, IPmManifest& manifest, IWorkerThread& thread );
    virtual ~PackageManager();

    int32_t Start( const char* configFile ) override;
    int32_t Stop() override;

    void SetPlatformDependencies( IPmPlatformDependencies* dependecies ) override;
    int32_t VerifyPacManConfig( const char* configFile ) override;

private:
    IPmConfig& m_config;
    IPmCloud& m_cloud;
    IPmManifest& m_manifest;
    IWorkerThread& m_thread;
    std::mutex m_mutex;
    std::string m_configFilename;

    IPmPlatformDependencies* m_dependencies;
    std::vector<PmInstalledPackage> m_packages;

    void PmWorkflowThread();
    std::chrono::milliseconds PmThreadWait();
    bool PmLoadConfig();
    bool PmLoadPackageList();
    bool PmGetUCIDId();
    bool PmCheckin();
    bool PmProcessComponent( const PmComponent& component );
    bool PmProcessManifest( const std::string& manifest);
    bool PmSendEvent( const PmEvent& event );
};
