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
    ON_CALL( *this, Publish( Matcher<ICloudEventBuilder&>( _ ) ) ).WillByDefault( Return( value ) );
    //ON_CALL( *this, Publish( Matcher<const std::string&>( _ ) ) ).WillByDefault( Return( value ) );
}

void MockCloudEventPublisher::ExpectPublishNotCalled()
{
    EXPECT_CALL( *this, Publish( Matcher<ICloudEventBuilder&>( _ ) ) ).Times( 0 );
    //EXPECT_CALL( *this, Publish( Matcher<const std::string&>( _ ) ) ).Times( 0 );
}

void MockCloudEventPublisher::MakePublishFailedEventsReturn( int32_t value )
{
    ON_CALL( *this, PublishFailedEvents() ).WillByDefault( Return( value ) );
}

void MockCloudEventPublisher::ExpectPublishFailedEventsNotCalled()
{
    EXPECT_CALL( *this, PublishFailedEvents() ).Times( 0 );
}