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

    if( ret != ERROR_SUCCESS || productCache.empty() )
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
    for( auto upgradeCodeRule : lookupProduct.msiUpgradeCode_discovery ) {
        m_methods.DiscoverByMsiUpgradeCode( lookupProduct, upgradeCodeRule, detectedInstallations );
        if( !detectedInstallations.empty() ) {
            return;
        }
    }

    for( auto msiRule : lookupProduct.msi_discovery )
    {
        m_methods.DiscoverByMsiRules( lookupProduct, msiRule, detectedInstallations, productCache );
        if( !detectedInstallations.empty() ) {
            return;
        }
    }

    for( auto regRule : lookupProduct.reg_discovery )
    {
        m_methods.DiscoverByRegistry( lookupProduct, regRule, detectedInstallations );
        if( !detectedInstallations.empty() ) {
            return;
        }
    }
}

void PackageDiscovery::ResolveAndDiscover(
    const std::filesystem::path& unresolvedPath,
    const std::filesystem::path& resolvedPath,
    std::string& out_knownFolderId,
    std::string& out_knownFolderIdConversion,
    std::vector<std::filesystem::path>& out_discoveredFiles )
{
    out_knownFolderId = "";
    out_knownFolderIdConversion = "";
    out_discoveredFiles.clear();

    if( unresolvedPath != resolvedPath )
    {
        //Resolved path is deferent which means we must calculate the knownfolderid
        std::string tempResolvedPath = resolvedPath.generic_u8string();
        std::string tempUnresolvedPath = unresolvedPath.generic_u8string();
        size_t first = tempUnresolvedPath.find( "<FOLDERID_" );
        size_t last = tempUnresolvedPath.find_first_of( ">" );
        out_knownFolderId = tempUnresolvedPath.substr( first, last + 1 );
        std::string remainingPath = tempUnresolvedPath.substr( last + 1, tempUnresolvedPath.length() );

        first = tempResolvedPath.find( remainingPath );

        out_knownFolderIdConversion = tempResolvedPath.substr( 0, first );
    }

    WindowsUtilities::FileSearchWithWildCard( resolvedPath, out_discoveredFiles );
    m_utf8PathVerifier.PruneInvalidPathsFromList( out_discoveredFiles );
}

void PackageDiscovery::DiscoverPackageConfigurables(
    const std::vector<PmProductDiscoveryConfigurable>& configurables,
    std::vector<PackageConfigInfo>& packageConfigs )
{
    for( auto& configurable : configurables )
    {
        std::string knownFolderId = "";
        std::string knownFolderIdConversion = "";
        std::vector<std::filesystem::path> discoveredFiles;
        bool usingDeployPath = false;

        if( !configurable.deployPath.empty() )
        {
            ResolveAndDiscover(
                configurable.unresolvedDeployPath,
                configurable.deployPath,
                knownFolderId,
                knownFolderIdConversion,
                discoveredFiles
            );

            usingDeployPath = discoveredFiles.size() > 0;
        }

        if( !usingDeployPath )
        {
            ResolveAndDiscover(
                configurable.unresolvedCfgPath,
                configurable.cfgPath,
                knownFolderId,
                knownFolderIdConversion,
                discoveredFiles
            );
        }

        if( discoveredFiles.size() > configurable.max_instances )
        {
            if( configurable.max_instances == 0 )
            {
                discoveredFiles = std::vector<std::filesystem::path>( discoveredFiles.begin(), discoveredFiles.begin() + 1 );
            }
            else
            {
                discoveredFiles = std::vector<std::filesystem::path>( discoveredFiles.begin(), discoveredFiles.begin() + configurable.max_instances );
            }
        }

        for( auto& discoveredFile : discoveredFiles )
        {
            PackageConfigInfo configInfo = {};
            bool uniqueConfigurable = true;

            std::string tempPath = discoveredFile.generic_u8string();
            if( knownFolderId != "" )
            {
                //We need to convert the path to include knownfolderid
                tempPath = tempPath.substr( knownFolderIdConversion.length(), tempPath.length() );
                tempPath = knownFolderId + tempPath;
            }

            configInfo.isDiscoveredAtDeployPath = usingDeployPath;

            if( usingDeployPath )
            {
                configInfo.deployPath = discoveredFile;
                configInfo.unresolvedDeployPath = std::filesystem::u8path( tempPath );
                configInfo.cfgPath = "";
                // Provide the original config path as this is needs to be sent in the checkin
                configInfo.unresolvedCfgPath = configurable.unresolvedCfgPath;
            }
            else
            {
                configInfo.deployPath = "";
                configInfo.unresolvedDeployPath = "";
                configInfo.cfgPath = discoveredFile;
                configInfo.unresolvedCfgPath = std::filesystem::u8path( tempPath );
            }

            for( auto& it : packageConfigs ) {
                if( usingDeployPath && ( configInfo.deployPath == it.deployPath ) ) {
                    uniqueConfigurable = false;
                    break;
                }
                else if( !configInfo.cfgPath.empty() && ( configInfo.cfgPath == it.cfgPath ) ) {
                    uniqueConfigurable = false;
                    break;
                } 
                else if( usingDeployPath && ( configInfo.deployPath == it.cfgPath ) ) {
                    WLOG_ERROR( L"Deploy path %s matches a previous config path. Bad cloud configuration", configInfo.deployPath.wstring().c_str() );
                    uniqueConfigurable = false;
                    break;
                }
                else if( !configInfo.cfgPath.empty() && ( configInfo.cfgPath == it.deployPath) ) {
                    WLOG_ERROR( L"Deploy path %s matches a previous config path. Bad cloud configuration", configInfo.cfgPath.wstring().c_str() );
                    uniqueConfigurable = false;
                    break;
                }
            }
            
            if( uniqueConfigurable ) {
                packageConfigs.push_back( configInfo );
            }
        }
    }
}
