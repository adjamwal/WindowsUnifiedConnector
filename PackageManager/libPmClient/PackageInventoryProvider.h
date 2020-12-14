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
    void SetDiscoveryList( const std::vector<PmDiscoveryComponent>& discoveryList ) override;

private:
    IFileUtil& m_fileUtil;
    ISslUtil& m_sslUtil;
    std::mutex m_mutex;
    std::vector<PmDiscoveryComponent> m_discoveryList;

    IPmPlatformDependencies* m_dependencies;
};
