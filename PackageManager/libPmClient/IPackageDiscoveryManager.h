#pragma once

struct PackageInventory;

class IPmPlatformDependencies;

class IPackageDiscoveryManager
{
public:
    IPackageDiscoveryManager() {}
    virtual ~IPackageDiscoveryManager() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual bool DiscoverPackages( PackageInventory& inventory ) = 0;
};