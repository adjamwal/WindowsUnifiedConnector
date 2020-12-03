#include "gtest/gtest.h"
#include "PmConfig.h"
#include "MockFileUtil.h"

#include <memory>

class TestPmConfig : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_fileUtil.reset( new NiceMock<MockFileUtil>() );

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
`   },
	"pm": {
        "url": "https://packagemanager.cisco.com/checkin",
        "event_url": "https://packagemanager.cisco.com/event/1"
    }
}
)";

    const std::string pmConfigData = R"(
{
    "pm": {
        "loglevel": 7,
        "CheckinInterval": 300000
    }
}
)";

    std::unique_ptr<MockFileUtil> m_fileUtil;
    std::unique_ptr<PmConfig> m_patient;
};

TEST_F( TestPmConfig, LoadWillReadBsFile )
{
    std::string bsfilename( "bs file" );

    m_fileUtil->MakeReadFileReturn( bsConfigData );

    EXPECT_CALL( *m_fileUtil, ReadFile( bsfilename ) );

    m_patient->LoadBsConfig( bsfilename );
}

TEST_F( TestPmConfig, LoadWillReadPmFile )
{
    std::string pmfilename( "pm file" );

    m_fileUtil->MakeReadFileReturn( pmConfigData );

    EXPECT_CALL( *m_fileUtil, ReadFile( pmfilename ) );

    m_patient->LoadPmConfig( pmfilename );
}

TEST_F( TestPmConfig, LoadWillSaveCloudUri )
{
    m_fileUtil->MakeReadFileReturn( bsConfigData );

    m_patient->LoadBsConfig( "filename" );

    EXPECT_EQ( m_patient->GetCloudCheckinUri(), "https://packagemanager.cisco.com/checkin" );
}

TEST_F( TestPmConfig, LoadWillSaveInterval )
{
    m_fileUtil->MakeReadFileReturn( pmConfigData );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_EQ( m_patient->GetCloudCheckinInterval(), 300000 );
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

TEST_F( TestPmConfig, LoadWillTryBackupFile )
{
    std::string filename( "filename" );

    InSequence s;

    EXPECT_CALL( *m_fileUtil, ReadFile( filename ) ).WillOnce( Return( "" ) );
    EXPECT_CALL( *m_fileUtil, ReadFile( filename + ".bak" ) ).WillOnce( Return( "" ) );

    m_patient->LoadPmConfig( filename );
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

TEST_F( TestPmConfig, VerifyBsFileIntegrityWillSucceedWillFailOnEmptyContents )
{
    m_fileUtil->MakeReadFileReturn( "" );

    EXPECT_NE( m_patient->VerifyBsFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, VerifyPmFileIntegrityWillSucceedWillFailOnEmptyContents )
{
    m_fileUtil->MakeReadFileReturn( "" );

    EXPECT_NE( m_patient->VerifyPmFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, VerifyFileIntegrityWillSucceedWillNotAcceptInvalidJson )
{
    m_fileUtil->MakeReadFileReturn( R"(
{
    "wrong": { }
)" );

    EXPECT_NE( m_patient->VerifyBsFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, VerifyFileIntegrityWillSucceedWillNotAcceptInvalidURL )
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

TEST_F( TestPmConfig, LoadSetDefaultInterval )
{
    m_fileUtil->MakeReadFileReturn( "" );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_EQ( m_patient->GetCloudCheckinInterval(), PM_CONFIG_INTERVAL_DEFAULT );
}

TEST_F( TestPmConfig, LoadSetDefaultLogLevel )
{
    m_fileUtil->MakeReadFileReturn( "" );

    m_patient->LoadPmConfig( "filename" );

    EXPECT_EQ( m_patient->GetLogLevel(), PM_CONFIG_LOGLEVEL_DEFAULT );
}