#pragma once

#include "IPackageInventoryProvider.h"
#include "PmTypes.h"
#include <mutex>

class IFileSysUtil;
class ISslUtil;

class PackageInventoryProvider : public IPackageInventoryProvider
{
public:
    PackageInventoryProvider( IFileSysUtil& fileUtil, ISslUtil& sslUtil );
    virtual ~PackageInventoryProvider();

    void Initialize( IPmPlatformDependencies* dep ) override;
    bool GetInventory( PackageInventory& inventory ) override;
    void SetCatalogDataset( const std::vector<PmProductDiscoveryRules>& catalogRules ) override;

private:
    IFileSysUtil& m_fileUtil;
    ISslUtil& m_sslUtil;
    std::mutex m_mutex;
    std::vector<PmProductDiscoveryRules> m_catalogRules;

    IPmPlatformDependencies* m_dependencies;
};
