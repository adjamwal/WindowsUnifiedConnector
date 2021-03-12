#include "gtest/gtest.h"
#include "UcUpgradeEventHandler.h"
#include "MockCloudEventBuilder.h"
#include "MockCloudEventPublisher.h"
#include "MockCloudEventStorage.h"
#include "MockPmPlatformDependencies.h"
#include "MockPmPlatformConfiguration.h"
#include "PmConstants.h"

#include <memory>

class TestUcUpgradeEventHandler : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_eventBuilder.reset( new NiceMock<MockCloudEventBuilder>() );
        m_eventPublisher.reset( new NiceMock<MockCloudEventPublisher>() );
        m_eventStorage.reset( new NiceMock<MockCloudEventStorage>() );
        m_dep.reset( new NiceMock<MockPmPlatformDependencies>() );
        m_pmConfiguration.reset( new NiceMock<MockPmPlatformConfiguration>() );

        m_patient.reset( new UcUpgradeEventHandler( *m_eventPublisher, *m_eventStorage, *m_eventBuilder ) );

        m_dep->MakeConfigurationReturn( *m_pmConfiguration );
    }

    void TearDown()
    {
        m_patient.reset();

        m_eventBuilder.reset();
        m_eventPublisher.reset();
        m_eventStorage.reset();
        m_dep.reset();
        m_pmConfiguration.reset();
    }

    std::unique_ptr<MockCloudEventBuilder> m_eventBuilder;
    std::unique_ptr<MockCloudEventPublisher> m_eventPublisher;
    std::unique_ptr<MockCloudEventStorage> m_eventStorage;
    std::unique_ptr<MockPmPlatformConfiguration> m_pmConfiguration;
    std::unique_ptr<MockPmPlatformDependencies> m_dep;

    std::unique_ptr<UcUpgradeEventHandler> m_patient;
};

TEST_F( TestUcUpgradeEventHandler, WillNotStoreUcUpgradeEventNotInitialized )
{
    EXPECT_CALL( *m_eventBuilder, GetPackageName() ).Times( 0 );
    
    m_patient->StoreUcUpgradeEvent( "" );
}

TEST_F( TestUcUpgradeEventHandler, StoreUcUpgradeEventWillClearOldEvents )
{
    m_patient->Initialize( m_dep.get() );
    ON_CALL( *m_eventBuilder, GetPackageName() ).WillByDefault( Return( UC_PACKAGE_NAME ) );

    EXPECT_CALL( *m_eventStorage, ReadAndRemoveEvents() );
    
    m_patient->StoreUcUpgradeEvent( "" );
}

TEST_F( TestUcUpgradeEventHandler, StoreUcUpgradeEventWillSaveOnlyUcEvents )
{
    m_patient->Initialize( m_dep.get() );
    ON_CALL( *m_eventBuilder, GetPackageName() ).WillByDefault( Return( UC_PACKAGE_NAME ) );

    EXPECT_CALL( *m_eventStorage, SaveEvent( Matcher<const std::string&>( _ ) ) );

    m_patient->StoreUcUpgradeEvent( "" );
}

TEST_F( TestUcUpgradeEventHandler, StoreUcUpgradeEventWillIgnoreOtherEvents )
{
    m_patient->Initialize( m_dep.get() );
    ON_CALL( *m_eventBuilder, GetPackageName() ).WillByDefault( Return( "OtherPackage" ) );

    m_eventStorage->ExpectSaveEventNotCalled();

    m_patient->StoreUcUpgradeEvent( "" );
}

TEST_F( TestUcUpgradeEventHandler, WillNotPublishEventNotInitialized )
{
    std::vector<std::string> events = { "Event1" };
    m_eventStorage->MakeReadAndRemoveEventsReturn( events );

    m_eventPublisher->ExpectPublishNotCalled();

    m_patient->PublishUcUpgradeEvent();
}

TEST_F( TestUcUpgradeEventHandler, PublishUcUpgradeEventWillFailWhenNoEventsFounds )
{
    m_patient->Initialize( m_dep.get() );
    std::vector<std::string> events;
    m_eventStorage->MakeReadAndRemoveEventsReturn( events );

    EXPECT_FALSE( m_patient->PublishUcUpgradeEvent() );
}

TEST_F( TestUcUpgradeEventHandler, PublishUcUpgradeEventWillFlagErrorWhenNotUpgraded )
{
    m_patient->Initialize( m_dep.get() );
    std::vector<std::string> events = { "Event1" };

    m_eventStorage->MakeReadAndRemoveEventsReturn( events );
    ON_CALL( *m_eventBuilder, GetPackageVersion() ).WillByDefault( Return( "new" ) );
    m_pmConfiguration->MakeGetPmVersionReturn( "old" );

    EXPECT_CALL( *m_eventBuilder, WithError( UCPM_EVENT_ERROR_COMPONENT_UC_UPDATE, _ ) );

    m_patient->PublishUcUpgradeEvent();
}

TEST_F( TestUcUpgradeEventHandler, PublishUcUpgradeEventWillSendUpgradeSuccess )
{
    m_patient->Initialize( m_dep.get() );
    std::vector<std::string> events = { "Event1" };
    std::string version = "version";
    m_eventStorage->MakeReadAndRemoveEventsReturn( events );
    ON_CALL( *m_eventBuilder, GetPackageVersion() ).WillByDefault( Return( version ) );
    m_pmConfiguration->MakeGetPmVersionReturn( version );

    EXPECT_CALL( *m_eventBuilder, WithError( _, _ ) ).Times( 0 );

    m_patient->PublishUcUpgradeEvent();
}
TEST_F( TestUcUpgradeEventHandler, PublishUcUpgradeEventWillPublish )
{
    m_patient->Initialize( m_dep.get() );
    std::vector<std::string> events = { "Event1" };

    m_eventStorage->MakeReadAndRemoveEventsReturn( events );

    EXPECT_CALL( *m_eventPublisher, Publish( _ ) );

    m_patient->PublishUcUpgradeEvent();
}

TEST_F( TestUcUpgradeEventHandler, PublishUcUpgradeEventWillSucceed )
{
    m_patient->Initialize( m_dep.get() );
    std::vector<std::string> events = { "Event1" };

    m_eventStorage->MakeReadAndRemoveEventsReturn( events );
    m_eventPublisher->MakePublishReturn( 200 );

    EXPECT_TRUE( m_patient->PublishUcUpgradeEvent() );
}
