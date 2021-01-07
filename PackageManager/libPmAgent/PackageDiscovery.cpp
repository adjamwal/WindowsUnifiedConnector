#include "pch.h"
#include "PackageDiscovery.h"
#include "WindowsUtilities.h"
#include "PmTypes.h"
#include "IUcLogger.h"
#include <codecvt>
#include "..\..\GlobalVersion.h"

#define IMMUNET_REG_KEY L"SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Immunet Protect"
#define UC_CONFIG_REG_KEY L"SOFTWARE\\Cisco\\SecureXYZ\\UnifiedConnector\\config"

PackageDiscovery::PackageDiscovery()
{
}

PackageDiscovery::~PackageDiscovery()
{
}

PackageInventory PackageDiscovery::GetInstalledPackages( const std::vector<PmDiscoveryComponent>& discoveryList )
{
    PackageInventory packages;

    packages.architecture = WindowsUtilities::Is64BitWindows() ? "x64" : "x86";
    packages.platform = "win";

    auto programList = WindowsUtilities::GetInstalledPrograms();

    for ( auto& interestingItem : discoveryList ) {
        if ( interestingItem.packageId == "uc" ) {
            packages.packages.push_back( BuildUcPackage() );
        }
        else if ( interestingItem.packageId == "amp" ) {
            try {
                packages.packages.push_back( HackBuildAmpPackage() );
            }
            catch ( std::exception ex ) {
                LOG_ERROR( "Failed to build Amp Package: %s", ex.what() );
            }
        }
        else {
            for ( auto& program : programList ) {
                if ( interestingItem.packageName == program.name ) {
                    PmInstalledPackage discoveredPackage;

                    LOG_DEBUG( "Found Matching package %s %s %s",
                        interestingItem.packageId.c_str(),
                        program.name.c_str(),
                        program.version.c_str()
                    );

                    discoveredPackage.packageName = interestingItem.packageId;
                    discoveredPackage.packageVersion = program.version;
                    PadBuildNumber( discoveredPackage.packageVersion );

                    packages.packages.push_back( discoveredPackage );
                    break;
                }
            }
        }
    }

    return packages;
}

void PackageDiscovery::PadBuildNumber( std::string& versionString )
{
    while ( std::count( versionString.begin(), versionString.end(), '.' ) < 3 ) {
        versionString += ".0";
    }
}

PmInstalledPackage PackageDiscovery::BuildUcPackage()
{
    PmInstalledPackage ucPackage;
    PackageConfigInfo ucConfig;
    std::wstring filepath;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    ucPackage.packageName = "uc";
    ucPackage.packageVersion = converter.to_bytes( STRFORMATPRODVER );
    PadBuildNumber( ucPackage.packageVersion );

    ucConfig.deleteConfig = false;

    filepath.clear();
    if ( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, UC_CONFIG_REG_KEY, L"UCID", filepath ) ) {
        throw( std::exception( "Failed to read UCID reg key" ) );
    }
    ucConfig.path = converter.to_bytes( filepath );
    ucPackage.configs.push_back( ucConfig );

    filepath.clear();
    if ( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, UC_CONFIG_REG_KEY, L"UCPM", filepath ) ) {
        throw( std::exception( "Failed to read UCPM reg key" ) );
    }
    ucConfig.path = converter.to_bytes( filepath );
    ucPackage.configs.push_back( ucConfig );

    filepath.clear();
    if ( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, UC_CONFIG_REG_KEY, L"Service", filepath ) ) {
        throw( std::exception( "Failed to read Service reg key" ) );
    }
    ucConfig.path = converter.to_bytes( filepath );
    ucPackage.configs.push_back( ucConfig );

    return ucPackage;
}

PmInstalledPackage PackageDiscovery::HackBuildAmpPackage()
{
    //TODO: This should be redone in Enterprise. The Discovery component should provide information on how
    // Discover AMP
    PmInstalledPackage ampPackage;
    PackageConfigInfo ucConfig;
    std::wstring displayName;
    std::wstring displayVersion;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    if ( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, IMMUNET_REG_KEY, L"DisplayName", displayName ) ) {
        throw( std::exception( "Failed to read AMP display name" ) );
    }
    else if ( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, IMMUNET_REG_KEY, L"DisplayVersion", displayVersion ) ) {
        throw( std::exception( "Failed to read AMP display version" ) );
    }
    else {
        if ( ( displayName == L"Immunet" ) || ( displayName == L"Cisco AMP for Endpoints Connector" ) ) {
            ampPackage.packageName = "amp";
        }
        else {
            std::string error = "Unexpected display name: ";
            error += converter.to_bytes( displayName );;
            throw( std::exception( error.c_str() ) );
        }

        ampPackage.packageVersion = converter.to_bytes( displayVersion );
        PadBuildNumber( ampPackage.packageVersion );

    }
    return ampPackage;
}