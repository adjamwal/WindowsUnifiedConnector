#include "pch.h"

#include "CloudEventBuilder.h"
#include "MockPmHttp.h"
#include "MockCloudEventStorage.h"
#include "CloudEventPublisher.h"
#include <memory>

class TestCloudEventPublisher : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_httpAdapter.reset( new NiceMock<MockPmHttp>() );
        m_eventStorage.reset( new NiceMock<MockCloudEventStorage>() );
        m_eventPublisher.reset( new CloudEventPublisher( *m_httpAdapter, *m_eventStorage, CLOUD_EVENT_PUBLISHING_URL ) );

        m_eventBuilder
            .WithUCID( "5B3861FF-2690-45D4-A49D-8F8CD18BBFC6" )
            .WithType( CloudEventType::pkgreconfig )
            .WithPackage( "amp", "1.0.0" )
            .WithOldFile( "oldfile", "oldhash123", 123 )
            .WithNewFile( "newfile", "newhash123", 234 )
            .WithError( 100, "some error" );
    }

    void TearDown()
    {
        m_eventBuilder.Reset();
        m_httpAdapter.reset();
        m_eventPublisher.reset();
        m_eventStorage.reset();
    }

    CloudEventBuilder m_eventBuilder;
    std::unique_ptr<MockPmHttp> m_httpAdapter;
    std::unique_ptr<MockCloudEventStorage> m_eventStorage;
    std::unique_ptr<ICloudEventPublisher> m_eventPublisher;
};

TEST_F( TestCloudEventPublisher, EventPublisherCallsHttpPost )
{
    m_httpAdapter->MakeHttpPostReturn( 5 );
    EXPECT_CALL( *m_httpAdapter, HttpPost( _, _, _, _, _ ) ).Times( 1 );
    EXPECT_EQ( 5, m_eventPublisher->Publish( m_eventBuilder ) );
}

TEST_F( TestCloudEventPublisher, EventPublisherStoresFailedEventHttpError )
{
    m_httpAdapter->MakeHttpPostReturn( -1 );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 1 );

    m_eventPublisher->Publish( m_eventBuilder );
}

TEST_F( TestCloudEventPublisher, EventPublisherStoresFailedEventHttpCode )
{
    m_httpAdapter->MakeHttpPostReturn( 0, 400 );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 1 );

    m_eventPublisher->Publish( m_eventBuilder );
}

TEST_F( TestCloudEventPublisher, EventPublisherSuccessDoesntStoreEvent )
{
    m_httpAdapter->MakeHttpPostReturn( 0 );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 0 );

    m_eventPublisher->Publish( m_eventBuilder );
}

TEST_F( TestCloudEventPublisher, EventPublisherSendFailedEventsWithEmptyList )
{
    std::vector<std::string> list = {};

    m_httpAdapter->MakeHttpPostReturn( 0 );
    m_eventStorage->MakeReadEventsReturn( list );

    EXPECT_CALL( *m_eventStorage, ReadEvents() ).Times( 1 );
    EXPECT_CALL( *m_httpAdapter, HttpPost( _, _, _, _, _ ) ).Times( 0 );

    m_eventPublisher->PublishFailedEvents();
}

TEST_F( TestCloudEventPublisher, EventPublisherSendFailedEvents )
{
    std::vector<std::string> list = { "one", "two" };

    m_httpAdapter->MakeHttpPostReturn( 0 );
    m_eventStorage->MakeReadEventsReturn( list );

    EXPECT_CALL( *m_httpAdapter, HttpPost( _, _, _, _, _ ) ).Times( 2 );

    m_eventPublisher->PublishFailedEvents();
}

TEST_F( TestCloudEventPublisher, EventPublisherSavesFailedEventsThatFailAgain )
{
    std::vector<std::string> list = { "one" };

    m_httpAdapter->MakeHttpPostReturn( -1 );
    m_eventStorage->MakeReadEventsReturn( list );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) ).Times( 1 );

    m_eventPublisher->PublishFailedEvents();
}