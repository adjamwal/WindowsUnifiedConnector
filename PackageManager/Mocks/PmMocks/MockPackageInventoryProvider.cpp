#include "MockPackageInventoryProvider.h"

MockPackageInventoryProvider::MockPackageInventoryProvider()
{
    MakeGetInventoryReturn( false );
}

MockPackageInventoryProvider::~MockPackageInventoryProvider()
{
}

void MockPackageInventoryProvider::MakeGetInventoryReturn( bool value )
{
    ON_CALL( *this, GetInventory( _ ) ).WillByDefault( Return( value ) );
}

void MockPackageInventoryProvider::ExpectGetInventoryIsNotCalled()
{
    EXPECT_CALL( *this, GetInventory( _ ) ).Times( 0 );
}

