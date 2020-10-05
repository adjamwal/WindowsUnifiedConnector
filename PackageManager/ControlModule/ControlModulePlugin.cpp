#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <tchar.h>
#include <string>
#include <Windows.h>
#include "PmControlPlugin.h"
#include "IUcLogger.h"

PM_MODULE_API
PM_MODULE_RESULT_T
CreateModuleInstance( IN OUT PM_MODULE_CTX_T* pPM_MODULE_CTX, IUcLogger* logger )
{
    if( nullptr == pPM_MODULE_CTX )
    {
        return PM_MODULE_INVALID_PARAM;
    }

    if( logger ) {
        SetUcLogger( logger );
    }
    // NOTE: not doing anything with PmControlPlugin singleton here
    // NOTE: not complaining here about any paths; will only know about paths in Start

    pPM_MODULE_CTX->fpInit = nullptr;
    pPM_MODULE_CTX->fpDeinit = nullptr;
    pPM_MODULE_CTX->fpStart = PmControlPlugin::StartPmAgent;
    pPM_MODULE_CTX->fpStop = PmControlPlugin::StopPmAgent;
    pPM_MODULE_CTX->fpSetOption = nullptr;
    pPM_MODULE_CTX->fpConfigUpdated = nullptr;
    return PM_MODULE_SUCCESS;
}

PM_MODULE_API
PM_MODULE_RESULT_T
ReleaseModuleInstance( IN OUT PM_MODULE_CTX_T* pPM_MODULE_CTX )
{
    if( nullptr == pPM_MODULE_CTX )
    {
        return PM_MODULE_INVALID_PARAM;
    }

    // NOTE: not doing anything with PmControlPlugin singleton here

    pPM_MODULE_CTX->fpStart = nullptr; // clear
    pPM_MODULE_CTX->fpStop = nullptr; // clear

    SetUcLogger( nullptr );

    return PM_MODULE_SUCCESS;
}
