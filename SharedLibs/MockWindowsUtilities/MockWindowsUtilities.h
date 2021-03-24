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

    MOCK_METHOD4( ReadRegistryStringA, bool( HKEY, const std::string&, const std::string&, std::string& ) );
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

    MOCK_METHOD1( ResolveKnownFolderId, std::string( const std::string& ) );
    void MakeResolveKnownFolderIdReturn( std::string value );
    void ExpectResolveKnownFolderIdIsNotCalled();

    MOCK_METHOD0( GetDataDir, std::wstring() );
    void MakeGetDataDirReturn( std::wstring value );
    void ExpectGetDataDirIsNotCalled();

    static MockWindowsUtilities* GetMockWindowUtilities();
    static void Init();
    static void Deinit();

private:
    static MockWindowsUtilities* s_mock;
    static std::mutex s_mutex;

    std::vector<WindowsUtilities::WindowsInstallProgram> m_defaultDiscoveryList;
};
