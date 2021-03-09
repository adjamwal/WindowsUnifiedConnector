#include "PackageDiscoveryManager.h"
#include "PmCloud.h"
#include "IPackageInventoryProvider.h"
#include "CatalogJsonParser.h"
#include "IUcLogger.h"
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

bool PackageDiscoveryManager::DiscoverPackages( PackageInventory& inventory )
{
    PrepareCatalogDataset();
    return m_packageInventoryProvider.GetInventory( inventory );
}

void PackageDiscoveryManager::PrepareCatalogDataset()
{
    std::string catalogList = m_catalogListRetriever.GetCloudCatalog();
    LOG_DEBUG( "Retrieved Catalog: %s", catalogList.c_str() );

    m_catalogJsonParser.Parse( catalogList, m_catalogDataset );
    
    m_packageInventoryProvider.SetCatalogDataset( m_catalogDataset );
}
