#pragma once

#include <string>
#include <vector>

class IPmPlatformDependencies;

struct PackageConfigInfo;
struct PackageInventory;
struct PmDiscoveryComponent;
struct PmProductDiscoveryRules;

class IPackageInventoryProvider
{
public:
    IPackageInventoryProvider() {}
    virtual ~IPackageInventoryProvider() {}

    virtual void Initialize( IPmPlatformDependencies* dep ) = 0;
    virtual bool GetInventory( PackageInventory& inventory) = 0;
    virtual void SetCatalogDataset( 
        const std::vector<PmDiscoveryComponent>& discoveryList,
        const std::vector<PmProductDiscoveryRules>& catalogRules ) = 0;
};
