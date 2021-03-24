#include "pch.h"
#include "PmTypes.h"
#include "IUcLogger.h"
#include "StringUtil.h"
#include "WindowsUtilities.h"
#include "PackageDiscoveryMethods.h"
#include <codecvt>
#include <regex>
#include "..\..\GlobalVersion.h"

#define IMMUNET_REG_KEY L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Immunet Protect"
#define UC_CONFIG_REG_KEY L"SOFTWARE\\Cisco\\SecureClient\\UnifiedConnector\\config"

PackageDiscoveryMethods::PackageDiscoveryMethods()
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
    //implement msi discovery method
}

void PackageDiscoveryMethods::DiscoverByRegistry(
    const PmProductDiscoveryRules& lookupProduct,
    const PmProductDiscoveryRegistryMethod& regRule,
    std::vector<PmInstalledPackage>& detectedInstallations )
{
    if( regRule.type != "registry" ) return;

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

    detectedInstallations.push_back( detected );
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
