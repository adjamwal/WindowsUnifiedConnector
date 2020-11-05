#pragma once

#include <Windows.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "IWinApiWrapper.h"

class MockWinApiWrapper : public IWinApiWrapper
{
public:
    MockWinApiWrapper();
    ~MockWinApiWrapper();

    MOCK_METHOD10( CreateProcessW, BOOL( LPCWSTR lpApplicationName,
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

    MOCK_METHOD2( WaitForSingleObject, DWORD( HANDLE, DWORD ) );
    void MakeWaitForSingleObjectReturn( DWORD value );

    MOCK_METHOD2( GetExitCodeProcess, BOOL( HANDLE hProcess, LPDWORD lpExitCode ) );
    void MakeGetExitCodeProcessReturn( BOOL value );

    MOCK_METHOD0( GetLastError, DWORD( ) );
    void MakeGetLastErrorReturn( DWORD );

    MOCK_METHOD4( SHGetKnownFolderPath, HRESULT( REFKNOWNFOLDERID rfid, DWORD dwFlags, HANDLE hToken, PWSTR* ppszPath) );
    void MakeSHGetKnownFolderPathReturn( HRESULT );
};
