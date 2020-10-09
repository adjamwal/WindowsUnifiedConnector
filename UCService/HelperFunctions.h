#pragma once

#include <string>
#include <Windows.h>

class HelperFunctions
{
public:
    static bool FileExists( const WCHAR* filename );
    static bool DirectoryExists( const WCHAR* dirname );
    static std::wstring GetExePath();
    static std::wstring GetDirPath( const std::wstring& path );
    static bool ReadRegistryString( _In_ HKEY hKey, _In_ const std::wstring& subKey, _In_ const std::wstring& valueName, _Out_ std::wstring& data );
};
