#include "pch.h"
#include "PackageDiscovery.h"
#include "WindowsUtilities.h"
#include "PmTypes.h"
#include "IUcLogger.h"
#include <codecvt>
#include <regex>
#include "..\..\GlobalVersion.h"

#define IMMUNET_REG_KEY L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Immunet Protect"
#define UC_CONFIG_REG_KEY L"SOFTWARE\\Cisco\\SecureClient\\UnifiedConnector\\config"

PackageDiscovery::PackageDiscovery()
{
}

PackageDiscovery::~PackageDiscovery()
{
}

PackageInventory PackageDiscovery::GetInstalledPackages( 
    const std::vector<PmDiscoveryComponent>& discoveryList, 
    const std::vector<PmProductDiscoveryRules>& catalogRules )
{
    PackageInventory inventory;

    inventory.architecture = WindowsUtilities::Is64BitWindows() ? "x64" : "x86";
    inventory.platform = "win";

    for ( auto& lookupItem : discoveryList )
    {
        PmProductDiscoveryRules matchingProduct;
        if( !GetMatchingProductRuleSet( lookupItem, catalogRules, matchingProduct ) )
        {
            LOG_DEBUG( "Missing product discovery rules: package %s, product %s", lookupItem.packageName.c_str(), lookupItem.packageProduct.c_str() );
            continue;
        }

        std::vector<PmInstalledPackage> detectedInstallations;
        ApplyDiscoveryForPackage( lookupItem, matchingProduct, detectedInstallations );
        for( auto& detectedItem : detectedInstallations )
        {
            inventory.packages.push_back( detectedItem );
        }
    }

    return inventory;
}

bool PackageDiscovery::GetMatchingProductRuleSet(
    const PmDiscoveryComponent& lookupItem,
    const std::vector<PmProductDiscoveryRules>& catalogRules,
    PmProductDiscoveryRules& matchingProduct )
{
    for( auto& productRuleSet : catalogRules )
    {
        if( lookupItem.packageProduct == productRuleSet.product )
        {
            matchingProduct = productRuleSet;
            return true;
        }
    }
    return false;
}

void PackageDiscovery::ApplyDiscoveryForPackage(
    const PmDiscoveryComponent& lookupItem,
    const PmProductDiscoveryRules& discoveryRuleSet,
    std::vector<PmInstalledPackage>& detectedInstallations )
{
    //TODO: use WindowsUtilities::ResolveKnownFolderId() to translate CSIDL paths for configurables

    for( auto msiRule : discoveryRuleSet.msi_discovery )
    {
        DiscoverPackageByMsi( lookupItem, msiRule, detectedInstallations );
    }
    for( auto regRule : discoveryRuleSet.reg_discovery )
    {
        DiscoverPackageByRegistry( lookupItem, regRule, detectedInstallations );
    }
}

//TODO: move actual discovery methods to separate class

void PackageDiscovery::DiscoverPackageByMsi(
    const PmDiscoveryComponent& lookupItem,
    const PmProductDiscoveryMsiMethod& msiRule, 
    std::vector<PmInstalledPackage>& detectedInstallations )
{
    //implement msi discovery method
}

void PackageDiscovery::DiscoverPackageByRegistry(
    const PmDiscoveryComponent& lookupItem,
    const PmProductDiscoveryRegistryMethod& regRule,
    std::vector<PmInstalledPackage>& detectedInstallations )
{
    //implement registry discovery method
}

void PackageDiscovery::PadBuildNumber( std::string& versionString )
{
    while ( std::count( versionString.begin(), versionString.end(), '.' ) < 3 ) {
        versionString += ".0";
    }
}
