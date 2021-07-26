#include "gtest/gtest.h"
#include "PmConfig.h"
#include "MockFileSysUtil.h"

#include <memory>

class TestPmConfig : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_fileUtil.reset( new NiceMock<MockFileSysUtil>() );
        m_aTimeStamp = std::filesystem::file_time_type::clock::now();
        m_patient.reset( new PmConfig( *m_fileUtil ) );
    }

    void TearDown()
    {
        m_patient.reset();
        m_fileUtil.reset();
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
        "WatchdogBufferMs": 250000
    }
}
)";

    std::unique_ptr<MockFileSysUtil> m_fileUtil;
    std::filesystem::file_time_type m_aTimeStamp;
    std::unique_ptr<PmConfig> m_patient;
};

TEST_F( TestPmConfig, LoadWillReadBsFile )
{
    std::filesystem::path bsfilename( "bs file" );

    m_fileUtil->MakeReadFileReturn( bsConfigData );

    EXPECT_CALL( *m_fileUtil, ReadFile( bsfilename ) );

    m_patient->LoadBsConfig( bsfilename.generic_u8string() );
}

TEST_F( TestPmConfig, LoadWillReadPmFile )
{
    std::filesystem::path pmfilename( "pm file" );

    m_fileUtil->MakeReadFileReturn( pmConfigData );

    EXPECT_CALL( *m_fileUtil, ReadFile( pmfilename ) );

    m_patient->LoadPmConfig( pmfilename.generic_u8string() );
}

TEST_F( TestPmConfig, LoadWillSaveCloudCheckinUri )
{
    m_fileUtil->MakeReadFileReturn( bsConfigData );

    m_patient->LoadBsConfig( "filename" );

    EXPECT_EQ( m_patient->GetCloudCheckinUri(), "https://packagemanager.cisco.com/checkin" );
}

TEST_F( TestPmConfig, LoadWillSaveCloudCatalogUri )
{
    m_fileUtil->MakeReadFileReturn( bsConfigData );

    m_patient->LoadBsConfig( "filename" );

    EXPECT_EQ( m_patient->GetCloudCatalogUri(), "https://packagemanager.cisco.com/catalog" );
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

TEST_F( TestPmConfig, LoadBsConfigWillSucceed )
{
    m_fileUtil->MakeReadFileReturn( bsConfigData );

    EXPECT_EQ( m_patient->LoadBsConfig( "filename" ), 0 );
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

    EXPECT_CALL( *m_fileUtil, ReadFile( filename ) ).WillOnce( Return( "" ) );
    EXPECT_CALL( *m_fileUtil, ReadFile( backupFilename ) ).WillOnce( Return( "" ) );

    m_patient->LoadPmConfig( filename.generic_u8string() );
}

TEST_F( TestPmConfig, VerifyBsFileIntegrityWillSucceed )
{
    m_fileUtil->MakeReadFileReturn( bsConfigData );

    EXPECT_EQ( m_patient->VerifyBsFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, VerifyPmFileIntegrityWillSucceed )
{
    m_fileUtil->MakeReadFileReturn( pmConfigData );

    EXPECT_EQ( m_patient->VerifyPmFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, VerifyBsFileIntegrityWillFailOnEmptyContents )
{
    m_fileUtil->MakeReadFileReturn( "" );

    EXPECT_NE( m_patient->VerifyBsFileIntegrity( "filename" ), 0 );
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

TEST_F( TestPmConfig, VerifyBsFileIntegrityWillNotAcceptInvalidJson )
{
    m_fileUtil->MakeReadFileReturn( R"(
{
    "wrong": { }
)" );

    EXPECT_NE( m_patient->VerifyBsFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, VerifyBsFileIntegrityWillNotAcceptInvalidURL )
{
    m_fileUtil->MakeReadFileReturn( R"(
{
	"pm": {
        "url": 1
	}
}
)" );

    EXPECT_NE( m_patient->VerifyBsFileIntegrity( "filename" ), 0 );
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

    EXPECT_EQ( m_patient->GetMaxFileCacheAge(), PM_CONFIG_MAX_CACHE_AGE_DEFAULT_SECS );
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