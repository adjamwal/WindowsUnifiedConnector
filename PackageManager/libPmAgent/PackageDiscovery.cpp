#include "pch.h"
#include "PackageDiscovery.h"
#include "PmTypes.h"
#include "IUcLogger.h"
#include "StringUtil.h"
#include "WindowsUtilities.h"
#include "PackageDiscoveryMethods.h"
#include <codecvt>
#include <regex>
#include "..\..\GlobalVersion.h"
#include <StringUtil.h>

PackageDiscovery::PackageDiscovery( IPackageDiscoveryMethods& methods, IPmPlatformComponentManager& componentManager )
    : m_methods( methods )
    , m_componentMgr( componentManager )
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

    for( auto& lookupProduct : catalogRules )
    {
        std::vector<PmInstalledPackage> detectedInstallations;
        ApplyDiscoveryMethods( lookupProduct, detectedInstallations );
        
        for( auto& detectedItem : detectedInstallations )
        {
            DiscoverPackageConfigurables( lookupProduct.configurables, detectedItem.configs );

            inventory.packages.push_back( detectedItem );
        }
    }

    m_lastDetectedPackages = inventory;

    return inventory;
}

PackageInventory PackageDiscovery::CachedInventory()
{
    return m_lastDetectedPackages;
}

void PackageDiscovery::ApplyDiscoveryMethods( const PmProductDiscoveryRules& lookupProduct,
    std::vector<PmInstalledPackage>& detectedInstallations )
{
    for ( auto upgradeCodeRule : lookupProduct.msiUpgradeCode_discovery ) {
        m_methods.DiscoverByMsiUpgradeCode( lookupProduct, upgradeCodeRule, detectedInstallations );
        if ( !detectedInstallations.empty() ) {
            return;
        }
    }

    for( auto msiRule : lookupProduct.msi_discovery )
    {
        m_methods.DiscoverByMsi( lookupProduct, msiRule, detectedInstallations );
        if ( !detectedInstallations.empty() ) {
            return;
        }
    }

    for( auto regRule : lookupProduct.reg_discovery )
    {
        m_methods.DiscoverByRegistry( lookupProduct, regRule, detectedInstallations );
        if ( !detectedInstallations.empty() ) {
            return;
        }
    }
}

void PackageDiscovery::DiscoverPackageConfigurables( 
    const std::vector<PmProductDiscoveryConfigurable>& configurables, 
    std::vector<PackageConfigInfo>& packageConfigs )
{
    for ( auto& configurable : configurables )
    {
        std::vector<std::filesystem::path> fileList;

        std::string resolvedPath = m_componentMgr.ResolvePath( configurable.path );

        m_componentMgr.FileSearchWithWildCard( resolvedPath, fileList );

        if ( configurable.max_instances == 0 )
        {
            if ( fileList.size() > configurable.max_instances )
            {
                fileList = std::vector<std::filesystem::path>( fileList.begin(), fileList.begin() + 1 );
            }
        }
        else
        {
            if ( fileList.size() > configurable.max_instances )
            {
                fileList = std::vector<std::filesystem::path>( fileList.begin(), fileList.begin() + configurable.max_instances );
            }
        }

        for ( auto &foundFile : fileList )
        {
            PackageConfigInfo configInfo = {};
            configInfo.path = foundFile.generic_string();
            packageConfigs.push_back( configInfo );
        }  
    }
}
