#include "MockUserImpersonator.h"
#include "MocksCommon.h"

MockUserImpersonator::MockUserImpersonator()
{
}

MockUserImpersonator::~MockUserImpersonator()
{
}

void MockUserImpersonator::MakeGetActiveUserSessionsReturn( bool value )
{
    ON_CALL( *this, GetActiveUserSessions( _ ) ).WillByDefault( Return( value ) );
}

void MockUserImpersonator::ExpectGetActiveUserSessionsNotCalled()
{
    EXPECT_CALL( *this, GetActiveUserSessions( _ ) ).Times( 0 );
}

void MockUserImpersonator::MakeRunProcessInSessionReturn( bool value )
{
    ON_CALL( *this, RunProcessInSession( _, _, _, _ ) ).WillByDefault( Return( value ) );
}

void MockUserImpersonator::ExpectRunProcessInSessionNotCalled()
{
    EXPECT_CALL( *this, RunProcessInSession( _, _, _, _ ) ).Times( 0 );
}