#include "pch.h"

#include "CloudEventBuilder.h"
#include "FileSysUtil.h"
#include "CloudEventStorage.h"
#include "MockPmPlatformDependencies.h"
#include "MockPmPlatformConfiguration.h"
#include <memory>
#include "MockUtf8PathVerifier.h"
#include "MockPmConfig.h"
#include "TimeUtil.h"
#include "PmConfig.h"

class TestCloudEventStorage : public ::testing::Test
{
protected:
    void SetUp()
    {
        std::string filename = "TestCloudEventStorage";

        m_mockConfig.reset( new NiceMock<MockPmConfig>() );
        m_mockConfig->MakeGetMaxEventTtlSReturn( PM_CONFIG_MAX_EVENT_TTL_SECS );

        m_utf8PathVerifier.reset( new NiceMock<MockUtf8PathVerifier>() );
        m_utf8PathVerifier->MakeIsPathValidReturn( true );

        m_fileUtil.reset( new FileSysUtil( *m_utf8PathVerifier ) );
        m_eventStorage.reset( new CloudEventStorage( filename, *m_fileUtil, *m_mockConfig ) );

        m_platformConfiguration.reset( new NiceMock<MockPmPlatformConfiguration>() );
        m_deps.reset( new NiceMock<MockPmPlatformDependencies>() );

        m_platformConfiguration->MakeGetInstallDirectoryReturn( "." );
        m_deps->MakeConfigurationReturn( *m_platformConfiguration );

        m_eventStorage->Initialize( m_deps.get() );

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
        m_deps.reset();
        m_platformConfiguration.reset();
        m_mockConfig.reset();
    }

    std::unique_ptr<MockUtf8PathVerifier> m_utf8PathVerifier;
    std::unique_ptr<IFileSysUtil> m_fileUtil;
    CloudEventBuilder m_eventBuilder1;
    CloudEventBuilder m_eventBuilder2;
    std::unique_ptr<ICloudEventStorage> m_eventStorage;
    std::unique_ptr<MockPmConfig> m_mockConfig;

    std::unique_ptr<MockPmPlatformConfiguration> m_platformConfiguration;
    std::unique_ptr<MockPmPlatformDependencies> m_deps;
};

TEST_F( TestCloudEventStorage, TestOneEventSavedToFile )
{
    std::vector<std::string> events;

    m_eventStorage->SaveEvent( m_eventBuilder1 );

    events = m_eventStorage->ReadAndRemoveEvents();

    EXPECT_EQ( events.size(), 1 );
}

TEST_F( TestCloudEventStorage, TestMultipleEventsSaveToFile )
{
    std::vector<std::string> events;

    std::string event1 = m_eventBuilder1.Build();
    std::string event2 = m_eventBuilder2.Build();

    m_eventStorage->SaveEvent( event1 );
    m_eventStorage->SaveEvent( event2 );
    m_eventStorage->SaveEvent( event1 );

    events = m_eventStorage->ReadAndRemoveEvents();

    EXPECT_EQ( events.size(), 3 );

    EXPECT_EQ( events.at( 0 ), event1 );
    EXPECT_EQ( events.at( 1 ), event2 );
    EXPECT_EQ( events.at( 2 ), event1 );
}

TEST_F( TestCloudEventStorage, ExpiredEventsWillNotBeSavedToFile )
{
    std::vector<std::string> events;

    //expired 8 seconds ago
    std::string rfcExpiredTime = TimeUtil::MillisToRFC3339
    ( 
        TimeUtil::Now_MilliTimeStamp() - ( 8 + PM_CONFIG_MAX_EVENT_TTL_SECS ) * 1000
    );
    m_eventBuilder1.WithTse( rfcExpiredTime );
    m_eventBuilder2.WithTse( rfcExpiredTime );

    m_eventStorage->SaveEvent( m_eventBuilder1 );
    m_eventStorage->SaveEvent( m_eventBuilder2 );
    m_eventStorage->SaveEvent( m_eventBuilder1 );

    events = m_eventStorage->ReadAndRemoveEvents();

    EXPECT_EQ( 0, events.size() );
}