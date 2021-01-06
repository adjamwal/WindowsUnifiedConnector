#pragma once

#include <vector>
#include "IPackageDiscoveryManager.h"
#include "PmTypes.h"

class IPackageInventoryProvider;

class PackageDiscoveryManager : public IPackageDiscoveryManager
{
public:
    PackageDiscoveryManager( IPackageInventoryProvider& packageInventoryProvider );
    ~PackageDiscoveryManager();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool DiscoverPackages( PackageInventory& inventory ) override;

private:
    IPackageInventoryProvider& m_packageInventoryProvider;
    std::vector<PmDiscoveryComponent> m_discoveryList;
    
    void SetupDiscoveryPackages();
};