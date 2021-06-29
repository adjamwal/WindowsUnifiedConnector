#include "pch.h"
#include "MockPackageDiscovery.h"


MockPackageDiscovery::MockPackageDiscovery()
{
    MakeDiscoverInstalledPackagesReturn( m_detectedPackages );
    MakeCachedInventoryReturn( m_cachedInventory );
}

MockPackageDiscovery::~MockPackageDiscovery()
{

}

void MockPackageDiscovery::MakeDiscoverInstalledPackagesReturn( PackageInventory value )
{
    m_detectedPackages = value;
    ON_CALL( *this, DiscoverInstalledPackages( _ ) ).WillByDefault( Return( m_detectedPackages ) );
}

void MockPackageDiscovery::ExpectDiscoverInstalledPackagesIsNotCalled()
{
    EXPECT_CALL( *this, DiscoverInstalledPackages( _ ) ).Times( 0 );
}

void MockPackageDiscovery::MakeCachedInventoryReturn( PackageInventory value )
{
    m_cachedInventory = value;
    ON_CALL( *this, CachedInventory() ).WillByDefault( Return( m_cachedInventory ) );
}

void MockPackageDiscovery::ExpectCachedInventoryIsNotCalled()
{
    EXPECT_CALL( *this, CachedInventory() ).Times( 0 );
}
