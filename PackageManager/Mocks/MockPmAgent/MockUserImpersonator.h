#pragma once

#include <gmock/gmock.h>
#include "IUserImpersonator.h"

class MockUserImpersonator : public IUserImpersonator
{
public:
    MockUserImpersonator();
    ~MockUserImpersonator();

    MOCK_METHOD1( GetActiveUserSessions, bool( std::vector<ULONG>& ) );
    void MakeGetActiveUserSessionsReturn( bool value );
    void ExpectGetActiveUserSessionsNotCalled();

    MOCK_METHOD4( RunProcessInSession, bool( const std::wstring&, const std::wstring&, const std::wstring&, ULONG ) );
    void MakeRunProcessInSessionReturn( bool value );
    void ExpectRunProcessInSessionNotCalled();
};