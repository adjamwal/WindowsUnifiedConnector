#include "pch.h"
#include "MockWindowsUtilities.h"

MockWindowsUtilities* MockWindowsUtilities::s_mock = nullptr;
std::mutex MockWindowsUtilities::s_mutex;

MockWindowsUtilities& MockWindowsUtilities::GetMockWindowUtilities()
{
    std::lock_guard<std::mutex> lock( s_mutex );
    if( !s_mock ) {
        throw( std::exception( "Mock not initiailzed" ) );
    }

    return *s_mock;
}

void MockWindowsUtilities::InitMock()
{
    std::lock_guard<std::mutex> lock( s_mutex );
    if( !s_mock ) {
        s_mock = new MockWindowsUtilities();
    }
}

void MockWindowsUtilities::DeinitMock() 
{
    std::lock_guard<std::mutex> lock( s_mutex );
    if( s_mock ) {
        delete s_mock;
        s_mock = nullptr;
    }
}

MockWindowsUtilities::MockWindowsUtilities()
{
    MakeGetExePathReturn( L"" );
    MakeGetDirPathReturn( L"" );
}

MockWindowsUtilities::~MockWindowsUtilities()
{

}

void MockWindowsUtilities::MakeFileExistsReturn( bool value )
{
    ON_CALL( *this, FileExists( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectFileExistsIsNotCalled()
{
    EXPECT_CALL( *this, FileExists( _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeDirectoryExistsReturn( bool value )
{
    ON_CALL( *this, DirectoryExists( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectDirectoryExistsIsNotCalled()
{
    EXPECT_CALL( *this, DirectoryExists( _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeGetExePathReturn( std::wstring value )
{
    ON_CALL( *this, GetExePath() ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectGetExePathIsNotCalled()
{
    EXPECT_CALL( *this, GetExePath() ).Times( 0 );
}

void MockWindowsUtilities::MakeGetDirPathReturn( std::wstring value )
{
    ON_CALL( *this, GetDirPath( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectGetDirPathIsNotCalled()
{
    EXPECT_CALL( *this, GetDirPath( _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeReadRegistryStringReturn( bool value )
{
    ON_CALL( *this, ReadRegistryString( _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectReadRegistryStringIsNotCalled()
{
    EXPECT_CALL( *this, ReadRegistryString( _, _, _, _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeIs64BitWindowsReturn( bool value )
{
    ON_CALL( *this, Is64BitWindows() ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectIs64BitWindowsIsNotCalled()
{
    EXPECT_CALL( *this, Is64BitWindows() ).Times( 0 );
}

void MockWindowsUtilities::MakeGetSysDirectoryReturn( bool value )
{
    ON_CALL( *this, GetSysDirectory( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectGetSysDirectoryIsNotCalled()
{
    EXPECT_CALL( *this, GetSysDirectory( _ ) ).Times( 0 );
}
