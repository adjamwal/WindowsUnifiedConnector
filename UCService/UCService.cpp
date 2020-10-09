#include "stdafx.h"

#include <Evntrace.h>
#include <sys/stat.h>
#include <string>
#include <vector>

#include "UCService.h"
#include "HelperFunctions.h"

#pragma region Service Constructor and Destructor

UCService::UCService(
    PWSTR pszServiceName,
    BOOL fCanStop,
    BOOL fCanShutdown,
    BOOL fCanPauseContinue )
    : ServiceBase( pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue )
    , m_codeSignVerifer()
    , m_ucmcpLoader( m_codeSignVerifer )
    , m_ucmcp( { 0 } )
    , m_isUcmcpLoaded( false )
{
    WLOG_DEBUG( L"created" );
}

UCService::~UCService( void )
{
    WLOG_DEBUG( L"destroyed" );
}

#pragma endregion


#pragma region Service Event Handlers

void UCService::OnStart( _In_ DWORD dwArgc, _In_ PWSTR* pszArgv )
{
    WLOG_DEBUG( L"in OnStart" );

    LoadPMControlModule();
}

void UCService::OnStop()
{
    WLOG_DEBUG( L"in OnStop" );

    UnloadPMControlModule();
    m_ucmcpLoader.UnloadDll();
}

#pragma endregion


#pragma region Module Control Functions

void UCService::LoadPMControlModule()
{
    if( m_isUcmcpLoaded )
    {
        WLOG_ERROR( L"PackageManager Control Module already running.");
        return;
    }

    std::wstring dllFullPath;
    if( !HelperFunctions::ReadRegistryString( HKEY_LOCAL_MACHINE, L"Software\\Cisco\\SecureXYZ\\UnifiedConnector\\UCPM", L"DllPath", dllFullPath ) )
    {
        WLOG_ERROR( L"Failed to read PackageManager Control Module data from registry" );
        return;
    }

    std::wstring pmPath( HelperFunctions::GetDirPath( dllFullPath ) );
    std::wstring pmConfigFile( pmPath );
    pmConfigFile.append( L"\\" );
    pmConfigFile.append( PM_MCP_CONFIG_FILENAME );

    if( !HelperFunctions::FileExists( pmConfigFile.c_str() ) )
    {
        WLOG_ERROR( L"PackageManager Control Module configuration file not found: %s", pmConfigFile.c_str() );
        return;
    }

    try
    {
        if( !m_ucmcpLoader.LoadDll( dllFullPath ) )
        {
            WLOG_ERROR( L"Failed to load %s", dllFullPath.c_str() );
            return;
        }
    }
    catch( std::exception &ex )
    {
        WLOG_ERROR( "Exception: %s", ex.what() );
    }
    
    m_ucmcp.nVersion = PM_MODULE_INTERFACE_VERSION;
    if( m_ucmcp.fpInit )
    {
        m_ucmcp.fpInit();
    }

    PM_MODULE_RESULT_T result;

    if( ( result = m_ucmcpLoader.CreateModule( &m_ucmcp, m_logger.get() ) ) != PM_MODULE_SUCCESS )
    {
        WLOG_ERROR( L"Failed to load PackageManager Control Module: CreateModuleInstance() returned %d.", result );
        return;
    }

    if( ( result = m_ucmcp.fpStart( pmPath.c_str(), pmPath.c_str(), pmConfigFile.c_str() ) ) != PM_MODULE_SUCCESS )
    {
        WLOG_ERROR( L"Failed to start PackageManager Control Module: fpStart() returned %d.", result );
        return;
    }

    m_isUcmcpLoaded = true;

    WLOG_DEBUG( L"PackageManager Control Module loaded and started." );
}

void UCService::UnloadPMControlModule()
{
    if( !m_isUcmcpLoaded )
    {
        WLOG_ERROR( L"PackageManager Control Module already released." );
        return;
    }

    PM_MODULE_RESULT_T result;

    if( ( result = m_ucmcp.fpStop() ) != PM_MODULE_SUCCESS )
    {
        WLOG_ERROR( L"Failed to stop PackageManager Control Module: fpStop() returned %d.", result );
        return;
    }

    if( ( result = m_ucmcpLoader.ReleaseModule( &m_ucmcp ) ) != PM_MODULE_SUCCESS )
    {
        WLOG_ERROR( L"Failed to release PackageManager Control Module: ReleaseModuleInstance() returned %d.", result );
        return;
    }

    m_isUcmcpLoaded = false;

    WLOG_DEBUG( L"PackageManager Control Module stopped and released." );
}

#pragma endregion
