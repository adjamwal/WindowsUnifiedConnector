#pragma once

#include "PackageManagerInternalModuleAPI.h"
#include "IUcLogger.h"
#include <cstdio>
#include <Windows.h>
#include <string>

class ICodesignVerifier;

class UCIDLoader
{
public:
    UCIDLoader( ICodesignVerifier& codeSignVerifier );
    virtual ~UCIDLoader();

    void LoadControlModule();
    void UnloadControlModule();

private:
    typedef PM_MODULE_RESULT_T( *CreateModuleFunc )( PM_MODULE_CTX_T* pPM_MODULE_CTX );
    typedef PM_MODULE_RESULT_T( *ReleaseModuleFunc ) ( PM_MODULE_CTX_T* pPM_MODULE_CTX );

    bool LoadDll( const std::wstring dllPath );
    void UnloadDll();

    PM_MODULE_RESULT_T CreateModule( PM_MODULE_CTX_T* pPM_MODULE_CTX );
    PM_MODULE_RESULT_T ReleaseModule( PM_MODULE_CTX_T* pPM_MODULE_CTX );

    ICodesignVerifier& m_codeSignVerifier;
    HMODULE m_controlLib;
    CreateModuleFunc m_createModule;
    ReleaseModuleFunc m_releaseModule;
    std::wstring m_loadedDllName;
    PM_MODULE_CTX_T m_context;
    bool m_isModuleLoaded;
};
