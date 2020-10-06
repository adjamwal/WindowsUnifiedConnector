#include "stdafx.h"

#include <Evntrace.h>
#include <sys/stat.h>
#include <string>
#include <vector>

#include "UCService.h"

HANDLE m_ServiceStopEvent = INVALID_HANDLE_VALUE;

UCService::UCService(
    PWSTR pszServiceName,
    BOOL fCanStop,
    BOOL fCanShutdown,
    BOOL fCanPauseContinue )
    : ServiceBase( pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue )
{
    LogMessage( __FUNCTIONW__, L"created" );
}

UCService::~UCService( void )
{
    LogMessage( __FUNCTIONW__, L"destroyed" );
}

bool UCService::FileExists( const char* filename )
{
    struct stat buffer;
    return ( stat( filename, &buffer ) == 0 );
}

bool UCService::DirectoryExists( const char* dirname )
{
    DWORD ftyp = GetFileAttributesA( dirname );
    if( ftyp == INVALID_FILE_ATTRIBUTES )
    {
        return false;
    }

    if( ftyp & FILE_ATTRIBUTE_DIRECTORY )
    {
        return true;
    }

    return false;
}

VOID CALLBACK ServiceWorkerThread(
    _In_ PTP_CALLBACK_INSTANCE /*Instance*/,
    _In_ PVOID Parameter,
    _In_ PTP_WORK /*Work*/ )
{
    UCService* pService = static_cast< UCService* >( Parameter );
    pService->LogMessage( __FUNCTIONW__, L"started" );

    //  Periodically check if the service has been requested to stop
    while( WaitForSingleObject( m_ServiceStopEvent, 0 ) != WAIT_OBJECT_0 )
    {
        //do some work
        pService->LogMessage( __FUNCTIONW__, L"... doing some work" );

        Sleep( 3000 );
    }

    CloseHandle( m_ServiceStopEvent );
    m_ServiceStopEvent = INVALID_HANDLE_VALUE;

    return;
}

void UCService::OnStart( _In_ DWORD dwArgc, _In_ PWSTR* pszArgv )
{
    LogMessage( __FUNCTIONW__, L"in OnStart" );

    if( m_ServiceStopEvent != INVALID_HANDLE_VALUE )
    {
        LogMessage( __FUNCTIONW__, L"OnStart error: ServiceWorkerThread still running", TRACE_LEVEL_ERROR );
        return;
    }

    m_ServiceStopEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    PTP_WORK_CALLBACK workcallback = ServiceWorkerThread;
    m_threadPoolWorker = CreateThreadpoolWork( workcallback, this, nullptr );

    if( NULL == m_threadPoolWorker )
    {
        LogMessage( __FUNCTIONW__, L" CreateThreadpoolWork failed", TRACE_LEVEL_ERROR );
    }

    SubmitThreadpoolWork( m_threadPoolWorker );
}

void UCService::OnStop()
{
    LogMessage( __FUNCTIONW__, L"in OnStop" );

    if( m_ServiceStopEvent == INVALID_HANDLE_VALUE )
    {
        LogMessage( __FUNCTIONW__, L"OnStop error: ServiceWorkerThread is not running", TRACE_LEVEL_ERROR );
        return;
    }

    // This will signal the worker thread to start shutting down
    SetEvent( m_ServiceStopEvent );
}