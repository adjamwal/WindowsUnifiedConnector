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
#include "IUtf8PathVerifier.h"


PackageDiscovery::PackageDiscovery( IPackageDiscoveryMethods& methods, IMsiApi& msiApi, IUtf8PathVerifier& utf8PathVerifier ) :
    m_methods( methods )
    , m_msiApi( msiApi )
    , m_utf8PathVerifier( utf8PathVerifier )
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

    std::vector<MsiApiProductInfo> productCache;
    auto ret = m_msiApi.QueryProducts( productCache );

    if ( ret != ERROR_SUCCESS || productCache.empty() )
    {
        LOG_ERROR( "Error getting products list from system  %d", ret );
    }

    for( auto& lookupProduct : catalogRules )
    {
        std::vector<PmInstalledPackage> detectedInstallations;
        ApplyDiscoveryMethods( lookupProduct, detectedInstallations, productCache );
        
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
    std::vector<PmInstalledPackage>& detectedInstallations,
    std::vector<MsiApiProductInfo>& productCache )
{
    for ( auto upgradeCodeRule : lookupProduct.msiUpgradeCode_discovery ) {
        m_methods.DiscoverByMsiUpgradeCode( lookupProduct, upgradeCodeRule, detectedInstallations );
        if ( !detectedInstallations.empty() ) {
            return;
        }
    }

    for ( auto msiRule : lookupProduct.msi_discovery )
    {
        m_methods.DiscoverByMsiRules( lookupProduct, msiRule, detectedInstallations, productCache );
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
        std::string knownFolderId = "";
        std::string knownFolderIdConversion = "";
        std::vector<std::filesystem::path> discoveredFiles;

        if ( configurable.unresolvedPath != configurable.path )
        {
            //Resolved path is deferent which means we must calculate the knownfolderid
            std::string tempResolvedPath = configurable.path.generic_u8string();
            std::string tempUnresolvedPath = configurable.unresolvedPath.generic_u8string();
            size_t first = tempUnresolvedPath.find( "<FOLDERID_" );
            size_t last = tempUnresolvedPath.find_first_of( ">" );
            knownFolderId = tempUnresolvedPath.substr( first, last + 1);
            std::string remainingPath = tempUnresolvedPath.substr( last + 1, tempUnresolvedPath.length() );

            first = tempResolvedPath.find( remainingPath );

            knownFolderIdConversion = tempResolvedPath.substr( 0, first );
        }

        WindowsUtilities::FileSearchWithWildCard( configurable.path, discoveredFiles );

        m_utf8PathVerifier.PruneInvalidPathsFromList( discoveredFiles );

        if ( discoveredFiles.size() > configurable.max_instances )
        {
            if ( configurable.max_instances == 0 )
            {
                discoveredFiles = std::vector<std::filesystem::path>( discoveredFiles.begin(), discoveredFiles.begin() + 1 );
            }
            else
            {
                discoveredFiles = std::vector<std::filesystem::path>( discoveredFiles.begin(), discoveredFiles.begin() + configurable.max_instances );
            }
        }
        
        for ( auto &discoveredFile : discoveredFiles )
        {
            PackageConfigInfo configInfo = {};

            std::string tempPath = discoveredFile.generic_u8string();

            if ( knownFolderId != "" )
            {
                //We need to convert the path to include knownfolderid
                tempPath = tempPath.substr( knownFolderIdConversion.length(), tempPath.length() );
                tempPath = knownFolderId + tempPath;
            }

            configInfo.path = tempPath;
            configInfo.unresolvedPath = configurable.unresolvedPath;
            packageConfigs.push_back( configInfo );
        }  
    }
}
