#include "stdafx.h"
#include <assert.h>
#include <strsafe.h>
#include <evntprov.h>
#include <iostream>
#include <fstream>  

#include "ServiceBase.h"

//
// UC Service trace event provider
// {BE2E810E-F19F-4C30-9295-09F113A0A7E0}
//
EXTERN_C __declspec( selectany ) const GUID UC_SERVICE_PROVIDER_GUID = { 0xBE2E810E, 0xF19F, 0x4C30,{ 0x92, 0x95,0x09, 0xF1, 0x13, 0xA0, 0xA7, 0xE0 } };

#pragma region Static Members

ServiceBase* ServiceBase::s_service = nullptr;

BOOL ServiceBase::Run( ServiceBase& service )
{
    s_service = &service;

    SERVICE_TABLE_ENTRY serviceTable[] =
    {
        { service.m_svcName, ServiceMain },
        { nullptr, nullptr }
    };

    return StartServiceCtrlDispatcher( serviceTable );
}

void WINAPI ServiceBase::ServiceMain( DWORD dwArgc, PWSTR* pszArgv )
{
    assert( s_service != NULL );

    s_service->m_statusHandle = RegisterServiceCtrlHandler(
        s_service->m_svcName, ServiceCtrlHandler );

    if( s_service->m_statusHandle == NULL )
    {
        throw GetLastError();
    }

    s_service->Start( dwArgc, pszArgv );
}

//   PARAMETERS:
//   * dwCtrlCode - the control code. This parameter can be one of the 
//   following values: 
//
//     SERVICE_CONTROL_CONTINUE
//     SERVICE_CONTROL_INTERROGATE
//     SERVICE_CONTROL_NETBINDADD
//     SERVICE_CONTROL_NETBINDDISABLE
//     SERVICE_CONTROL_NETBINDREMOVE
//     SERVICE_CONTROL_PARAMCHANGE
//     SERVICE_CONTROL_PAUSE
//     SERVICE_CONTROL_SHUTDOWN
//     SERVICE_CONTROL_STOP
//
//   This parameter can also be a user-defined control code ranges from 128 
//   to 255.
//
void WINAPI ServiceBase::ServiceCtrlHandler( DWORD dwCtrl )
{
    switch( dwCtrl )
    {
    case SERVICE_CONTROL_STOP: s_service->Stop(); break;
    case SERVICE_CONTROL_PAUSE: s_service->Pause(); break;
    case SERVICE_CONTROL_CONTINUE: s_service->Continue(); break;
    case SERVICE_CONTROL_SHUTDOWN: s_service->Shutdown(); break;
    case SERVICE_CONTROL_INTERROGATE: break;
    default: break;
    }
}

#pragma endregion


#pragma region Service Constructor and Destructor

ServiceBase::ServiceBase(
    PWSTR pszServiceName,
    BOOL fCanStop,
    BOOL fCanShutdown,
    BOOL fCanPauseContinue )
    : m_debugLogFile()
    , m_svcName( pszServiceName )
    , m_statusHandle( nullptr )
    , m_etwRegHandle( NULL )
{
    InitializeDebugLogFile();
    InitializeEventLog( fCanStop, fCanShutdown, fCanPauseContinue );
}

ServiceBase::~ServiceBase( void )
{
    if( m_etwRegHandle != NULL )
    {
        EventUnregister( m_etwRegHandle );
    }
    m_debugLogFile.close();
}

#pragma endregion


#pragma region Service Start, Stop, Pause, Continue, and Shutdown

void ServiceBase::Start( DWORD dwArgc, PWSTR* pszArgv )
{
    LogMessage( __FUNCTIONW__, L"UCService trying to start." );
    try
    {
        SetServiceStatus( SERVICE_START_PENDING );
        OnStart( dwArgc, pszArgv );
        SetServiceStatus( SERVICE_RUNNING );
    }
    catch( DWORD dwError )
    {
        LogMessage( __FUNCTIONW__, L"UCService Start", TRACE_LEVEL_ERROR, dwError );
        SetServiceStatus( SERVICE_STOPPED, dwError );
    }
    catch( ... )
    {
        LogMessage( __FUNCTIONW__, L"UCService failed to start.", TRACE_LEVEL_ERROR );
        SetServiceStatus( SERVICE_STOPPED );
    }
}


void ServiceBase::OnStart( DWORD dwArgc, PWSTR* pszArgv )
{
    LogMessage( __FUNCTIONW__, L"" );
}

void ServiceBase::Stop()
{
    LogMessage( __FUNCTIONW__, L"UCService trying to stop." );

    DWORD dwOriginalState = m_status.dwCurrentState;
    try
    {
        SetServiceStatus( SERVICE_STOP_PENDING );
        OnStop();
        SetServiceStatus( SERVICE_STOPPED );
    }
    catch( DWORD dwError )
    {
        LogMessage( __FUNCTIONW__, L"UCService Stop", TRACE_LEVEL_ERROR, dwError );
        SetServiceStatus( dwOriginalState );
    }
    catch( ... )
    {
        LogMessage( __FUNCTIONW__, L"UCService failed to stop.", TRACE_LEVEL_ERROR );
        SetServiceStatus( dwOriginalState );
    }
}

void ServiceBase::OnStop()
{
    LogMessage( __FUNCTIONW__, L"" );
}

void ServiceBase::Pause()
{
    LogMessage( __FUNCTIONW__, L"UCService trying to pause." );
    try
    {
        SetServiceStatus( SERVICE_PAUSE_PENDING );
        OnPause();
        SetServiceStatus( SERVICE_PAUSED );
    }
    catch( DWORD dwError )
    {
        LogMessage( __FUNCTIONW__, L"UCService Pause", TRACE_LEVEL_ERROR, dwError );
        SetServiceStatus( SERVICE_RUNNING );
    }
    catch( ... )
    {
        LogMessage( __FUNCTIONW__, L"failed to pause.", TRACE_LEVEL_ERROR );
        SetServiceStatus( SERVICE_RUNNING );
    }
}

void ServiceBase::OnPause()
{
    LogMessage( __FUNCTIONW__, L"" );
}

void ServiceBase::Continue()
{
    LogMessage( __FUNCTIONW__, L"UCService trying to continue." );
    try
    {
        SetServiceStatus( SERVICE_CONTINUE_PENDING );
        OnContinue();
        SetServiceStatus( SERVICE_RUNNING );
    }
    catch( DWORD dwError )
    {
        LogMessage( __FUNCTIONW__, L"UCService Continue", TRACE_LEVEL_ERROR, dwError );
        SetServiceStatus( SERVICE_PAUSED );
    }
    catch( ... )
    {
        LogMessage( __FUNCTIONW__, L"UCService failed to resume.", TRACE_LEVEL_ERROR );
        SetServiceStatus( SERVICE_PAUSED );
    }
}

void ServiceBase::OnContinue()
{
    LogMessage( __FUNCTIONW__, L"" );
}

void ServiceBase::Shutdown()
{
    LogMessage( __FUNCTIONW__, L"UCService trying to shut down." );
    try
    {
        OnShutdown();
        SetServiceStatus( SERVICE_STOPPED );
    }
    catch( DWORD dwError )
    {
        LogMessage( __FUNCTIONW__, L"UCService Shutdown", TRACE_LEVEL_ERROR, dwError );
    }
    catch( ... )
    {
        LogMessage( __FUNCTIONW__, L"UCService failed to shut down.", TRACE_LEVEL_ERROR );
    }
}

void ServiceBase::OnShutdown()
{
    LogMessage( __FUNCTIONW__, L"" );
}

#pragma endregion


#pragma region Helper Functions

void ServiceBase::SetServiceStatus( _In_ DWORD dwCurrentState, _In_ DWORD dwWin32ExitCode, _In_ DWORD dwWaitHint )
{
    static DWORD dwCheckPoint = 1;

    m_status.dwCurrentState = dwCurrentState;
    m_status.dwWin32ExitCode = dwWin32ExitCode;
    m_status.dwWaitHint = dwWaitHint;

    m_status.dwCheckPoint =
        ( ( dwCurrentState == SERVICE_RUNNING ) ||
        ( dwCurrentState == SERVICE_STOPPED ) ) ?
        0 : dwCheckPoint++;

    ::SetServiceStatus( m_statusHandle, &m_status );
}

//   * wType - the type of event to be logged. The parameter can be one of 
//     the following values.
//
//     EVENTLOG_SUCCESS
//     EVENTLOG_AUDIT_SUCCESS
//     EVENTLOG_AUDIT_FAILURE
//     EVENTLOG_WARNING_TYPE
//     EVENTLOG_ERROR_TYPE
//     TRACE_LEVEL_INFORMATION
//     TRACE_LEVEL_ERROR
//     TRACE_LEVEL_CRITICAL
//     TRACE_LEVEL_FATAL
//
void ServiceBase::LogMessage( _In_ PWSTR pszFunction, _In_ PWSTR pszMessage, _In_ BYTE bLevel, _In_ DWORD dwError )
{
    wchar_t szMessage[ 260 ];
    if( bLevel == EVENTLOG_SUCCESS || bLevel == EVENTLOG_AUDIT_SUCCESS || bLevel == EVENTLOG_WARNING_TYPE  || bLevel == TRACE_LEVEL_INFORMATION )
    {
        StringCchPrintf( szMessage, ARRAYSIZE( szMessage ), L"[%s]%s(): %s", m_svcName, pszFunction, pszMessage );
    }
    else //EVENTLOG_AUDIT_FAILURE EVENTLOG_ERROR_TYPE TRACE_LEVEL_ERROR TRACE_LEVEL_CRITICAL TRACE_LEVEL_FATAL
    {
        StringCchPrintf( szMessage, ARRAYSIZE( szMessage ), L"[%s]%s() failed w/err 0x%08lx. %s", m_svcName, pszFunction, dwError, pszMessage );
    }

    if( m_etwRegHandle != NULL )
    {
        EventWriteString( m_etwRegHandle, bLevel, 0, pszMessage );
    }
    m_debugLogFile << szMessage << std::endl;
}

void ServiceBase::InitializeDebugLogFile()
{
    std::wstring debugFName( GetExeDirectory() );
    debugFName.append( m_svcName );
    debugFName.append( L".log" );
    m_debugLogFile.open( debugFName, std::wofstream::out | std::wofstream::app | std::ofstream::out | std::wofstream::ate );
}

void ServiceBase::InitializeEventLog( BOOL fCanStop, BOOL fCanShutdown, BOOL fCanPauseContinue )
{
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_status.dwCurrentState = SERVICE_START_PENDING;

    DWORD dwControlsAccepted = 0;
    if( fCanStop )
        dwControlsAccepted |= SERVICE_ACCEPT_STOP;
    if( fCanShutdown )
        dwControlsAccepted |= SERVICE_ACCEPT_SHUTDOWN;
    if( fCanPauseContinue )
        dwControlsAccepted |= SERVICE_ACCEPT_PAUSE_CONTINUE;
    m_status.dwControlsAccepted = dwControlsAccepted;

    m_status.dwWin32ExitCode = NO_ERROR;
    m_status.dwServiceSpecificExitCode = 0;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;

    NTSTATUS status = EventRegister( &UC_SERVICE_PROVIDER_GUID,
        nullptr,
        nullptr,
        &m_etwRegHandle );

    if( status != ERROR_SUCCESS )
    {
        m_etwRegHandle = NULL;
        LogMessage( __FUNCTIONW__, L"EventRegister failed, provider not registered", TRACE_LEVEL_ERROR, status );
    }
}

std::wstring ServiceBase::GetExeDirectory()
{
    TCHAR buffer[ MAX_PATH ] = { 0 };
    GetModuleFileName( NULL, buffer, ARRAYSIZE( buffer ) );
    std::wstring::size_type pos = std::wstring( buffer ).find_last_of( L"/\\" );
    return std::wstring( buffer ).substr( 0, pos + 1 );
}

#pragma endregion