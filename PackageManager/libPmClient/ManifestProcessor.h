#pragma once

#include "IManifestProcessor.h"
#include "IPmManifest.h"
#include "IComponentPackageProcessor.h"
#include <mutex>

class ManifestProcessor : public IManifestProcessor
{
public:
    ManifestProcessor( IPmManifest& manifest, IComponentPackageProcessor& componentProcessor );
    virtual ~ManifestProcessor();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool ProcessManifest( std::string checkinManifest ) override;

private:
    void PreDownloadAllFromManifest();
    void ProcessDownloadedPackagesAndConfigs();

    std::mutex m_mutex;
    IPmManifest& m_manifest;
    IComponentPackageProcessor& m_componentProcessor;
};
