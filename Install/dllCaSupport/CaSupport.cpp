#include "pch.h"
#include "CaSupport.h"
#include "IUcLogger.h"

extern "C" CA_SUPPORT_API void TestLogger( IUcLogger * logger )
{
    SetUcLogger( logger );
    LOG_DEBUG( "This is a test" );
}
