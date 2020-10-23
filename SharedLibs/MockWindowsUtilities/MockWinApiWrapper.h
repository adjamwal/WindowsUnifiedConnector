#pragma once

#include "pch.h"
#include "IWinApiWrapper.h"

class MockWinApiWrapper : public IWinApiWrapper
{
public:
    MockWinApiWrapper();
    ~MockWinApiWrapper();

    MOCK_METHOD( BOOL, CreateProcessW, ( LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation ) );
    void MakeCreateProcessWReturn( BOOL value );

    MOCK_METHOD( DWORD, WaitForSingleObject, ( HANDLE, DWORD ) );
    void MakeWaitForSingleObjectReturn( DWORD value );

    MOCK_METHOD( BOOL, GetExitCodeProcess, ( HANDLE hProcess, LPDWORD lpExitCode ) );
    void MakeGetExitCodeProcessReturn( BOOL value );

    MOCK_METHOD( DWORD, GetLastError, ( ) );
    void MakeGetLastErrorReturn( DWORD );

    MOCK_METHOD( HRESULT, SHGetKnownFolderPath, ( REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath) );
    void MakeSHGetKnownFolderPathReturn( HRESULT );
};