#pragma once

#include <string>
#include <Windows.h>
#include <vector>

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
    static bool DirectoryExists(const WCHAR* dirname);
    static std::wstring GetExePath();
    static std::wstring GetDirPath(const std::wstring& path);
    static bool ReadRegistryString(_In_ HKEY hKey, _In_ const std::wstring& subKey, _In_ const std::wstring& valueName, _Out_ std::wstring& data);
    static bool ReadRegistryStringA( _In_ HKEY hKey, _In_ const std::string& subKey, _In_ const std::string& valueName, _In_ DWORD flags, _Out_ std::string& data );
    static bool Is64BitWindows();
    static bool GetSysDirectory( std::string& path );
    static std::vector<WindowsInstallProgram> GetInstalledPrograms();
    static std::string ResolveKnownFolderId( const std::string& knownFolderId );
    static std::wstring GetLogDir();
};
