#include "MocksCommon.h"
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
            .WithError( 100, "some error" )
            .WithSubError( 400, "http" );
    }

    void TearDown()
    {
        m_eventBuilder.Reset();
        m_restoredEvent.Reset();
    }

    void BuildAndDeserialize()
    {
        std::string eventJson = m_eventBuilder.Build();
        CloudEventBuilder::Deserialize( m_restoredEvent, eventJson );
    }

    CloudEventBuilder m_eventBuilder;
    CloudEventBuilder m_restoredEvent;
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

    ASSERT_TRUE( eventJson.find( "\"err\":{\"code\":100,\"msg\":\"some error\",\"sub_code\":400,\"sub_type\":\"http\"}" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, DeserializeCompletesSuccessfully )
{
    std::string eventJson = m_eventBuilder.Build();
    CloudEventBuilder restoredEvent;

    ASSERT_TRUE( CloudEventBuilder::Deserialize( restoredEvent, eventJson ) );
}

TEST_F( TestCloudEventBuilder, DeserializeRestoresOriginalUCID )
{
    BuildAndDeserialize();

    ASSERT_TRUE( m_restoredEvent.Build().find( "\"ucid\":\"5B3861FF-2690-45D4-A49D-8F8CD18BBFC6\"}" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, DeserializeRestoresOriginalEventType )
{
    BuildAndDeserialize();

    ASSERT_TRUE( m_restoredEvent.Build().find( "\"type\":\"pkg-reconfig\"" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, DeserializeRestoresOriginalPackage )
{
    BuildAndDeserialize();

    ASSERT_TRUE( m_restoredEvent.Build().find( "\"package\":\"amp/1.0.0\"" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, DeserializeRestoresOriginalOldFile )
{
    BuildAndDeserialize();

    ASSERT_TRUE( m_restoredEvent.Build().find( "\"old\":[{\"path\":\"oldfile\",\"sha256\":\"oldhash123\",\"size\":123}]" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, DeserializeRestoresOriginalNewFile )
{
    BuildAndDeserialize();

    ASSERT_TRUE( m_restoredEvent.Build().find( "\"new\":[{\"path\":\"newfile\",\"sha256\":\"newhash123\",\"size\":234}]" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, DeserializeRestoresOriginalError )
{
    BuildAndDeserialize();

    ASSERT_TRUE( m_restoredEvent.Build().find( "\"err\":{\"code\":100,\"msg\":\"some error\",\"sub_code\":400,\"sub_type\":\"http\"}" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, EventJSONContainsOldFileBackslash )
{
    m_eventBuilder.WithOldFile( "c:\\path\\test.file", "oldhash", 123 );

    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"old\":[{\"path\":\"c:/path/test.file\",\"sha256\":\"oldhash\",\"size\":123}]" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, EventJSONContainsOldFileForwardslash )
{
    m_eventBuilder.WithOldFile( "c:/path/test.file", "oldhash", 123 );

    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"old\":[{\"path\":\"c:/path/test.file\",\"sha256\":\"oldhash\",\"size\":123}]" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, EventJSONContainsNewFileBackslash )
{
    m_eventBuilder.WithNewFile( "c:\\path\\test.file", "newhash", 123 );

    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"new\":[{\"path\":\"c:/path/test.file\",\"sha256\":\"newhash\",\"size\":123}]" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, EventJSONContainsNewFileForwardslash )
{
    m_eventBuilder.WithNewFile( "c:/path/test.file", "newhash", 123 );

    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"new\":[{\"path\":\"c:/path/test.file\",\"sha256\":\"newhash\",\"size\":123}]" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, EventJSONShaIsNotSentWhenNewFileShaIsEmpty )
{
    m_eventBuilder.WithNewFile( "c:/path/test.file", "", 123 );

    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"new\":[{\"path\":\"c:/path/test.file\",\"size\":123}]" ) != std::string::npos );
}

TEST_F( TestCloudEventBuilder, EventJSONShaIsNotSentWhenOldFileShaIsEmpty )
{
    m_eventBuilder.WithOldFile( "c:/path/test.file", "", 123 );

    std::string eventJson = m_eventBuilder.Build();

    ASSERT_TRUE( eventJson.find( "\"old\":[{\"path\":\"c:/path/test.file\",\"size\":123}]" ) != std::string::npos );
}