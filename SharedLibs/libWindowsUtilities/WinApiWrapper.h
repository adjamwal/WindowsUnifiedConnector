#pragma once

#include "IWinApiWrapper.h"

class WinApiWrapper : public IWinApiWrapper
{
public:
    WinApiWrapper();

    BOOL CreateProcessW(
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation ) override;

    DWORD WaitForSingleObject( HANDLE hHandle, DWORD dwMilliseconds ) override;

    BOOL GetExitCodeProcess( HANDLE hProcess, LPDWORD lpExitCode ) override;

    DWORD GetLastError() override;

    HRESULT SHGetKnownFolderPath( REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath ) override;

    UINT MsiEnumProductsExW(
        LPCWSTR szProductCode,
        LPCWSTR szUserSid,
        DWORD dwContext,
        DWORD dwIndex,
        WCHAR* szInstalledProductCode,
        MSIINSTALLCONTEXT* pdwInstalledContext,
        LPWSTR szSid,
        LPDWORD pcchSid ) override;

    INSTALLSTATE MsiQueryProductStateW(
        LPCWSTR szProduct ) override;

    UINT MsiGetProductInfoExW(
        LPCWSTR szProductCode,
        LPCWSTR szUserSid,
        MSIINSTALLCONTEXT dwContext,
        LPCWSTR szProperty,
        LPWSTR szValue,
        LPDWORD pcchValue ) override;

    UINT MsiGetProductPropertyW(
        MSIHANDLE hProduct,
        LPCWSTR szProperty,
        LPWSTR lpValueBuf,
        LPDWORD pcchValueBuf ) override;

    UINT MsiEnumRelatedProductsW(
        LPCWSTR lpUpgradeCode,
        DWORD dwReserved,
        DWORD iProductIndex,
        LPWSTR lpProductBuf ) override;

    HANDLE FindFirstFileExW(
        LPCWSTR lpFileName,
        FINDEX_INFO_LEVELS fInfoLevelId,
        LPVOID lpFindFileData,
        FINDEX_SEARCH_OPS fSearchOp,
        LPVOID lpSearchFilter,
        DWORD dwAdditionalFlags ) override;

    BOOL FindNextFileW(
        HANDLE hFindFile,
        LPWIN32_FIND_DATAW lpFindFileData ) override;

    BOOL FindClose(
        HANDLE hFindFile ) override;

    BOOL InitiateSystemShutdownExA(
        LPSTR lpMachineName,
        LPSTR lpMessage,
        DWORD dwTimeout,
        BOOL  bForceAppsClosed,
        BOOL  bRebootAfterShutdown,
        DWORD dwReason
    ) override;

    BOOL ExitWindowsEx( UINT uFlags, DWORD dwReason ) override;

    BOOL WTSEnumerateSessionsW(
        HANDLE hServer,
        DWORD Reserved,
        DWORD Version,
        PWTS_SESSION_INFOW* ppSessionInfo,
        DWORD* pCount
    ) override;

    void WTSFreeMemory( PVOID pMemory ) override;

    BOOL WTSQueryUserToken( ULONG SessionId, PHANDLE phToken ) override;

    BOOL CloseHandle( HANDLE hObject ) override;

    BOOL CreateEnvironmentBlock( LPVOID* lpEnvironment, HANDLE hToken, BOOL bInherit ) override;

    BOOL DestroyEnvironmentBlock( LPVOID  lpEnvironment ) override;

    BOOL CreateProcessAsUserW(
        HANDLE hToken,
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        //LPSECURITY_ATTRIBUTES lpProcessAttributes, USE null
        //LPSECURITY_ATTRIBUTES lpThreadAttributes, use null
        //BOOL bInheritHandles, use FALSE
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation
    ) override;
};
