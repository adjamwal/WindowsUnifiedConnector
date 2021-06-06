#pragma once
#pragma once

#include <vector>
#include <string>
#include <Windows.h>

class IUserImpersonator
{
public:
    virtual ~IUserImpersonator() {};

    virtual bool GetActiveUserSessions( std::vector<ULONG>& sessionList ) = 0;
    virtual bool RunProcessInSession( const std::wstring& processName, 
        const std::wstring& args, 
        const std::wstring& workingDir,
        ULONG sessionId ) = 0;
};