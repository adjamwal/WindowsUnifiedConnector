#pragma once

#include "MocksCommon.h"
#include "ICloudEventPublisher.h"

class MockCloudEventPublisher : public ICloudEventPublisher
{
public:
    MockCloudEventPublisher();
    ~MockCloudEventPublisher();

    MOCK_METHOD1( SetToken, void( const std::string& ) );
    void ExpectSetTokenNotCalled();

    MOCK_METHOD1( Publish, int32_t( ICloudEventBuilder& ) );
    void MakePublishReturn( int32_t value );
    void ExpectPublishNotCalled();

    MOCK_METHOD0( PublishFailedEvents, int32_t() );
    void MakePublishFailedEventsReturn( int32_t value );
    void ExpectPublishFailedEventsNotCalled();
};