#pragma once

#include "IPackageInventoryProvider.h"
#include "PmTypes.h"
#include <mutex>

class IFileUtil;
class ISslUtil;

class PackageInventoryProvider : public IPackageInventoryProvider
{
public:
    PackageInventoryProvider( IFileUtil& fileUtil, ISslUtil& sslUtil );
    virtual ~PackageInventoryProvider();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool GetInventory( PackageInventory& inventory ) override;
    void SetCatalogDataset( 
        const std::vector<PmDiscoveryComponent>& discoveryList,
        const std::vector<PmProductDiscoveryRules>& catalogRules ) override;

private:
    IFileUtil& m_fileUtil;
    ISslUtil& m_sslUtil;
    std::mutex m_mutex;
    std::vector<PmDiscoveryComponent> m_catalogDataset;
    std::vector<PmProductDiscoveryRules> m_catalogRules;

    IPmPlatformDependencies* m_dependencies;
};
