#pragma once

#include <Windows.h>
#include "gmock/gmock.h"
#include <string>
#include <mutex>

class MockWindowsUtilities
{
public:
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

    MOCK_METHOD0( Is64BitWindows, bool() );
    void MakeIs64BitWindowsReturn( bool value );
    void ExpectIs64BitWindowsIsNotCalled();

    MOCK_METHOD1( GetSysDirectory, bool( std::string& ) );
    void MakeGetSysDirectoryReturn( bool value );
    void ExpectGetSysDirectoryIsNotCalled();

    static MockWindowsUtilities* GetMockWindowUtilities();
    static void Init();
    static void Deinit();

private:
    MockWindowsUtilities();
    ~MockWindowsUtilities();

    static MockWindowsUtilities* s_mock;
    static std::mutex s_mutex;
};
