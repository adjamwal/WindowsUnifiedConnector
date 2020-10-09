#pragma once

#include "PackageManagerInternalModuleAPI.h"
#include "IUcLogger.h"
#include <cstdio>
#include <Windows.h>
#include <string>

class ICodesignVerifier;

typedef PM_MODULE_RESULT_T( *CreateModuleFunc )( PM_MODULE_CTX_T* pPM_MODULE_CTX, IUcLogger* logger );
typedef PM_MODULE_RESULT_T( *ReleaseModuleFunc ) ( PM_MODULE_CTX_T* pPM_MODULE_CTX );

class UCMCPLoader
{
public:
    UCMCPLoader( ICodesignVerifier& codeSignVerifier );
    virtual ~UCMCPLoader();

    bool LoadDll( const std::wstring dllPath );
    void UnloadDll();

    PM_MODULE_RESULT_T CreateModule( PM_MODULE_CTX_T* pPM_MODULE_CTX, IUcLogger* logger );
    PM_MODULE_RESULT_T ReleaseModule( PM_MODULE_CTX_T* pPM_MODULE_CTX );

private:
    ICodesignVerifier& m_codeSignVerifier;
    HMODULE m_controlLib;
    CreateModuleFunc m_createModule;
    ReleaseModuleFunc m_releaseModule;
    std::wstring m_loadedDllName;
};
