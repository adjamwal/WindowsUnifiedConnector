#include "pch.h"
#include "MockWindowsUtilities.h"
#include "WindowsUtilities.h"


bool WindowsUtilities::FileExists( const WCHAR* filename )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->FileExists( filename );
}

std::string WindowsUtilities::ReadFileContents( const WCHAR* filename )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->ReadFileContents( filename );
}

uint32_t WindowsUtilities::GetFileModifyTime( const WCHAR* filename )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->GetFileModifyTime( filename );
}

bool WindowsUtilities::DirectoryExists( const WCHAR* dirname )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->DirectoryExists( dirname );
}

std::wstring WindowsUtilities::GetExePath()
{
    return MockWindowsUtilities::GetMockWindowUtilities()->GetExePath();
}

std::wstring WindowsUtilities::GetDirPath( const std::wstring& path )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->GetDirPath( path );
}

bool WindowsUtilities::ReadRegistryString( _In_ HKEY hKey, _In_ const std::wstring& subKey, _In_ const std::wstring& valueName, _Out_ std::wstring& data )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->ReadRegistryString( hKey, subKey, valueName, data );
}

bool WindowsUtilities::Is64BitWindows()
{
    return MockWindowsUtilities::GetMockWindowUtilities()->Is64BitWindows();
}

bool WindowsUtilities::GetSysDirectory( std::string& path )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->GetSysDirectory( path );
}

std::vector<WindowsUtilities::WindowsInstallProgram> WindowsUtilities::GetInstalledPrograms()
{
    return MockWindowsUtilities::GetMockWindowUtilities()->GetInstalledPrograms();
}

std::string WindowsUtilities::ResolveKnownFolderId( const std::string& knownFolderId )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->ResolveKnownFolderId( knownFolderId );
}

std::wstring WindowsUtilities::GetDataDir()
{
    return MockWindowsUtilities::GetMockWindowUtilities()->GetDataDir();
}

std::string utf8_encode( const std::wstring& wstr )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->utf8_encode();
}

std::wstring utf8_decode( const std::string& str )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->utf8_decode();
}