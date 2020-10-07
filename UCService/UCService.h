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
    virtual ~UCService( void );

protected:
    virtual void OnStart( _In_ DWORD dwArgc, _In_ PWSTR* pszArgv ) override;
    virtual void OnStop() override;

private:
    bool DirectoryExists( const char* dirname );
    bool FileExists( const char* filename );
};