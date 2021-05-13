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

HANDLE WinApiWrapper::FindFirstFileExW(
    LPCWSTR lpFileName,
    FINDEX_INFO_LEVELS fInfoLevelId,
    LPVOID lpFindFileData,
    FINDEX_SEARCH_OPS fSearchOp,
    LPVOID lpSearchFilter,
    DWORD dwAdditionalFlags )
{
    return ::FindFirstFileExW(
        lpFileName,
        fInfoLevelId,
        lpFindFileData,
        fSearchOp,
        lpSearchFilter,
        dwAdditionalFlags );
}

BOOL WinApiWrapper::FindNextFileW(
    HANDLE hFindFile,
    LPWIN32_FIND_DATAW lpFindFileData )
{
    return ::FindNextFileW(
        hFindFile,
        lpFindFileData );
}

BOOL WinApiWrapper::FindClose(
    HANDLE hFindFile )
{
    return ::FindClose(
        hFindFile );
}

BOOL WinApiWrapper::InitiateSystemShutdownExA(
    LPSTR lpMachineName,
    LPSTR lpMessage,
    DWORD dwTimeout,
    BOOL  bForceAppsClosed,
    BOOL  bRebootAfterShutdown,
    DWORD dwReason
)
{
    HANDLE           hToken;
    TOKEN_PRIVILEGES tkp;

    //https://docs.microsoft.com/en-us/windows/win32/shutdown/how-to-shut-down-the-system
    ::OpenProcessToken( ::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken );

    ::LookupPrivilegeValue( NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[ 0 ].Luid );

    tkp.PrivilegeCount = 1; // set 1 privilege
    tkp.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;

    // get the shutdown privilege for this process
    ::AdjustTokenPrivileges( hToken, FALSE, &tkp, 0, ( PTOKEN_PRIVILEGES )NULL, 0 );

    return ::InitiateSystemShutdownExA(
        lpMachineName,
        lpMessage,
        dwTimeout,
        bForceAppsClosed,
        bRebootAfterShutdown,
        dwReason
    );
}

BOOL WinApiWrapper::ExitWindowsEx( UINT  uFlags, DWORD dwReason )
{
    HANDLE           hToken = NULL;
    TOKEN_PRIVILEGES tkp;

    //https://docs.microsoft.com/en-us/windows/win32/shutdown/how-to-shut-down-the-system
    ::OpenProcessToken( ::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken );

    ::LookupPrivilegeValue( NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[ 0 ].Luid );

    tkp.PrivilegeCount = 1; // set 1 privilege
    tkp.Privileges[ 0 ].Attributes = SE_PRIVILEGE_ENABLED;

    // get the shutdown privilege for this process
    ::AdjustTokenPrivileges( hToken, FALSE, &tkp, 0, ( PTOKEN_PRIVILEGES )NULL, 0 );

    return ::ExitWindowsEx( uFlags, dwReason );
}
