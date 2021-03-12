#include "pch.h"

#include "CloudEventBuilder.h"
#include "MockPmCloud.h"
#include "MockCloudEventStorage.h"
#include "MockPmConfig.h"
#include "CloudEventPublisher.h"
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
    m_httpAdapter->MakePostReturn( 5 );
    EXPECT_CALL( *m_httpAdapter, Post( _, _, _, _, _ ) ).Times( 1 );
    EXPECT_EQ( 5, m_eventPublisher->Publish( m_eventBuilder ) );
}

TEST_F( TestCloudEventPublisher, EventPublisherStoresFailedEventHttpError )
{
    m_httpAdapter->MakePostReturn( -1 );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 1 );

    m_eventPublisher->Publish( m_eventBuilder );
}

TEST_F( TestCloudEventPublisher, EventPublisherStoresFailedEventHttpCode )
{
    m_httpAdapter->MakePostReturn( 400 );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 1 );

    m_eventPublisher->Publish( m_eventBuilder );
}

TEST_F( TestCloudEventPublisher, EventPublisherSuccessDoesntStoreEvent )
{
    m_httpAdapter->MakePostReturn( 200 );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 0 );

    m_eventPublisher->Publish( m_eventBuilder );
}

TEST_F( TestCloudEventPublisher, EventPublisherSendFailedEventsWithEmptyList )
{
    std::vector<std::string> list = {};

    m_httpAdapter->MakePostReturn( 0 );
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

    m_httpAdapter->MakePostReturn( 0 );
    m_eventStorage->MakeReadAndRemoveEventsReturn( list );

    EXPECT_CALL( *m_httpAdapter, Post( _, _, _, _, _ ) ).Times( 2 );

    m_eventPublisher->PublishFailedEvents();
}

TEST_F( TestCloudEventPublisher, EventPublisherSavesFailedEventsThatFailAgain )
{
    std::vector<std::string> list;
    list.push_back( m_eventBuilder.Build() );

    m_httpAdapter->MakePostReturn( -1 );
    m_eventStorage->MakeReadAndRemoveEventsReturn( list );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 1 );

    m_eventPublisher->PublishFailedEvents();
}

TEST_F( TestCloudEventPublisher, EventPublisherDoesntPublishInvalidFailedEvents )
{
    std::vector<std::string> list;
    list.push_back( "{ event : \"\" }" );

    m_httpAdapter->MakePostReturn( -1 );
    m_eventStorage->MakeReadAndRemoveEventsReturn( list );

    EXPECT_CALL( *m_httpAdapter, Post( _, _, _, _, _ ) ).Times( 0 );

    m_eventPublisher->PublishFailedEvents();
}

TEST_F( TestCloudEventPublisher, EventPublisherDoesntSaveInvalidFailedEvents )
{
    std::vector<std::string> list;
    list.push_back( "{ event : \"\" }" );

    m_httpAdapter->MakePostReturn( -1 );
    m_eventStorage->MakeReadAndRemoveEventsReturn( list );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 0 );

    m_eventPublisher->PublishFailedEvents();
}