#include "pch.h"
#include "CaSupport.h"
#include "UcidFacade.h"
#include "EventFacade.h"
#include "IUcLogger.h"

extern "C" CA_SUPPORT_API void TestLogger( IUcLogger * logger )
{
    SetUcLogger( logger );
    LOG_DEBUG( "This is a test" );
}

extern "C" CA_SUPPORT_API bool CollectUCData( IUcLogger * logger, std::string & url, std::string & ucid, std::string & ucidToken )
{
    SetUcLogger( logger );
    LOG_DEBUG( __FUNCTION__": Enter" );

    UcidFacade ucidFacade;
    bool result = ucidFacade.CollectUCData( url, ucid, ucidToken );

    LOG_DEBUG( __FUNCTION__": Exit" );
    SetUcLogger( NULL );
    return result;
}

extern "C" CA_SUPPORT_API bool SendEventOnUninstallBegin( IUcLogger * logger, std::string & url, std::string & productVersion, std::string & ucid, std::string & ucidToken )
{
    SetUcLogger( logger );

    EventFacade eventFacade;
    bool result = eventFacade.SendEventOnUninstallBegin( url, productVersion, ucid, ucidToken );

    SetUcLogger( NULL );

    return result;
}

extern "C" CA_SUPPORT_API bool SendEventOnUninstallError( IUcLogger * logger, std::string & url, std::string & productVersion, std::string & ucid, std::string & ucidToken )
{
    SetUcLogger( logger );

    EventFacade eventFacade;
    bool result = eventFacade.SendEventOnUninstallError( url, productVersion, ucid, ucidToken );

    SetUcLogger( NULL );

    return result;
}

extern "C" CA_SUPPORT_API bool SendEventOnUninstallComplete( IUcLogger * logger, std::string & url, std::string & productVersion, std::string & ucid, std::string & ucidToken )
{
    SetUcLogger( logger );

    EventFacade eventFacade;
    bool result = eventFacade.SendEventOnUninstallComplete( url, productVersion, ucid, ucidToken );

    SetUcLogger( NULL );

    return result;
}
