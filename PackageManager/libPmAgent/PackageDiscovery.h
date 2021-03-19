#pragma once

#include "IPackageDiscovery.h"
#include "PmTypes.h"
#include <string>

class PackageDiscovery : public IPackageDiscovery
{
public:
    PackageDiscovery();
    ~PackageDiscovery();

    PackageInventory DiscoverInstalledPackages( const std::vector<PmProductDiscoveryRules>& catalogRules ) override;
    PackageInventory CachedInventory() override;

private:
    void ApplyDiscoveryMethods(
        const PmProductDiscoveryRules& lookupProduct,
        std::vector<PmInstalledPackage>& detectedInstallations );

    void DiscoverByMsi(
        const PmProductDiscoveryRules& lookupProduct,
        const PmProductDiscoveryMsiMethod& msiRule,
        std::vector<PmInstalledPackage>& detectedInstallations );

    void DiscoverByRegistry(
        const PmProductDiscoveryRules& lookupProduct,
        const PmProductDiscoveryRegistryMethod& regRule,
        std::vector<PmInstalledPackage>& detectedInstallations );

    void PadBuildNumber( std::string& versionString );

    PackageInventory m_lastDetectedPackages;
};
