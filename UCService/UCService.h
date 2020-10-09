#pragma once

#include "stdafx.h"
#include "ServiceBase.h"
#include "PackageManagerInternalModuleAPI.h"
#include "UCMCPLoader.h"

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
    void LoadPMControlModule();
    void UnloadPMControlModule();

    UCMCPLoader m_ucmcpLoader;
    PM_MODULE_CTX_T m_ucmcp;
    bool m_isUcmcpLoaded;
};