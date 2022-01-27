#include "ComponentPackageProcessor.h"
#include "IInstallerCacheManager.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IFileSysUtil.h"
#include "ISslUtil.h"
#include "IPackageConfigProcessor.h"
#include "IUcidAdapter.h"
#include "ICloudEventBuilder.h"
#include "ICloudEventPublisher.h"
#include "IUcUpgradeEventHandler.h"
#include "IWatchdog.h"
#include "PmLogger.h"
#include "PmConstants.h"
#include "PackageException.h"
#include "RandomUtil.h"
#include "WinError.h"
#include "StringUtil.h"
#include <sstream>
#include <iostream>
#include <vector>

ComponentPackageProcessor::ComponentPackageProcessor(
    IInstallerCacheManager& installerManager,
    IFileSysUtil& fileUtil,
    ISslUtil& sslUtil,
    IPackageConfigProcessor& configProcessor,
    IUcidAdapter& ucidAdapter,
    ICloudEventBuilder& eventBuilder,
    ICloudEventPublisher& eventPublisher,
    IUcUpgradeEventHandler& ucUpgradeEventHandler,
    IWatchdog& watchdog )
    : m_installerManager( installerManager )
    , m_fileUtil( fileUtil )
    , m_sslUtil( sslUtil )
    , m_configProcessor( configProcessor )
    , m_ucidAdapter( ucidAdapter )
    , m_eventBuilder( eventBuilder )
    , m_eventPublisher( eventPublisher )
    , m_ucUpgradeEventHandler( ucUpgradeEventHandler )
    , m_watchdog( watchdog )
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

bool ComponentPackageProcessor::PreDownloadedBinaryExists( PmComponent& componentPackage )
{
    bool result = !componentPackage.downloadedInstallerPath.empty() &&
        m_fileUtil.FileExists( componentPackage.downloadedInstallerPath ) &&
        ( m_fileUtil.FileSize( componentPackage.downloadedInstallerPath ) > 0 &&
            componentPackage.downloadErrorMsg.empty() );

    LOG_DEBUG( "Package %s, result=%d",
        componentPackage.productAndVersion.c_str(),
        result );

    return result;
}

bool ComponentPackageProcessor::HasConfigs( PmComponent& componentPackage )
{
    bool result = componentPackage.configs.size() > 0;

    LOG_DEBUG( "Package %s, result=%d",
        componentPackage.productAndVersion.c_str(),
        result );

    return result;
}

bool ComponentPackageProcessor::DownloadPackageBinary( PmComponent& componentPackage )
{
    bool rtn = false;
    componentPackage.downloadedInstallerPath = "";
    if( componentPackage.installerUrl.length() != 0 && componentPackage.installerType.length() != 0 ) {


        std::string installerPath;
        std::stringstream ssError;
        ssError << "Package " << componentPackage.productAndVersion << ": ";

        try {
            componentPackage.downloadedInstallerPath = m_installerManager.DownloadOrUpdateInstaller( componentPackage );
            LOG_DEBUG( "Downloaded: %s", componentPackage.downloadedInstallerPath.generic_u8string().c_str() );
        }
        catch( PackageException& ex ) {
            ssError << ex.what();
            componentPackage.downloadErrorMsg = ssError.str();
            componentPackage.downloadSubError = ex.whatSubError();
        }
        catch( ... ) {
            ssError << "Unknown exception while pre-downloading " << componentPackage.installerUrl;
            componentPackage.downloadErrorMsg = ssError.str();
        }

        rtn = PreDownloadedBinaryExists( componentPackage );
    }

    m_watchdog.Kick();
    return rtn;
}

bool ComponentPackageProcessor::ProcessPackageBinary( PmComponent& componentPackage )
{
    bool rtn = false;
    std::stringstream ssError;
    std::optional<std::string> tempSha256;
    uint64_t installerSize = 0;

    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_dependencies ) {
        LOG_ERROR( "Dependencies not initialized" );
        rtn = false;
    }
    else if( componentPackage.installerUrl.length() == 0 || componentPackage.installerType.length() == 0 ) {
        //nothing to install for config-only packages (i.e. packages that don't have an installerUrl)
        //return success, to ensure configuration gets processed
        rtn = true;
    }
    else {
        if( !componentPackage.downloadedInstallerPath.empty() ) {
            installerSize = m_fileUtil.FileSize( componentPackage.downloadedInstallerPath );
            LOG_DEBUG( "File %s, size %ld",
                componentPackage.downloadedInstallerPath.generic_u8string().c_str(),
                installerSize );
        }

        m_eventPublisher.SetToken( m_ucidAdapter.GetAccessToken() );

        m_eventBuilder.Reset();
        m_eventBuilder.WithUCID( m_ucidAdapter.GetIdentity() );
        m_eventBuilder.WithPackageID( componentPackage.productAndVersion );
        m_eventBuilder.WithType( CloudEventType::pkginstall );

        std::string localVersion;
        if( IsPackageFoundLocally( m_eventBuilder.GetPackageName(), m_eventBuilder.GetPackageVersion(), localVersion ) ) {
            m_eventBuilder.WithFrom( localVersion );
        }

        m_eventBuilder.WithNewFile(
            componentPackage.installerUrl,
            componentPackage.installerHash,
            installerSize );

        try {
            if( !PreDownloadedBinaryExists( componentPackage ) ) {
                if( !componentPackage.downloadErrorMsg.empty() )
                {
                    ssError << componentPackage.downloadErrorMsg;
                }
                else
                {
                    ssError << "Failed to pre-download " << componentPackage.installerUrl;
                }
                throw PackageException(
                    ssError.str(),
                    UCPM_EVENT_ERROR_COMPONENT_DOWNLOAD,
                    componentPackage.downloadSubError
                );
            }

            tempSha256 = m_sslUtil.CalculateSHA256( componentPackage.downloadedInstallerPath );
            if( !tempSha256.has_value() ) {
                ssError << "Failed to calculate sha256 of " << componentPackage.downloadedInstallerPath;
                throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_HASH_CALC );
            }

            m_eventBuilder.WithNewFile(
                componentPackage.installerUrl,
                tempSha256.has_value() ? tempSha256.value() : componentPackage.installerHash,
                m_fileUtil.FileSize( componentPackage.downloadedInstallerPath ) );

            // only validate hash if installerHash is not empty
            if( !componentPackage.installerHash.empty() &&
                tempSha256.value() != componentPackage.installerHash ) {
                ssError << "Failed to match hash of download. Calculated Hash: " << tempSha256.value() <<
                    ", Cloud Hash: " << componentPackage.installerHash;
                throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_HASH_MISMATCH );
            }

            m_ucUpgradeEventHandler.StoreUcUpgradeEvent( m_eventBuilder.Build() );
            std::string updErrText;
            int32_t updErrCode = m_dependencies->ComponentManager().UpdateComponent( componentPackage, updErrText );


            if( ( updErrCode == ERROR_SUCCESS_REBOOT_REQUIRED || updErrCode == ERROR_SUCCESS_RESTART_REQUIRED ) && componentPackage.installerType == "msi" ) {
                LOG_DEBUG( "Installer '%s' succeeded, but requires a reboot",
                    componentPackage.downloadedInstallerPath.generic_u8string().c_str() );
                componentPackage.postInstallRebootRequired = true;
                m_eventBuilder.WithError( UCPM_EVENT_SUCCESS_REBOOT_REQ, "Reboot required event" );
            }
            else if( updErrCode == ERROR_SUCCESS_REBOOT_INITIATED && componentPackage.installerType == "msi" ) {
                LOG_DEBUG( "Installer '%s' succeeded, reboot initiated by msi",
                    componentPackage.downloadedInstallerPath.generic_u8string().c_str() );
                m_eventBuilder.WithError( UCPM_EVENT_SUCCESS_REBOOT_INIT, "Reboot initiated event" );
            }
            else if( updErrCode != 0 ) {
                ssError << "Failed to update package. Error: " << updErrCode << ": " << updErrText;
                throw PackageException( ssError.str(), UCPM_EVENT_ERROR_COMPONENT_UPDATE );
            }
            else {
                LOG_DEBUG( "Installer '%s' succeeded",
                    componentPackage.downloadedInstallerPath.generic_u8string().c_str() );
            }

            m_installerManager.DeleteInstaller( componentPackage.downloadedInstallerPath );

            rtn = true;
        }
        catch( PackageException& ex ) {
            m_eventBuilder.WithError( ex.whatCode(), ex.what() );
            m_eventBuilder.WithSubError( ex.whatSubError().subErrorCode, ex.whatSubError().subErrorType );
            LOG_ERROR( "%s", ex.what() );
        }
        catch( std::exception& ex ) {
            m_eventBuilder.WithError( UCPM_EVENT_ERROR_UNDEFINED_EXCEPTION, ex.what() );
            LOG_ERROR( "%s", ex.what() );
        }
        catch( ... ) {
            m_eventBuilder.WithError( UCPM_EVENT_ERROR_UNDEFINED_EXCEPTION, "Unknown processing exception" );
            LOG_ERROR( "Unknown processing exception" );
        }

        m_eventPublisher.Publish( m_eventBuilder );
    }

    m_watchdog.Kick();
    return rtn;
}

bool ComponentPackageProcessor::IsPackageFoundLocally( const std::string& name, const std::string& version, std::string& localVersion )
{
    if( !m_dependencies ) return false;

    PackageInventory inventory;
    m_dependencies->ComponentManager().GetCachedInventory( inventory );

    for( auto item : inventory.packages )
    {
        if( StringUtil::EqualsIgnoreCase( name, item.product ) )
        {
            LOG_DEBUG( "Package match: '%s', '%s'", name.c_str(), version.c_str() );
            localVersion = item.version;
            return true;
        }
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
            LOG_DEBUG( "Process config %s, for package %s", 
                config.deployPath.empty() ? config.cfgPath.generic_u8string().c_str() : config.deployPath.generic_u8string().c_str(),
                componentPackage.productAndVersion.c_str() );
            config.forProductAndVersion = componentPackage.productAndVersion;
            processed = m_configProcessor.ProcessConfig( config );
        }
        catch( std::exception& e )
        {
            LOG_ERROR( "Failed to process %s, %s", 
                config.deployPath.empty() ? config.cfgPath.generic_u8string().c_str() : config.deployPath.generic_u8string().c_str(),
                e.what() );
        }

        failedConfigs += processed ? 0 : 1;
    }

    if( failedConfigs > 0 )
    {
        LOG_ERROR( "Failed to process %d configs for package %s", failedConfigs, componentPackage.productAndVersion.c_str() );
    }

    return failedConfigs == 0;
}
