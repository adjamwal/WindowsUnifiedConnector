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
    m_debugLogFile << SERVICE_NAME << " created" << std::endl;
}

UCService::~UCService( void )
{
    m_debugLogFile << SERVICE_NAME << " destroyed" << std::endl;
}

void UCService::WriteEventLogEntry( PWSTR pszMessage, BYTE bLevel )
{
    __super::WriteEventLogEntry( pszMessage, bLevel );
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
    pService->m_debugLogFile << "ServiceWorkerThread started" << std::endl;

    //  Periodically check if the service has been requested to stop
    while( WaitForSingleObject( m_ServiceStopEvent, 0 ) != WAIT_OBJECT_0 )
    {
        //do some work
        pService->m_debugLogFile << "ServiceWorkerThread ... doing some work" << std::endl;
        pService->WriteEventLogEntry( SERVICE_NAME" ServiceWorkerThread ... doing some work", TRACE_LEVEL_ERROR );

        Sleep( 3000 );
    }

    CloseHandle( m_ServiceStopEvent );
    m_ServiceStopEvent = INVALID_HANDLE_VALUE;

    return;
}

void UCService::OnStart( _In_ DWORD dwArgc, _In_ PWSTR* pszArgv )
{
    WriteEventLogEntry( SERVICE_NAME" in OnStart", TRACE_LEVEL_INFORMATION );

    if( m_ServiceStopEvent != INVALID_HANDLE_VALUE )
    {
        WriteEventLogEntry( SERVICE_NAME" OnStart error: ServiceWorkerThread still running", TRACE_LEVEL_ERROR );
        return;
    }

    m_ServiceStopEvent = CreateEvent( NULL, TRUE, FALSE, NULL );

    PTP_WORK_CALLBACK workcallback = ServiceWorkerThread;
    m_threadPoolWorker = CreateThreadpoolWork( workcallback, this, nullptr );

    if( NULL == m_threadPoolWorker )
    {
        WriteEventLogEntry( SERVICE_NAME" CreateThreadpoolWork failed", TRACE_LEVEL_ERROR );
    }

    SubmitThreadpoolWork( m_threadPoolWorker );
}

void UCService::OnStop()
{
    WriteEventLogEntry( SERVICE_NAME" in OnStop", TRACE_LEVEL_INFORMATION );

    if( m_ServiceStopEvent == INVALID_HANDLE_VALUE )
    {
        WriteEventLogEntry( SERVICE_NAME" OnStop error: ServiceWorkerThread is not running", TRACE_LEVEL_ERROR );
        return;
    }

    // This will signal the worker thread to start shutting down
    SetEvent( m_ServiceStopEvent );
}