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

    std::wstring bsConfigFile;
    std::wstring pmConfigFile;

    // --bootstrap " + L"\"" + m_tstrBsConfigPath + L"\"" + L" --config-path
    int i = 0;
    for ( i = 0; i < argc; i++ ) {
        if ( std::wstring( _T( "--bootstrap" ) ) == argv[i] ) {
            if ( ++i < argc ) {
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

    PmAgentContainer agentContainer( L"", L"" );
    int32_t rtn = agentContainer.pmAgent().VerifyBsConfig( bsConfigFile );

    LOG_DEBUG( "Verify Bs: %d", rtn );

    if ( rtn == 0)
    {
        rtn = agentContainer.pmAgent().VerifyPmConfig( pmConfigFile );

        LOG_DEBUG( "Verify Pm %d", rtn );
    }
    
    LOG_DEBUG( "Exit: %d", rtn );

    return rtn;
}

