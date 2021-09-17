#include <Windows.h>
#include "CscLog.h"

void _uclogDebugMessage( LOG_LEVEL logLevel, const char* pFormatString, ... )
{
    va_list args;
    va_start( args, pFormatString );

    switch( logLevel ) {
    case CSCLOG_ERROR:
        break;
    case CSCLOG_WARN:
        break;
    case CSCLOG_INFO:
        break;
    case CSCLOG_TRACE:
        break;
    }

    va_end( args );
}
