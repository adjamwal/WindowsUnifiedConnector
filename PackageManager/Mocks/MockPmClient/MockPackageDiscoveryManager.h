#pragma once

#include <gmock/gmock.h>
#include "IPackageDiscoveryManager.h"
#include "PmTypes.h"

class MockPackageDiscoveryManager : public IPackageDiscoveryManager
{
public:
    MockPackageDiscoveryManager();
    virtual ~MockPackageDiscoveryManager();

    MOCK_METHOD1( Initialize, void( IPmPlatformDependencies* dep ) );
    void ExpectInitializeIsNotCalled();

    MOCK_METHOD1( DiscoverPackages, bool( PackageInventory& inventory ) );
    void MakeDiscoverPackagesReturn( bool value );
    void ExpectDiscoverPackagesIsNotCalled();
};