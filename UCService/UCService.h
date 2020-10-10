#pragma once

#include "stdafx.h"
#include "ServiceBase.h"
#include "PackageManagerInternalModuleAPI.h"
#include "UCIDLoader.h"
#include "UCMCPLoader.h"
#include "CodesignVerifier.h"

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
    CodesignVerifier m_codeSignVerifer;
    UCIDLoader m_ucidLoader;
    UCMCPLoader m_ucmcpLoader;
};