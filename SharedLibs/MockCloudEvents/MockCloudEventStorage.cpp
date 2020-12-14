#include "pch.h"
#include "MockCloudEventStorage.h"

MockCloudEventStorage::MockCloudEventStorage()
{

}

MockCloudEventStorage::~MockCloudEventStorage()
{

}

void MockCloudEventStorage::MakeSaveEventReturn( bool value )
{
    ON_CALL( *this, SaveEvent( Matcher<const std::string&>( _ ) ) ).WillByDefault( Return( value ) );
}

void MockCloudEventStorage::ExpectSaveEventNotCalled()
{
    EXPECT_CALL( *this, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 0 );
}

void MockCloudEventStorage::MakeReadEventsReturn( std::vector<std::string> value )
{
    ON_CALL( *this, ReadEvents() ).WillByDefault( Return( value ) );
}

void MockCloudEventStorage::ExpectReadEventsNotCalled()
{
    EXPECT_CALL( *this, ReadEvents() ).Times( 0 );
}
