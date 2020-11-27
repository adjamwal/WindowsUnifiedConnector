#include "pch.h"
#include "MockCloudEventPublisher.h"

MockCloudEventPublisher::MockCloudEventPublisher()
{

}

MockCloudEventPublisher::~MockCloudEventPublisher()
{

}

void MockCloudEventPublisher::MakePublishReturn( int32_t value )
{
    ON_CALL( *this, Publish( _ ) ).WillByDefault( Return( value ) );
}

void MockCloudEventPublisher::ExpectPublishNotCalled()
{
    EXPECT_CALL( *this, Publish( _ ) ).Times( 0 );
}

void MockCloudEventPublisher::MakePublishFailedEventsReturn( int32_t value )
{
    ON_CALL( *this, PublishFailedEvents() ).WillByDefault( Return( value ) );
}

void MockCloudEventPublisher::ExpectPublishFailedEventsNotCalled()
{
    EXPECT_CALL( *this, PublishFailedEvents() ).Times( 0 );
}