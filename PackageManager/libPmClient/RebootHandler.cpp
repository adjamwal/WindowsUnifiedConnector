#include "RebootHandler.h"
#include "IPmConfig.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformComponentManager.h"
#include "IUcLogger.h"

RebootHandler::RebootHandler( IPmConfig& config ) :
    m_config( config )
    , m_dependencies( nullptr )
    , m_rebootRequired( false )
    , m_lastRebootNotification( 0 )
{
}

RebootHandler::~RebootHandler()
{

}

void RebootHandler::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    m_dependencies = dep;
}

bool RebootHandler::HandleReboot( bool rebootRequired )
{
    std::lock_guard<std::mutex> lock( m_mutex );
    
    if( !m_dependencies ) {
        return false;
    }

    m_rebootRequired = m_rebootRequired || rebootRequired;

    if( m_rebootRequired ) {
        if( !m_config.AllowPostInstallReboots() ) {
            LOG_DEBUG( "Reboots disabled by config" );
        }
        else if( !IsRebootThrottled() ) {
            m_lastRebootNotification = time( NULL );
            m_dependencies->ComponentManager().NotifySystemRestart();
        }
    }

    return true;
}

bool RebootHandler::IsRebootThrottled()
{
    bool rtn = false;

    uint32_t throttle_s = m_config.GetRebootThrottleS();

    if( ( m_lastRebootNotification + throttle_s ) >= time( NULL ) ) {
        LOG_DEBUG( "Reboots throttled until %d", m_lastRebootNotification + throttle_s );
        rtn = true;
    }

    return rtn;
}