#include "pch.h"

#include "CloudEventBuilder.h"
#include "FileUtil.h"
#include "CloudEventStorage.h"
#include <memory>

class TestCloudEventStorage : public ::testing::Test
{
protected:
    void SetUp()
    {
        std::string filename = "TestCloudEventStorage";
        m_fileUtil.reset( new FileUtil() );
        m_eventStorage.reset( new CloudEventStorage( filename, *m_fileUtil ) );

        m_eventBuilder1
            .WithUCID( "5B3861FF-2690-45D4-A49D-8F8CD18BBFC6" )
            .WithType( CloudEventType::pkgreconfig )
            .WithPackage( "amp", "1.0.0" )
            .WithOldFile( "oldfile", "oldhash123", 123 )
            .WithNewFile( "newfile", "newhash123", 234 )
            .WithError( 100, "some error" );

        m_eventBuilder2
            .WithUCID( "6361E811-BEFE-477A-9D1C-231D4E9C2CF3" )
            .WithType( CloudEventType::pkgreconfig )
            .WithPackage( "amp", "1.0.0" )
            .WithOldFile( "oldfile", "oldhash123", 123 )
            .WithNewFile( "newfile", "newhash123", 234 )
            .WithError( 100, "some error" );
    }

    void TearDown()
    {
        m_eventBuilder1.Reset();
        m_eventBuilder2.Reset();
        m_eventStorage.reset();
        m_fileUtil.reset();
    }

    std::unique_ptr<IFileUtil> m_fileUtil;
    CloudEventBuilder m_eventBuilder1;
    CloudEventBuilder m_eventBuilder2;
    std::unique_ptr<ICloudEventStorage> m_eventStorage;
};

TEST_F( TestCloudEventStorage, TestOneEventSavedToFile )
{
    std::vector<std::string> events;

    m_eventStorage->SaveEvent( m_eventBuilder1 );

    events = m_eventStorage->ReadEvents();

    EXPECT_EQ( events.size(), 1 );
}

TEST_F( TestCloudEventStorage, TestMultipleEventsSaveToFile )
{
    std::vector<std::string> events;

    m_eventStorage->SaveEvent( m_eventBuilder1 );
    m_eventStorage->SaveEvent( m_eventBuilder2 );
    m_eventStorage->SaveEvent( m_eventBuilder1 );

    events = m_eventStorage->ReadEvents();

    EXPECT_EQ( events.size(), 3 );

    ASSERT_TRUE( events.at( 0 ).find( "\"ucid\":\"5B3861FF-2690-45D4-A49D-8F8CD18BBFC6\"" ) != std::string::npos );
    ASSERT_TRUE( events.at( 1 ).find( "\"ucid\":\"6361E811-BEFE-477A-9D1C-231D4E9C2CF3\"" ) != std::string::npos );
    ASSERT_TRUE( events.at( 2 ).find( "\"ucid\":\"5B3861FF-2690-45D4-A49D-8F8CD18BBFC6\"" ) != std::string::npos );
}