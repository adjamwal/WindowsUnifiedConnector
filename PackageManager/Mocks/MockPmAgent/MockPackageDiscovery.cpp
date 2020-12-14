#include "framework.h"
#include "MockPackageDiscovery.h"
#include "MocksCommon.h"

MockPackageDiscovery::MockPackageDiscovery()
{
    MakeGetInstalledPackagesReturn( m_inventory );
}

MockPackageDiscovery::~MockPackageDiscovery()
{

}

void MockPackageDiscovery::MakeGetInstalledPackagesReturn( PackageInventory value )
{
    ON_CALL( *this, GetInstalledPackages( _ ) ).WillByDefault( Return( value ) );
}

void MockPackageDiscovery::ExpectGetInstalledPackagesIsNotCalled()
{
    EXPECT_CALL( *this, GetInstalledPackages( _ ) ).Times( 0 );
}