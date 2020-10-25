#include "pch.h"
#include "WinApiWrapper.h"

WinApiWrapper::WinApiWrapper()
{

}

WinApiWrapper::~WinApiWrapper()
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
