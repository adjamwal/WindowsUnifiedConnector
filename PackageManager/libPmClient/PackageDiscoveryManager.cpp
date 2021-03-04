#include "PackageDiscoveryManager.h"
#include "PmCloud.h"
#include "IPackageInventoryProvider.h"
#include "IUcLogger.h"
#include <mutex>
#include <sstream>

PackageDiscoveryManager::PackageDiscoveryManager( 
    ICatalogListRetriever& catalogListRetriever,
    IPackageInventoryProvider& packageInventoryProvider ) 
    : m_catalogListRetriever( catalogListRetriever )
    , m_packageInventoryProvider( packageInventoryProvider )
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
    std::lock_guard<std::mutex> lock( m_mutex );
    PrepareCatalogDataset();
    return m_packageInventoryProvider.GetInventory( inventory );
}

void PackageDiscoveryManager::PrepareCatalogDataset()
{
    std::string catalogList = m_catalogListRetriever.GetCloudCatalog();
    LOG_DEBUG( "Retrieved Catalog: %s", catalogList.c_str() );

    m_catalogDataset.clear();
    PmDiscoveryComponent catalogEntry;

    catalogEntry.packageId = "uc";
    catalogEntry.packageName = "Cisco Unified Connector";
    m_catalogDataset.push_back( catalogEntry );

    catalogEntry.packageId = "Immunet";
    catalogEntry.packageName = "Immunet";
    m_catalogDataset.push_back( catalogEntry );

    for( uint32_t i = 0; i < 10; i++ ) {
        std::stringstream ss;
        ss << "test-package-" << i + 1;
        catalogEntry.packageId = ss.str();
        catalogEntry.packageName = "TestPackage";
        m_catalogDataset.push_back( catalogEntry );
    }

    m_packageInventoryProvider.SetCatalogDataset( m_catalogDataset );
}
