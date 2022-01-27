// PacakgeManager.cpp : Defines the entry point for the application.
//

#include "PackageManager.h"
#include "PmLogger.h"
#include "IWorkerThread.h"
#include "IPmBootstrap.h"
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
#include "IRebootHandler.h"
#include "ICatalogJsonParser.h"
#include "PmTypes.h"
#include "IWatchdog.h"
#include "IProxyDiscovery.h"
#include "IProxyConsumer.h"
#include "PmProxyDiscoverySubscriber.h"
#include <sstream>

using namespace std;

PackageManager::PackageManager( IPmBootstrap& bootstrap,
    IPmConfig& config,
    IPmCloud& cloud,
    IInstallerCacheManager& installerCacheMgr,
    IPackageDiscoveryManager& packageDiscoveryManager,
    ICheckinFormatter& checkinFormatter,
    ICatalogJsonParser& catalogJsonParser,
    IUcidAdapter& ucidAdapter,
    ICertsAdapter& certsAdapter,
    ICheckinManifestRetriever& manifestRetriever,
    IManifestProcessor& manifestProcessor,
    ICloudEventPublisher& cloudEventPublisher,
    ICloudEventStorage& cloudEventStorage,
    IUcUpgradeEventHandler& ucUpgradeEventHandler,
    IRebootHandler& rebootHandler,
    IWorkerThread& thread,
    IWatchdog& watchdog,
    IProxyConsumer& proxyDiscoverySubscriber,
    IProxyDiscovery& proxyDiscovery )
    : m_bootstrap( bootstrap )
    , m_config( config )
    , m_cloud( cloud )
    , m_installerCacheMgr( installerCacheMgr )
    , m_packageDiscoveryManager( packageDiscoveryManager )
    , m_checkinFormatter( checkinFormatter )
    , m_catalogJsonParser( catalogJsonParser )
    , m_ucidAdapter( ucidAdapter )
    , m_certsAdapter( certsAdapter )
    , m_manifestRetriever( manifestRetriever )
    , m_manifestProcessor( manifestProcessor )
    , m_cloudEventPublisher( cloudEventPublisher )
    , m_cloudEventStorage( cloudEventStorage )
    , m_ucUpgradeEventHandler( ucUpgradeEventHandler )
    , m_rebootHandler( rebootHandler )
    , m_thread( thread )
    , m_watchdog( watchdog )
    , m_proxyDiscoverySubscriber( proxyDiscoverySubscriber )
    , m_proxyDiscovery( proxyDiscovery )
    , m_dependencies( nullptr )
    , m_useShorterInterval( false )
{

}

PackageManager::~PackageManager()
{

}

int32_t PackageManager::Start( const char* pmConfigFile, const char* pmBootstrapFile )
{
    int32_t rtn = -1;
    LOG_DEBUG( "Enter " );
    std::lock_guard<std::mutex> lock( m_mutex );

    m_pmConfigFile = pmConfigFile;
    m_pmBootstrapFile = pmBootstrapFile;

    if( !m_dependencies ) {
        LOG_ERROR( "Platform dependencies not provided. Cannot start Package Manager" );
    }
    else {
        if( !LoadPmConfig() ) {
            LOG_DEBUG( "Failed to load Pm configuration" );
        }

        if( !LoadPmBootstrap() ) {
            LOG_DEBUG( "Failed to load Pm Bootstrap" );
        }

        m_proxyDiscovery.RegisterForProxyNotifications( &m_proxyDiscoverySubscriber );
        PmCheckForProxies( false );

        m_watchdog.Start(
            std::bind( &PackageManager::PmWatchdogWait, this ),
            std::bind( &PackageManager::PmWatchdogFired, this )
        );
        m_thread.Start(
            std::bind( &PackageManager::PmThreadWait, this ),
            std::bind( &PackageManager::PmWorkflowThread, this )
        );
        rtn = 0;
    }

    LOG_DEBUG( "Exit %d", rtn );
    return rtn;
}

int32_t PackageManager::Stop()
{
    int32_t rtn = -1;
    LOG_DEBUG( "Enter " );
    std::lock_guard<std::mutex> lock( m_mutex );

    m_proxyDiscovery.UnregisterForProxyNotifications( &m_proxyDiscoverySubscriber );
    m_watchdog.Stop();

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
        m_cloud.Initialize( m_dependencies );
        m_cloud.SetUserAgent( m_dependencies->Configuration().GetHttpUserAgent() );
        m_cloud.SetShutdownFunc( [this] { return IsRunning(); } );
        m_ucUpgradeEventHandler.Initialize( m_dependencies );
        m_rebootHandler.Initialize( m_dependencies );
        m_catalogJsonParser.Initialize( m_dependencies );
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

    if( m_useShorterInterval )
    {
        LOG_DEBUG( "Using NetworkFailureRetryInterval" );
        return std::chrono::milliseconds( m_config.GetNetworkFailureRetryInterval() );
    }
    else
    {
        LOG_DEBUG( "Using CloudCheckinIntervalMs" );
        return std::chrono::milliseconds( m_config.GetCloudCheckinIntervalMs() );
    }
}

void PackageManager::PmWorkflowThread()
{
    LOG_DEBUG( "Enter " );

    if( m_config.PmConfigFileChanged( m_pmConfigFile ) && !LoadPmConfig() ) {
        LOG_ERROR( "Failed to load PM configuration" );
    }

    PackageInventory inventory;
    bool isRebootRequired = false;
    std::vector<PmProductDiscoveryRules> productDiscoveryRules;

    m_watchdog.Kick();
    PmCheckForProxies( true );

    try {
        UpdateSslCerts();
    }
    catch( std::exception& ex ) {
        LOG_ERROR( "UpdateSslCerts failed: %s", ex.what() );
        m_useShorterInterval = true;
        return;
    }
    catch( ... ) {
        LOG_ERROR( "UpdateSslCerts failed: Unknown exception" );
        m_useShorterInterval = true;
        return;
    }

    try {
        if( !m_initialUcUpgradeEventSent )
        {
            std::string token = m_ucidAdapter.GetAccessToken();
            if( !token.empty() ) {
                m_cloud.SetToken( token );
                m_cloud.SetCerts( m_certsAdapter.GetCertsList() );
                m_ucUpgradeEventHandler.PublishUcUpgradeEvent();

                m_initialUcUpgradeEventSent = true;
            }
        }
    }
    catch( std::exception& ex ) {
        m_useShorterInterval = true;
        LOG_ERROR( "Failed to send uc upgrade event: %s", ex.what() );
    }
    catch( ... ) {
        m_useShorterInterval = true;
        LOG_ERROR( "Failed to send uc upgrade event: Unknown exception" );
    }

    try {
        productDiscoveryRules = m_packageDiscoveryManager.PrepareCatalogDataset();

        m_packageDiscoveryManager.DiscoverPackages( productDiscoveryRules, inventory );
    }
    catch( std::exception& ex ) {
        LOG_ERROR( "PackageDiscovery failed: %s", ex.what() );
        m_useShorterInterval = true;
        return;
    }
    catch( ... ) {
        LOG_ERROR( "PackageDiscovery failed: Unknown exception" );
        m_useShorterInterval = true;
        return;
    }

    m_watchdog.Kick();

    try {
        std::string manifest = m_manifestRetriever.GetCheckinManifest( m_checkinFormatter.GetJson( inventory ) );

        LOG_DEBUG( "Checkin manifest: %s", manifest.c_str() );

        if( !m_manifestProcessor.ProcessManifest( manifest, isRebootRequired ) )
        {
            LOG_ERROR( "ProcessManifest failed" );
        }

        //new config might've been deployed
        if( m_config.PmConfigFileChanged( m_pmConfigFile ) && !LoadPmConfig() ) {
            LOG_DEBUG( "Failed to load PM configuration" );
        }

        m_useShorterInterval = false;
    }
    catch( std::exception& ex ) {
        m_useShorterInterval = true;
        LOG_ERROR( "Checkin failed: %s", ex.what() );
    }
    catch( ... ) {
        m_useShorterInterval = true;
        LOG_ERROR( "Checkin failed: Unknown exception" );
    }

    m_watchdog.Kick();

    try {
        LOG_DEBUG( "Post Checkin Steps" );
        m_cloudEventPublisher.PublishFailedEvents();
        m_installerCacheMgr.PruneInstallers( m_config.GetMaxFileCacheAgeS() );

        m_rebootHandler.HandleReboot( isRebootRequired );
    }
    catch( std::exception& ex ) {
        LOG_ERROR( "Post Checkin failed: %s", ex.what() );
    }
    catch( ... ) {
        LOG_ERROR( "Post Checkin failed: Unknown exception" );
    }
}

void PackageManager::PmCheckForProxies( bool discoverAsync = true )
{
    std::wstring proxyTestUrl, proxyPacURL;
    try {
        if( discoverAsync ) //async discovery
        {
            m_proxyDiscovery.StartProxyDiscoveryAsync( proxyTestUrl.c_str(), proxyPacURL.c_str() );
        }
        else //synchronous discovery
        {
            PROXY_INFO_LIST proxyList;
            m_proxyDiscovery.ProxyDiscoverAndNotifySync( proxyTestUrl.c_str(), proxyPacURL.c_str(), proxyList );
        }
    }
    catch( std::exception& ex ) {
        LOG_ERROR( "Proxy discovery error: %s", ex.what() );
    }
    catch( ... ) {
        LOG_ERROR( "Proxy discovery error: Unknown exception" );
    }
}

bool PackageManager::LoadPmConfig()
{
    return m_config.LoadPmConfig( m_pmConfigFile ) == 0;
}

bool PackageManager::LoadPmBootstrap()
{
    return m_bootstrap.LoadPmBootstrap( m_pmBootstrapFile ) == 0;
}

int32_t PackageManager::VerifyPmConfig( const char* pmConfigFile )
{
    return m_config.VerifyPmFileIntegrity( pmConfigFile );
}

std::chrono::milliseconds PackageManager::PmWatchdogWait()
{
    return std::chrono::milliseconds( m_config.GetWatchdogTimeoutMs() );
}

void PackageManager::PmWatchdogFired()
{
    LOG_CRITICAL( "Triggering Crash" );
    int* x = NULL;
    int y = *x;
    LOG_CRITICAL( ": Y is %d", y );
}

void PackageManager::UpdateSslCerts()
{
    if( m_dependencies && m_useShorterInterval ) {
        PmHttpCertList emptyCertList = { 0 };

        LOG_WARNING( "Updating SSL certs" );
        m_dependencies->Configuration().UpdateCertStoreForUrl( m_bootstrap.GetIdentifyUri() );
        m_cloud.SetCerts( emptyCertList );
        m_certsAdapter.ReloadCerts();
        m_cloud.SetCerts( m_certsAdapter.GetCertsList() );
    }
}
