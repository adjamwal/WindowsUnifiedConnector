#pragma once

#include <string>
#include <mutex>

class MockWindowsUtilities
{
public:
    MOCK_METHOD1( FileExists, bool( const WCHAR* ) );
    void MakeFileExistsReturn( bool value );
    void ExpectFileExistsIsNotCalled();
    
    MOCK_METHOD1( DirectoryExists, bool( const WCHAR* ) );
    void MakeDirectoryExistsReturn( bool value );
    void ExpectDirectoryExistsIsNotCalled();
    
    MOCK_METHOD0( GetExePath, std::wstring() );
    void MakeGetExePathReturn( std::wstring value );
    void ExpectGetExePathIsNotCalled();

    MOCK_METHOD1( GetDirPath, std::wstring( const std::wstring& path ) );
    void MakeGetDirPathReturn( std::wstring value );
    void ExpectGetDirPathIsNotCalled();

    MOCK_METHOD4( ReadRegistryString, bool( _In_ HKEY hKey, _In_ const std::wstring& subKey, _In_ const std::wstring& valueName, _Out_ std::wstring& data ) );
    void MakeReadRegistryStringReturn( bool value );
    void ExpectReadRegistryStringIsNotCalled();

    MOCK_METHOD0( Is64BitWindows, bool() );
    void MakeIs64BitWindowsReturn( bool value );
    void ExpectIs64BitWindowsIsNotCalled();

    MOCK_METHOD1( GetSysDirectory, bool( std::string& path ) );
    void MakeGetSysDirectoryReturn( bool value );
    void ExpectGetSysDirectoryIsNotCalled();

    static MockWindowsUtilities& GetMockWindowUtilities();
    static void InitMock();
    static void DeinitMock();

private:
    MockWindowsUtilities();
    ~MockWindowsUtilities();

    static MockWindowsUtilities* s_mock;
    static std::mutex s_mutex;
};
