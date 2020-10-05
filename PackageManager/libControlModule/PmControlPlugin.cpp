#include "pch.h"
#include <iostream>
#include <Windows.h>
#include "IUcLogger.h"
#include "PmControlPlugin.h"

PmControlPlugin&
    PmControlPlugin::GetInstance( const std::wstring& rtstrBasepath, const std::wstring& rtstrConfigpath )
{
    static PmControlPlugin s_ctrlPluginInstance( rtstrBasepath, rtstrConfigpath );
    return s_ctrlPluginInstance;
}

PM_MODULE_RESULT_T
    to_pm_result( const PM_STATUS eUCIDStatus )
{
    switch( eUCIDStatus )
    {
    case PM_STATUS::PM_OK: return PM_MODULE_SUCCESS;
        // TODO: define more mappings
    default:                   return PM_MODULE_GENERAL_ERROR;
    }
}

PM_MODULE_RESULT_T
    PmControlPlugin::StartPmAgent(
        const TCHAR* pszBasePath,
        const TCHAR* pszDataPath,
        const TCHAR* pszConfigPath )
{
    try
    {
        PmControlPlugin& rCtrlPlugin = GetInstance( pszBasePath, pszConfigPath );
        std::lock_guard<std::mutex> lck( rCtrlPlugin.m_mtxAgentCtrl );
        if( rCtrlPlugin.m_bIsProcessStarted )
        {
            return PM_MODULE_ALREADY_STARTED;
        }

        PM_STATUS retStatus = rCtrlPlugin.m_AgentCtrlInst.Start();
        if( PM_STATUS::PM_OK == retStatus )
        {
            rCtrlPlugin.m_bIsProcessStarted = true;
            return PM_MODULE_SUCCESS;
        }
        else
        {
            LOG_ERROR( "Failed to start agent with return code [%d]", retStatus );
            return to_pm_result( retStatus );
        }
    }
    catch( const std::exception& rExcep )
    {
        // handle exception
        LOG_ERROR( "Exception : [%s] ", rExcep.what() );
    }

    return PM_MODULE_GENERAL_ERROR;
}

PM_MODULE_RESULT_T
    PmControlPlugin::StopPmAgent()
{
    try
    {
        PmControlPlugin& rCtrlPlugin = GetInstance();
        std::lock_guard<std::mutex> lck( rCtrlPlugin.m_mtxAgentCtrl );
        if( !rCtrlPlugin.m_bIsProcessStarted )
        {
            return PM_MODULE_NOT_STARTED;
        }

        PM_STATUS retStatus = rCtrlPlugin.m_AgentCtrlInst.Stop();
        if( PM_STATUS::PM_OK == retStatus )
        {
            rCtrlPlugin.m_bIsProcessStarted = false;
            return PM_MODULE_SUCCESS;
        }

        LOG_ERROR( "Failed to stop agent with return code [%d]", retStatus );

        return to_pm_result( retStatus );
    }
    catch( const std::exception& e )
    {
        LOG_WARNING( "UCID agent not started: %s", e.what() );
        return PM_MODULE_NOT_STARTED;
    }
}

