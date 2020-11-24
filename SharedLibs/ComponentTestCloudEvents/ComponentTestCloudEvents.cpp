#include "pch.h"

#include "CloudEventBuilder.h"
#include "MockHttpAdapter.h"
#include "CloudEventPublisher.h"
#include "IUcLogger.h"
#include "EnumStringConversion.h"
#include "json\json.h"
#include <memory>

class ComponentTestCloudEvents : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_httpAdapter.reset( new NiceMock<MockHttpAdapter>() );
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
    std::unique_ptr<MockHttpAdapter> m_httpAdapter;
    std::unique_ptr<ICloudEventPublisher> m_eventPublisher;
};

TEST_F( ComponentTestCloudEvents, EventJSONContainsUCID )
{
    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"ucid\":\"5B3861FF-2690-45D4-A49D-8F8CD18BBFC6\"" ) != std::string::npos );
}

TEST_F( ComponentTestCloudEvents, EventJSONContainsEventType )
{
    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"type\":\"pkg-reconfig\"" ) != std::string::npos );
}

TEST_F( ComponentTestCloudEvents, EventJSONContainsPackage )
{
    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"package\":\"amp/1.0.0\"" ) != std::string::npos );
}

TEST_F( ComponentTestCloudEvents, EventJSONContainsOldFile )
{
    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"old\":[{\"path\":\"oldfile\",\"sha256\":\"oldhash123\",\"size\":123}]" ) != std::string::npos );
}

TEST_F( ComponentTestCloudEvents, EventJSONContainsNewFile )
{
    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"new\":[{\"path\":\"newfile\",\"sha256\":\"newhash123\",\"size\":234}]" ) != std::string::npos );
}

TEST_F( ComponentTestCloudEvents, EventJSONContainsError )
{
    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"err\":{\"code\":100,\"msg\":\"some error\"}" ) != std::string::npos );
}

TEST_F( ComponentTestCloudEvents, EventPublisherCallsHttpPost )
{
    EXPECT_CALL( *m_httpAdapter, HttpPost( _, _, _, _, _ ) ).Times( 1 );

    m_eventPublisher->Publish( m_eventBuilder );
}
