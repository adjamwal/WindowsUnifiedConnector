#include "MockRebootHandler.h"

MockRebootHandler::MockRebootHandler()
{

}

MockRebootHandler::~MockRebootHandler()
{

}

void MockRebootHandler::ExpectInitializeNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockRebootHandler::MakeHandleRebootReturn( bool value )
{
    ON_CALL( *this, HandleReboot( _ ) ).WillByDefault( Return( value ) );
}

void MockRebootHandler::ExpectHandleRebootNotCalled()
{
    EXPECT_CALL( *this, HandleReboot( _ ) ).Times( 0 );
}