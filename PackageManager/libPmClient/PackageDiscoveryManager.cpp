#include "PackageDiscoveryManager.h"
#include "PmCloud.h"
#include "IPackageInventoryProvider.h"
#include "CatalogJsonParser.h"
#include "PmLogger.h"
#include <mutex>
#include <sstream>

PackageDiscoveryManager::PackageDiscoveryManager( 
    ICatalogListRetriever& catalogListRetriever,
    IPackageInventoryProvider& packageInventoryProvider,
    ICatalogJsonParser& catalogJsonParser )
    : m_catalogListRetriever( catalogListRetriever )
    , m_packageInventoryProvider( packageInventoryProvider )
    , m_catalogJsonParser( catalogJsonParser )
{
}

PackageDiscoveryManager::~PackageDiscoveryManager()
{
}

void PackageDiscoveryManager::Initialize( IPmPlatformDependencies* dep )
{
    m_packageInventoryProvider.Initialize( dep );
}

std::vector<PmProductDiscoveryRules> PackageDiscoveryManager::PrepareCatalogDataset()
{
    std::string catalogList = m_catalogListRetriever.GetCloudCatalog();
    LOG_DEBUG( "Retrieved Catalog: %s", catalogList.c_str() );

    std::vector<PmProductDiscoveryRules> catalogProductRules;
    m_catalogJsonParser.Parse( catalogList, catalogProductRules );

    return catalogProductRules;
}

bool PackageDiscoveryManager::DiscoverPackages( std::vector<PmProductDiscoveryRules> catalogProductRules, PackageInventory& inventory )
{
    m_packageInventoryProvider.SetCatalogDataset( catalogProductRules );

    return m_packageInventoryProvider.GetInventory( inventory );
}
