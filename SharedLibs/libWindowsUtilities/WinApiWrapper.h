#pragma once

#include "IWinApiWrapper.h"

class WinApiWrapper : public IWinApiWrapper
{
public:
    WinApiWrapper();
    ~WinApiWrapper();

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
};

