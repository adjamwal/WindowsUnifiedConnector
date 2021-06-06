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

void MockWinApiWrapper::MakeMsiEnumProductsExWReturn( UINT value )
{
    ON_CALL( *this, MsiEnumProductsExW( _, _, _, _, _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::MakeMsiQueryProductStateWReturn( INSTALLSTATE value )
{
    ON_CALL( *this, MsiQueryProductStateW( _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::MakeMsiGetProductInfoExWReturn( UINT value )
{
    ON_CALL( *this, MsiGetProductInfoExW( _, _, _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::MakeMsiGetProductPropertyWReturn( UINT value )
{
    ON_CALL( *this, MsiGetProductPropertyW( _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::MakeMsiEnumRelatedProductsWReturn( UINT value )
{
    ON_CALL( *this, MsiEnumRelatedProductsW( _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::MakeFindFirstFileExWReturn( HANDLE value )
{
    ON_CALL( *this, FindFirstFileExW( _, _, _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::MakeFindNextFileWReturn( BOOL value )
{
    ON_CALL( *this, FindNextFileW( _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::MakeFindCloseReturn( BOOL value )
{
    ON_CALL( *this, FindClose( _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::MakeWTSEnumerateSessionsWReturn( BOOL value )
{
    ON_CALL( *this, WTSEnumerateSessionsW( _, _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::ExpectWTSEnumerateSessionsWNotCalled()
{
    EXPECT_CALL( *this, WTSEnumerateSessionsW( _, _, _, _, _ ) ).Times( 0 );
}

void MockWinApiWrapper::ExpectWTSFreeMemoryNotCalled()
{
    EXPECT_CALL( *this, WTSFreeMemory( _ ) ).Times( 0 );
}

void MockWinApiWrapper::MakeWTSQueryUserTokenReturn( BOOL value )
{
    ON_CALL( *this, WTSQueryUserToken( _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::ExpectWTSQueryUserTokenNotCalled()
{
    EXPECT_CALL( *this, WTSQueryUserToken( _, _ ) ).Times( 0 );
}

void MockWinApiWrapper::MakeCloseHandleReturn( BOOL value )
{
    ON_CALL( *this, CloseHandle( _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::ExpectCloseHandleNotCalled()
{
    EXPECT_CALL( *this, CloseHandle( _ ) ).Times( 0 );
}

void MockWinApiWrapper::MakeCreateEnvironmentBlockReturn( BOOL value )
{
    ON_CALL( *this, CreateEnvironmentBlock( _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::ExpectCreateEnvironmentBlockNotCalled()
{
    EXPECT_CALL( *this, CreateEnvironmentBlock( _, _, _ ) ).Times( 0 );
}

void MockWinApiWrapper::MakeDestroyEnvironmentBlockReturn( BOOL value )
{
    ON_CALL( *this, DestroyEnvironmentBlock( _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::ExpectDestroyEnvironmentBlockNotCalled()
{
    EXPECT_CALL( *this, DestroyEnvironmentBlock( _ ) ).Times( 0 );
}

void MockWinApiWrapper::MakeCreateProcessAsUserWReturn( BOOL value )
{
    ON_CALL( *this, CreateProcessAsUserW( _, _, _, _, _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWinApiWrapper::ExpectCreateProcessAsUserWNotCalled()
{
    EXPECT_CALL( *this, CreateProcessAsUserW( _, _, _, _, _, _, _, _ ) ).Times( 0 );
}