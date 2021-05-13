// PacakgeManager.cpp : Defines the entry point for the application.
//

#include "PackageManager.h"
#include "PmLogger.h"
#include "IWorkerThread.h"
#include "IPmConfig.h"
#include "IPmCloud.h"
#include "IPackageDiscoveryManager.h"
#include "ICheckinFormatter.h"
#include "IUcidAdapter.h"
#include "ICertsAdapter.h"
#include "IPmHttp.h"
#include "ICheckinManifestRetriever.h"
#include "IManifestProcessor.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IPmPlatformConfiguration.h"
#include "ICloudEventPublisher.h"
#include "ICloudEventStorage.h"
#include "IUcUpgradeEventHandler.h"
#include "IInstallerCacheManager.h"
#include "PmTypes.h"
#include <sstream>

using namespace std;

PackageManager::PackageManager( IPmConfig& config,
    IPmCloud& cloud,
    IInstallerCacheManager& installerCacheMgr,
    IPackageDiscoveryManager& packageDiscoveryManager,
    ICheckinFormatter& checkinFormatter,
    IUcidAdapter& ucidAdapter,
    ICertsAdapter& certsAdapter,
    ICheckinManifestRetriever& manifestRetriever,
    IManifestProcessor& manifestProcessor,
    ICloudEventPublisher& cloudEventPublisher,
    ICloudEventStorage& cloudEventStorage,
    IUcUpgradeEventHandler& ucUpgradeEventHandler,
    IWorkerThread& thread ) :
    m_config( config )
    , m_cloud( cloud )
    , m_installerCacheMgr( installerCacheMgr )
    , m_packageDiscoveryManager( packageDiscoveryManager )
    , m_checkinFormatter( checkinFormatter )
    , m_ucidAdapter( ucidAdapter )
    , m_certsAdapter( certsAdapter )
    , m_manifestRetriever( manifestRetriever )
    , m_manifestProcessor( manifestProcessor )
    , m_cloudEventPublisher( cloudEventPublisher )
    , m_cloudEventStorage( cloudEventStorage )
    , m_ucUpgradeEventHandler( ucUpgradeEventHandler )
    , m_thread( thread )
    , m_dependencies( nullptr )
{

}

PackageManager::~PackageManager()
{

}

int32_t PackageManager::Start( const char* bsConfigFile, const char* pmConfigFile )
{
    int32_t rtn = -1;
    LOG_DEBUG( "Enter " );
    std::lock_guard<std::mutex> lock( m_mutex );

    m_bsConfigFile = bsConfigFile;
    m_pmConfigFile = pmConfigFile;

    if( !m_dependencies ) {
        LOG_ERROR( "Platform dependencies not provided. Cannot start Package Manager" );
    }
    else {
        if( !LoadPmConfig() ) {
            LOG_DEBUG( "Failed to load Pm configuration" );
        }

        if( !LoadBsConfig() ) {
            LOG_ERROR( "Failed to load Bs configuration" );
        }
        else {
            std::string token = m_ucidAdapter.GetAccessToken();
            if( !token.empty() ) {
                m_cloud.SetToken( token );
                m_cloud.SetCerts( m_certsAdapter.GetCertsList() );
                m_ucUpgradeEventHandler.PublishUcUpgradeEvent();
            }

            m_thread.Start(
                std::bind( &PackageManager::PmThreadWait, this ),
                std::bind( &PackageManager::PmWorkflowThread, this )
            );
            rtn = 0;
        }
    }

    LOG_DEBUG( "Exit %d", rtn );
    return rtn;
}

int32_t PackageManager::Stop()
{
    int32_t rtn = -1;
    LOG_DEBUG( "Enter " );
    std::lock_guard<std::mutex> lock( m_mutex );

    if( !m_thread.IsRunning() ) {
        LOG_DEBUG( "Package Manager is not running" );
    }
    else {
        m_thread.Stop();
        rtn = 0;
    }

    LOG_DEBUG( "Exit %d", rtn );
    return rtn;
}

bool PackageManager::IsRunning()
{
    std::lock_guard<std::mutex> lock( m_mutex );
    return m_thread.IsRunning();
}

void PackageManager::SetPlatformDependencies( IPmPlatformDependencies* dependecies )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_dependencies = dependecies;

    try
    {
        m_ucidAdapter.Initialize( m_dependencies );
        m_certsAdapter.Initialize( m_dependencies );
        m_manifestProcessor.Initialize( m_dependencies );
        m_packageDiscoveryManager.Initialize( m_dependencies );
        m_cloudEventStorage.Initialize( m_dependencies );
        m_cloud.SetUserAgent( m_dependencies->Configuration().GetHttpUserAgent() );
        m_cloud.SetShutdownFunc( [this] { return !IsRunning(); } );
        m_ucUpgradeEventHandler.Initialize( m_dependencies );
    }
    catch( std::exception& ex )
    {
        LOG_ERROR( "Initialization failed: %s", ex.what() );
    }
}

std::chrono::milliseconds PackageManager::PmThreadWait()
{
    if( m_config.PmConfigFileChanged( m_pmConfigFile ) && !LoadPmConfig() ) {
        LOG_DEBUG( "Failed to load modified Pm configuration" );
    }

    return std::chrono::milliseconds( m_config.GetCloudCheckinIntervalMs() );
}

void PackageManager::PmWorkflowThread()
{
    LOG_DEBUG( "Enter " );

    if( m_config.PmConfigFileChanged( m_pmConfigFile ) && !LoadPmConfig() ) {
        LOG_ERROR( "Failed to load PM configuration" );
    }

    PackageInventory inventory;
    bool isRebootRequired = false;

    try {
        m_packageDiscoveryManager.DiscoverPackages( inventory );
    }
    catch( std::exception& ex ) {
        LOG_ERROR( "PackageDiscovery failed: %s", ex.what() );
        return;
    }
    catch ( ... ) {
        LOG_ERROR( "PackageDiscovery failed: Unkown expcetion" );
        return;
    }

    try {
        std::string manifest = m_manifestRetriever.GetCheckinManifestFrom(
            m_config.GetCloudCheckinUri(),
            m_checkinFormatter.GetJson( inventory )
        );

        LOG_DEBUG( "Checkin manifest: %s", manifest.c_str() );

        if( !m_manifestProcessor.ProcessManifest( manifest, isRebootRequired ) )
        {
            LOG_ERROR( "ProcessManifest failed" );
        }

        //new config might've been deployed
        if( m_config.PmConfigFileChanged( m_pmConfigFile ) && !LoadPmConfig() ) {
            LOG_DEBUG( "Failed to load PM configuration" );
        }
    }
    catch( std::exception& ex ) {
        LOG_ERROR( "Checkin failed: %s", ex.what() );
    }
    catch ( ... ) {
        LOG_ERROR( "Checkin failed: Unknown exception" );
    }

    try {
        LOG_DEBUG( "Post Checkin Steps" );
        m_cloudEventPublisher.PublishFailedEvents();
        m_installerCacheMgr.PruneInstallers( m_config.GetMaxFileCacheAge() );

        if( isRebootRequired )
        {
            if( m_config.AllowPostInstallReboots() ) 
            {
                m_dependencies->ComponentManager().InitiateSystemRestart();
            }
            else
            { 
                LOG_ERROR( "Post-install reboots disabled by PM configuration" );
            }
        }
    }
    catch ( std::exception& ex ) {
        LOG_ERROR( "Post Checkin failed: %s", ex.what() );
    }
    catch ( ... ) {
        LOG_ERROR( "Post Checkin failed: Unknown exception" );
    }
}

bool PackageManager::LoadBsConfig()
{
    return m_config.LoadBsConfig( m_bsConfigFile ) == 0;
}

bool PackageManager::LoadPmConfig()
{
    return m_config.LoadPmConfig( m_pmConfigFile ) == 0;
}

bool PackageManager::PmSendEvent( const PmEvent& event )
{
    return false;
}

int32_t PackageManager::VerifyBsConfig( const char* bsConfigFile )
{
    return m_config.VerifyBsFileIntegrity( bsConfigFile );
}

int32_t PackageManager::VerifyPmConfig( const char* pmConfigFile )
{
    return m_config.VerifyPmFileIntegrity( pmConfigFile );
}
