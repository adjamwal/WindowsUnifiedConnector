/**************************************************************************
 *       Copyright (c) 2020, Cisco Systems, All Rights Reserved
 ***************************************************************************
 *
 *  @file:    PmAgentController.h
 *
 ***************************************************************************
 * @desc Child process controller for Pm Agent
 ***************************************************************************/
#pragma once

#include <thread>
#include <mutex>
#include <string>
#include <Windows.h>
#include "PmTypes.h"

#define PM_AGENT_BINARY "ucpm.exe"
#define BS_CONFIG_FILE "bs.json"
#define PM_CONFIG_FILE "pm.json"

class ICodesignVerifier;

class PmAgentController
{
public:
    /**
    * Constructor
    * @param[in] rstrPath - Path to the child process
    */
    PmAgentController( ICodesignVerifier& codesignVerifier, 
        const std::wstring& rtstrPath, 
        const std::wstring& rtstrConfigPath);

    ~PmAgentController();
    /**
    * Starts and monitors the child process
    * @return PM_STATUS
    */
    PM_STATUS Start();
    /**
    * Sends a Stop signal to the child process
    * Terminates if unresponsive.
    * @return PM_STATUS
    */
    PM_STATUS Stop();
private:
    enum eProcStatus {
        eProcess_Terminated = 0,
        eProcess_Active
    };
    void cleanup();
    void monitorProcess();
    PM_STATUS killIfRunning();
    PM_STATUS startProcess();
    PM_STATUS stopProcess();
    eProcStatus waitForProcess();

    ICodesignVerifier& m_codesignVerifier;
    std::wstring m_tstrProcessPath;
    std::wstring m_tstrBsConfigPath;
    std::wstring m_tstrPmConfigPath;
    std::thread m_threadMonitor;
    bool m_bIsProcessStartedByPlugin;
    std::mutex m_mutex;
    HANDLE m_hChildStdinWr = INVALID_HANDLE_VALUE;
    HANDLE m_hProcess = INVALID_HANDLE_VALUE;

};

