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

    void MakeLoadReadFileSucceed()
    {
        m_fileUtil->MakeReadFileReturn( R"(
{
    "cloud": {
        "CheckinUri": "https://packagemanager.cisco.com/checkin",
        "CheckinInterval": 1000
    }
}
)" );
    }

    std::unique_ptr<MockFileUtil> m_fileUtil;
    std::unique_ptr<PmConfig> m_patient;
};

TEST_F( TestPmConfig, LoadWillReadFile )
{
    std::string filename( "Some file" );
    MakeLoadReadFileSucceed();

    EXPECT_CALL( *m_fileUtil, ReadFile( filename ) );

    m_patient->Load( filename );
}

TEST_F( TestPmConfig, LoadWillSaveCloudUri )
{
    MakeLoadReadFileSucceed();

    m_patient->Load( "filename" );

    EXPECT_EQ( m_patient->GetCloudUri(), "https://packagemanager.cisco.com/checkin" );
}

TEST_F( TestPmConfig, LoadWillSaveInterval )
{
    MakeLoadReadFileSucceed();

    m_patient->Load( "filename" );

    EXPECT_EQ( m_patient->GetCloudInterval(), 1000 );
}

TEST_F( TestPmConfig, LoadWillSucceed )
{
    MakeLoadReadFileSucceed();

    EXPECT_EQ( m_patient->Load( "filename" ), 0 );
}

TEST_F( TestPmConfig, LoadWillTryBackupFile )
{
    std::string filename( "filename" );

    InSequence s;

    EXPECT_CALL( *m_fileUtil, ReadFile( filename ) ).WillOnce( Return( "" ) );
    EXPECT_CALL( *m_fileUtil, ReadFile( filename + ".bak" ) ).WillOnce( Return( "" ) );

    m_patient->Load( filename );
}

TEST_F( TestPmConfig, VerifyFileIntegrityWillSucceed )
{
    MakeLoadReadFileSucceed();

    EXPECT_EQ( m_patient->VerifyFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, VerifyFileIntegrityWillSucceedWillFailOnEmptyContents )
{
    m_fileUtil->MakeReadFileReturn( "" );

    EXPECT_NE( m_patient->VerifyFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, VerifyFileIntegrityWillSucceedWillNotAcceptInvalidJson )
{
    m_fileUtil->MakeReadFileReturn( R"(
{
    "cloud": {
        "CheckinUri": "https://packagemanager.cisco.com/checkin"
    }
)" );

    EXPECT_NE( m_patient->VerifyFileIntegrity( "filename" ), 0 );
}

TEST_F( TestPmConfig, VerifyFileIntegrityWillSucceedWillNotAcceptInvalidURL )
{
    m_fileUtil->MakeReadFileReturn( R"(
{
    "cloud": {
        "CheckinUri": 1,
        "CheckinInterval": 1000
    }
}
)" );

    EXPECT_NE( m_patient->VerifyFileIntegrity( "filename" ), 0 );
}