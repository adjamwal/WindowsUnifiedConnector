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

    int i = 0;
    for( i = 0; i < argc; i++ ) {
        if( std::wstring( _T( "--config-path" ) ) == argv[ i ] ) {
            if( ++i < argc ) {
                //TODO: Validate config?
                break;
            }
            else {
                LOG_ERROR( "config file not provided" );
                return 1;
            }
        }
    }

    WLOG_DEBUG( L"Config file %s", argv[ i ] );
    std::wstring configFile = argv[ i ];
    PmAgentContainer agentContainer( configFile );
    agentContainer.pmAgent().Start();

    //Wait for termination signal from parent process
    WaitForTermination();

    agentContainer.pmAgent().Stop();

    LOG_DEBUG( "Exit" );
    return 0;
}

