#include "pch.h"
#include "WindowsUtilities.h"
#include <locale>
#include <codecvt>
#include <fstream>
#include <ShlObj.h>
#include <Msi.h>

bool WindowsUtilities::FileExists(const WCHAR* filename)
{
    struct _stat stFileInfo;
    return (_wstat(filename, &stFileInfo) == 0);
}

std::string WindowsUtilities::ReadFileContents( const WCHAR* filename )
{
    std::string contents;

    if( FileExists( filename ) ) {
        std::ifstream file( filename );

        if( file.is_open() ) {
            file.seekg( 0, std::ios::end );
            contents.resize( file.tellg() );
            file.seekg( 0, std::ios::beg );

            file.read( &contents[ 0 ], contents.size() );
        }

    }

    return contents;
}

uint32_t WindowsUtilities::GetFileModifyTime( const WCHAR* filename )
{
    struct _stat stFileInfo = { 0 };
    _wstat( filename, &stFileInfo );

    return ( uint32_t )stFileInfo.st_mtime;
}

bool WindowsUtilities::DirectoryExists(const WCHAR* dirname)
{
    DWORD ftyp = GetFileAttributes(dirname);
    if (ftyp == INVALID_FILE_ATTRIBUTES)
    {
        return false;
    }

    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
    {
        return true;
    }

    return false;
}

std::wstring WindowsUtilities::GetExePath()
{
    WCHAR buffer[MAX_PATH] = { 0 };

    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"/\\");

    return std::wstring(buffer).substr(0, pos);
}

std::wstring WindowsUtilities::GetDirPath(const std::wstring& path)
{
    std::wstring::size_type pos = std::wstring(path).find_last_of(L"/\\");

    return path.substr(0, pos);
}

bool WindowsUtilities::ReadRegistryString(_In_ HKEY hKey, _In_ const std::wstring& subKey, _In_ const std::wstring& valueName, _Out_ std::wstring& data)
{
    DWORD dataSize{};
    LONG retCode = ::RegGetValue(hKey, subKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, nullptr, &dataSize);

    if (retCode != ERROR_SUCCESS)
    {
        return false;
    }

    data.resize(dataSize / sizeof(wchar_t));

    retCode = ::RegGetValue(hKey, subKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, &data[0], &dataSize);
    if (retCode != ERROR_SUCCESS)
    {
        return false;
    }

    DWORD stringLengthInWchars = dataSize / sizeof(wchar_t);
    stringLengthInWchars--; // Exclude the NUL written by the Win32 API
    data.resize(stringLengthInWchars);

    return true;
}

bool WindowsUtilities::Is64BitWindows()
{
#if defined(_WIN64)
    return TRUE;  // 64-bit programs run only on Win64
#elif defined(_WIN32)
    // 32-bit programs run on both 32-bit and 64-bit Windows so we must sniff
    BOOL f64 = FALSE;
    return IsWow64Process(GetCurrentProcess(), &f64) && f64;
#else
    return FALSE; // Win64 does not support Win16
#endif
}

bool WindowsUtilities::GetSysDirectory( std::string& path )
{
    bool ret = false;
    PWSTR tmpPath = nullptr;

    HRESULT result = ::SHGetKnownFolderPath( FOLDERID_System, KF_FLAG_DEFAULT, NULL, &tmpPath );

    if ( SUCCEEDED( result ) )
    {
        if ( tmpPath != nullptr )
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring systemPath( tmpPath );

            path = converter.to_bytes( systemPath );
        }
        
        ret = true;
    }
    
    return ret;
}

#define GUID_SIZE 39
std::vector<WindowsUtilities::WindowsInstallProgram> WindowsUtilities::GetInstalledPrograms()
{
    DWORD dwIndex = 0;
    DWORD dwStatus = ERROR_SUCCESS;
    static const DWORD max_size = 1024;
    DWORD cchdata = max_size;
    wchar_t data[ max_size ] = { 0 };
    wchar_t szProductCode[ GUID_SIZE ] = { 0 };
    std::vector<WindowsInstallProgram> list;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

    do {
        WindowsInstallProgram item;
        memset( szProductCode, 0, sizeof( szProductCode ) );
        dwStatus = MsiEnumProducts( dwIndex, szProductCode );
        if( dwStatus != ERROR_SUCCESS ) {
            break;
        }

        cchdata = max_size;
        if( MsiGetProductInfo( szProductCode, INSTALLPROPERTY_PRODUCTNAME, data, &cchdata ) == 0 ) {
            item.name = converter.to_bytes( data );
            cchdata = max_size;
            if( MsiGetProductInfoW( szProductCode, INSTALLPROPERTY_VERSIONSTRING, data, &cchdata ) == 0 ) {
                item.version = converter.to_bytes( data );
                list.push_back( item );
            }
        }
        dwIndex++;
    } while( dwStatus == ERROR_SUCCESS );

    return list;
}