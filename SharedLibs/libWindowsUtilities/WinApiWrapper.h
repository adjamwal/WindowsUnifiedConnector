#pragma once

#include "IWinApiWrapper.h"

class WinApiWrapper : public IWinApiWrapper
{
public:
    WinApiWrapper();

    virtual BOOL CreateProcessW(
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation );

    virtual DWORD WaitForSingleObject( HANDLE hHandle, DWORD dwMilliseconds );

    virtual BOOL GetExitCodeProcess( HANDLE hProcess, LPDWORD lpExitCode );

    virtual DWORD GetLastError();

    virtual HRESULT SHGetKnownFolderPath( REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath );

    virtual UINT MsiEnumProductsExW(
        LPCWSTR szProductCode,
        LPCWSTR szUserSid,
        DWORD dwContext,
        DWORD dwIndex,
        WCHAR* szInstalledProductCode,
        MSIINSTALLCONTEXT* pdwInstalledContext,
        LPWSTR szSid,
        LPDWORD pcchSid );

    virtual INSTALLSTATE MsiQueryProductStateW(
        LPCWSTR szProduct );

    virtual UINT MsiGetProductInfoExW(
        LPCWSTR szProductCode,
        LPCWSTR szUserSid,
        MSIINSTALLCONTEXT dwContext,
        LPCWSTR szProperty,
        LPWSTR szValue,
        LPDWORD pcchValue );

    virtual UINT MsiGetProductPropertyW(
        MSIHANDLE hProduct,
        LPCWSTR szProperty,
        LPWSTR lpValueBuf,
        LPDWORD pcchValueBuf );

    virtual UINT MsiEnumRelatedProductsW(
        LPCWSTR lpUpgradeCode,
        DWORD dwReserved,
        DWORD iProductIndex,
        LPWSTR lpProductBuf );

    virtual HANDLE FindFirstFileExW(
        LPCWSTR lpFileName,
        FINDEX_INFO_LEVELS fInfoLevelId,
        LPVOID lpFindFileData,
        FINDEX_SEARCH_OPS fSearchOp,
        LPVOID lpSearchFilter,
        DWORD dwAdditionalFlags );

    virtual BOOL FindNextFileW(
        HANDLE hFindFile,
        LPWIN32_FIND_DATAW lpFindFileData );

    virtual BOOL FindClose(
        HANDLE hFindFile );
};

