#include "pch.h"

#include "PmAgentController.h"
#include "ICodesignVerifier.h"
#include "IUcLogger.h"
#include <iostream>
#include <tchar.h>
#include <Windows.h>
#include <Psapi.h>

#define MAX_PROC_LIMIT 1024
#define MAX_RETRY_COUNT 3
#define RESTART_DELAY_CHRONO 30s

using namespace std::chrono_literals;

PmAgentController::PmAgentController( ICodesignVerifier& codeSignVerifier, const std::wstring& rtstrPath, const std::wstring& rtstrConfigPath ) :
    m_codesignVerifier( codeSignVerifier )
    , m_tstrProcessPath( rtstrPath + L"\\" + _T( PM_AGENT_BINARY ) ) 
    , m_tstrBsConfigPath( rtstrConfigPath + L"\\" + _T( BS_CONFIG_FILE ) )
    , m_tstrPmConfigPath( rtstrConfigPath + L"\\" + _T( PM_CONFIG_FILE ) )
    , m_bIsProcessStartedByPlugin( false )
{
    if( rtstrPath.empty() )
    {
        throw std::invalid_argument( "AgentController basepath has not been set" );
    }
}

PmAgentController::~PmAgentController()
{
    Stop();
    cleanup();
}

PM_STATUS PmAgentController::Start()
{
    //check if previous process is running
    // and kill it.
    if( PM_STATUS::PM_OK != killIfRunning() )
    {
        LOG_ERROR( "Could not kill previous running instance" );
        return PM_STATUS::PM_FAIL;
    }

    //start process.
    if( PM_STATUS::PM_OK != startProcess() )
    {
        LOG_ERROR( "Could not start the process." );
        return PM_STATUS::PM_FAIL;
    }

    m_bIsProcessStartedByPlugin = true;
    LOG_DEBUG( "Process successfully launched." );

    //launch thread to monitor process.
    m_threadMonitor = std::thread( &PmAgentController::monitorProcess, this );
    return PM_STATUS::PM_OK;
}

PM_STATUS PmAgentController::Stop()
{
    auto status = PM_STATUS::PM_ERROR;
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        if( m_bIsProcessStartedByPlugin )
        {
            if( PM_STATUS::PM_OK != stopProcess() )
            {
                LOG_ERROR( "Could not stop the process." );
                return PM_STATUS::PM_FAIL;
            }
            m_bIsProcessStartedByPlugin = false;
            LOG_DEBUG( "Process successfully stopped." );
        }
    }
    //Wait for monitor thread to exit
    if( m_threadMonitor.joinable() )
    {
        m_threadMonitor.join();
    }
    return PM_STATUS::PM_OK;
}

void PmAgentController::monitorProcess()
{
    while( eProcess_Terminated == waitForProcess() )
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        LOG_DEBUG( "Child process signalled..." );
        //check if the process was terminated by us
        //and if yes, stop monitoring.
        if( !m_bIsProcessStartedByPlugin )
        {
            break;
        }

        LOG_WARNING( "Child process terminated. Starting it again." );
        std::this_thread::sleep_for( RESTART_DELAY_CHRONO );
        startProcess();
    }
    LOG_DEBUG( "Exiting monitor thread" );
}

void PmAgentController::cleanup()
{
    if( INVALID_HANDLE_VALUE != m_hChildStdinWr )
    {
        CloseHandle( m_hChildStdinWr );
        m_hChildStdinWr = INVALID_HANDLE_VALUE;
    }
    if( INVALID_HANDLE_VALUE != m_hProcess )
    {
        CloseHandle( m_hProcess );
        m_hProcess = INVALID_HANDLE_VALUE;
    }
}

PmAgentController::eProcStatus PmAgentController::waitForProcess()
{
    if( WAIT_OBJECT_0 == WaitForSingleObject( m_hProcess, INFINITE ) )
    {
        return eProcess_Terminated;
    }
    return eProcess_Active;
}

PM_STATUS PmAgentController::killIfRunning()
{
    auto retStatus = PM_STATUS::PM_ERROR;

    HANDLE hProcess = NULL;
    DWORD* pProcessIds = NULL;
    char lpBaseName[ MAX_PATH ] = { 0 };
    DWORD dwProcesses = -1, procNameSize = -1;

    DWORD maxProcSize = 0;
    int retryCount = 0;

    //retry till MAX_RETRY_COUNT by increasing the size of array with MAX_PROC_LIMIT
    do
    {
        retryCount++;
        maxProcSize += MAX_PROC_LIMIT;
        delete[] pProcessIds;
        pProcessIds = new DWORD[ maxProcSize ];

        if( FALSE == EnumProcesses( pProcessIds, maxProcSize, &dwProcesses ) )
        {
            LOG_ERROR( "Enumerating through all Processes Failed." );
            goto safe_exit;
        }
        dwProcesses /= sizeof( DWORD );

    } while( ( retryCount < MAX_RETRY_COUNT ) && ( dwProcesses == maxProcSize ) );

    for( DWORD i = 0; i < dwProcesses; i++ )
    {
        hProcess = OpenProcess(
            PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
            FALSE, /* bInheritHandle */
            pProcessIds[ i ]
        );

        // If the process couldn't be opened, it's probably a system process and shouldn't be terminated.
        if( NULL != hProcess )
        {
            // This will list the name of the current process
            procNameSize = GetModuleBaseNameA( hProcess, NULL, lpBaseName, MAX_PATH );

            // check If name has 0 characters, it's probably not the process enumerated for.
            if( 0 != procNameSize )
            {
                lpBaseName[ procNameSize ] = '\0';

                // check if name is different from the one which is searched
                if( PM_AGENT_BINARY == std::string( lpBaseName ) )
                {
                    LOG_DEBUG( "Process name is [%s] and process pid is [%d]", lpBaseName, pProcessIds[ i ] );
                    if( TerminateProcess( hProcess, 0 ) )
                    {
                        LOG_DEBUG( "Process name = [%s] with pid = [%d] terminated.", PM_AGENT_BINARY, pProcessIds[ i ] );
                        break;
                    }
                    else
                    {
                        LOG_ERROR( "TerminateProcess failed." );
                        goto safe_exit;
                    }
                }
            }
            CloseHandle( hProcess );
            hProcess = NULL;
        }
    }

    retStatus = PM_STATUS::PM_OK;

safe_exit:
    delete[] pProcessIds;
    pProcessIds = NULL;

    if( NULL != hProcess )
    {
        CloseHandle( hProcess );
        hProcess = NULL;
    }

    return retStatus;
}

PM_STATUS CodeSignToPmStatus( CodesignStatus status ) 
{
    switch( status )
    {
    case CodesignStatus::CODE_SIGNER_SUCCESS:
        return PM_STATUS::PM_OK;
    case CodesignStatus::CODE_SIGNER_ERROR:
        return PM_STATUS::PM_ERROR;
    case CodesignStatus::CODE_SIGNER_INVALID:
        return PM_STATUS::PM_INVAL;
    case CodesignStatus::CODE_SIGNER_EXPIRED:
        return PM_STATUS::PM_CODE_SIGN_EXPIRED;
    case CodesignStatus::CODE_SIGNER_MISMATCH:
        return PM_STATUS::PM_CODE_SIGNER_MISMATCH;
    case CodesignStatus::CODE_SIGNER_VERIFICATION_FAILED:
        return PM_STATUS::PM_CODE_SIGN_VERIFICATION_FAILED;
    }

    return PM_STATUS::PM_ERROR;
}

PM_STATUS PmAgentController::startProcess()
{
    auto status = PM_STATUS::PM_ERROR;
    const DWORD dwCmdlineLen = 1024;
    TCHAR tszCmdline[ dwCmdlineLen ] = { 0 };
    DWORD bRetStatus = -1;

    //Verify CodeSign
    status = CodeSignToPmStatus( m_codesignVerifier.Verify( m_tstrProcessPath, SIGNER_CISCO, SIGTYPE_DEFAULT ) );
    if( PM_STATUS::PM_OK != status )
    {
        LOG_ERROR( "Sign verification failed for the process" );
        return status;
    }

    SECURITY_ATTRIBUTES saAttr;
    STARTUPINFO siStartInfo;
    PROCESS_INFORMATION piProcInfo;
    HANDLE hChildStdinRd = INVALID_HANDLE_VALUE;

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = sizeof( SECURITY_ATTRIBUTES );
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    // command line passed as : 'process path' + _T("--config-path") + 'process config path'
    // to provide ease if additional arguments are needed.
    std::wstring tstrProcessArgs = m_tstrProcessPath + L" --bootstrap " + L"\"" + m_tstrBsConfigPath + L"\"" + L" --config-file " + L"\"" + m_tstrPmConfigPath + L"\"";

    //Create a pipe for the child process's STDIN
    if( !CreatePipe( &hChildStdinRd, &m_hChildStdinWr, &saAttr, 0 ) )
    {
        LOG_ERROR( "StdinRd CreatePipe failure" );
        status = PM_STATUS::PM_FAIL;
        goto graceful_exit;
    }

    // Ensure the write handle to the pipe for STDIN is not inherited.
    if( !SetHandleInformation( m_hChildStdinWr, HANDLE_FLAG_INHERIT, 0 ) )
    {
        LOG_ERROR( "SetHandleInformation failure" );
        status = PM_STATUS::PM_FAIL;
        goto graceful_exit;
    }

    // Set up members of the PROCESS_INFORMATION structure.
    ZeroMemory( &piProcInfo, sizeof( PROCESS_INFORMATION ) );

    // Set up members of the STARTUPINFO structure.
    ZeroMemory( &siStartInfo, sizeof( siStartInfo ) );
    siStartInfo.cb = sizeof( siStartInfo );
    siStartInfo.hStdInput = hChildStdinRd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    if( wcscpy_s( tszCmdline, dwCmdlineLen, tstrProcessArgs.c_str() ) != 0 ) {
        LOG_ERROR( "wcscpy_s failure" );
        status = PM_STATUS::PM_INSUFFICIENT_BUFFER;
        goto graceful_exit;
    }

    WLOG_DEBUG( L"StartingProcess: %s", tszCmdline );
    // Create the child process.
    bRetStatus = CreateProcess( NULL, //application name
        tszCmdline,     // command line
        NULL,          // process security attributes
        NULL,          // primary thread security attributes
        TRUE,          // handles are inherited
        CREATE_NO_WINDOW,  // creation flags
        NULL,          // use parent's environment
        NULL,          // use parent's current directory
        &siStartInfo,  // STARTUPINFO pointer
        &piProcInfo );  // receives PROCESS_INFORMATION

    if( !bRetStatus )
    {
        WLOG_ERROR( L"CreateProcess failed for command: %s", tszCmdline );
        status = PM_STATUS::PM_FAIL;
        goto graceful_exit;
    }
    else
    {
        m_hProcess = piProcInfo.hProcess;
        CloseHandle( piProcInfo.hThread );
        status = PM_STATUS::PM_OK;
    }

graceful_exit:
    if( INVALID_HANDLE_VALUE != hChildStdinRd )
    {
        CloseHandle( hChildStdinRd );
        hChildStdinRd = INVALID_HANDLE_VALUE;
    }
    if( PM_STATUS::PM_OK != status )
    {
        cleanup();
    }

    return status;
}

PM_STATUS PmAgentController::stopProcess()
{
    //instead of sending a message, we will
    //close the write handle and this would
    //trigger an error in the child process's read pipe
    if( INVALID_HANDLE_VALUE != m_hChildStdinWr )
    {
        CloseHandle( m_hChildStdinWr );
        m_hChildStdinWr = INVALID_HANDLE_VALUE;
    }

    if( INVALID_HANDLE_VALUE == m_hProcess )
    {
        return PM_STATUS::PM_ERROR;
    }

    //wait for 5s and terminate if not responding.
    if( WAIT_OBJECT_0 != WaitForSingleObject( m_hProcess, 5000 ) )
    {
        LOG_WARNING( "Process hasn't terminated after 5s. Force terminate..." );
        if( !TerminateProcess( m_hProcess, 0 ) )
        {
            LOG_ERROR( "Failed to force terminate child." );
            return PM_STATUS::PM_ERROR;
        }
        CloseHandle( m_hProcess );
        m_hProcess = INVALID_HANDLE_VALUE;
    }
    return PM_STATUS::PM_OK;
}

