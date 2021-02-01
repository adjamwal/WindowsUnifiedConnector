#include "stdafx.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ServiceInstaller.h"
#include "UCService.h"
#include "Main.h"

int wmain( int argc, wchar_t* argv[] )
{
    if( ( argc > 1 ) && ( ( *argv[ 1 ] == L'-' || ( *argv[ 1 ] == L'/' ) ) ) )
    {
        if( _wcsicmp( L"install", argv[ 1 ] + 1 ) == 0 )
        {
            // Install the service when the arg is "-install" or "/install". 
            InstallService(
                SERVICE_NAME,               // Name of service 
                SERVICE_DISPLAY_NAME,       // Name to display 
                SERVICE_START_TYPE,         // Service start type 
                SERVICE_DEPENDENCIES,       // Dependencies 
                SERVICE_ACCOUNT,            // Service running account 
                SERVICE_PASSWORD            // Password of the account 
            );
        }
        else if( _wcsicmp( L"remove", argv[ 1 ] + 1 ) == 0 )
        {
            // Uninstall the service when the arg is "-remove" or "/remove". 
            UninstallService( SERVICE_NAME );
        }
        else
        {
            DisplayHelp();
        }
    }
    else
    {
        DisplayHelp();

        try
        {
            UCService service( SERVICE_NAME );
            if ( !ServiceBase::Run( service ) )
            {
                wprintf( L"%s failed to run w/err 0x%08lx\n", SERVICE_NAME, GetLastError() );
            }
        }
        catch ( std::exception& ex )
        {
            LOG_ERROR( "Failed to run service: %s", ex.what() );
        }
    }

    return 0;
}

void DisplayHelp()
{
    wprintf( L"Parameters:\n" );
    wprintf( L" -install  to install the %s.\n", SERVICE_NAME );
    wprintf( L" -remove   to remove the %s.\n", SERVICE_NAME );
}
