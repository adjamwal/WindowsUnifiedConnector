#pragma once

#include <vector>

struct PmProductDiscoveryRules;
struct PmProductDiscoveryMsiMethod;
struct PmProductDiscoveryRegistryMethod;
struct PmProductDiscoveryMsiUpgradeCodeMethod;
struct PmInstalledPackage;
struct MsiApiProductInfo;

class IPackageDiscoveryMethods
{
public:
    IPackageDiscoveryMethods() {};
    virtual ~IPackageDiscoveryMethods() {};

    virtual void DiscoverByMsi(
        const PmProductDiscoveryRules& lookupProduct,
        const PmProductDiscoveryMsiMethod& msiRule,
        std::vector<PmInstalledPackage>& detectedInstallations ) = 0;

    virtual void DiscoverByRegistry(
        const PmProductDiscoveryRules& lookupProduct,
        const PmProductDiscoveryRegistryMethod& regRule,
        std::vector<PmInstalledPackage>& detectedInstallations ) = 0;

    virtual void DiscoverByMsiUpgradeCode(
        const PmProductDiscoveryRules& lookupProduct,
        const PmProductDiscoveryMsiUpgradeCodeMethod& upgradeCodeRule,
        std::vector<PmInstalledPackage>& detectedInstallations ) = 0;

    virtual void DiscoverByMsiRules(
        const PmProductDiscoveryRules& lookupProduct,
        const PmProductDiscoveryMsiMethod& msiRules,
        std::vector<PmInstalledPackage>& detectedInstallations,
        std::vector<MsiApiProductInfo>& productCache ) = 0;
};
