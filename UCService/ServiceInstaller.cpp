#include "stdafx.h"

#include <stdio.h>
#include <windows.h>

#include "ServiceInstaller.h"

void InstallService(
    _In_ PWSTR pszServiceName,
    _In_ PWSTR pszDisplayName,
    _In_ DWORD dwStartType,
    _In_ PWSTR pszDependencies,
    _In_ PWSTR pszAccount,
    _In_ PWSTR pszPassword )
{
    SC_HANDLE schSCManager = nullptr;
    SC_HANDLE schService = nullptr;

    wchar_t szPath[ MAX_PATH ];
    if( GetModuleFileName( nullptr, szPath, ARRAYSIZE( szPath ) ) == 0 )
    {
        wprintf( L"GetModuleFileName failed w/err 0x%08lx\n", GetLastError() );
        goto Cleanup;
    }

    schSCManager = OpenSCManager( nullptr, nullptr, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE );
    if( schSCManager == nullptr )
    {
        wprintf( L"OpenSCManager failed w/err 0x%08lx\n", GetLastError() );
        goto Cleanup;
    }

    schService = CreateService(
        schSCManager,                   // SCManager database
        pszServiceName,                 // Name of service
        pszDisplayName,                 // Name to display
        SERVICE_QUERY_STATUS | SERVICE_START,  // Desired access
        SERVICE_WIN32_OWN_PROCESS,      // Service type
        dwStartType,                    // Service start type
        SERVICE_ERROR_NORMAL,           // Error control type
        szPath,                         // Service's binary
        nullptr,                           // No load ordering group
        nullptr,                           // No tag identifier
        pszDependencies,                // Dependencies
        pszAccount,                     // Service running account
        pszPassword                     // Password of the account
    );

    if( schService == nullptr )
    {
        wprintf( L"CreateService failed w/err 0x%08lx\n", GetLastError() );
        goto Cleanup;
    }

    wprintf( L"%s is installed.\n", pszServiceName );

    if( StartService( schService, 0, nullptr ) == 0 )
    {
        wprintf( L"StartService failed w/err 0x%08lx\n", GetLastError() );
        goto Cleanup;
    }

    wprintf( L"%s is started.\n", pszServiceName );

Cleanup:
    if( schSCManager )
    {
        CloseServiceHandle( schSCManager );
        schSCManager = nullptr;
    }
    if( schService )
    {
        CloseServiceHandle( schService );
        schService = nullptr;
    }
}

void UninstallService( _In_ PWSTR pszServiceName )
{
    SC_HANDLE schSCManager = nullptr;
    SC_HANDLE schService = nullptr;
    SERVICE_STATUS ssSvcStatus = {};

    schSCManager = OpenSCManager( nullptr, nullptr, SC_MANAGER_CONNECT );
    if( schSCManager == nullptr )
    {
        wprintf( L"OpenSCManager failed w/err 0x%08lx\n", GetLastError() );
        goto Cleanup;
    }

    schService = OpenService( schSCManager, pszServiceName, SERVICE_STOP |
        SERVICE_QUERY_STATUS | DELETE );
    if( schService == nullptr )
    {
        wprintf( L"OpenService failed w/err 0x%08lx\n", GetLastError() );
        goto Cleanup;
    }

    if( ControlService( schService, SERVICE_CONTROL_STOP, &ssSvcStatus ) )
    {
        wprintf( L"Stopping %s.", pszServiceName );
        Sleep( 1000 );

        while( QueryServiceStatus( schService, &ssSvcStatus ) )
        {
            if( ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING )
            {
                wprintf( L"." );
                Sleep( 1000 );
            }
            else break;
        }

        if( ssSvcStatus.dwCurrentState == SERVICE_STOPPED )
        {
            wprintf( L"\n%s is stopped.\n", pszServiceName );
        }
        else
        {
            wprintf( L"\n%s failed to stop.\n", pszServiceName );
        }
    }

    if( !DeleteService( schService ) )
    {
        wprintf( L"DeleteService failed w/err 0x%08lx\n", GetLastError() );
        goto Cleanup;
    }

    wprintf( L"%s is removed.\n", pszServiceName );

Cleanup:
    if( schSCManager )
    {
        CloseServiceHandle( schSCManager );
        schSCManager = nullptr;
    }
    if( schService )
    {
        CloseServiceHandle( schService );
        schService = nullptr;
    }
}