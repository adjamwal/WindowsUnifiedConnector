#include "pch.h"
#include "WinApiWrapper.h"

WinApiWrapper::WinApiWrapper()
{

}

BOOL WinApiWrapper::CreateProcessW( LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation )
{
    return ::CreateProcessW( lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation );
}

DWORD WinApiWrapper::WaitForSingleObject( HANDLE hHandle, DWORD dwMilliseconds )
{
    return ::WaitForSingleObject( hHandle, dwMilliseconds );
}

BOOL WinApiWrapper::GetExitCodeProcess( HANDLE hProcess, LPDWORD lpExitCode )
{
    return ::GetExitCodeProcess( hProcess, lpExitCode );
}

DWORD WinApiWrapper::GetLastError()
{
    return ::GetLastError();
}

HRESULT WinApiWrapper::SHGetKnownFolderPath( REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath )
{
    return ::SHGetKnownFolderPath( rfid, dwFlags, hToken, ppszPath );
}

UINT WinApiWrapper::MsiEnumProductsExW(
    LPCWSTR szProductCode,
    LPCWSTR szUserSid,
    DWORD dwContext,
    DWORD dwIndex,
    WCHAR* szInstalledProductCode,
    MSIINSTALLCONTEXT* pdwInstalledContext,
    LPWSTR szSid,
    LPDWORD pcchSid )
{
    return ::MsiEnumProductsExW( 
        szProductCode, 
        szUserSid,
        dwContext,
        dwIndex,
        szInstalledProductCode,
        pdwInstalledContext,
        szSid,
        pcchSid );
}

INSTALLSTATE WinApiWrapper::MsiQueryProductStateW(
    LPCWSTR szProduct )
{
    return ::MsiQueryProductStateW( szProduct );
}

UINT WinApiWrapper::MsiGetProductInfoExW(
    LPCWSTR szProductCode,
    LPCWSTR szUserSid,
    MSIINSTALLCONTEXT dwContext,
    LPCWSTR szProperty,
    LPWSTR szValue,
    LPDWORD pcchValue )
{
    return ::MsiGetProductInfoExW(
        szProductCode,
        szUserSid,
        dwContext,
        szProperty,
        szValue,
        pcchValue );
}

UINT WinApiWrapper::MsiGetProductPropertyW(
    MSIHANDLE hProduct,
    LPCWSTR szProperty,
    LPWSTR lpValueBuf,
    LPDWORD pcchValueBuf )
{
    return ::MsiGetProductPropertyW(
        hProduct,
        szProperty,
        lpValueBuf,
        pcchValueBuf );
}

UINT WinApiWrapper::MsiEnumRelatedProductsW(
    LPCWSTR lpUpgradeCode,
    DWORD dwReserved,
    DWORD iProductIndex,
    LPWSTR lpProductBuf )
{
    return ::MsiEnumRelatedProductsW(
        lpUpgradeCode,
        dwReserved,
        iProductIndex,
        lpProductBuf );
}
