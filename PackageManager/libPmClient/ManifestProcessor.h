#pragma once

#include "IManifestProcessor.h"
#include "IPmManifest.h"
#include "IComponentPackageProcessor.h"
#include <mutex>
#include <vector>

class ManifestProcessor : public IManifestProcessor
{
public:
    ManifestProcessor( IPmManifest& manifest, IComponentPackageProcessor& componentProcessor );
    virtual ~ManifestProcessor();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool ProcessManifest( std::string checkinManifest, bool& isRebootRequired ) override;

private:
    void PreDownloadAllFromManifest( std::vector<PmComponent>& packages );
    void ProcessDownloadedPackagesAndConfigs( std::vector<PmComponent>& packages, bool& isRebootRequired );

    std::mutex m_mutex;
    IPmManifest& m_manifest;
    IComponentPackageProcessor& m_componentProcessor;
};
