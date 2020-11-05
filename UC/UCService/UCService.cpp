#include "stdafx.h"

#include <Evntrace.h>
#include <sys/stat.h>
#include <string>
#include <vector>

#include "UCService.h"
#include "WindowsUtilities.h"

#pragma region Service Constructor and Destructor

UCService::UCService(
    PWSTR pszServiceName,
    BOOL fCanStop,
    BOOL fCanShutdown,
    BOOL fCanPauseContinue )
    : ServiceBase( pszServiceName, fCanStop, fCanShutdown, fCanPauseContinue )
    , m_codeSignVerifer()
    , m_ucidLoader( m_codeSignVerifer )
    , m_ucmcpLoader( m_codeSignVerifer )
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

    try
    {
        m_ucidLoader.LoadControlModule();
    }
    catch( ... )
    {
        WLOG_ERROR( L"Error loading UCID Module" );
    }

    try
    {
        m_ucmcpLoader.LoadControlModule();
    }
    catch( ... )
    {
        WLOG_ERROR( L"Error loading UCMCP Module" );
    }
}

void UCService::OnStop()
{
    WLOG_DEBUG( L"in OnStop" );

    try
    {
        m_ucmcpLoader.UnloadControlModule();
    }
    catch( ... )
    {
        WLOG_ERROR( L"Error unloading UCMCP Module" );
    }
    try
    {
        m_ucidLoader.UnloadControlModule();
    }
    catch( ... )
    {
        WLOG_ERROR( L"Error unloading UCID Module" );
    }
}

#pragma endregion
