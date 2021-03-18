#pragma once

#include "IPackageDiscovery.h"
#include "PmTypes.h"
#include <string>

class PackageDiscovery : public IPackageDiscovery
{
public:
    PackageDiscovery();
    ~PackageDiscovery();

    PackageInventory GetInstalledPackages( 
        const std::vector<PmDiscoveryComponent>& discoveryList,
        const std::vector<PmProductDiscoveryRules>& catalogRules
    ) override;

private:
    bool GetMatchingProductRuleSet(
        const PmDiscoveryComponent& lookupItem,
        const std::vector<PmProductDiscoveryRules>& catalogRules,
        PmProductDiscoveryRules& ruleSet );
    void ApplyDiscoveryForPackage(
        const PmDiscoveryComponent& lookupItem,
        const PmProductDiscoveryRules& discoveryRuleSet,
        std::vector<PmInstalledPackage>& detectedInstallations );
    void DiscoverPackageByMsi(
        const PmDiscoveryComponent& lookupItem,
        const PmProductDiscoveryMsiMethod& msiRule,
        std::vector<PmInstalledPackage>& detectedInstallations );
    void DiscoverPackageByRegistry(
        const PmDiscoveryComponent& lookupItem,
        const PmProductDiscoveryRegistryMethod& regRule,
        std::vector<PmInstalledPackage>& detectedInstallations );

    void PadBuildNumber( std::string& versionString );
};
