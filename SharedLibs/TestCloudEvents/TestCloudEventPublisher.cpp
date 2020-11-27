#include "pch.h"

#include "CloudEventBuilder.h"
#include "MockPmHttp.h"
#include "CloudEventPublisher.h"
#include <memory>

class TestCloudEventPublisher : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_httpAdapter.reset( new NiceMock<MockPmHttp>() );
        m_eventPublisher.reset( new CloudEventPublisher( *m_httpAdapter, "https://4m2294wzqf.execute-api.us-west-1.amazonaws.com/dev/identify/event" ) );

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
    }

    CloudEventBuilder m_eventBuilder;
    std::unique_ptr<MockPmHttp> m_httpAdapter;
    std::unique_ptr<ICloudEventPublisher> m_eventPublisher;
};

TEST_F( TestCloudEventPublisher, EventPublisherCallsHttpPost )
{
    m_httpAdapter->MakeHttpPostReturn( 201 );
    EXPECT_CALL( *m_httpAdapter, HttpPost( _, _, _, _, _ ) ).Times( 1 );
    EXPECT_EQ( 201, m_eventPublisher->Publish( m_eventBuilder ) );
}
