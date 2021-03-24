#pragma once

#include "IPackageDiscovery.h"
#include "PmTypes.h"
#include "Windows.h"
#include <string>

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

    IPackageDiscoveryMethods& m_methods;
    PackageInventory m_lastDetectedPackages;
};
