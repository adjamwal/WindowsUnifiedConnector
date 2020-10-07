#pragma once

#include <iostream>
#include <fstream>  
#include <Evntrace.h>
#include <memory>
#include "UcLogFile.h"
#include "UcLogger.h"

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

protected:

    virtual void OnStart( DWORD dwArgc, PWSTR* pszArgv );
    virtual void OnStop();
    virtual void OnPause();
    virtual void OnContinue();
    virtual void OnShutdown();
    void SetServiceStatus( DWORD dwCurrentState,
        DWORD dwWin32ExitCode = NO_ERROR,
        DWORD dwWaitHint = 0 );

    PWSTR m_serviceName;
    std::unique_ptr<IUcLogFile> m_logFile;
    std::unique_ptr<IUcLogger> m_logger;
    REGHANDLE m_etwRegHandle;
private:

    static void WINAPI ServiceMain( DWORD dwArgc, PWSTR* pszArgv );
    static void WINAPI ServiceCtrlHandler( DWORD dwCtrl );

    void InitializeLogging( BOOL fCanStop, BOOL fCanShutdown, BOOL fCanPauseContinue );
    void DeinitializeLogging();

    void Start( DWORD dwArgc, PWSTR* pszArgv );
    void Pause();
    void Continue();
    void Shutdown();

    static ServiceBase* s_service;
    SERVICE_STATUS_HANDLE m_statusHandle;
    SERVICE_STATUS m_status;
};