#pragma once

#include <mutex>
#include <vector>
#include "ICatalogListRetriever.h"
#include "IPackageDiscoveryManager.h"
#include "PmTypes.h"

class ICatalogListRetriever;
class IPackageInventoryProvider;

class PackageDiscoveryManager : public IPackageDiscoveryManager
{
public:
    PackageDiscoveryManager( ICatalogListRetriever& catalogListRetriever, IPackageInventoryProvider& packageInventoryProvider );
    ~PackageDiscoveryManager();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool DiscoverPackages( PackageInventory& inventory ) override;

private:
    std::mutex m_mutex;
    ICatalogListRetriever& m_catalogListRetriever;
    IPackageInventoryProvider& m_packageInventoryProvider;
    std::vector<PmDiscoveryComponent> m_catalogDataset;
    
    void PrepareCatalogDataset();
};
