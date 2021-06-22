#pragma once

#include "IPackageDiscovery.h"
#include "PmTypes.h"
#include "Windows.h"
#include <string>
#include "IPmPlatformComponentManager.h"
#include "MsiApi.h"

class IPackageDiscoveryMethods;
class IUtf8PathVerifier;

class PackageDiscovery : public IPackageDiscovery
{
public:
    PackageDiscovery( IPackageDiscoveryMethods& methods, IMsiApi& msiApi, IUtf8PathVerifier& utf8PathVerifier );
    ~PackageDiscovery();

    PackageInventory DiscoverInstalledPackages( const std::vector<PmProductDiscoveryRules>& catalogRules ) override;
    PackageInventory CachedInventory() override;

private:
    void ApplyDiscoveryMethods(
        const PmProductDiscoveryRules& lookupProduct,
        std::vector<PmInstalledPackage>& detectedInstallations,
        std::vector<MsiApiProductInfo>& productCache );

    void DiscoverPackageConfigurables( 
        const std::vector<PmProductDiscoveryConfigurable>& configurables, 
        std::vector<PackageConfigInfo>& packageConfigs );

    IPackageDiscoveryMethods& m_methods;
    PackageInventory m_lastDetectedPackages;
    IMsiApi& m_msiApi;
    IUtf8PathVerifier& m_utf8PathVerifier;
};
