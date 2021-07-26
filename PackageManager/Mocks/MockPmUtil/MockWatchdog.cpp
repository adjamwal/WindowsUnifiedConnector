#include "MockWatchdog.h"

MockWatchdog::MockWatchdog()
{

}

MockWatchdog::~MockWatchdog()
{

}

void MockWatchdog::ExpectStartIsNotCalled()
{
    EXPECT_CALL( *this, Start( _, _ ) ).Times( 0 );
}

void MockWatchdog::ExpectStopIsNotCalled()
{
    EXPECT_CALL( *this, Stop() ).Times( 0 );
}

void MockWatchdog::ExpectKickIsNotCalled()
{
    EXPECT_CALL( *this, Kick() ).Times( 0 );
}
