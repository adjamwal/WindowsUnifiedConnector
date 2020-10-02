#pragma once

#include "stdafx.h"
#include "ServiceBase.h"

class UCService : public ServiceBase
{
public:

    UCService( PWSTR pszServiceName,
        BOOL fCanStop = TRUE,
        BOOL fCanShutdown = TRUE,
        BOOL fCanPauseContinue = FALSE );

    void WriteEventLogEntry( PWSTR pszMessage, BYTE bLevel ) override;
    virtual ~UCService( void );

protected:
    bool DirectoryExists( const char* dirname );
    bool FileExists( const char* filename );
    virtual void OnStart( _In_ DWORD dwArgc, _In_ PWSTR* pszArgv ) override;
    virtual void OnStop() override;

private:
    PTP_WORK m_threadPoolWorker = nullptr;
};