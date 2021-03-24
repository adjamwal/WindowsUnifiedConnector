#pragma once

#include <windows.h>
#include <Shlobj.h>
#include <Msi.h>

class IWinApiWrapper
{
public:
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
        LPPROCESS_INFORMATION lpProcessInformation ) = 0;

    virtual DWORD WaitForSingleObject( HANDLE hHandle, DWORD dwMilliseconds ) = 0;

    virtual BOOL GetExitCodeProcess( HANDLE hProcess, LPDWORD lpExitCode ) = 0;

    virtual DWORD GetLastError() = 0;

    virtual HRESULT SHGetKnownFolderPath( REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath ) = 0;

    virtual UINT MsiEnumProductsExW(
        LPCWSTR szProductCode,
        LPCWSTR szUserSid,
        DWORD dwContext,
        DWORD dwIndex,
        WCHAR* szInstalledProductCode,
        MSIINSTALLCONTEXT* pdwInstalledContext,
        LPWSTR szSid,
        LPDWORD pcchSid ) = 0;

    virtual INSTALLSTATE MsiQueryProductStateW(
        LPCWSTR szProduct ) = 0;

    virtual UINT MsiGetProductInfoExW(
        LPCWSTR szProductCode,
        LPCWSTR szUserSid,
        MSIINSTALLCONTEXT dwContext,
        LPCWSTR szProperty,
        LPWSTR szValue,
        LPDWORD pcchValue ) = 0;

    virtual UINT MsiGetProductPropertyW(
        MSIHANDLE hProduct,
        LPCWSTR szProperty,
        LPWSTR lpValueBuf,
        LPDWORD pcchValueBuf ) = 0;

    virtual UINT MsiEnumRelatedProductsW(
        LPCWSTR lpUpgradeCode,
        DWORD dwReserved,
        DWORD iProductIndex,
        LPWSTR lpProductBuf ) = 0;
};