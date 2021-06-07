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

    std::wstring bsConfigFile;
    if ( !WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE, L"Software\\Cisco\\SecureClient\\UnifiedConnector\\config", L"Bootstrapper", bsConfigFile ) )
    {
        throw std::exception( "Failed to read bootstrapper config path from registry");
    }

    if ( !WindowsUtilities::FileExists( bsConfigFile.c_str() ) )
    {
        throw std::exception( "Boostrapper config file not found" );
    }

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

    try {
        FixUcdtShortcut();
    }
    catch( ... ) {
        WLOG_ERROR( L"Error Fixing Ucdt Shortcut" );
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

void UCService::FixUcdtShortcut()
{
    std::wstring shortcutPath;

    if( WindowsUtilities::ReadRegistryString( HKEY_LOCAL_MACHINE,
        L"Software\\Cisco\\SecureClient\\UnifiedConnector\\UCSERVICE",
        L"ucdt Shortcut",
        shortcutPath ) ) {

        if( !shortcutPath.empty() ) {
            WLOG_DEBUG( L"Fixing ucdt shortcut" );

            if( !WindowsUtilities::AllowEveryoneAccessToFile( shortcutPath ) ) {
                WLOG_ERROR( "Failed on %s", shortcutPath.c_str() );
            }
        }
        else {
            WLOG_ERROR( "shortcut is empty" );
        }
    }
    else {
        WLOG_ERROR( "Failed to read ucdt Shortcut from registry" );
    }
}