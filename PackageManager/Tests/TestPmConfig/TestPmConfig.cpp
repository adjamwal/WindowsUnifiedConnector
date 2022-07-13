#include "gtest/gtest.h"
#include "PmConfig.h"
#include "MockFileSysUtil.h"
#include "MockUcidAdapter.h"
#include <memory>

class TestPmConfig : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_urls = {
            "www.event.com",
            "www.checkin.com",
            "www.catalog.com",
        };

        m_fileUtil.reset( new NiceMock<MockFileSysUtil>() );
        m_ucidAdapter.reset( new NiceMock<MockUcidAdapter>() );
        m_aTimeStamp = std::filesystem::file_time_type::clock::now();
        m_patient.reset( new PmConfig( *m_fileUtil, *m_ucidAdapter ) );
    }

    void TearDown()
    {
        m_patient.reset();
        m_fileUtil.reset();
        m_ucidAdapter.reset();
    }

    const std::string bsConfigData = R"(
{
    "id": {
        "url": "https://packagemanager.cisco.com/identify"
    },
    "pm": {
        "url": "https://packagemanager.cisco.com/checkin",
        "event_url": "https://packagemanager.cisco.com/event/1",
        "catalog_url": "https://packagemanager.cisco.com/catalog"
    }
}
)";

    //test with a custom CheckinInterval value different than PM_CONFIG_INTERVAL_DEFAULT_MS
    const std::string pmConfigData = R"(
{
    "pm": {
        "loglevel": 7,
        "CheckinInterval": 150000,
        "MaxStartupDelay": 200000,
        "maxFileCacheAge_s": 1000,
        "AllowPostInstallReboots": true,
        "RebootThrottleS": 1000,
        "WatchdogBufferMs": 250000,
        "MaxEventTTL_s": 60,
        "NetworkFailureRetryInterval": 300000
    }
}
)";

    std::unique_ptr<MockFileSysUtil> m_fileUtil;
    std::unique_ptr<MockUcidAdapter> m_ucidAdapter;

    std::filesystem::file_time_type m_aTimeStamp;
    PmUrlList m_urls;
    std::unique_ptr<PmConfig> m_patient;
};

TEST_F( TestPmConfig, LoadWillReadPmFile )
{
    std::filesystem::path pmfilename( "pm file" );

    m_fileUtil->MakeReadFileReturn( pmConfigData );

    EXPECT_CALL( *m_fileUtil, ReadTextFile( pmfilename ) );

    m_patient->LoadPmConfig( pmfilename.generic_u8string() );
}

TEST_F( TestPmConfig, LoadWillSaveCustomCheckinInterval )
{
    m_fileUtil->MakeReadFileReturn( pmConfigData );

    m_patient->LoadPmConfig( "filename" );
    m_patient->GetCloudCheckinIntervalMs(); //discard 1st returned value since it is random

    EXPECT_EQ( m_patient->GetCloudCheckinIntervalMs(), 150000 );
}

TEST_F( TestPmConfig, FirstCheckinIntervalIsRandomizedWithinCustomRange )
{
    m_fileUtil->MakeReadFileReturn( pmConfigData );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_LE( m_patient->GetCloudCheckinIntervalMs(), 200000U );
}

TEST_F( TestPmConfig, LoadPmConfigWillSucceed )
{
    m_fileUtil->MakeReadFileReturn( pmConfigData );

    EXPECT_EQ( m_patient->LoadPmConfig( "filename" ), 0 );
}

TEST_F( TestPmConfig, PmConfigFileChangedIsTrueBeforeLoading )
{
    m_fileUtil->MakeFileExistsReturn( true );
    m_fileUtil->MakeFileTimeReturn( m_aTimeStamp );

    EXPECT_EQ( true, m_patient->PmConfigFileChanged( "filename" ) );
}

TEST_F( TestPmConfig, PmConfigFileChangedIsFalseAfterLoading )
{
    m_fileUtil->MakeFileExistsReturn( true );
    m_fileUtil->MakeFileTimeReturn( m_aTimeStamp );
    m_fileUtil->MakeReadFileReturn( pmConfigData );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_EQ( false, m_patient->PmConfigFileChanged( "filename" ) );
}

TEST_F( TestPmConfig, LoadWillTryBackupFile )
{
    std::filesystem::path filename( "filename" );
    std::filesystem::path backupFilename( "filename.bak" );

    InSequence s;

    EXPECT_CALL( *m_fileUtil, ReadTextFile( filename ) ).WillOnce( Return( "" ) );
    EXPECT_CALL( *m_fileUtil, ReadTextFile( backupFilename ) ).WillOnce( Return( "" ) );

    m_patient->LoadPmConfig( filename.generic_u8string() );
}

TEST_F( TestPmConfig, VerifyPmFileIntegrityWillSucceed )
{
    m_fileUtil->MakeReadFileReturn( pmConfigData );

    EXPECT_EQ( m_patient->VerifyPmFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, VerifyPmFileIntegrityWillFailOnEmptyContents )
{
    m_fileUtil->MakeReadFileReturn( "" );

    EXPECT_NE( m_patient->VerifyPmFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, VerifyPmFileIntegrityWillFailWhenMaxFileCacheAgeNotProvided )
{
    m_fileUtil->MakeReadFileReturn( R"(
{
    "pm": {
        "loglevel": 7,
        "CheckinInterval": 150000,
        "MaxStartupDelay": 200000,
        "AllowPostInstallReboots": true
    }
}
)" );

    EXPECT_NE( m_patient->VerifyPmFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, VerifyPmFileIntegrityWillFailIfMaxFileCacheAgeIsInvalid )
{
    m_fileUtil->MakeReadFileReturn( R"(
{
    "pm": {
        "loglevel": 7,
        "CheckinInterval": 150000,
        "MaxStartupDelay": 200000,
        "maxFileCacheAge_s": "Invalid",
        "AllowPostInstallReboots": true
    }
}
)" );

    EXPECT_NE( m_patient->VerifyPmFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, LoadingEmptyConfigSetsDefaultInterval )
{
    m_fileUtil->MakeReadFileReturn( "" );

    m_patient->LoadPmConfig( "filename" );
    m_patient->GetCloudCheckinIntervalMs(); //discard 1st returned value since it is random

    EXPECT_EQ( m_patient->GetCloudCheckinIntervalMs(), PM_CONFIG_INTERVAL_DEFAULT_MS );
}

//Not a great test... but not worthwihle to mock out RandomUtil
TEST_F(TestPmConfig, LoadingEmptyConfigSetsDefaultMaxStartupDelay)
{
    m_fileUtil->MakeReadFileReturn( "" );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_LE( m_patient->GetCloudCheckinIntervalMs(), ( uint32_t )PM_CONFIG_INTERVAL_DEFAULT_MS );
}
TEST_F( TestPmConfig, LoadingEmptyConfigSetsDefaultLogLevel )
{
    m_fileUtil->MakeReadFileReturn( "" );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_EQ( m_patient->GetLogLevel(), PM_CONFIG_LOGLEVEL_DEFAULT );
}

TEST_F( TestPmConfig, LoadingEmptyConfigSetsMaxFileCacheAge )
{
    m_fileUtil->MakeReadFileReturn( "" );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_EQ( m_patient->GetMaxFileCacheAgeS(), PM_CONFIG_MAX_CACHE_AGE_DEFAULT_SECS );
}

TEST_F( TestPmConfig, LoadingEmptyConfigSetsMaxEventTTL )
{
    m_fileUtil->MakeReadFileReturn( "" );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_EQ( PM_CONFIG_MAX_EVENT_TTL_SECS, m_patient->GetMaxEventTtlS() );
}

TEST_F( TestPmConfig, LoadingEmptyConfigSetsAllowPostInstallReboots )
{
    m_fileUtil->MakeReadFileReturn( "" );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_EQ( m_patient->AllowPostInstallReboots(), false );
}

TEST_F( TestPmConfig, LoadingEmptyConfigSetsRebootThrottle )
{
    m_fileUtil->MakeReadFileReturn( "" );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_EQ( m_patient->GetRebootThrottleS(), PM_CONFIG_REBOOT_THROTTLE_DEFAULT_SECS );
}

TEST_F( TestPmConfig, LoadingEmptyConfigSetsWatchDogInterval )
{
    m_fileUtil->MakeReadFileReturn( "" );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_EQ( m_patient->GetWatchdogTimeoutMs(), PM_CONFIG_INTERVAL_DEFAULT_MS + PM_CONFIG_WATCHDOG_BUFFER_DEFAULT_MS );
}

TEST_F( TestPmConfig, CanOverrideWatchdogBuffer )
{
    m_fileUtil->MakeReadFileReturn( pmConfigData );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_EQ( m_patient->GetWatchdogTimeoutMs(), 150000 + 250000 );
}

TEST_F( TestPmConfig, WillAddDefaultWatchdogBufferToTimeout )
{
    m_fileUtil->MakeReadFileReturn( R"(
{
    "pm": {
        "loglevel": 7,
        "CheckinInterval": 150000,
        "MaxStartupDelay": 200000,
        "maxFileCacheAge_s": 1000,
        "AllowPostInstallReboots": true,
        "RebootThrottleS": 1000
    }
}
)" );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_EQ( m_patient->GetWatchdogTimeoutMs(), 150000 + PM_CONFIG_WATCHDOG_BUFFER_DEFAULT_MS );
}

TEST_F( TestPmConfig, CanGetCatalogUrl )
{
    ON_CALL( *m_ucidAdapter, GetUrls ).WillByDefault( DoAll( SetArgReferee<0>( m_urls ), Return( true ) ) );

    EXPECT_EQ( m_patient->GetCloudCatalogUri(), m_urls.catalogUrl );
}

TEST_F( TestPmConfig, CanGetCheckinUrl )
{
    ON_CALL( *m_ucidAdapter, GetUrls ).WillByDefault( DoAll( SetArgReferee<0>( m_urls ), Return( true ) ) );

    EXPECT_EQ( m_patient->GetCloudCheckinUri(), m_urls.checkinUrl );
}

TEST_F( TestPmConfig, CanGetEventUrl )
{
    ON_CALL( *m_ucidAdapter, GetUrls ).WillByDefault( DoAll( SetArgReferee<0>( m_urls ), Return( true ) ) );

    EXPECT_THAT( m_patient->GetCloudEventUri(), HasSubstr( m_urls.eventUrl ) );
}

TEST_F( TestPmConfig, GetEventUrlWillAppendApiVersion )
{
    ON_CALL( *m_ucidAdapter, GetUrls ).WillByDefault( DoAll( SetArgReferee<0>( m_urls ), Return( true ) ) );

    EXPECT_THAT( m_patient->GetCloudEventUri(), HasSubstr( "/1") );
}