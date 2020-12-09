#include "gmock/gmock.h"
#include "MockUcUpgradeEventHandler.h"

MockUcUpgradeEventHandler::MockUcUpgradeEventHandler()
{
}

MockUcUpgradeEventHandler::~MockUcUpgradeEventHandler()
{
}

void MockUcUpgradeEventHandler::ExpectInitializeIsNotCalled()
{
    EXPECT_CALL( *this, Initialize( _ ) ).Times( 0 );
}

void MockUcUpgradeEventHandler::MakeStoreUcUpgradeEventReturn( bool value )
{
    ON_CALL( *this, StoreUcUpgradeEvent( _ ) ).WillByDefault( Return( value ) );
}

void MockUcUpgradeEventHandler::ExpectStoreUcUpgradeEventIsNotCalled()
{
    EXPECT_CALL( *this, StoreUcUpgradeEvent( _ ) ).Times( 0 );
}

void MockUcUpgradeEventHandler::MakePublishUcUpgradeEventReturn( bool value )
{
    ON_CALL( *this, PublishUcUpgradeEvent() ).WillByDefault( Return( value ) );
}

void MockUcUpgradeEventHandler::ExpectPublishUcUpgradeEventIsNotCalled()
{
    EXPECT_CALL( *this, PublishUcUpgradeEvent() ).Times( 0 );
}