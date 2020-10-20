// PackageMananger.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include <shellapi.h>
#include <string>
#include "UcLogger.h"
#include "UcLogFile.h"
#include "PmAgent.h"
#include "PmAgentContainer.h"

int APIENTRY wWinMain( _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow )
{
    UcLogFile logFile;
    logFile.Init();

    UcLogger logger( logFile );
    logger.SetLogLevel( IUcLogger::LOG_ERROR );
    SetUcLogger( &logger );

    WLOG_DEBUG( L"Enter %s", lpCmdLine );

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW( lpCmdLine, &argc );

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
    PmAgentContainer agentContainer( L"" );
    int32_t rtn = agentContainer.pmAgent().VerifyConfig( configFile );

    LOG_DEBUG( "Exit %d", rtn );
    return rtn;
}

