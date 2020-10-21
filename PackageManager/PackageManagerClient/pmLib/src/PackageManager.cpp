// PacakgeManager.cpp : Defines the entry point for the application.
//

#include "PackageManager.h"
#include "PmVersion.h"
#include "PmLogger.h"
#include "IWorkerThread.h"
#include "IPmConfig.h"
#include "IPmCloud.h"
#include "IPmManifest.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IPmPlatformConfiguration.h"
#include "PmTypes.h"

using namespace std;

void PmVersion( int& major, int &minor)
{
    major = PackageManager_VERSION_MAJOR;
    minor = PackageManager_VERSION_MINOR;
}

PackageManager::PackageManager( IPmConfig& config, IPmCloud& cloud, IPmManifest& manifest, IWorkerThread& thread ) :
    m_config( config )
    , m_cloud( cloud )
    , m_manifest( manifest )
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

    if( !PmLoadPackageList() ) {
        LOG_ERROR( "Failed to load PM PackageList" );
    }
    
    //get ucid token movee this code to where appropriately needed
    std::string token;
    if ( m_dependencies->Configuration().GetIdentityToken( token ) )
    {
        LOG_ERROR( "GetIdentityToken: %s", token.c_str() );
    }

    if( !PmCheckin() ) {
        LOG_ERROR( "Package Manager Checkin failed" );
    }

    PmPackage package;
    package.Type = PackageType::MSI;
    package.Path = "C:\\dev\\WindowsUnifiedConnector\\Install\\Installer\\bin\\x64\\Release\\en-US\\";
    package.Name = "Cisco-UC-Installer-x64.msi";
    package.CmdLine = "";

    m_dependencies->ComponentManager().UpdateComponent( package );
}

bool PackageManager::PmLoadConfig()
{
    return m_config.Load( m_configFilename ) == 0;
}

bool PackageManager::PmLoadPackageList()
{
    //Initial implementation
    //Load packages from file

    //For release this should discover installed packages
    //If empty then add one package... The package manager

    return false;
}

bool PackageManager::PmCheckin()
{
    bool rtn = false;
    std::string payload, response;
    //Convert m_packages to string;

    if( m_cloud.Checkin( payload, response ) != 0 ) {
        LOG_ERROR( "cloud checkin failed" );
        //PmSendEvent() failed checkin request
    }
    else {
        if( PmProcessManifest( response ) != 0 ) {
            LOG_ERROR( "process manifest failed" );
        }
        else {
            rtn = true;
        }
    }

    return rtn;
}

bool PackageManager::PmProcessComponent( const PmComponent& component )
{
    return false;
}

bool PackageManager::PmProcessManifest( const std::string& manifest )
{
    bool rtn = true;
    if( m_manifest.ParseManifest( manifest ) != 0 ) {
        LOG_ERROR( "process manifest failed" );
        //PmSendEvent() bad manifest
        rtn = false;
    }
    else {
        for( auto package : m_manifest.GetPackageList() ) {
            if( PmProcessComponent( package ) != 0 ) {
                LOG_ERROR( "process package failed" );
                //PmSendEvent() failed
                rtn = false;
            }
            else {
                //PmSendEvent() success
            }
        }
    }
    return false;
}

bool PackageManager::PmSendEvent( const PmEvent& event )
{
    return false;
}
