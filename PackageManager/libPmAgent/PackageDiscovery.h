#pragma once

#include "IPackageDiscovery.h"
#include "PmTypes.h"
#include "Windows.h"
#include <string>
#include "IPmPlatformComponentManager.h"

class IPackageDiscoveryMethods;

class PackageDiscovery : public IPackageDiscovery
{
public:
    PackageDiscovery( IPackageDiscoveryMethods& methods );
    ~PackageDiscovery();

    PackageInventory DiscoverInstalledPackages( const std::vector<PmProductDiscoveryRules>& catalogRules ) override;
    PackageInventory CachedInventory() override;

private:
    void ApplyDiscoveryMethods(
        const PmProductDiscoveryRules& lookupProduct,
        std::vector<PmInstalledPackage>& detectedInstallations );

    void DiscoverPackageConfigurables( 
        const std::vector<PmProductDiscoveryConfigurable>& configurables, 
        std::vector<PackageConfigInfo>& packageConfigs );

    IPackageDiscoveryMethods& m_methods;
    PackageInventory m_lastDetectedPackages;
};
