#include "pch.h"
#include "IUcLogger.h"
#include <mutex>
#include <time.h>
#include <Windows.h>

/**
 * Default Logger. Only logs to stdout in debug mode
 * Used for unit/component tests
 */
class DefaultUCLogger : public IUcLogger
{
public:
    DefaultUCLogger();
    ~DefaultUCLogger();


    virtual void Log( Severity severity, const char* msgFormatter, ... ) override;
    virtual void Log( Severity severity, const wchar_t* msgFormatter, ... ) override;
    virtual void Log( Severity severity, const char* msgFormatter, va_list args ) override;
    virtual void Log( Severity severity, const wchar_t* msgFormatter, va_list args ) override;

    virtual void SetLogLevel( Severity severity ) override;
};

static DefaultUCLogger defaultLogger;
static IUcLogger* globalLogger = &defaultLogger;
static std::mutex g_logMutex;

IUcLogger* GetUcLogger()
{
    return globalLogger;
}

void SetUcLogger( IUcLogger* logger )
{
    globalLogger = logger ? logger : &defaultLogger;
}


DefaultUCLogger::DefaultUCLogger()
{
}

DefaultUCLogger::~DefaultUCLogger()
{
}

#ifdef _DEBUG
static void printTime()
{
    char tstr[ 32 ] = {};
    time_t now = time( NULL );
    struct tm tm;
    localtime_s( &tm, &now );
    strftime( tstr, sizeof( tstr ), "%b %d %H:%M:%S", &tm );
    printf( "%s ", tstr );
}

#endif

void DefaultUCLogger::Log( Severity severity, const char* msgFormatter, ... )
{
#ifdef _DEBUG
    std::lock_guard<std::mutex> lock( g_logMutex );
    printTime();

    va_list args;
    va_start( args, msgFormatter );

    vprintf( msgFormatter, args );
    printf( "\n" );
    va_end( args );
    fflush( stdout );
#endif
}

void DefaultUCLogger::Log( Severity severity, const wchar_t* msgFormatter, ... )
{
#ifdef _DEBUG
    std::lock_guard<std::mutex> lock( g_logMutex );
    printTime();

    va_list args;
    va_start( args, msgFormatter );

    vwprintf( msgFormatter, args );
    printf( "\n" );
    va_end( args );
    fflush( stdout );
#endif
}

void DefaultUCLogger::Log( Severity severity, const char* msgFormatter, va_list args )
{
#ifdef _DEBUG
    std::lock_guard<std::mutex> lock( g_logMutex );
    printTime();

    vprintf( msgFormatter, args );
    printf( "\n" );
    fflush( stdout );
#endif
}

void DefaultUCLogger::Log( Severity severity, const wchar_t* msgFormatter, va_list args )
{
#ifdef _DEBUG
    std::lock_guard<std::mutex> lock( g_logMutex );
    printTime();

    vwprintf( msgFormatter, args );
    printf( "\n" );
    fflush( stdout );
#endif
}

void DefaultUCLogger::SetLogLevel( Severity severity )
{

}