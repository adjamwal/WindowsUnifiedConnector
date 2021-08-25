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

bool WindowsUtilities::WriteFileContents( const WCHAR* filename, const uint8_t* content, const size_t contentLen )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->WriteFileContents( filename, content, contentLen );
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

bool WindowsUtilities::ReadRegistryStringA( _In_ HKEY hKey, _In_ const std::string& subKey, _In_ const std::string& valueName, _In_ DWORD flags, _Out_ std::string& data )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->ReadRegistryStringA( hKey, subKey, valueName, flags, data );
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

std::string WindowsUtilities::ResolveKnownFolderIdForDefaultUser( const std::string& knownFolderId )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->ResolveKnownFolderIdForDefaultUser( knownFolderId );
}

std::string WindowsUtilities::ResolveKnownFolderIdForCurrentUser( const std::string& knownFolderId )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->ResolveKnownFolderIdForCurrentUser( knownFolderId );
}

std::wstring WindowsUtilities::GetLogDir()
{
    return MockWindowsUtilities::GetMockWindowUtilities()->GetLogDir();
}

std::string WindowsUtilities::ResolvePath( const std::string& basePath )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->ResolvePath( basePath );
}

int32_t WindowsUtilities::FileSearchWithWildCard( const std::filesystem::path& path, std::vector<std::filesystem::path>& results )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->FileSearchWithWildCard( path, results );
}

int32_t WindowsUtilities::SearchFiles( std::filesystem::path searchPath,
    std::vector<std::filesystem::path>::iterator begin,
    std::vector<std::filesystem::path>::iterator end,
    std::vector<std::filesystem::path>& results )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->SearchFiles( searchPath, begin, end, results );
}

bool WindowsUtilities::AllowEveryoneAccessToFile( const std::filesystem::path& path )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->AllowEveryoneAccessToFile( path );
}

bool WindowsUtilities::AllowBuiltinUsersReadAccessToPath( const std::filesystem::path& path )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->AllowBuiltinUsersReadAccessToPath( path );
}

bool WindowsUtilities::SetSidAccessToPath( const std::filesystem::path& path, const std::wstring& userSid, TRUSTEE_TYPE trusteeType, DWORD accessPermissions )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->SetSidAccessToPath( path, userSid, trusteeType, accessPermissions );
}

bool WindowsUtilities::SetWellKnownGroupAccessToPath( const std::filesystem::path& path, WELL_KNOWN_SID_TYPE wellKnownSid, DWORD accessPermissions, bool disableInheritance )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->SetWellKnownGroupAccessToPath( path, wellKnownSid, accessPermissions, disableInheritance );
}

bool WindowsUtilities::SetNamedUserAccessToPath( const std::filesystem::path& path, const std::wstring& userName, DWORD accessPermissions )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->SetNamedUserAccessToPath( path, userName, accessPermissions );
}

bool WindowsUtilities::SetPathOwnership( const std::filesystem::path& path, WELL_KNOWN_SID_TYPE userOrGroupSid, TRUSTEE_TYPE trusteeType )
{
    return MockWindowsUtilities::GetMockWindowUtilities()->SetPathOwnership( path, userOrGroupSid, trusteeType );
}
