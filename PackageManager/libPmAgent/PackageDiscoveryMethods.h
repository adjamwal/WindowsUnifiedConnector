#pragma once

#include "IPackageDiscoveryMethods.h"
#include "PmTypes.h"
#include "Windows.h"
#include <string>

class PackageDiscoveryMethods : public IPackageDiscoveryMethods
{
public:
    PackageDiscoveryMethods();
    ~PackageDiscoveryMethods();

    void DiscoverByMsi(
        const PmProductDiscoveryRules& lookupProduct,
        const PmProductDiscoveryMsiMethod& msiRule,
        std::vector<PmInstalledPackage>& detectedInstallations );

    void DiscoverByRegistry(
        const PmProductDiscoveryRules& lookupProduct,
        const PmProductDiscoveryRegistryMethod& regRule,
        std::vector<PmInstalledPackage>& detectedInstallations );

private:
    bool DecodeRegistryPath( const PmProductDiscoveryRegKeyDef& keyDef,
        HKEY& root, std::string& subKey, std::string& valueName, std::string& error );

    void PadBuildNumber( std::string& versionString );
};
