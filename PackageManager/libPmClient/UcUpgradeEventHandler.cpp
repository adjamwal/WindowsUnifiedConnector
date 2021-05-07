#include "UcUpgradeEventHandler.h"
#include "CloudEventBuilder.h"
#include "ICloudEventPublisher.h"
#include "ICloudEventStorage.h"
#include "IPmPlatformDependencies.h"
#include "IPmPlatformConfiguration.h"
#include "PmConstants.h"

UcUpgradeEventHandler::UcUpgradeEventHandler( ICloudEventPublisher& cloudEventPublisher, 
    ICloudEventStorage& ucUpgradeEventStorage, 
    ICloudEventBuilder& eventBuilder ) :
    m_cloudEventPublisher( cloudEventPublisher )
    , m_ucUpgradeEventStorage( ucUpgradeEventStorage )
    , m_eventBuilder( eventBuilder )
    , m_dependencies( nullptr )
{

}

UcUpgradeEventHandler::~UcUpgradeEventHandler()
{

}

void UcUpgradeEventHandler::Initialize( IPmPlatformDependencies* dep )
{
    std::lock_guard<std::mutex> lock( m_mutex );

    m_dependencies = dep;
    m_ucUpgradeEventStorage.Initialize( dep );
}

bool UcUpgradeEventHandler::StoreUcUpgradeEvent( const std::string& event )
{
    bool rtn = false;
    std::lock_guard<std::mutex> lock( m_mutex );

    if ( !m_dependencies ) {
        return rtn;
    }

    m_eventBuilder.FromJson( event );
    if ( m_eventBuilder.GetPackageName() == UC_PACKAGE_NAME ) {
        //Clear Events
        auto previousEvents = m_ucUpgradeEventStorage.ReadAndRemoveEvents();
        rtn = m_ucUpgradeEventStorage.SaveEvent( event );
    }

    return rtn;
}

bool UcUpgradeEventHandler::PublishUcUpgradeEvent()
{
    bool rtn = false;
    std::lock_guard<std::mutex> lock( m_mutex );

    if ( !m_dependencies ) {
        return rtn;
    }

    std::vector<std::string> events = m_ucUpgradeEventStorage.ReadAndRemoveEvents();
    for ( auto&& e : events ) {
        m_eventBuilder.FromJson( e );

        if ( m_eventBuilder.GetPackageVersion() != m_dependencies->Configuration().GetPmVersion() ) {
            m_eventBuilder.WithError( UCPM_EVENT_ERROR_COMPONENT_UC_UPDATE, "Unified Connector failed to upgrade" );
        }

        int publishResultCode = m_cloudEventPublisher.Publish( m_eventBuilder );
        rtn |= publishResultCode == 200;
    }

    return rtn;
}