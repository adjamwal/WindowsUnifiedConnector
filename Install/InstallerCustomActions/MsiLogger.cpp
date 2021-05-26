#include "stdafx.h"
#include "MsiLogger.h"
#include <time.h>
#include <mutex>
#include <Windows.h>
#include <Msi.h>

MsiLogger::MsiLogger() :
    m_logLevel( LOG_DEBUG )
{
}

MsiLogger::~MsiLogger()
{
}

void MsiLogger::SetLogLevel( Severity logLevel )
{
    if ( ( logLevel >= LOG_EMERGENCY ) && ( logLevel <= LOG_DEBUG ) ) {
        if ( m_logLevel != logLevel ) {
            m_logLevel = logLevel;
            Log( m_logLevel, "Set Debug Level to %d", m_logLevel );
        }
    }
    else {
        Log( LOG_ERROR, "Invalid Debug level %d", logLevel );
    }
}

void MsiLogger::Log( Severity serverity, const char* msgFormatter, ... )
{
    va_list  args;
    va_start( args, msgFormatter );

    Log( serverity, msgFormatter, args );

    va_end( args );
}

void MsiLogger::Log( Severity serverity, const wchar_t* msgFormatter, ... )
{
    va_list  args;
    va_start( args, msgFormatter );

    Log( serverity, msgFormatter, args );

    va_end( args );
}

void MsiLogger::Log( Severity serverity, const char* msgFormatter, va_list args )
{
    if ( serverity <= LOG_ERROR ) {
        LogWithError( serverity, msgFormatter, args );
    }
    else if ( serverity <= m_logLevel ) {
        size_t length = _vscprintf( msgFormatter, args ) + 1;   // vsnprintf returns 1 character less???
        char* logLine = ( char* )calloc( 1, length + 1 );

        if ( logLine ) {
            char tstr[ 32 ] = {};
            time_t now = time( NULL );
            struct tm tm;
            localtime_s( &tm, &now );
            strftime( tstr, sizeof( tstr ), "%b %d %H:%M:%S", &tm );

            vsnprintf_s( logLine, length, _TRUNCATE, msgFormatter, args );
            WcaLog( LOGMSG_STANDARD, "%s: %s", tstr, logLine );

            free( logLine );
        }
        else {
            WcaLogError( LOGMSG_STANDARD, __FUNCTION__ "calloc failed" );
        }
    }
}

void MsiLogger::Log( Severity serverity, const wchar_t* msgFormatter, va_list args )
{
    if ( serverity <= LOG_ERROR ) {
        LogWithError( serverity, msgFormatter, args );
    }
    else if ( serverity <= m_logLevel ) {
        size_t length = _vscwprintf( msgFormatter, args ) + 1;   // vsnprintf returns 1 character less???
        wchar_t* logLine = ( wchar_t* )calloc( 1, ( sizeof( wchar_t ) * ( length + 1 ) ) );

        if ( logLine ) {
            char tstr[ 32 ] = {};
            time_t now = time( NULL );
            struct tm tm;
            localtime_s( &tm, &now );
            strftime( tstr, sizeof( tstr ), "%b %d %H:%M:%S", &tm );

            _vsnwprintf_s( logLine, length, _TRUNCATE, msgFormatter, args );

            WcaLog( LOGMSG_STANDARD, "%s: %S", tstr, logLine );

            free( logLine );
        }
        else {
            WcaLogError( LOGMSG_STANDARD, __FUNCTION__ "calloc failed" );
        }
    }
}

void MsiLogger::LogWithError( Severity level, const char* msgFormatter, va_list args )
{
    DWORD lastError = GetLastError();
    char ebuffer[ 256 ] = { 0 };

    /*
    * Retrieve Windows error code and format it, convert to char *
    */
    if ( lastError ) {
        FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            lastError,
            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
            ebuffer,
            sizeof( ebuffer ) / sizeof( char ),
            NULL );

        // Reset the last error value to zero
        SetLastError( 0 );
    }

    size_t length = _vscprintf( msgFormatter, args ) + 1;   // vsnprintf returns 1 character less???
    length += sizeof( ebuffer );                                    // Add space for GetLastError
    char* logLine = ( char* )calloc( 1, length + 1 );

    if ( logLine ) {
        char tstr[ 32 ] = {};
        time_t now = time( NULL );
        struct tm tm;
        localtime_s( &tm, &now );
        strftime( tstr, sizeof( tstr ), "%b %d %H:%M:%S", &tm );

        vsnprintf_s( logLine, length, _TRUNCATE, msgFormatter, args );

        if ( lastError ) {
            char errstring[ 256 ] = { 0 };
            snprintf( errstring, sizeof( errstring ) - 1, " : %i : %s", lastError, ebuffer );
            strncat_s( logLine, length, errstring, sizeof( errstring ) );
        }

        WcaLogError( LOGMSG_STANDARD, "%s %s", tstr, logLine );

        free( logLine );
    }
    else {
        WcaLogError( LOGMSG_STANDARD, __FUNCTION__ "calloc failed" );
    }
}

void MsiLogger::LogWithError( Severity level, const wchar_t* msgFormatter, va_list args )
{
    DWORD lastError = GetLastError();
    wchar_t ebuffer[ 256 ] = { 0 };

    /*
    * Retrieve Windows error code and format it, convert to char *
    */
    if ( lastError ) {
        FormatMessageW( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            lastError,
            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
            ebuffer,
            sizeof( ebuffer ) / sizeof( char ),
            NULL );

        // Reset the last error value to zero
        SetLastError( 0 );
    }

    size_t length = _vscwprintf( msgFormatter, args ) + 1;   // vsnprintf returns 1 character less???
    length += sizeof( ebuffer );                                    // Add space for GetLastError
    wchar_t* logLine = ( wchar_t* )calloc( 1, ( sizeof( wchar_t ) * ( length + 1 ) ) );

    if ( logLine ) {
        char tstr[ 32 ] = {};
        time_t now = time( NULL );
        struct tm tm;
        localtime_s( &tm, &now );
        strftime( tstr, sizeof( tstr ), "%b %d %H:%M:%S", &tm );

        _vsnwprintf_s( logLine, length, _TRUNCATE, msgFormatter, args );

        if ( lastError ) {
            wchar_t errstring[ 256 ] = { 0 };
            _snwprintf_s( errstring, sizeof( errstring ) - 1, L" : %i : %s", lastError, ebuffer );
            wcsncat_s( logLine, length, errstring, sizeof( errstring ) / sizeof( wchar_t ) );
        }

        WcaLogError( LOGMSG_STANDARD, "%s: %S", tstr, logLine );

        free( logLine );
    }
    else {
        WcaLogError( LOGMSG_STANDARD, __FUNCTION__ "calloc failed" );
    }
}
