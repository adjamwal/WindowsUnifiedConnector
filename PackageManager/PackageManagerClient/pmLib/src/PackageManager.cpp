// PacakgeManager.cpp : Defines the entry point for the application.
//

#include "PackageManager.h"
#include "PmVersion.h"
#include "PmLogger.h"
#include "IWorkerThread.h"
#include "IPmConfig.h"
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
    IPackageInventoryProvider& packageInventoryProvider,
    ICheckinFormatter& checkinFormatter,
    ITokenAdapter& tokenAdapter,
    ICertsAdapter& certsAdapter,
    ICheckinManifestRetriever& manifestRetriever,
    IManifestProcessor& manifestProcessor,
    IWorkerThread& thread ) :
    m_config( config )
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

int32_t PackageManager::Start( const char* configFile )
{
    int32_t rtn = -1;
    LOG_DEBUG( "Enter " );
    std::lock_guard<std::mutex> lock( m_mutex );

    m_configFilename = configFile;

    if( !m_dependencies ) {
        LOG_ERROR( "Platform dependencies not provided. Cannot start Package Manager" );
    }
    else {
        if( !PmLoadConfig() ) {
            LOG_ERROR( "Failed to load PM configuration" );
        }
        else if( PmThreadWait() == std::chrono::microseconds( 0 ) ) {
            LOG_ERROR( "PM Interval not configured" );
        }
        else {
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

void PackageManager::SetPlatformDependencies( IPmPlatformDependencies* dependecies )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_dependencies = dependecies;

    try
    {
        m_tokenAdapter.Initialize( m_dependencies );
        m_certsAdapter.Initialize( m_dependencies );
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

    if( !PmLoadConfig() ) {
        LOG_ERROR( "Failed to load PM configuration" );
        //Send event? might fail without a config/cloudURL
    }

    try
    {
        PackageInventory inventory( { 0 } );
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

bool PackageManager::PmLoadConfig()
{
    return m_config.Load( m_configFilename ) == 0;
}

bool PackageManager::PmSendEvent( const PmEvent& event )
{
    return false;
}

int32_t PackageManager::VerifyPacManConfig( const char* configFile )
{
    return m_config.VerifyFileIntegrity( configFile );
}
