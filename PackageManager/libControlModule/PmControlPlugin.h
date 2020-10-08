#pragma once

#include <tchar.h>
#include <string>
#include <mutex>
#include "PackageManagerInternalModuleAPI.h"
#include "PmAgentController.h"
#include "CodesignVerifier.h"

class PmControlPlugin
{
public:
    // static member functions

    static PmControlPlugin&
        GetInstance( const std::wstring& rtstrBasePath = _T( "" ), const std::wstring& rtstrConfigPath = _T( "" ) );

    static PM_MODULE_RESULT_T
        StartPmAgent( const TCHAR* pszBasePath, const TCHAR* pszDataPath, const TCHAR* pszConfigPath );

    static PM_MODULE_RESULT_T
        StopPmAgent();

private:
    // private member functions

    PmControlPlugin( const std::wstring& rtstrBasePath, const std::wstring& rtstrConfigPath ) :
        m_AgentCtrlInst( m_codeSignVerifier, rtstrBasePath, rtstrConfigPath )
        , m_bIsProcessStarted( false )
    {
    }

    // prevent all copy and move construction and assignments
    PmControlPlugin( PmControlPlugin&& ) = delete;

    // private data members
    CodesignVerifier m_codeSignVerifier;
    PmAgentController m_AgentCtrlInst;
    std::mutex m_mtxAgentCtrl; // protect instance returned by GetInstance()
    bool m_bIsProcessStarted;  // tracking bool for module Start/Stop
};
