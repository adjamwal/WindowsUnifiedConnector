#include "stdafx.h"
#include "HelperFunctions.h"

bool HelperFunctions::FileExists( const WCHAR* filename )
{
    struct _stat stFileInfo;
    return ( _wstat( filename, &stFileInfo ) == 0 );
}

bool HelperFunctions::DirectoryExists( const WCHAR* dirname )
{
    DWORD ftyp = GetFileAttributes( dirname );
    if( ftyp == INVALID_FILE_ATTRIBUTES )
    {
        return false;
    }

    if( ftyp & FILE_ATTRIBUTE_DIRECTORY )
    {
        return true;
    }

    return false;
}

std::wstring HelperFunctions::GetExePath()
{
    WCHAR buffer[ MAX_PATH ] = { 0 };

    GetModuleFileName( NULL, buffer, MAX_PATH );
    std::wstring::size_type pos = std::wstring( buffer ).find_last_of( L"/\\" );

    return std::wstring( buffer ).substr( 0, pos );
}

bool HelperFunctions::ReadRegistryString( _In_ HKEY hKey, _In_ const std::wstring& subKey, _In_ const std::wstring& valueName, _Out_ std::wstring& data )
{
    DWORD dataSize {};
    LONG retCode = ::RegGetValue( hKey, subKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, nullptr, &dataSize );

    if( retCode != ERROR_SUCCESS )
    {
        return false;
    }

    data.resize( dataSize / sizeof( wchar_t ) );

    retCode = ::RegGetValue( hKey, subKey.c_str(), valueName.c_str(), RRF_RT_REG_SZ, nullptr, &data[ 0 ], &dataSize );
    if( retCode != ERROR_SUCCESS )
    {
        return false;
    }

    DWORD stringLengthInWchars = dataSize / sizeof( wchar_t );
    stringLengthInWchars--; // Exclude the NUL written by the Win32 API
    data.resize( stringLengthInWchars );

    return true;
}

