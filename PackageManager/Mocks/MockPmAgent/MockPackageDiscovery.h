#pragma once

#include <gmock/gmock.h>
#include "PmTypes.h"
#include "IPackageDiscovery.h"

class MockPackageDiscovery : public IPackageDiscovery
{
public:
    MockPackageDiscovery();
    ~MockPackageDiscovery();

    MOCK_METHOD1( GetInstalledPackages, PackageInventory( const std::vector<PmDiscoveryComponent>& ) );
    void MakeGetInstalledPackagesReturn( PackageInventory packages );
    void ExpectGetInstalledPackagesIsNotCalled();

private:
    PackageInventory m_inventory;
};