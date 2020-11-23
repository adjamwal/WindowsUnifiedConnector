// PacakgeManager.cpp : Defines the entry point for the application.
//

#include "PackageManager.h"
#include "PmVersion.h"
#include "PmLogger.h"
#include "IWorkerThread.h"
#include "IPmConfig.h"
#include "IPmCloud.h"
#include "PackageInventoryProvider.h"
#include "CheckinFormatter.h"
#include "TokenAdapter.h"
#include "CertsAdapter.h"
#include "CheckinManifestRetriever.h"
#include "ManifestProcessor.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IPmPlatformConfiguration.h"
#include "PmTypes.h"

using namespace std;

void PmVersion( int& major, int& minor )
{
    major = PackageManager_VERSION_MAJOR;
    minor = PackageManager_VERSION_MINOR;
}

PackageManager::PackageManager( IPmConfig& config,
    IPmCloud& cloud,
    IPackageInventoryProvider& packageInventoryProvider,
    ICheckinFormatter& checkinFormatter,
    ITokenAdapter& tokenAdapter,
    ICertsAdapter& certsAdapter,
    ICheckinManifestRetriever& manifestRetriever,
    IManifestProcessor& manifestProcessor,
    IWorkerThread& thread ) :
    m_config( config )
    , m_cloud( cloud )
    , m_packageInventoryProvider( packageInventoryProvider )
    , m_checkinFormatter( checkinFormatter )
    , m_tokenAdapter( tokenAdapter )
    , m_certsAdapter( certsAdapter )
    , m_manifestRetriever( manifestRetriever )
    , m_manifestProcessor( manifestProcessor )
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
        if ( !LoadPmConfig() ) {
            LOG_DEBUG( "Failed to load Pm configuration" );
        }

        if( !LoadBsConfig() ) {
            LOG_ERROR( "Failed to load Bs configuration" );
        }
        else if( PmThreadWait() == std::chrono::microseconds( 0 ) ) {
            LOG_ERROR( "PM Interval not configured" );
        }
        else {
            SetupDiscoveryPackages();

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
        m_tokenAdapter.Initialize( m_dependencies );
        m_certsAdapter.Initialize( m_dependencies );
        m_manifestProcessor.Initialize( m_dependencies );
        m_packageInventoryProvider.Initialize( m_dependencies );
        m_cloud.SetUserAgent( m_dependencies->Configuration().GetHttpUserAgent() );
        m_cloud.SetShutdownFunc( [this] { return !IsRunning(); } );
    }
    catch( std::exception& ex )
    {
        LOG_ERROR( "Initialization failed: %s", ex.what() );
    }
}

std::chrono::milliseconds PackageManager::PmThreadWait()
{
    return std::chrono::milliseconds( m_config.GetCloudInterval() );
}

void PackageManager::PmWorkflowThread()
{
    LOG_DEBUG( "Enter " );

    if ( !LoadPmConfig() ) {
        LOG_ERROR( "Failed to load PM configuration" );
        //Send event? might fail without a config/cloudURL
    }

    try
    {
        PackageInventory inventory;
        m_packageInventoryProvider.GetInventory( inventory );

        std::string manifest = m_manifestRetriever.GetCheckinManifestFrom(
            m_config.GetCloudUri(),
            m_checkinFormatter.GetJson( inventory )
        );

        LOG_DEBUG( "Checkin manifest: %s", manifest.c_str() );
        m_manifestProcessor.ProcessManifest( manifest );
    }
    catch( std::exception& ex )
    {
        LOG_ERROR( "Checkin failed: %s", ex.what() );
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

void PackageManager::SetupDiscoveryPackages()
{
    //TODO: This should be fetched from the cloud for enterprise
    PmDiscoveryComponent discoveryItem;
    discoveryItem.packageId = "amp";
    discoveryItem.packageName = "Immunet";
    m_discoveryList.push_back( discoveryItem );

    discoveryItem.packageId = "amp";
    discoveryItem.packageName = "Cisco AMP for Endpoints Connector";
    m_discoveryList.push_back( discoveryItem );

    discoveryItem.packageId = "test-package-1";
    discoveryItem.packageName = "TestPackage";
    m_discoveryList.push_back( discoveryItem );

    discoveryItem.packageId = "test-package-2";
    discoveryItem.packageName = "TestPackage";
    m_discoveryList.push_back( discoveryItem );

    discoveryItem.packageId = "test-package-3";
    discoveryItem.packageName = "TestPackage";
    m_discoveryList.push_back( discoveryItem );

    discoveryItem.packageId = "test-package-4";
    discoveryItem.packageName = "TestPackage";
    m_discoveryList.push_back( discoveryItem );

    m_packageInventoryProvider.SetDiscoveryList( m_discoveryList );
}