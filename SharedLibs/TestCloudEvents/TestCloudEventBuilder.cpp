#include "pch.h"

#include "CloudEventBuilder.h"
#include <memory>

class TestCloudEventBuilder : public ::testing::Test
{
protected:
    void SetUp()
    {
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
    }

    CloudEventBuilder m_eventBuilder;
};

TEST_F( TestCloudEventBuilder, EventJSONContainsUCID )
{
    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"ucid\":\"5B3861FF-2690-45D4-A49D-8F8CD18BBFC6\"" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, EventJSONContainsEventType )
{
    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"type\":\"pkg-reconfig\"" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, EventJSONContainsPackage )
{
    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"package\":\"amp/1.0.0\"" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, EventJSONContainsOldFile )
{
    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"old\":[{\"path\":\"oldfile\",\"sha256\":\"oldhash123\",\"size\":123}]" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, EventJSONContainsNewFile )
{
    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"new\":[{\"path\":\"newfile\",\"sha256\":\"newhash123\",\"size\":234}]" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, EventJSONContainsError )
{
    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"err\":{\"code\":100,\"msg\":\"some error\"}" ) != std::string::npos );
}
