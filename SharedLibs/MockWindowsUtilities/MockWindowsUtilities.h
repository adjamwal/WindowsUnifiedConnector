#pragma once

#include <Windows.h>
#include "gmock/gmock.h"
#include <string>
#include <mutex>
#include <vector>
#include "WindowsUtilities.h"

class MockWindowsUtilities
{
public:
    MockWindowsUtilities();
    ~MockWindowsUtilities();

    MOCK_METHOD1( FileExists, bool( const WCHAR* ) );
    void MakeFileExistsReturn( bool value );
    void ExpectFileExistsIsNotCalled();
    
    MOCK_METHOD1( ReadFileContents, std::string( const WCHAR* ) );
    void MakeReadFileContentsReturn( std::string value );
    void ExpectReadFileContentsIsNotCalled();

    MOCK_METHOD1( GetFileModifyTime, uint32_t( const WCHAR* ) );
    void MakeGetFileModifyTimeReturn( uint32_t value );
    void ExpectGetFileModifyTimeIsNotCalled();

    MOCK_METHOD3( WriteFileContents, bool( const WCHAR*, const uint8_t*, const size_t) );
    void MakeWriteFileContentsReturn( bool  value );
    void ExpectWriteFileContentsIsNotCalled();

    MOCK_METHOD1( DirectoryExists, bool( const WCHAR* ) );
    void MakeDirectoryExistsReturn( bool value );
    void ExpectDirectoryExistsIsNotCalled();
    
    MOCK_METHOD0( GetExePath, std::wstring() );
    void MakeGetExePathReturn( std::wstring value );
    void ExpectGetExePathIsNotCalled();

    MOCK_METHOD1( GetDirPath, std::wstring( const std::wstring& ) );
    void MakeGetDirPathReturn( std::wstring value );
    void ExpectGetDirPathIsNotCalled();

    MOCK_METHOD4( ReadRegistryString, bool( HKEY, const std::wstring&, const std::wstring&, std::wstring& ) );
    void MakeReadRegistryStringReturn( bool value );
    void ExpectReadRegistryStringIsNotCalled();

    MOCK_METHOD5( ReadRegistryStringA, bool( HKEY, const std::string&, const std::string&, DWORD flags, std::string& ) );
    void MakeReadRegistryStringAReturn( bool value );
    void ExpectReadRegistryStringAIsNotCalled();

    MOCK_METHOD0( Is64BitWindows, bool() );
    void MakeIs64BitWindowsReturn( bool value );
    void ExpectIs64BitWindowsIsNotCalled();

    MOCK_METHOD1( GetSysDirectory, bool( std::string& ) );
    void MakeGetSysDirectoryReturn( bool value );
    void ExpectGetSysDirectoryIsNotCalled();

    MOCK_METHOD0( GetInstalledPrograms, std::vector<WindowsUtilities::WindowsInstallProgram>() );
    void MakeGetInstalledProgramsReturn( std::vector<WindowsUtilities::WindowsInstallProgram>& value );
    void ExpectGetInstalledProgramsIsNotCalled();

    MOCK_METHOD1( ResolveKnownFolderIdForDefaultUser, std::string( const std::string& ) );
    void MakeResolveKnownFolderIdReturn( std::string value );
    void ExpectResolveKnownFolderIdIsNotCalled();

    MOCK_METHOD1( ResolveKnownFolderIdForCurrentUser, std::string( const std::string& ) );
    void MakeResolveKnownFolderIdForCurrentUserReturn( std::string value );
    void ExpectResolveKnownFolderIdForCurrentUserIsNotCalled();

    MOCK_METHOD0( GetLogDir, std::wstring() );
    void MakeGetLogDirReturn( std::wstring value );
    void ExpectGetLogDirIsNotCalled();

    MOCK_METHOD1( ResolvePath, std::string( const std::string& ) );
    void MakeResolvePathReturn( std::string value );
    void ExpectResolvePathIsNotCalled();

    MOCK_METHOD2( FileSearchWithWildCard, int32_t( const std::filesystem::path&, std::vector<std::filesystem::path>& ) );
    void MakeFileSearchWithWildCardReturn( int32_t value );
    void ExpectFileSearchWithWildCardIsNotCalled();

    MOCK_METHOD4( SearchFiles, int32_t( std::filesystem::path searchPath,
        std::vector<std::filesystem::path>::iterator begin,
        std::vector<std::filesystem::path>::iterator end,
        std::vector<std::filesystem::path>& results ) );
    void MakeSearchFilesReturn( int32_t value );
    void ExpectSearchFilesIsNotCalled();

    MOCK_METHOD1( AllowEveryoneAccessToFile, bool( const std::filesystem::path& ) );
    void MakeAllowEveryoneAccessToFileReturn( bool value );
    void ExpectAllowEveryoneAccessToFileNotCalled();

    MOCK_METHOD1( AllowBuiltinUsersReadAccessToPath, bool( const std::filesystem::path& ) );
    void MakeAllowBuiltinUsersReadAccessToPathReturn( bool value );
    void ExpectAllowBuiltinUsersReadAccessToPathNotCalled();

    MOCK_METHOD4( SetSidAccessToPath, bool( const std::filesystem::path&, const std::wstring&, TRUSTEE_TYPE, DWORD ) );
    void MakeSetSidAccessToPathReturn( bool value );
    void ExpectSetSidAccessToPathNotCalled();

    MOCK_METHOD4( SetWellKnownGroupAccessToPath, bool( const std::filesystem::path&, WELL_KNOWN_SID_TYPE, DWORD, bool ) );
    void MakeSetWellKnownGroupAccessToPathReturn( bool value );
    void ExpectSetWellKnownGroupAccessToPathNotCalled();

    MOCK_METHOD3( SetNamedUserAccessToPath, bool( const std::filesystem::path&, const std::wstring&, DWORD ) );
    void MakeSetNamedUserAccessToPathReturn( bool value );
    void ExpectSetNamedUserAccessToPathNotCalled();

    MOCK_METHOD3( SetPathOwnership, bool ( const std::filesystem::path&, WELL_KNOWN_SID_TYPE, TRUSTEE_TYPE ) );
    void MakeSetPathOwnershipReturn( bool value );
    void ExpectSetPathOwnershipNotCalled();

    MOCK_METHOD1( WinHttpGet, bool( const std::string& ) );
    void MakeWinHttpGetReturn( bool value );
    void ExpectWinHttpGetNotCalled();

    MOCK_METHOD1( GetTimeZoneOffset, bool( int32_t& ) );
    void MakeGetTimeZoneOffsetReturn( bool value );
    void ExpectGetTimeZoneOffsetNotCalled();

    static MockWindowsUtilities* GetMockWindowUtilities();
    static void Init();
    static void Deinit();

private:
    static MockWindowsUtilities* s_mock;
    static std::mutex s_mutex;

    std::vector<WindowsUtilities::WindowsInstallProgram> m_defaultDiscoveryList;
};
