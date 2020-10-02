#include "stdafx.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ServiceInstaller.h"
#include "UCService.h"

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
    }
    else
    {
        wprintf( L"Parameters:\n" );
        wprintf( L" -install  to install the UC service.\n" );
        wprintf( L" -remove   to remove the UC service.\n" );

        UCService service( SERVICE_NAME );
        if( !ServiceBase::Run( service ) )
        {
            wprintf( L"UC Service failed to run w/err 0x%08lx\n", GetLastError() );
        }
    }

    return 0;
}