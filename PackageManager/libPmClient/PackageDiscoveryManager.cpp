#include "PackageDiscoveryManager.h"
#include "IPackageInventoryProvider.h"
#include <sstream>

PackageDiscoveryManager::PackageDiscoveryManager( IPackageInventoryProvider& packageInventoryProvider ) :
    m_packageInventoryProvider( packageInventoryProvider )
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
    SetupDiscoveryPackages();
    return m_packageInventoryProvider.GetInventory( inventory );
}

// TODO: This will need to be re-written in enterprise to fetch this from somewhere
void PackageDiscoveryManager::SetupDiscoveryPackages()
{
    PmDiscoveryComponent discoveryItem;
    discoveryItem.packageId = "amp";
    discoveryItem.packageName = "Immunet";
    m_discoveryList.push_back( discoveryItem );

    discoveryItem.packageId = "amp";
    discoveryItem.packageName = "Cisco AMP for Endpoints Connector";
    m_discoveryList.push_back( discoveryItem );

    for ( uint32_t i = 0; i < 10; i++ ) {
        std::stringstream ss;
        ss << "test-package-" << i + 1;
        discoveryItem.packageId = ss.str();
        discoveryItem.packageName = "TestPackage";
        m_discoveryList.push_back( discoveryItem );
    }

    m_packageInventoryProvider.SetDiscoveryList( m_discoveryList );
}
