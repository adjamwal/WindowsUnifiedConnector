#include "pch.h"
#include "ConsoleLogger.h"

static void printTime()
{
    char tstr[ 32 ] = {};
    time_t now = time( NULL );
    struct tm tm;
    localtime_s( &tm, &now );
    strftime( tstr, sizeof( tstr ), "%b %d %H:%M:%S", &tm );
    printf( "%s ", tstr );
}

void ConsoleLogger::Log( Severity severity, const char* msgFormatter, ... )
{
    std::lock_guard<std::mutex> lock( m_logMutex );
    printTime();

    va_list args;
    va_start( args, msgFormatter );

    vprintf( msgFormatter, args );
    printf( "\n" );
    va_end( args );
    fflush( stdout );
}

void ConsoleLogger::Log( Severity severity, const wchar_t* msgFormatter, ... )
{
    std::lock_guard<std::mutex> lock( m_logMutex );
    printTime();

    va_list args;
    va_start( args, msgFormatter );

    vwprintf( msgFormatter, args );
    printf( "\n" );
    va_end( args );
    fflush( stdout );
}

void ConsoleLogger::Log( Severity severity, const char* msgFormatter, va_list args )
{
    std::lock_guard<std::mutex> lock( m_logMutex );
    printTime();

    vprintf( msgFormatter, args );
    printf( "\n" );
    fflush( stdout );
}

void ConsoleLogger::Log( Severity severity, const wchar_t* msgFormatter, va_list args )
{
    std::lock_guard<std::mutex> lock( m_logMutex );
    printTime();

    vwprintf( msgFormatter, args );
    printf( "\n" );
    fflush( stdout );
}

void ConsoleLogger::SetLogLevel( Severity severity )
{

}
