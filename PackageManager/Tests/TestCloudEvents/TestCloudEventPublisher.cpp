#include "pch.h"

#include "CloudEventBuilder.h"
#include "MockPmCloud.h"
#include "MockCloudEventStorage.h"
#include "MockPmConfig.h"
#include "CloudEventPublisher.h"
#include "TimeUtil.h"
#include "PmConfig.h"
#include <memory>

class TestCloudEventPublisher : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_httpAdapter.reset( new NiceMock<MockPmCloud>() );
        m_eventStorage.reset( new NiceMock<MockCloudEventStorage>() );
        m_pmConfig.reset( new NiceMock<MockPmConfig>() );
        m_eventPublisher.reset( new CloudEventPublisher( *m_httpAdapter, *m_eventStorage, *m_pmConfig ) );

        m_pmConfig->MakeGetMaxEventTtlSReturn( PM_CONFIG_MAX_EVENT_TTL_SECS );

        m_eventBuilder
            .WithUCID( "5B3861FF-2690-45D4-A49D-8F8CD18BBFC6" )
            .WithType( CloudEventType::pkgreconfig )
            .WithPackage( "amp", "1.0.0" )
            .WithOldFile( "oldfile", "oldhash123", 123 )
            .WithNewFile( "newfile", "newhash123", 234 )
            .WithError( 100, "some error" );

        m_eventBuilderTwo
            .WithUCID( "6B3861FF-2690-45D4-A49D-8F8CD18BBFC6" )
            .WithType( CloudEventType::pkgreconfig )
            .WithPackage( "vpn", "2.0.0" )
            .WithOldFile( "oldfile", "oldhash1234", 1234 )
            .WithNewFile( "newfile", "newhash1234", 2345 )
            .WithError( 100, "some error" );
    }

    void TearDown()
    {
        m_eventBuilder.Reset();
        m_eventBuilderTwo.Reset();
        m_httpAdapter.reset();
        m_eventPublisher.reset();
        m_eventStorage.reset();
        m_pmConfig.reset();
    }

    CloudEventBuilder m_eventBuilder;
    CloudEventBuilder m_eventBuilderTwo;
    std::unique_ptr<MockPmCloud> m_httpAdapter;
    std::unique_ptr<MockCloudEventStorage> m_eventStorage;
    std::unique_ptr<MockPmConfig> m_pmConfig;
    std::unique_ptr<ICloudEventPublisher> m_eventPublisher;
};

TEST_F( TestCloudEventPublisher, EventPublisherRetrievesTheEventUriFromConfig )
{
    EXPECT_CALL( *m_pmConfig, GetCloudEventUri() );
    m_eventPublisher->Publish( m_eventBuilder );
}

TEST_F( TestCloudEventPublisher, EventPublisherCallsHttpPost )
{
    m_httpAdapter->MakePostReturn( true, "", { 5, 0 } );
    EXPECT_CALL( *m_httpAdapter, Post( _, _, _, _, _ ) ).Times( 1 );
    EXPECT_EQ( 5, m_eventPublisher->Publish( m_eventBuilder ) );
}

TEST_F( TestCloudEventPublisher, EventPublisherStoresFailedEventHttpError )
{
    m_httpAdapter->MakePostReturn( false, "", { -1, 0 } );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 1 );

    m_eventPublisher->Publish( m_eventBuilder );
}

TEST_F( TestCloudEventPublisher, EventPublisherWillNotStoreMalformedEvents )
{
    m_httpAdapter->MakePostReturn( false, "", { 400, 0 } );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 0 );

    m_eventPublisher->Publish( m_eventBuilder );
}

TEST_F( TestCloudEventPublisher, EventPublisherStoresFailedEventHttpCode )
{
    m_httpAdapter->MakePostReturn( false, "", { 401, 0 } );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 1 );

    m_eventPublisher->Publish( m_eventBuilder );
}

TEST_F( TestCloudEventPublisher, EventPublisherSuccessDoesntStoreEvent )
{
    m_httpAdapter->MakePostReturn( true, "", { 200, 0 } );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 0 );

    m_eventPublisher->Publish( m_eventBuilder );
}

TEST_F( TestCloudEventPublisher, EventPublisherSendFailedEventsWithEmptyList )
{
    std::vector<std::string> list = {};

    m_httpAdapter->MakePostReturn( false, "", { 0, 0 } );
    m_eventStorage->MakeReadAndRemoveEventsReturn( list );

    EXPECT_CALL( *m_eventStorage, ReadAndRemoveEvents() ).Times( 1 );
    EXPECT_CALL( *m_httpAdapter, Post( _, _, _, _, _ ) ).Times( 0 );

    m_eventPublisher->PublishFailedEvents();
}

TEST_F( TestCloudEventPublisher, EventPublisherSendFailedEvents )
{
    std::vector<std::string> list;
    list.push_back( m_eventBuilder.Build() );
    list.push_back( m_eventBuilderTwo.Build() );

    m_httpAdapter->MakePostReturn( false, "", { 0, 0 } );
    m_eventStorage->MakeReadAndRemoveEventsReturn( list );

    EXPECT_CALL( *m_httpAdapter, Post( _, _, _, _, _ ) ).Times( 2 );

    m_eventPublisher->PublishFailedEvents();
}

TEST_F( TestCloudEventPublisher, EventPublisherWillNotSendFailedEventsAfterTseExpiry )
{
    std::vector<std::string> list;

    //expired 8 seconds ago
    std::string rfcExpiredTime = TimeUtil::MillisToRFC3339
    ( 
        TimeUtil::Now_MilliTimeStamp() - ( 8 + PM_CONFIG_MAX_EVENT_TTL_SECS ) * 1000 
    );

    m_eventBuilder.WithTse( rfcExpiredTime );
    m_eventBuilderTwo.WithTse( rfcExpiredTime );

    list.push_back( m_eventBuilder.Build() );
    list.push_back( m_eventBuilderTwo.Build() );

    m_httpAdapter->MakePostReturn( false, "", { 0, 0 } );
    m_eventStorage->MakeReadAndRemoveEventsReturn( list );

    m_httpAdapter->ExpectPostIsNotCalled();

    m_eventPublisher->PublishFailedEvents();
}

TEST_F( TestCloudEventPublisher, EventPublisherSavesFailedEventsThatFailAgain )
{
    std::vector<std::string> list;
    list.push_back( m_eventBuilder.Build() );

    m_httpAdapter->MakePostReturn( false, "", { -1, 0 } );
    m_eventStorage->MakeReadAndRemoveEventsReturn( list );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 1 );

    m_eventPublisher->PublishFailedEvents();
}

TEST_F( TestCloudEventPublisher, EventPublisherDoesntPublishInvalidFailedEvents )
{
    std::vector<std::string> list;
    list.push_back( "{ event : \"\" }" );

    m_httpAdapter->MakePostReturn( false, "", { -1, 0 } );
    m_eventStorage->MakeReadAndRemoveEventsReturn( list );

    EXPECT_CALL( *m_httpAdapter, Post( _, _, _, _, _ ) ).Times( 0 );

    m_eventPublisher->PublishFailedEvents();
}

TEST_F( TestCloudEventPublisher, EventPublisherDoesntSaveInvalidFailedEvents )
{
    std::vector<std::string> list;
    list.push_back( "{ event : \"\" }" );

    m_httpAdapter->MakePostReturn( false, "", { -1, 0 } );
    m_eventStorage->MakeReadAndRemoveEventsReturn( list );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 0 );

    m_eventPublisher->PublishFailedEvents();
}
