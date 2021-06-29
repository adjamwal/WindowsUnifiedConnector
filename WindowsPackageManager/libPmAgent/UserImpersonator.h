#pragma once

#include "IUserImpersonator.h"
#include <Windows.h>
#include <wtsapi32.h>

class IWinApiWrapper;

class UserImpersonator : public IUserImpersonator
{
public:
    UserImpersonator( IWinApiWrapper& winApiWrapper );
    ~UserImpersonator();

    bool GetActiveUserSessions( std::vector<ULONG>& sessionList ) override;
    bool RunProcessInSession( const std::wstring& processName, 
        const std::wstring& args, 
        const std::wstring& workingDir, 
        ULONG sessionId ) override;

private:
    IWinApiWrapper& m_winApi;

    bool RunProcessAs( HANDLE userToken, 
        const std::wstring& processName, 
        const std::wstring& args, 
        const std::wstring& workingDir );

    bool EnumerateSessions(
        HANDLE hserver,
        DWORD reserved,
        DWORD version,
        PWTS_SESSION_INFO* ppSessionInfo,
        DWORD* pCount );
};