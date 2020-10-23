#pragma once

#include <windows.h>
#include <Shlobj.h>

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
};