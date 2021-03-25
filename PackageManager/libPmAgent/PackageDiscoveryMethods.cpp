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

//NOTE: use WindowsUtilities::ResolveKnownFolderId() to translate CSIDL paths for configurables

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

    if ( retCode != ERROR_SUCCESS )
    {
        LOG_ERROR( "Error with FindRelatedProducts while searchings %ws, %ws: %d", name, publisher, retCode );
    }

    for ( auto listItem : msiList )
    {
        PmInstalledPackage detected = {};
        detected.version = converter.to_bytes( listItem.Properties.VersionString );
        detected.product = lookupProduct.product;
        detected.SetConfigInfo( lookupProduct.configurables );

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

    if( !DecodeRegistryPath( regRule.install, regRoot, regSubKey, regValueName, errorStr ) ) {
        LOG_ERROR( "Failed to decode registry path '%s': %s",
            regRule.install.key.c_str(), errorStr.c_str() );
        return;
    }

    if( !WindowsUtilities::ReadRegistryStringA( regRoot, regSubKey, regValueName, data ) || data.empty() )
    {
        LOG_INFO( "Failed to detect product '%s' in registry by install key '%s'",
            lookupProduct.product.c_str(), regRule.install.key.c_str() );
        return;
    }

    if( !DecodeRegistryPath( regRule.version, regRoot, regSubKey, regValueName, errorStr ) ) {
        LOG_ERROR( "Failed to decode registry path '%s': %s",
            regRule.version.key.c_str(), errorStr.c_str() );
        return;
    }

    if( !WindowsUtilities::ReadRegistryStringA( regRoot, regSubKey, regValueName, data ) || data.empty() )
    {
        LOG_INFO( "Failed to detect product '%s' in registry by version key '%s'",
            lookupProduct.product.c_str(), regRule.version.key.c_str() );
        return;
    }

    detected.version = data;
    PadBuildNumber( detected.version );

    detected.SetConfigInfo( lookupProduct.configurables );

    detectedInstallations.push_back( detected );
}

void PackageDiscoveryMethods::DiscoverByMsiUpgradeCode( const PmProductDiscoveryRules& lookupProduct, const PmProductDiscoveryMsiUpgradeCodeMethod& upgradeCodeRule, std::vector<PmInstalledPackage>& detectedInstallations )
{
    if ( upgradeCodeRule.type != UC_CATALOG_DISCOVERY_TYPE_MSI_UPGRADE_CODE ) return;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring upgradeCode = converter.from_bytes( upgradeCodeRule.upgradeCode );

    auto [retCode, msiList] = m_msiApi.FindRelatedProducts( upgradeCode );

    if ( retCode != ERROR_SUCCESS )
    {
        LOG_ERROR( "Error with FindRelatedProducts while searchings %ws: %d", upgradeCode, retCode );
    }

    for ( auto listItem : msiList )
    {
        PmInstalledPackage detected = {};
        detected.version = converter.to_bytes( listItem.Properties.VersionString );
        detected.product = lookupProduct.product;
        detected.SetConfigInfo( lookupProduct.configurables );

        detectedInstallations.push_back( detected );
    }
}

bool PackageDiscoveryMethods::DecodeRegistryPath( const PmProductDiscoveryRegKeyDef& keyDef,
    HKEY& root, std::string& subKey, std::string& valueName, std::string& error )
{
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

    if( keyDef.type == "WOW6432" && WindowsUtilities::Is64BitWindows() ) {
        switch( ( ULONG )root )
        {
        case ( ULONG )HKEY_CLASSES_ROOT:
            subKey = "WOW6432Node";
            break;
        case ( ULONG )HKEY_LOCAL_MACHINE:
            if( StringUtil::NoCaseEquals( regStr[ 1 ], "SOFTWARE" ) ) {
                subKey = "SOFTWARE\\WOW6432Node";
                subkeyBegin++;
            }
            break;
        case ( ULONG )HKEY_CURRENT_USER:
            if( StringUtil::NoCaseEquals( regStr[ 1 ], "Software" ) ) {
                subKey = "Software\\WOW6432Node";
                subkeyBegin++;
            }
            break;
        }
    }

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
