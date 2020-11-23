#pragma once
#include "MocksCommon.h"

#include "IPackageInventoryProvider.h"
#include "PmTypes.h"

class MockPackageInventoryProvider : public IPackageInventoryProvider
{
public:
    MockPackageInventoryProvider();
    ~MockPackageInventoryProvider();

    MOCK_METHOD1( GetInventory, bool( PackageInventory& ) );
    void MakeGetInventoryReturn( bool value );
    void ExpectGetInventoryIsNotCalled();

};
