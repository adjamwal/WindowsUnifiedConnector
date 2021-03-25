#pragma once

#include "IPackageDiscoveryMethods.h"
#include "PmTypes.h"
#include "Windows.h"
#include <string>

class IMsiApi;

class PackageDiscoveryMethods : public IPackageDiscoveryMethods
{
public:
    PackageDiscoveryMethods( IMsiApi& msiApi );
    ~PackageDiscoveryMethods();

    void DiscoverByMsi(
        const PmProductDiscoveryRules& lookupProduct,
        const PmProductDiscoveryMsiMethod& msiRule,
        std::vector<PmInstalledPackage>& detectedInstallations );

    void DiscoverByRegistry(
        const PmProductDiscoveryRules& lookupProduct,
        const PmProductDiscoveryRegistryMethod& regRule,
        std::vector<PmInstalledPackage>& detectedInstallations );

    void DiscoverByMsiUpgradeCode(
        const PmProductDiscoveryRules& lookupProduct,
        const PmProductDiscoveryMsiUpgradeCodeMethod& upgradeCodeRule,
        std::vector<PmInstalledPackage>& detectedInstallations );

private:
    bool DecodeRegistryPath( const PmProductDiscoveryRegKeyDef& keyDef,
        HKEY& root, std::string& subKey, std::string& valueName, std::string& error );

    void PadBuildNumber( std::string& versionString );

    void CopyDiscoveryConfigurablesToPackageConfig(
        std::vector<PackageConfigInfo>& configInfoList,
        const std::vector<PmProductDiscoveryConfigurable>& productDiscoveryConfigurables );

    IMsiApi& m_msiApi;
};
