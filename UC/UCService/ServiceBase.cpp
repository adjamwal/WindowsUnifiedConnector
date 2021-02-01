#include "stdafx.h"
#include <assert.h>
#include <strsafe.h>
#include <evntprov.h>
#include <iostream>
#include <fstream>  

#include "DbgHelp.h"
#include "ServiceBase.h"
#include "IUcLogger.h"
#include "WindowsUtilities.h"

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
        { service.m_serviceName, ServiceMain },
        { nullptr, nullptr }
    };

    return StartServiceCtrlDispatcher( serviceTable );
}

void WINAPI ServiceBase::ServiceMain( DWORD dwArgc, PWSTR* pszArgv )
{
    DWORD dwError = 0;

    assert( s_service != NULL );

    s_service->m_statusHandle = RegisterServiceCtrlHandler(
        s_service->m_serviceName, ServiceCtrlHandler );

    if( s_service->m_statusHandle == NULL )
    {
        dwError = GetLastError();
        WLOG_ERROR( L"RegisterServiceCtrlHandler failed, error %d", dwError );
        throw dwError;
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
    : m_serviceName( pszServiceName )
    , m_logMgr( nullptr )
    , m_etwRegHandle( 0 )
    , m_statusHandle( nullptr )
    , m_crashHandler( CrashHandlerClient( nullptr ) )
{
    StartCrashHandler();
    InitializeLogging( fCanStop, fCanShutdown, fCanPauseContinue );
}

ServiceBase::~ServiceBase( void )
{
    DeinitializeLogging();
    StopCrashHandler();
}

#pragma endregion


#pragma region Service Start, Stop, Pause, Continue, and Shutdown

void ServiceBase::Start( DWORD dwArgc, PWSTR* pszArgv )
{
    WLOG_DEBUG( L"Service trying to start." );
    try
    {
        SetServiceStatus( SERVICE_START_PENDING );
        OnStart( dwArgc, pszArgv );
        SetServiceStatus( SERVICE_RUNNING );
    }
    catch( DWORD dwError )
    {
        WLOG_ERROR( L"Service failed to start, error %d", dwError );
        SetServiceStatus( SERVICE_STOPPED, dwError );
    }
    catch ( std::exception& ex )
    {
        LOG_ERROR( "Service failed to start: %s", ex.what() );
        SetServiceStatus( SERVICE_STOPPED );
    }
    catch( ... )
    {
        WLOG_ERROR( L"Service failed to start." );
        SetServiceStatus( SERVICE_STOPPED );
    }
}

void ServiceBase::OnStart( DWORD dwArgc, PWSTR* pszArgv )
{
    WLOG_DEBUG( L"Enter" );
}

void ServiceBase::Stop()
{
    WLOG_DEBUG( L"Service trying to stop." );

    DWORD dwOriginalState = m_status.dwCurrentState;
    try
    {
        SetServiceStatus( SERVICE_STOP_PENDING );
        OnStop();
        SetServiceStatus( SERVICE_STOPPED );
    }
    catch( DWORD dwError )
    {
        WLOG_ERROR( L"Service failed to stop, error %d", dwError );
        SetServiceStatus( dwOriginalState );
    }
    catch( ... )
    {
        WLOG_ERROR( L"Service failed to stop." );
        SetServiceStatus( dwOriginalState );
    }
}

void ServiceBase::OnStop()
{
    WLOG_DEBUG( L"Enter" );
}

void ServiceBase::Pause()
{
    WLOG_DEBUG( L"Service trying to pause." );
    try
    {
        SetServiceStatus( SERVICE_PAUSE_PENDING );
        OnPause();
        SetServiceStatus( SERVICE_PAUSED );
    }
    catch( DWORD dwError )
    {
        WLOG_ERROR( L"Service failed to pause, error %d", dwError );
        SetServiceStatus( SERVICE_RUNNING );
    }
    catch( ... )
    {
        WLOG_ERROR( L"Service failed to pause." );
        SetServiceStatus( SERVICE_RUNNING );
    }
}

void ServiceBase::OnPause()
{
    WLOG_DEBUG( L"Enter" );
}

void ServiceBase::Continue()
{
    WLOG_DEBUG( L"Service trying to continue." );
    try
    {
        SetServiceStatus( SERVICE_CONTINUE_PENDING );
        OnContinue();
        SetServiceStatus( SERVICE_RUNNING );
    }
    catch( DWORD dwError )
    {
        WLOG_ERROR( L"Service failed to continue, error %d", dwError );
        SetServiceStatus( SERVICE_PAUSED );
    }
    catch( ... )
    {
        WLOG_ERROR( L"Service failed to continue." );
        SetServiceStatus( SERVICE_PAUSED );
    }
}

void ServiceBase::OnContinue()
{
    WLOG_DEBUG( L"Enter" );
}

void ServiceBase::Shutdown()
{
    WLOG_DEBUG( L"Service trying to shut down." );
    try
    {
        OnShutdown();
        SetServiceStatus( SERVICE_STOPPED );
    }
    catch( DWORD dwError )
    {
        WLOG_ERROR( L"Service failed to shutdown, error %d", dwError );
    }
    catch( ... )
    {
        WLOG_ERROR( L"Service failed to shut down." );
    }
}

void ServiceBase::OnShutdown()
{
    WLOG_DEBUG( L"Enter" );
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

void ServiceBase::InitializeLogging( BOOL fCanStop, BOOL fCanShutdown, BOOL fCanPauseContinue )
{
    m_logMgr.reset( new ServiceLogManager() );
    m_logMgr->Start();

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
        m_etwRegHandle = 0;
        WLOG_ERROR( L"EventRegister failed, provider not registered." );
    }
    else
    {
        WLOG_DEBUG( L"EventLog initialized." );
        EventWriteString( m_etwRegHandle, EVENTLOG_SUCCESS, 0, __FUNCTIONW__ L": EventLog initialized." );
    }
}

void ServiceBase::DeinitializeLogging()
{
    try
    {
        m_logMgr.reset();

        if( m_etwRegHandle != 0 )
        {
            EventUnregister( m_etwRegHandle );
        }
    }
    catch( ... ) { };
}

void ServiceBase::StartCrashHandler()
{
    std::wstring dataDir = WindowsUtilities::GetLogDir();

    m_crashHandler.Init( dataDir.c_str(), MiniDumpWithFullMemory );
    m_crashHandler.SetupCrashHandler();
}

void ServiceBase::StopCrashHandler()
{
    m_crashHandler.RemoveCrashHandler();
}

#pragma endregion
