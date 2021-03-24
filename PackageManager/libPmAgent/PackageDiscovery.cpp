#include "pch.h"
#include "PackageDiscovery.h"
#include "WindowsUtilities.h"
#include "PmTypes.h"
#include "IUcLogger.h"
#include "MsiApi.h"
#include <codecvt>
#include <regex>
#include "..\..\GlobalVersion.h"
#include <StringUtil.h>

#define IMMUNET_REG_KEY L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Immunet Protect"
#define UC_CONFIG_REG_KEY L"SOFTWARE\\Cisco\\SecureClient\\UnifiedConnector\\config"

PackageDiscovery::PackageDiscovery( IMsiApi& msiApi ) :
    m_msiApi( msiApi )
{
}

PackageDiscovery::~PackageDiscovery()
{
}

PackageInventory PackageDiscovery::DiscoverInstalledPackages( const std::vector<PmProductDiscoveryRules>& catalogRules )
{
    PackageInventory inventory;

    inventory.architecture = WindowsUtilities::Is64BitWindows() ? "x64" : "x86";
    inventory.platform = "win";

    // LOG_DEBUG( "Missing product discovery rules: package %s, product %s", lookupProduct.packageName.c_str(), lookupProduct.packageProduct.c_str() );

    for( auto& lookupProduct : catalogRules )
    {
        std::vector<PmInstalledPackage> detectedInstallations;
        ApplyDiscoveryMethods( lookupProduct, detectedInstallations );

        for( auto& detectedItem : detectedInstallations )
        {
            inventory.packages.push_back( detectedItem );
        }
    }

    //TODO: review usage - do we need a mutex lock here?
    m_lastDetectedPackages = inventory;

    return inventory;
}

PackageInventory PackageDiscovery::CachedInventory()
{
    //TODO: review usage - do we need a mutex lock here?
    return m_lastDetectedPackages;
}

void PackageDiscovery::ApplyDiscoveryMethods( const PmProductDiscoveryRules& lookupProduct,
    std::vector<PmInstalledPackage>& detectedInstallations )
{
    for( auto msiRule : lookupProduct.msi_discovery )
    {
        DiscoverByMsi( lookupProduct, msiRule, detectedInstallations );
    }
    for( auto regRule : lookupProduct.reg_discovery )
    {
        DiscoverByRegistry( lookupProduct, regRule, detectedInstallations );
    }
}

//NOTE: use WindowsUtilities::ResolveKnownFolderId() to translate CSIDL paths for configurables

void PackageDiscovery::DiscoverByMsi(
    const PmProductDiscoveryRules& lookupProduct,
    const PmProductDiscoveryMsiMethod& msiRule,
    std::vector<PmInstalledPackage>& detectedInstallations )
{
    if ( msiRule.type != "msi" ) return;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring name = converter.from_bytes( msiRule.name );
    std::wstring publisher = converter.from_bytes( msiRule.vendor );

    auto[retCode, msiList] = m_msiApi.FindProductsByNameAndPublisher( name, publisher );

    for ( auto listItem : msiList )
    {
        PmInstalledPackage detected = {};
        detected.version = converter.to_bytes( listItem.Properties.VersionString );
        detected.product = converter.to_bytes( listItem.Properties.InstalledProductName );

        detectedInstallations.push_back( detected );
    }
}

void PackageDiscovery::DiscoverByRegistry(
    const PmProductDiscoveryRules& lookupProduct,
    const PmProductDiscoveryRegistryMethod& regRule,
    std::vector<PmInstalledPackage>& detectedInstallations )
{
    //implement registry discovery method
}

void PackageDiscovery::PadBuildNumber( std::string& versionString )
{
    while( std::count( versionString.begin(), versionString.end(), '.' ) < 3 ) {
        versionString += ".0";
    }
}
