#pragma once

#include <iostream>
#include <fstream>  

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

    std::wofstream m_debugLogFile;

protected:

    virtual void OnStart( DWORD dwArgc, PWSTR* pszArgv );
    virtual void OnStop();
    virtual void OnPause();
    virtual void OnContinue();
    virtual void OnShutdown();
    void SetServiceStatus( DWORD dwCurrentState,
        DWORD dwWin32ExitCode = NO_ERROR,
        DWORD dwWaitHint = 0 );

    virtual void WriteEventLogEntry( PWSTR pszMessage, BYTE bLevel );
    void WriteErrorLogEntry( PWSTR pszFunction,
        DWORD dwError = GetLastError() );

private:

    static void WINAPI ServiceMain( DWORD dwArgc, PWSTR* pszArgv );
    static void WINAPI ServiceCtrlHandler( DWORD dwCtrl );
    std::wstring GetExePath();

    void Start( DWORD dwArgc, PWSTR* pszArgv );
    void Pause();
    void Continue();
    void Shutdown();

    static ServiceBase* s_service;
    PWSTR m_name;
    SERVICE_STATUS m_status;
    SERVICE_STATUS_HANDLE m_statusHandle;
    REGHANDLE m_etwRegHandle;
};