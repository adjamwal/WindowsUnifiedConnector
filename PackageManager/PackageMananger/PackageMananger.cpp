// PackageMananger.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "PackageMananger.h"
#include <shellapi.h>
#include <string>
#include "UcLogger.h"
#include "UcLogFile.h"
#include "PmAgent.h"
#include "PmAgentContainer.h"
#include "CrashHandlerClient.h"
#include "WindowsUtilities.h"

void WaitForTermination()
{
    //Get STDIN handle of the process
    auto hStdin = GetStdHandle( STD_INPUT_HANDLE );
    if( INVALID_HANDLE_VALUE == hStdin )
    {
        LOG_ERROR( "Failed to get STDIN handle" );
        return;
    }

    //Wait for any input from parent process, including close of write end of pipe.
    char chData = 0;
    DWORD dwBytesRead = 0;
    ReadFile( hStdin, &chData, sizeof( chData ), &dwBytesRead, NULL );
}

int APIENTRY wWinMain( _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow )
{
    std::wstring dataDir = WindowsUtilities::GetDataDir();

    CrashHandlerClient crashClient( nullptr );
    crashClient.Init( dataDir.c_str(), MiniDumpNormal );
    crashClient.SetupCrashHandler();

    UcLogFile logFile;
    logFile.Init();

    UcLogger logger( logFile );
    logger.SetLogLevel( IUcLogger::LOG_DEBUG );
    SetUcLogger( &logger );

    WLOG_DEBUG( L"Enter %s", lpCmdLine );

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW( lpCmdLine, &argc );

    // TODO: Setup crash file generation

    if( argc < 2 ) {
        LOG_ERROR( "Error: not enough arguments" );
        return 1;
    }

    std::wstring bsConfigFile;
    std::wstring pmConfigFile;

    // --bootstrap " + L"\"" + m_tstrBsConfigPath + L"\"" + L" --config-path
    int i = 0;
    for( i = 0; i < argc; i++ ) {
        if( std::wstring( _T( "--bootstrap" ) ) == argv[ i ] ) {
            if( ++i < argc ) {
                WLOG_DEBUG( L"bs config file %ls", argv[i] );
                bsConfigFile = argv[i];
                //TODO: Validate config?
            }
            else {
                LOG_ERROR( "bs config file not provided" );
                return 1;
            }
        }
        else if ( std::wstring( _T( "--config-file" ) ) == argv[i] )
        {
            if ( ++i < argc ) {
                WLOG_DEBUG( L"pm config file %ls", argv[i] );
                pmConfigFile = argv[i];
                //TODO: Validate config?
            }
            else {
                LOG_ERROR( "pm config file not provided" );
                return 1;
            }
        }
    }

    PmAgentContainer agentContainer( bsConfigFile, pmConfigFile );
    agentContainer.pmAgent().Start();

    //Wait for termination signal from parent process
    WaitForTermination();

    agentContainer.pmAgent().Stop();

    crashClient.RemoveCrashHandler();
    LOG_DEBUG( "Exit" );
    return 0;
}

