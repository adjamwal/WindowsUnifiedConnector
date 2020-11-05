#pragma once

struct PackageConfigInfo;
class IPmPlatformDependencies;

class IPackageConfigProcessor
{
public:
    IPackageConfigProcessor() {}
    virtual ~IPackageConfigProcessor() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual bool ProcessConfig( PackageConfigInfo& config ) = 0;
};