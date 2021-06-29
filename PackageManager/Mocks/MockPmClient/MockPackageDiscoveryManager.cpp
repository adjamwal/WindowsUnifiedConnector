#include "MocksCommon.h"
#include "MockPackageDiscoveryManager.h"

MockPackageDiscoveryManager::MockPackageDiscoveryManager()
{
}

MockPackageDiscoveryManager::~MockPackageDiscoveryManager()
{
}

void MockPackageDiscoveryManager::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockPackageDiscoveryManager::MakeDiscoverPackagesReturn( bool value )
{
    ON_CALL( *this, DiscoverPackages( _ ) ).WillByDefault( Return( value ) );
}

void MockPackageDiscoveryManager::ExpectDiscoverPackagesIsNotCalled()
{
    EXPECT_CALL( *this, DiscoverPackages( _ ) ).Times( 0 );
}