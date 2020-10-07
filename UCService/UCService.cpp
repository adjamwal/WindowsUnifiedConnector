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
}

void UCService::OnStop()
{
    LOG_DEBUG( "in OnStop" );
}

#pragma endregion


#pragma region Helper Functions

bool UCService::FileExists( const char* filename )
{
    struct stat buffer;
    return ( stat( filename, &buffer ) == 0 );
}

bool UCService::DirectoryExists( const char* dirname )
{
    DWORD ftyp = GetFileAttributesA( dirname );
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

#pragma endregion
