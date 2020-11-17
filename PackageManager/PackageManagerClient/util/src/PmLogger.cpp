#include "PMLogger.h"
#include <time.h>
#include <mutex>

class DefaultPMLogger : public IPMLogger
{
public:
    DefaultPMLogger();
    ~DefaultPMLogger();


    virtual void Log( Severity serverity, const char* msgFormatter, ... ) override;
    virtual void Log( Severity serverity, const wchar_t* msgFormatter, ... ) override;
    virtual void Log( Severity serverity, const char* msgFormatter, va_list args ) override;
    virtual void Log( Severity serverity, const wchar_t* msgFormatter, va_list args ) override;

    virtual void SetLogLevel( Severity serverity ) override;
};

static DefaultPMLogger defaultLogger;
static IPMLogger* globalLogger = &defaultLogger;
static std::mutex g_logMutex;

DefaultPMLogger::DefaultPMLogger()
{
}

DefaultPMLogger::~DefaultPMLogger()
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
void DefaultPMLogger::Log( Severity serverity, const char* msgFormatter, ... )
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

void DefaultPMLogger::Log( Severity serverity, const wchar_t* msgFormatter, ... )
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

void DefaultPMLogger::Log( Severity serverity, const char* msgFormatter, va_list args )
{
#ifdef _DEBUG
    std::lock_guard<std::mutex> lock( g_logMutex );
    printTime();

    vprintf( msgFormatter, args );
    printf( "\n" );
    fflush( stdout );
#endif
}

void DefaultPMLogger::Log( Severity serverity, const wchar_t* msgFormatter, va_list args )
{
#ifdef _DEBUG
    std::lock_guard<std::mutex> lock( g_logMutex );
    printTime();

    vwprintf( msgFormatter, args );
    printf( "\n" );
    fflush( stdout );
#endif
}

void DefaultPMLogger::SetLogLevel( Severity serverity )
{

}

IPMLogger* GetPMLogger()
{
    return globalLogger;
}

void SetPMLogger( IPMLogger* logger )
{
    globalLogger = logger ? logger : &defaultLogger;
}
