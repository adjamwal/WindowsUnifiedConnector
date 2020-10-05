#pragma once

#include <iostream>
#include <fstream>  
#include <Evntrace.h>

class ServiceBase
{
public:

    static BOOL Run( ServiceBase& service );

    ServiceBase( PWSTR pszServiceName,
        BOOL fCanStop = TRUE,
        BOOL fCanShutdown = TRUE,
        BOOL fCanPauseContinue = FALSE );
    virtual ~ServiceBase( void );

    void Stop();

    void LogMessage( _In_ PWSTR pszFunction, _In_ PWSTR pszMessage, _In_ BYTE bLevel = TRACE_LEVEL_INFORMATION, _In_ DWORD dwError = GetLastError() );

protected:

    virtual void OnStart( DWORD dwArgc, PWSTR* pszArgv );
    virtual void OnStop();
    virtual void OnPause();
    virtual void OnContinue();
    virtual void OnShutdown();
    void SetServiceStatus( DWORD dwCurrentState,
        DWORD dwWin32ExitCode = NO_ERROR,
        DWORD dwWaitHint = 0 );

private:

    static void WINAPI ServiceMain( DWORD dwArgc, PWSTR* pszArgv );
    static void WINAPI ServiceCtrlHandler( DWORD dwCtrl );

    void InitializeDebugLogFile();
    void InitializeEventLog( BOOL fCanStop, BOOL fCanShutdown, BOOL fCanPauseContinue );
    std::wstring GetExeDirectory();

    void Start( DWORD dwArgc, PWSTR* pszArgv );
    void Pause();
    void Continue();
    void Shutdown();

    static ServiceBase* s_service;
    std::wofstream m_debugLogFile;
    PWSTR m_svcName;
    SERVICE_STATUS_HANDLE m_statusHandle;
    REGHANDLE m_etwRegHandle;
    SERVICE_STATUS m_status;
};