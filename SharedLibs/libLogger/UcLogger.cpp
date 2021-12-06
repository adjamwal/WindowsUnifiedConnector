#include "pch.h"
#include "UcLogger.h"
#include "IUcLogFile.h"
#include <time.h>
#include <mutex>
#include <Windows.h>


UcLogger::UcLogger( IUcLogFile& logFile ) :
    m_logLevel( LOG_ERROR )
    , m_logFile( logFile )
{
}

UcLogger::~UcLogger()
{
}

void UcLogger::SetLogLevel( Severity logLevel )
{
    if( ( logLevel >= LOG_ALERT ) && ( logLevel <= LOG_DEBUG ) ) {
        if( m_logLevel != logLevel ) {
            m_logLevel = logLevel;
            Log( m_logLevel, "Set Debug Level to %d", m_logLevel );
        }
    }
    else {
        Log( LOG_ERROR, "Invalid Debug level %d", logLevel );
    }
}

void UcLogger::Log( Severity severity, const char* msgFormatter, ... )
{
    va_list  args;
    va_start( args, msgFormatter );

    Log( severity, msgFormatter, args );

    va_end( args );
}

void UcLogger::Log( Severity severity, const wchar_t* msgFormatter, ... )
{
    va_list  args;
    va_start( args, msgFormatter );

    Log( severity, msgFormatter, args );

    va_end( args );
}

void UcLogger::Log( Severity severity, const char* msgFormatter, va_list args )
{
    if( severity <= LOG_ERROR ) {
        LogWithError( severity, msgFormatter, args );
    }
    else if( severity <= m_logLevel ) {
        size_t length = _vscprintf( msgFormatter, args ) + 1;   // vsnprintf returns 1 character less???
        char* logLine = ( char* )calloc( 1, length + 1 );

        if( logLine ) {
            vsnprintf_s( logLine, length, _TRUNCATE, msgFormatter, args );

            m_logFile.WriteLogLine( LogLevelStr( severity ), logLine );

            free( logLine );
        }
        else {
            m_logFile.WriteLogLine( LogLevelStr( severity ), __FUNCTION__ ": calloc failed" );
        }
    }
}

void UcLogger::Log( Severity severity, const wchar_t* msgFormatter, va_list args )
{
    if( severity <= LOG_ERROR ) {
        LogWithError( severity, msgFormatter, args );
    }
    else if( severity <= m_logLevel ) {
        size_t length = _vscwprintf( msgFormatter, args ) + 1;   // vsnprintf returns 1 character less???
        wchar_t* logLine = ( wchar_t* )calloc( 1, ( sizeof( wchar_t ) * ( length + 1 ) ) );

        if( logLine ) {
            _vsnwprintf_s( logLine, length, _TRUNCATE, msgFormatter, args );

            m_logFile.WriteLogLine( LogLevelStrW( severity ), logLine );

            free( logLine );
        }
        else {
            m_logFile.WriteLogLine( LogLevelStr( severity ), __FUNCTION__ ": calloc failed" );
        }
    }
}

void UcLogger::LogWithError( Severity level, const char* msgFormatter, va_list args )
{
    DWORD lastError = GetLastError();
    char ebuffer[ 256 ] = { 0 };

    /*
    * Retrieve Windows error code and format it, convert to char *
    */
    if( lastError ) {
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

    if( logLine ) {
        vsnprintf_s( logLine, length, _TRUNCATE, msgFormatter, args );

        if( lastError ) {
            char errstring[ 256 ] = { 0 };
            snprintf( errstring, sizeof( errstring ) - 1, " : %i : %s", lastError, ebuffer );
            strncat_s( logLine, length, errstring, sizeof( errstring ) );
        }

        m_logFile.WriteLogLine( LogLevelStr( level ), logLine );

        free( logLine );
    }
    else {
        m_logFile.WriteLogLine( LogLevelStr( LOG_ERROR ), __FUNCTION__ ": calloc failed" );
    }
}

void UcLogger::LogWithError( Severity level, const wchar_t* msgFormatter, va_list args )
{
    DWORD lastError = GetLastError();
    wchar_t ebuffer[ 256 ] = { 0 };

    /*
    * Retrieve Windows error code and format it, convert to char *
    */
    if( lastError ) {
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

    if( logLine ) {
        _vsnwprintf_s( logLine, length, _TRUNCATE, msgFormatter, args );

        if( lastError ) {
            wchar_t errstring[ 256 ] = { 0 };
            _snwprintf_s( errstring, sizeof( errstring ) - 1, L" : %i : %s", lastError, ebuffer );
            wcsncat_s( logLine, length, errstring, sizeof( errstring ) / sizeof( wchar_t ) );
        }

        m_logFile.WriteLogLine( LogLevelStrW( level ), logLine );

        free( logLine );
    }
    else {
        m_logFile.WriteLogLine( LogLevelStr( level ), __FUNCTION__ ": calloc failed" );
    }
}

const char* UcLogger::LogLevelStr( Severity level )
{
    const char* levelStr = "";

    switch( level ) {
    case LOG_ALERT:
        levelStr = "ALERT";
        break;
    case LOG_CRITICAL:
        levelStr = "CRITICAL";
        break;
    case LOG_ERROR:
        levelStr = "ERROR";
        break;
    case LOG_WARNING:
        levelStr = "WARNING";
        break;
    case LOG_NOTICE:
        levelStr = "NOTICE";
        break;
    case LOG_INFO:
        levelStr = "INFO";
        break;
    case LOG_DEBUG:
        levelStr = "DEBUG";
        break;
    default:
        break;
    }

    return levelStr;
}

const wchar_t* UcLogger::LogLevelStrW( Severity level )
{
    const wchar_t* levelStr = L"";

    switch( level ) {
    case LOG_ALERT:
        levelStr = L"ALERT";
        break;
    case LOG_CRITICAL:
        levelStr = L"CRITICAL";
        break;
    case LOG_ERROR:
        levelStr = L"ERROR";
        break;
    case LOG_WARNING:
        levelStr = L"WARNING";
        break;
    case LOG_NOTICE:
        levelStr = L"NOTICE";
        break;
    case LOG_INFO:
        levelStr = L"INFO";
        break;
    case LOG_DEBUG:
        levelStr = L"DEBUG";
        break;
    default:
        break;
    }

    return levelStr;
}