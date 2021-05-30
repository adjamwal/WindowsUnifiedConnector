#include "pch.h"
#include "CaSupport.h"
#include "UcidFacade.h"
#include "IUcLogger.h"

extern "C" CA_SUPPORT_API void TestLogger( IUcLogger * logger )
{
    SetUcLogger( logger );
    LOG_DEBUG( "This is a test" );
}

extern "C" CA_SUPPORT_API bool GetUcidAndToken( IUcLogger * logger, std::string & ucid, std::string & ucidToken )
{
    SetUcLogger( logger );
    LOG_DEBUG( __FUNCTION__": Enter" );

    UcidFacade ucidFacade;
    bool result = ucidFacade.FetchCredentials( ucid, ucidToken );

    LOG_DEBUG( __FUNCTION__": Exit" );
    return result;
}
