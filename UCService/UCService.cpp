#include "stdafx.h"

#include <Evntrace.h>
#include <sys/stat.h>
#include <string>
#include <vector>

#include "UCService.h"

#pragma region Service Constructor and Destructor

UCService::UCService(
    PWSTR pszServiceName,
    BOOL fCanStop,
    BOOL fCanShutdown,
    BOOL fCanPauseContinue )
    : ServiceBase( pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue )
    , m_ucpm( { 0 } )
    , m_isUcpmLoaded( false )
{
    LOG_DEBUG( "created" );
}

UCService::~UCService( void )
{
    LOG_DEBUG( "destroyed" );
}

#pragma endregion


#pragma region Service Event Handlers

void UCService::OnStart( _In_ DWORD dwArgc, _In_ PWSTR* pszArgv )
{
    LOG_DEBUG( "in OnStart" );

    LoadPMControlModule();
}

void UCService::OnStop()
{
    LOG_DEBUG( "in OnStop" );

    UnloadPMControlModule();
}

#pragma endregion


#pragma region Module Control Functions

void UCService::LoadPMControlModule()
{
    if( m_isUcpmLoaded )
    {
        LOG_ERROR( "PackageManager Control Module already running.");
        return;
    }

    std::wstring serviceDir( GetExePath() );
    std::wstring pmConfigFile( serviceDir );
    pmConfigFile.append( L"\\" );
    pmConfigFile.append( PM_MCP_CONFIG_FILENAME );

    if( !FileExists( pmConfigFile.c_str() ) )
    {
        LOG_ERROR( "PackageManager Control Module configuration file not found: %s", pmConfigFile.c_str() );
        return;
    }

    m_ucpm.nVersion = PM_MODULE_INTERFACE_VERSION;
    if( m_ucpm.fpInit )
    {
        m_ucpm.fpInit();
    }

    PM_MODULE_RESULT_T result;

    if( ( result = CreateModuleInstance( &m_ucpm, m_logger.get() ) ) != PM_MODULE_SUCCESS )
    {
        LOG_ERROR( "Failed to load PackageManager Control Module: CreateModuleInstance() returned %d.", result );
        return;
    }

    if( ( result = m_ucpm.fpStart( serviceDir.c_str(), serviceDir.c_str(), pmConfigFile.c_str() ) ) != PM_MODULE_SUCCESS )
    {
        LOG_ERROR( "Failed to start PackageManager Control Module: fpStart() returned %d.", result );
        return;
    }

    m_isUcpmLoaded = true;

    LOG_DEBUG( "PackageManager Control Module loaded and started." );
}

void UCService::UnloadPMControlModule()
{
    if( !m_isUcpmLoaded )
    {
        LOG_ERROR( "PackageManager Control Module already released." );
        return;
    }

    PM_MODULE_RESULT_T result;

    if( ( result = m_ucpm.fpStop() ) != PM_MODULE_SUCCESS )
    {
        LOG_ERROR( "Failed to stop PackageManager Control Module: fpStop() returned %d.", result );
        return;
    }

    if( ( result = ReleaseModuleInstance( &m_ucpm ) ) != PM_MODULE_SUCCESS )
    {
        LOG_ERROR( "Failed to release PackageManager Control Module: ReleaseModuleInstance() returned %d.", result );
        return;
    }

    m_isUcpmLoaded = false;

    LOG_DEBUG( "PackageManager Control Module stopped and released." );
}

#pragma endregion

#pragma region Helper Functions

bool UCService::FileExists( const WCHAR* filename )
{
    struct _stat stFileInfo;
    return ( _wstat( filename, &stFileInfo ) == 0 );
}

bool UCService::DirectoryExists( const WCHAR* dirname )
{
    DWORD ftyp = GetFileAttributes( dirname );
    if( ftyp == INVALID_FILE_ATTRIBUTES )
    {
        return false;
    }

    if( ftyp & FILE_ATTRIBUTE_DIRECTORY )
    {
        return true;
    }

    return false;
}

std::wstring UCService::GetExePath()
{
    WCHAR buffer[ MAX_PATH ] = { 0 };

    GetModuleFileName( NULL, buffer, MAX_PATH );
    std::wstring::size_type pos = std::wstring( buffer ).find_last_of( L"/\\" );

    return std::wstring( buffer ).substr( 0, pos );
}

#pragma endregion
