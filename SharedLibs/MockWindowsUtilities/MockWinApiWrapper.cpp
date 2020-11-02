#include "pch.h"
#include "MockWinApiWrapper.h"

MockWinApiWrapper::MockWinApiWrapper()
{
}

MockWinApiWrapper::~MockWinApiWrapper()
{
}

void MockWinApiWrapper::MakeCreateProcessWReturn( BOOL value )
{
    ON_CALL( *this, CreateProcessW( _, _, _, _, _, _, _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::MakeWaitForSingleObjectReturn( DWORD value )
{
    ON_CALL( *this, WaitForSingleObject( _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::MakeGetExitCodeProcessReturn( BOOL value )
{
    ON_CALL( *this, GetExitCodeProcess( _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::MakeGetLastErrorReturn( DWORD value )
{
    ON_CALL( *this, GetLastError( ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::MakeSHGetKnownFolderPathReturn( HRESULT value )
{
    ON_CALL( *this, SHGetKnownFolderPath( _, _, _, _ ) ).WillByDefault( Return( value ) );
}
