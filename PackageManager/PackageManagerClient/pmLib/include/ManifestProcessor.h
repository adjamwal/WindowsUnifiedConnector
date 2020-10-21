#pragma once

#include "IManifestProcessor.h"
#include "IPmManifest.h"
#include "IComponentPackageProcessor.h"

class ManifestProcessor : public IManifestProcessor
{
public:
    ManifestProcessor( IPmManifest& manifest, IComponentPackageProcessor componentProcessor );
    virtual ~ManifestProcessor();

    bool ProcessManifest( std::string checkinManifest ) override;

private:
    IPmManifest& m_manifest;
    IComponentPackageProcessor& m_componentProcessor;
};
