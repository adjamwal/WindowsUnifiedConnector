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
    virtual bool ProcessComponentPackage( PmComponent& componentPackage ) = 0;
};
