#pragma once

#include <gmock/gmock.h>
#include "PmTypes.h"
#include "IPackageDiscoveryMethods.h"
#include "MsiApi.h"

class MockPackageDiscoveryMethods : public IPackageDiscoveryMethods
{
public:
    MockPackageDiscoveryMethods();
    ~MockPackageDiscoveryMethods();

    MOCK_METHOD3( DiscoverByMsi, void( const PmProductDiscoveryRules&, const PmProductDiscoveryMsiMethod&, std::vector<PmInstalledPackage>& ) );
    void ExpectDiscoverByMsiIsNotCalled();

    MOCK_METHOD3( DiscoverByRegistry, void( const PmProductDiscoveryRules&, const PmProductDiscoveryRegistryMethod&, std::vector<PmInstalledPackage>& ) );
    void ExpectDiscoverByRegistryIsNotCalled();

    MOCK_METHOD3( DiscoverByMsiUpgradeCode, void( const PmProductDiscoveryRules&, const PmProductDiscoveryMsiUpgradeCodeMethod&, std::vector<PmInstalledPackage>& ) );
    void ExpectDiscoverByMsiUpgradeCodeIsNotCalled();

    MOCK_METHOD4( DiscoverByMsiRules, void( const PmProductDiscoveryRules&, const PmProductDiscoveryMsiMethod&, std::vector<PmInstalledPackage>&, std::vector<MsiApiProductInfo>& ) );
    void ExpectDiscoverByMsiRulesIsNotCalled();
};
