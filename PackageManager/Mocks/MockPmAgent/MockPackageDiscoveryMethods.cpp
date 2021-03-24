#include "framework.h"
#include "MockPackageDiscoveryMethods.h"
#include "MocksCommon.h"

MockPackageDiscoveryMethods::MockPackageDiscoveryMethods()
{
}

MockPackageDiscoveryMethods::~MockPackageDiscoveryMethods()
{
}

void MockPackageDiscoveryMethods::ExpectDiscoverByMsiIsNotCalled()
{
    EXPECT_CALL( *this, DiscoverByMsi( _, _, _ ) ).Times( 0 );
}

void MockPackageDiscoveryMethods::ExpectDiscoverByRegistryIsNotCalled()
{
    EXPECT_CALL( *this, DiscoverByRegistry( _, _, _ ) ).Times( 0 );
}

void MockPackageDiscoveryMethods::ExpectDiscoverByMsiUpgradeCodeIsNotCalled()
{
    EXPECT_CALL( *this, DiscoverByRegistry( _, _, _ ) ).Times( 0 );
}
