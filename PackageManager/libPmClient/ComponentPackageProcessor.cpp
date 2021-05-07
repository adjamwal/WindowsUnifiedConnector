#include "ComponentPackageProcessor.h"
#include "IInstallerCacheManager.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IFileUtil.h"
#include "ISslUtil.h"
#include "IPackageConfigProcessor.h"
#include "IUcidAdapter.h"
#include "ICloudEventBuilder.h"
#include "ICloudEventPublisher.h"
#include "IUcUpgradeEventHandler.h"
#include "PmLogger.h"
#include "PmConstants.h"
#include "PackageException.h"
#include "RandomUtil.h"
#include "WinError.h"
#include <sstream>
#include <iostream>
#include <vector>

ComponentPackageProcessor::ComponentPackageProcessor(
    IInstallerCacheManager& installerManager,
    IFileUtil& fileUtil,
    ISslUtil& sslUtil,
    IPackageConfigProcessor& configProcessor,
    IUcidAdapter& ucidAdapter,
    ICloudEventBuilder& eventBuilder,
    ICloudEventPublisher& eventPublisher,
    IUcUpgradeEventHandler& ucUpgradeEventHandler )
    : m_installerManager( installerManager )
    , m_fileUtil( fileUtil )
    , m_sslUtil( sslUtil )
    , m_configProcessor( configProcessor )
    , m_ucidAdapter( ucidAdapter )
    , m_eventBuilder( eventBuilder )
    , m_eventPublisher( eventPublisher )
    , m_ucUpgradeEventHandler( ucUpgradeEventHandler )
    , m_dependencies( nullptr )
    , m_fileCount( 0 )
{
}

ComponentPackageProcessor::~ComponentPackageProcessor()
{
}

void ComponentPackageProcessor::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_dependencies = dep;
    m_configProcessor.Initialize( dep );
    m_ucUpgradeEventHandler.Initialize( dep );
    m_installerManager.Initialize( dep );
}

bool ComponentPackageProcessor::HasDownloadedBinary( PmComponent& componentPackage )
{
    bool result = !componentPackage.downloadedInstallerPath.empty() &&
        m_fileUtil.FileExists( componentPackage.downloadedInstallerPath ) &&
        ( m_fileUtil.FileSize( componentPackage.downloadedInstallerPath ) > 0 );

    LOG_DEBUG( __FUNCTION__ ": Package %s, result=%d",
        componentPackage.productAndVersion.c_str(),
        result );

    return result;
}

bool ComponentPackageProcessor::HasConfigs( PmComponent& componentPackage )
{
    bool result = componentPackage.configs.size() > 0;

    LOG_DEBUG( __FUNCTION__ ": Package %s, result=%d",
        componentPackage.productAndVersion.c_str(),
        result );

    return result;
}

bool ComponentPackageProcessor::DownloadPackageBinary( PmComponent& componentPackage )
{
    componentPackage.downloadedInstallerPath = "";
    if( componentPackage.installerUrl.length() == 0 || componentPackage.installerType.length() == 0 )
    {
        return false;
    }

    std::string installerPath;

    componentPackage.downloadedInstallerPath = m_installerManager.DownloadOrUpdateInstaller( componentPackage );

    return HasDownloadedBinary( componentPackage );
}

bool ComponentPackageProcessor::ProcessPackageBinary( PmComponent& componentPackage )
{
    bool rtn = false;
    std::stringstream ssError;
    std::optional<std::string> tempSha256;
    size_t installerSize = 0;

    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_dependencies )
    {
        LOG_ERROR( __FUNCTION__ ": Dependencies not initialized" );
        return false;
    }

    if ( !componentPackage.downloadedInstallerPath.empty() ) {
        installerSize = m_fileUtil.FileSize( componentPackage.downloadedInstallerPath );
        LOG_DEBUG( __FUNCTION__ ": File %s, size %ld",
            componentPackage.downloadedInstallerPath.c_str(),
            installerSize );
    }

    m_eventPublisher.SetToken( m_ucidAdapter.GetAccessToken() );

    m_eventBuilder.Reset();
    m_eventBuilder.WithUCID( m_ucidAdapter.GetIdentity() );
    m_eventBuilder.WithPackageID( componentPackage.productAndVersion );

    bool isAlreadyInstalled = IsPackageFoundLocally( m_eventBuilder.GetPackageName(), m_eventBuilder.GetPackageVersion() );
    m_eventBuilder.WithType( isAlreadyInstalled ? CloudEventType::pkgreconfig : CloudEventType::pkginstall );

    m_eventBuilder.WithNewFile(
        componentPackage.installerUrl,
        componentPackage.installerHash,
        installerSize );

    try
    {
        if ( componentPackage.downloadedInstallerPath.empty() ) {
            ssError << "Failed to download " << componentPackage.installerUrl;
            throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_DOWNLOAD );
        }

        tempSha256 = m_sslUtil.CalculateSHA256( componentPackage.downloadedInstallerPath );
        if( !tempSha256.has_value() )
        {
            ssError << "Failed to calculate sha256 of " << componentPackage.downloadedInstallerPath;
            throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_HASH_CALC );
        }

        m_eventBuilder.WithNewFile(
            componentPackage.installerUrl,
            tempSha256.has_value() ? tempSha256.value() : componentPackage.installerHash,
            m_fileUtil.FileSize( componentPackage.downloadedInstallerPath ) );

        // only validate hash if installerHash is not empty
        if( !componentPackage.installerHash.empty() &&
            tempSha256.value() != componentPackage.installerHash )
        {
            ssError << "Failed to match hash of download. Calculated Hash: " << tempSha256.value() <<
                ", Cloud Hash: " << componentPackage.installerHash;
            throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_HASH_MISMATCH );
        }

        m_ucUpgradeEventHandler.StoreUcUpgradeEvent( m_eventBuilder.Build() );
        std::string updErrText;
        int32_t updErrCode = m_dependencies->ComponentManager().UpdateComponent( componentPackage, updErrText );


        if( ( updErrCode == ERROR_SUCCESS_REBOOT_REQUIRED || updErrCode == ERROR_SUCCESS_RESTART_REQUIRED ) && componentPackage.installerType == "msi" )
        {
            LOG_DEBUG( __FUNCTION__ ": Installer '%s' succeeded, but requires a reboot",
                componentPackage.downloadedInstallerPath.c_str() );
            componentPackage.postInstallRebootRequired = true;
            m_eventBuilder.WithError( UCPM_EVENT_SUCCESS_REBOOT_REQ, "Reboot required event" );
        }
        else if( updErrCode == ERROR_SUCCESS_REBOOT_INITIATED && componentPackage.installerType == "msi" )
        {
            LOG_DEBUG( __FUNCTION__ ": Installer '%s' succeeded, reboot initiated by msi",
                componentPackage.downloadedInstallerPath.c_str() );
            m_eventBuilder.WithError( UCPM_EVENT_SUCCESS_REBOOT_INIT, "Reboot initiated event" );
        }
        else if( updErrCode != 0 )
        {
            ssError << "Failed to update package. Error: " << updErrCode << ": " << updErrText;
            throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_UPDATE );
        }

        m_installerManager.DeleteInstaller( componentPackage.downloadedInstallerPath );

        rtn = true;
    }
    catch( PackageException& ex )
    {
        m_eventBuilder.WithError( ex.whatCode(), ex.what() );
        LOG_ERROR( __FUNCTION__ ": %s", ex.what() );
    }
    catch( std::exception& ex )
    {
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_UNDEFINED_EXCEPTION, ex.what() );
        LOG_ERROR( __FUNCTION__ ": %s", ex.what() );
    }
    catch( ... )
    {
        m_eventBuilder.WithError( UCPM_EVENT_ERROR_UNDEFINED_EXCEPTION, "Unknown processing exception" );
        LOG_ERROR( __FUNCTION__ ": Unknown processing exception" );
    }

    m_eventPublisher.Publish( m_eventBuilder );

    return rtn;
}

bool ComponentPackageProcessor::IsPackageFoundLocally( const std::string& name, const std::string& version )
{
    if( !m_dependencies ) return false;

    PackageInventory inventory;
    m_dependencies->ComponentManager().GetCachedInventory( inventory );

    for( auto item : inventory.packages )
    {
        if( item.product == name &&
            ( version.empty() || version == item.version )
            ) return true;
    }

    return false;
}

bool ComponentPackageProcessor::ProcessConfigsForPackage( PmComponent& componentPackage )
{
    int failedConfigs = 0;
    for( auto config : componentPackage.configs )
    {
        bool processed = false;

        try
        {
            LOG_DEBUG( __FUNCTION__ ": Process config %s, for package %s", config.path.c_str(), componentPackage.productAndVersion.c_str() );
            config.forProductAndVersion = componentPackage.productAndVersion;
            processed = m_configProcessor.ProcessConfig( config );
        }
        catch( ... )
        {
            LOG_ERROR( __FUNCTION__ ": Failed to process %s", config.path.c_str() );
        }

        failedConfigs += processed ? 0 : 1;
    }

    if( failedConfigs > 0 )
    {
        LOG_ERROR( __FUNCTION__ ": Failed to process %d configs for package %s", failedConfigs, componentPackage.productAndVersion.c_str() );
    }

    return failedConfigs == 0;
}
