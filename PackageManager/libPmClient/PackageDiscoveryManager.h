#pragma once

#include <mutex>
#include <vector>
#include "ICatalogListRetriever.h"
#include "IPackageDiscoveryManager.h"
#include "PmTypes.h"

class ICatalogListRetriever;
class IPackageInventoryProvider;
class ICatalogJsonParser;

class PackageDiscoveryManager : public IPackageDiscoveryManager
{
public:
    PackageDiscoveryManager( 
        ICatalogListRetriever& catalogListRetriever, 
        IPackageInventoryProvider& packageInventoryProvider,
        ICatalogJsonParser& catalogJsonParser );
    ~PackageDiscoveryManager();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool DiscoverPackages( PackageInventory& inventory ) override;

private:
    ICatalogListRetriever& m_catalogListRetriever;
    IPackageInventoryProvider& m_packageInventoryProvider;
    ICatalogJsonParser& m_catalogJsonParser;
    std::mutex m_mutex;

    void PrepareCatalogDataset();
};
