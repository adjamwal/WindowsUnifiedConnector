#pragma once

#include "PmTypes.h"

struct PackageInventory;

class IPmPlatformDependencies;

class IPackageDiscoveryManager
{
public:
    IPackageDiscoveryManager() {}
    virtual ~IPackageDiscoveryManager() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;

    virtual std::vector<PmProductDiscoveryRules> PrepareCatalogDataset() = 0;
    virtual bool DiscoverPackages( std::vector<PmProductDiscoveryRules> catalogProductRules, PackageInventory& inventory ) = 0;
};
