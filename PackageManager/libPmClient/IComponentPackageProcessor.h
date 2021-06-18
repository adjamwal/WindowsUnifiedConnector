#pragma once

#include <string>

class IPmPlatformDependencies;

struct PmComponent;

class IComponentPackageProcessor
{
public:
    IComponentPackageProcessor() {}
    virtual ~IComponentPackageProcessor() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;

    virtual bool PreDownloadedBinaryExists( PmComponent& componentPackage ) = 0;
    virtual bool HasConfigs( PmComponent& componentPackage ) = 0;

    virtual bool DownloadPackageBinary( PmComponent& componentPackage ) = 0;

    virtual bool ProcessPackageBinary( PmComponent& componentPackage ) = 0;
    virtual bool ProcessConfigsForPackage( PmComponent& componentPackage ) = 0;
};
