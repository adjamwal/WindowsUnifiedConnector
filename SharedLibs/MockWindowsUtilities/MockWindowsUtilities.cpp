#include "pch.h"
#include "MockWindowsUtilities.h"

MockWindowsUtilities* MockWindowsUtilities::s_mock = nullptr;
std::mutex MockWindowsUtilities::s_mutex;

void MockWindowsUtilities::Init()
{
    std::lock_guard<std::mutex> lock( s_mutex );
    if( !s_mock ) {
        s_mock = new NiceMock<MockWindowsUtilities>();
    }
}

void MockWindowsUtilities::Deinit()
{
    if( s_mock ) {
        delete s_mock;
        s_mock = nullptr;
    }
}

MockWindowsUtilities* MockWindowsUtilities::GetMockWindowUtilities()
{
    std::lock_guard<std::mutex> lock( s_mutex );
    if( !s_mock ) {
        throw( std::exception( "Mock not initiailzed" ) );
    }

    return s_mock;
}

MockWindowsUtilities::MockWindowsUtilities()
{
    MakeReadFileContentsReturn( "" );
    MakeGetExePathReturn( L"" );
    MakeGetDirPathReturn( L"" );
    MakeGetInstalledProgramsReturn( m_defaultDiscoveryList );
    MakeGetLogDirReturn( L"" );
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

void MockWindowsUtilities::MakeReadFileContentsReturn( std::string value )
{
    ON_CALL( *this, ReadFileContents( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectReadFileContentsIsNotCalled()
{
    EXPECT_CALL( *this, ReadFileContents( _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeGetFileModifyTimeReturn( uint32_t value )
{
    ON_CALL( *this, GetFileModifyTime( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectGetFileModifyTimeIsNotCalled()
{
    EXPECT_CALL( *this, GetFileModifyTime( _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeWriteFileContentsReturn( bool  value )
{
    ON_CALL( *this, WriteFileContents( _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectWriteFileContentsIsNotCalled()
{
    EXPECT_CALL( *this, WriteFileContents( _, _, _ ) ).Times( 0 );
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

void MockWindowsUtilities::MakeReadRegistryStringAReturn( bool value )
{
    ON_CALL( *this, ReadRegistryStringA( _, _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectReadRegistryStringAIsNotCalled()
{
    EXPECT_CALL( *this, ReadRegistryStringA( _, _, _, _, _ ) ).Times( 0 );
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

void MockWindowsUtilities::MakeGetInstalledProgramsReturn( std::vector<WindowsUtilities::WindowsInstallProgram>& value )
{
    ON_CALL( *this, GetInstalledPrograms() ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectGetInstalledProgramsIsNotCalled()
{
    EXPECT_CALL( *this, GetInstalledPrograms() ).Times( 0 );
}

void MockWindowsUtilities::MakeResolveKnownFolderIdReturn( std::string value )
{
    ON_CALL( *this, ResolveKnownFolderIdForDefaultUser( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectResolveKnownFolderIdIsNotCalled()
{
    EXPECT_CALL( *this, ResolveKnownFolderIdForDefaultUser( _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeResolveKnownFolderIdForCurrentUserReturn( std::string value )
{
    ON_CALL( *this, ResolveKnownFolderIdForCurrentUser( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectResolveKnownFolderIdForCurrentUserIsNotCalled()
{
    EXPECT_CALL( *this, ResolveKnownFolderIdForCurrentUser( _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeGetLogDirReturn( std::wstring value )
{
    ON_CALL( *this, GetLogDir() ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectGetLogDirIsNotCalled()
{
    EXPECT_CALL( *this, GetLogDir() ).Times( 0 );
}

void MockWindowsUtilities::MakeResolvePathReturn( std::string value )
{
    ON_CALL( *this, ResolvePath( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectResolvePathIsNotCalled()
{
    EXPECT_CALL( *this, ResolvePath( _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeFileSearchWithWildCardReturn( int32_t value )
{
    ON_CALL( *this, FileSearchWithWildCard( _, _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectFileSearchWithWildCardIsNotCalled()
{
    EXPECT_CALL( *this, FileSearchWithWildCard( _, _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeSearchFilesReturn( int32_t value )
{
    ON_CALL( *this, SearchFiles( _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectSearchFilesIsNotCalled()
{
    EXPECT_CALL( *this, SearchFiles( _, _, _, _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeAllowEveryoneAccessToFileReturn( bool value )
{
    ON_CALL( *this, AllowEveryoneAccessToFile( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectAllowEveryoneAccessToFileNotCalled()
{
    EXPECT_CALL( *this, AllowEveryoneAccessToFile( _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeAllowBuiltinUsersReadAccessToPathReturn( bool value )
{
    ON_CALL( *this, AllowBuiltinUsersReadAccessToPath( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectAllowBuiltinUsersReadAccessToPathNotCalled()
{
    EXPECT_CALL( *this, AllowBuiltinUsersReadAccessToPath( _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeSetSidAccessToPathReturn( bool value )
{
    ON_CALL( *this, SetSidAccessToPath( _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectSetSidAccessToPathNotCalled()
{
    EXPECT_CALL( *this, SetSidAccessToPath( _, _, _, _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeSetWellKnownGroupAccessToPathReturn( bool value )
{
    ON_CALL( *this, SetWellKnownGroupAccessToPath( _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectSetWellKnownGroupAccessToPathNotCalled()
{
    EXPECT_CALL( *this, SetWellKnownGroupAccessToPath( _, _, _, _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeSetNamedUserAccessToPathReturn( bool value )
{
    ON_CALL( *this, SetNamedUserAccessToPath( _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectSetNamedUserAccessToPathNotCalled()
{
    EXPECT_CALL( *this, SetNamedUserAccessToPath( _, _, _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeSetPathOwnershipReturn( bool value )
{
    ON_CALL( *this, SetPathOwnership( _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectSetPathOwnershipNotCalled()
{
    EXPECT_CALL( *this, SetPathOwnership( _, _, _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeWinHttpGetReturn( bool value )
{
    ON_CALL( *this, WinHttpGet( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectWinHttpGetNotCalled()
{
    EXPECT_CALL( *this, WinHttpGet( _ ) ).Times( 0 );
}

void MockWindowsUtilities::MakeGetTimeZoneOffsetReturn( bool value )
{
    ON_CALL( *this, GetTimeZoneOffset( _ ) ).WillByDefault( Return( value ) );
}

void MockWindowsUtilities::ExpectGetTimeZoneOffsetNotCalled()
{
    EXPECT_CALL( *this, GetTimeZoneOffset( _ ) ).Times( 0 );
}
