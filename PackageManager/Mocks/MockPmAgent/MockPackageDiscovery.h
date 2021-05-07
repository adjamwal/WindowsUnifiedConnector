#pragma once

#include <gmock/gmock.h>
#include "PmTypes.h"
#include "IPackageDiscovery.h"

class MockPackageDiscovery : public IPackageDiscovery
{
public:
    MockPackageDiscovery();
    ~MockPackageDiscovery();

    MOCK_METHOD1( DiscoverInstalledPackages, PackageInventory( const std::vector<PmProductDiscoveryRules>& ) );
    void MakeDiscoverInstalledPackagesReturn( PackageInventory value );
    void ExpectDiscoverInstalledPackagesIsNotCalled();

    MOCK_METHOD0( CachedInventory, PackageInventory() );
    void MakeCachedInventoryReturn( PackageInventory value );
    void ExpectCachedInventoryIsNotCalled();

private:
    PackageInventory m_detectedPackages;
    PackageInventory m_cachedInventory;
};