#pragma once

#include <string>
#include <Windows.h>
#include <vector>
#include <filesystem>

class WindowsUtilities
{
public:
    struct WindowsInstallProgram {
        std::string name;
        std::string version;
    };

    static bool FileExists(const WCHAR* filename);
    static std::string ReadFileContents( const WCHAR* filename );
    static uint32_t GetFileModifyTime( const WCHAR* filename );
    static bool WriteFileContents( const WCHAR* filename, const uint8_t* content, const size_t contentLen );
    static bool DirectoryExists(const WCHAR* dirname);
    static std::wstring GetExePath();
    static std::wstring GetDirPath(const std::wstring& path);
    static bool ReadRegistryString(_In_ HKEY hKey, _In_ const std::wstring& subKey, _In_ const std::wstring& valueName, _Out_ std::wstring& data);
    static bool ReadRegistryStringA( _In_ HKEY hKey, _In_ const std::string& subKey, _In_ const std::string& valueName, _In_ DWORD flags, _Out_ std::string& data );
    static bool Is64BitWindows();
    static bool GetSysDirectory( std::string& path );
    static std::vector<WindowsInstallProgram> GetInstalledPrograms();
    static std::string ResolveKnownFolderIdForDefaultUser( const std::string& knownFolderId );
    static std::string ResolveKnownFolderIdForCurrentUser( const std::string& knownFolderId );
    static std::wstring GetLogDir();
    static std::string ResolvePath( const std::string& basePath );

    static int32_t FileSearchWithWildCard(
        const std::filesystem::path& searchPath,
        std::vector<std::filesystem::path>& results );

    static int32_t SearchFiles( std::filesystem::path searchPath,
        std::vector<std::filesystem::path>::iterator begin,
        std::vector<std::filesystem::path>::iterator end,
        std::vector<std::filesystem::path>& results );

private:
    static std::string ResolveKnownFolderId( const std::string& knownFolderId, HANDLE userHandle );
};
