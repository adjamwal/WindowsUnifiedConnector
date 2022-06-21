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

    MOCK_METHOD8( MsiEnumProductsExW, UINT(
        LPCWSTR szProductCode,
        LPCWSTR szUserSid,
        DWORD dwContext,
        DWORD dwIndex,
        WCHAR* szInstalledProductCode,
        MSIINSTALLCONTEXT* pdwInstalledContext,
        LPWSTR szSid,
        LPDWORD pcchSid ) );
    void MakeMsiEnumProductsExWReturn( UINT value );

    MOCK_METHOD1( MsiQueryProductStateW, INSTALLSTATE(
        LPCWSTR szProduct ) );
    void MakeMsiQueryProductStateWReturn( INSTALLSTATE value );
    
    MOCK_METHOD6( MsiGetProductInfoExW, UINT(
        LPCWSTR szProductCode,
        LPCWSTR szUserSid,
        MSIINSTALLCONTEXT dwContext,
        LPCWSTR szProperty,
        LPWSTR szValue,
        LPDWORD pcchValue ) );
    void MakeMsiGetProductInfoExWReturn( UINT value );

    MOCK_METHOD4( MsiGetProductPropertyW, UINT(
        MSIHANDLE hProduct,
        LPCWSTR szProperty,
        LPWSTR lpValueBuf,
        LPDWORD pcchValueBuf ) );
    void MakeMsiGetProductPropertyWReturn( UINT value );

    MOCK_METHOD4( MsiEnumRelatedProductsW, UINT(
        LPCWSTR lpUpgradeCode,
        DWORD dwReserved,
        DWORD iProductIndex,
        LPWSTR lpProductBuf ) );
    void MakeMsiEnumRelatedProductsWReturn( UINT value );

    MOCK_METHOD6( FindFirstFileExW, HANDLE(
        LPCWSTR lpFileName,
        FINDEX_INFO_LEVELS fInfoLevelId,
        LPVOID lpFindFileData,
        FINDEX_SEARCH_OPS fSearchOp,
        LPVOID lpSearchFilter,
        DWORD dwAdditionalFlags ) );
    void MakeFindFirstFileExWReturn( HANDLE value );

    MOCK_METHOD2( FindNextFileW, BOOL(
        HANDLE hFindFile,
        LPWIN32_FIND_DATAW lpFindFileData ) );
    void MakeFindNextFileWReturn( BOOL value );

    MOCK_METHOD1( FindClose, BOOL(
        HANDLE hFindFile  ) );
    void MakeFindCloseReturn( BOOL value );

    MOCK_METHOD6( InitiateSystemShutdownExA, 
        BOOL( LPSTR, LPSTR, DWORD, BOOL, BOOL, DWORD ) );

    MOCK_METHOD2( ExitWindowsEx, BOOL( UINT, DWORD ) );

    MOCK_METHOD5( WTSEnumerateSessionsW, BOOL( HANDLE, DWORD, DWORD, PWTS_SESSION_INFOW*, DWORD* ) );
    void MakeWTSEnumerateSessionsWReturn( BOOL value );
    void ExpectWTSEnumerateSessionsWNotCalled();

    MOCK_METHOD1( WTSFreeMemory, void( PVOID ) );
    void ExpectWTSFreeMemoryNotCalled();

    MOCK_METHOD2( WTSQueryUserToken, BOOL( ULONG, PHANDLE ) );
    void MakeWTSQueryUserTokenReturn( BOOL value );
    void ExpectWTSQueryUserTokenNotCalled();

    MOCK_METHOD1( CloseHandle, BOOL( HANDLE ) );
    void MakeCloseHandleReturn( BOOL value );
    void ExpectCloseHandleNotCalled();

    MOCK_METHOD3( CreateEnvironmentBlock, BOOL( LPVOID*, HANDLE, BOOL ) );
    void MakeCreateEnvironmentBlockReturn( BOOL value );
    void ExpectCreateEnvironmentBlockNotCalled();

    MOCK_METHOD1( DestroyEnvironmentBlock, BOOL( LPVOID ) );
    void MakeDestroyEnvironmentBlockReturn( BOOL value );
    void ExpectDestroyEnvironmentBlockNotCalled();

    MOCK_METHOD8( CreateProcessAsUserW, BOOL(
        HANDLE,
        LPCWSTR,
        LPWSTR,
        DWORD,
        LPVOID,
        LPCWSTR,
        LPSTARTUPINFOW,
        LPPROCESS_INFORMATION
    ) );
    void MakeCreateProcessAsUserWReturn( BOOL value );
    void ExpectCreateProcessAsUserWNotCalled();

    MOCK_METHOD1( Sleep, void( DWORD ) );
    void ExpectSleepNotCalled();
};
