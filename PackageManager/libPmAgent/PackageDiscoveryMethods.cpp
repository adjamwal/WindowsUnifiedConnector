#include "pch.h"
#include "PmTypes.h"
#include "IUcLogger.h"
#include "StringUtil.h"
#include "WindowsUtilities.h"
#include "PackageDiscoveryMethods.h"
#include "MsiApi.h"
#include <codecvt>
#include <regex>
#include "..\..\GlobalVersion.h"
#include <CatalogJsonParser.cpp>

#define IMMUNET_REG_KEY L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Immunet Protect"
#define UC_CONFIG_REG_KEY L"SOFTWARE\\Cisco\\SecureClient\\UnifiedConnector\\config"

PackageDiscoveryMethods::PackageDiscoveryMethods( IMsiApi& msiApi ) :
    m_msiApi( msiApi )
{
}

PackageDiscoveryMethods::~PackageDiscoveryMethods()
{
}

void PackageDiscoveryMethods::DiscoverByMsi(
    const PmProductDiscoveryRules& lookupProduct,
    const PmProductDiscoveryMsiMethod& msiRule,
    std::vector<PmInstalledPackage>& detectedInstallations )
{
    if ( msiRule.type != UC_CATALOG_DISCOVERY_TYPE_MSI ) return;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring name = converter.from_bytes( msiRule.name );
    std::wstring publisher = converter.from_bytes( msiRule.vendor );

    auto [retCode, msiList] = m_msiApi.FindProductsByNameAndPublisher( name, publisher );

    if ( retCode != ERROR_SUCCESS || msiList.empty() )
    {
        LOG_DEBUG( "DiscoverByMsi could not find %s, %s: %d", msiRule.name.c_str(), msiRule.vendor.c_str(), retCode );
    }

    for ( auto listItem : msiList )
    {
        PmInstalledPackage detected = {};
        detected.version = converter.to_bytes( listItem.Properties.VersionString );
        detected.product = lookupProduct.product;

        LOG_DEBUG( "DiscoverByMsi found: %s, %s, %s",
            msiRule.name.c_str(), 
            msiRule.vendor.c_str(),
            converter.to_bytes( listItem.InstalledProductCode ).c_str() );

        detectedInstallations.push_back( detected );
    }
}

void PackageDiscoveryMethods::DiscoverByRegistry(
    const PmProductDiscoveryRules& lookupProduct,
    const PmProductDiscoveryRegistryMethod& regRule,
    std::vector<PmInstalledPackage>& detectedInstallations )
{
    if( regRule.type != UC_CATALOG_DISCOVERY_TYPE_REGISTRY ) return;
    
    PmInstalledPackage detected = {};
    detected.product = lookupProduct.product;

    HKEY regRoot;
    std::string regSubKey;
    std::string regValueName;
    std::string errorStr;
    std::string data;
    DWORD flags = 0;

    if ( regRule.install.type == "WOW6432" && WindowsUtilities::Is64BitWindows() ) {
        flags = RRF_RT_REG_SZ | RRF_SUBKEY_WOW6432KEY;
    }
    else {
        flags = RRF_RT_REG_SZ;
    }

    if( !DecodeRegistryPath( regRule.install, regRoot, regSubKey, regValueName, errorStr ) ) {
        LOG_ERROR( "Failed to decode registry path '%s': %s",
            regRule.install.key.c_str(), errorStr.c_str() );
        return;
    }

    if( !WindowsUtilities::ReadRegistryStringA( regRoot, regSubKey, regValueName, flags, data ) )
    {
        LOG_ERROR( "Failed to detect product '%s' in registry by install key '%s'",
            lookupProduct.product.c_str(), regRule.install.key.c_str() );
        return;
    }

    if ( data.empty() )
    {
        LOG_DEBUG( "Detected '%s' in registry by install key but data is empty.", lookupProduct.product.c_str() );
    }

    if ( regRule.version.type == "WOW6432" && WindowsUtilities::Is64BitWindows() ) {
        flags = RRF_RT_REG_SZ | RRF_SUBKEY_WOW6432KEY;
    }
    else {
        flags = RRF_RT_REG_SZ;
    }

    if( !DecodeRegistryPath( regRule.version, regRoot, regSubKey, regValueName, errorStr ) ) {

        LOG_ERROR( "Failed to decode registry path '%s': %s",
            regRule.version.key.c_str(), errorStr.c_str() );
        return;
    }

    if( !WindowsUtilities::ReadRegistryStringA( regRoot, regSubKey, regValueName, flags, data ) )
    {
        LOG_ERROR( "Failed to detect product '%s' in registry by version key '%s'",
            lookupProduct.product.c_str(), regRule.version.key.c_str() );
        return;
    }

    if ( data.empty() )
    {
        LOG_ERROR( "Detected '%s' in registry by version key but data is empty.", lookupProduct.product.c_str() );
        return;
    }

    detected.version = data;
    PadBuildNumber( detected.version );

    LOG_DEBUG( "DiscoverByRegistry found: %s, %s, %s (%s)",
        lookupProduct.product.c_str(),
        regRule.install.key.c_str(),
        regRule.version.key.c_str(),
        detected.version.c_str() );

    detectedInstallations.push_back( detected );
}

void PackageDiscoveryMethods::DiscoverByMsiUpgradeCode( const PmProductDiscoveryRules& lookupProduct, const PmProductDiscoveryMsiUpgradeCodeMethod& upgradeCodeRule, std::vector<PmInstalledPackage>& detectedInstallations )
{
    if ( upgradeCodeRule.type != UC_CATALOG_DISCOVERY_TYPE_MSI_UPGRADE_CODE ) return;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring upgradeCode = converter.from_bytes( upgradeCodeRule.upgradeCode );

    auto [retCode, msiList] = m_msiApi.FindRelatedProducts( upgradeCode );

    if ( retCode != ERROR_SUCCESS || msiList.empty() )
    {
        LOG_DEBUG( "DiscoverByMsiUpgradeCode could not find %s: %d", upgradeCodeRule.upgradeCode.c_str(), retCode );
    }

    for ( auto listItem : msiList )
    {
        PmInstalledPackage detected = {};
        detected.version = converter.to_bytes( listItem.Properties.VersionString );
        detected.product = lookupProduct.product;

        LOG_DEBUG( "DiscoverByMsiUpgradeCode found: %s, %s",
            upgradeCodeRule.upgradeCode.c_str(),
            converter.to_bytes( listItem.InstalledProductCode ).c_str() );

        detectedInstallations.push_back( detected );
    }
}

void PackageDiscoveryMethods::DiscoverByMsiRules( 
    const PmProductDiscoveryRules& lookupProduct, 
    const PmProductDiscoveryMsiMethod& msiRule, 
    std::vector<PmInstalledPackage>& detectedInstallations,
    std::vector<MsiApiProductInfo>& productCache )
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    bool found = false;
    std::wstring name = converter.from_bytes( msiRule.name );
    std::wstring publisher = converter.from_bytes( msiRule.vendor );

    for ( auto product : productCache )
    {
        if ( !name.empty() &&
            !publisher.empty() &&
            name.compare( product.Properties.InstalledProductName ) == 0 &&
            publisher.compare( product.Properties.Publisher ) == 0 )
        {
            PmInstalledPackage detected = {};
            detected.version = converter.to_bytes( product.Properties.VersionString );
            detected.product = lookupProduct.product;

            LOG_DEBUG( "DiscoverByMsi found: %s, %s, %s",
                msiRule.name.c_str(),
                msiRule.vendor.c_str(),
                converter.to_bytes( product.InstalledProductCode ).c_str() );

            detectedInstallations.push_back( detected );
            found = true;
        }
    }

    if ( !found )
    {
        LOG_DEBUG( "Could not find %s, %s", msiRule.name.c_str(), msiRule.vendor.c_str() );
    }
}

bool PackageDiscoveryMethods::DecodeRegistryPath( const PmProductDiscoveryRegKeyDef& keyDef,
    HKEY& root, std::string& subKey, std::string& valueName, std::string& error )
{
    subKey.clear();
    valueName.clear();
    error.clear();

    auto regStr = StringUtil::Split( keyDef.key, '\\' );

    //must have at least 3 parts such as: root \\ subkey \\ valueName
    if( regStr.size() < 2 )
    {
        error = "Invalid registry string: " + keyDef.key;
        return false;
    }

    if( regStr[ 0 ] == "HKCR" ) {
        root = HKEY_CLASSES_ROOT;
    }
    else if( regStr[ 0 ] == "HKLM" ) {
        root = HKEY_LOCAL_MACHINE;
    }
    else if( regStr[ 0 ] == "HKCU" ) {
        root = HKEY_CURRENT_USER;
    }
    else {
        error = "Unsupported registry root: " + regStr[ 0 ];
        return false;
    }

    //skip reg root
    int subkeyBegin = 1;

    //last token is reserved for valueName, not part of the subKey
    valueName = regStr[ regStr.size() - 1 ];

    for( int i = subkeyBegin; i < regStr.size() - 1; i++ ) {
        subKey += ( subKey.empty() ? "" : "\\" ) + regStr[ i ];
    }

    return true;
}

void PackageDiscoveryMethods::PadBuildNumber( std::string& versionString )
{
    while( std::count( versionString.begin(), versionString.end(), '.' ) < 3 ) {
        versionString += ".0";
    }
}
