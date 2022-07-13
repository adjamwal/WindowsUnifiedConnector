#include "gtest/gtest.h"
#include "PmBootstrap.h"
#include "MockFileSysUtil.h"
#include <memory>

class TestPmBootstrap : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_fileUtil.reset( new NiceMock<MockFileSysUtil>() );
        m_patient.reset( new PmBootstrap( *m_fileUtil ) );
    }

    void TearDown()
    {
        m_patient.reset();
        m_fileUtil.reset();
    }

    const std::string bsConfigData = R"(
{
    "business_id": "BusinessId",
    "installer_key": "InstallerKey",
    "identify_url": "https://someDomain/identify",
    "event_url": "https://someDomain/event"
}
)";

    std::unique_ptr<MockFileSysUtil> m_fileUtil;
    std::unique_ptr<PmBootstrap> m_patient;
};

TEST_F( TestPmBootstrap, LoadWillReadFile )
{
    std::filesystem::path pmfilename( "pm file" );

    m_fileUtil->MakeReadFileReturn( bsConfigData );

    EXPECT_CALL( *m_fileUtil, ReadTextFile( pmfilename ) );

    m_patient->LoadPmBootstrap( pmfilename.generic_u8string() );
}

TEST_F( TestPmBootstrap, LoadWillSaveIdenitfyUrl )
{
    m_fileUtil->MakeReadFileReturn( bsConfigData );

    m_patient->LoadPmBootstrap( "filename" );

    EXPECT_EQ( m_patient->GetIdentifyUri(), "https://someDomain/identify" );
}

TEST_F( TestPmBootstrap, LoadConfigWillSucceed )
{
    m_fileUtil->MakeReadFileReturn( bsConfigData );

    EXPECT_EQ( m_patient->LoadPmBootstrap( "filename" ), 0 );
}

TEST_F( TestPmBootstrap, LoadConfigWillFailOnEmptyFile )
{
    m_fileUtil->MakeReadFileReturn( "" );

    EXPECT_NE( m_patient->LoadPmBootstrap( "filename" ), 0 );
}

TEST_F( TestPmBootstrap, LoadConfigWillFailOnInvalidJson )
{
    m_fileUtil->MakeReadFileReturn( "{" );

    EXPECT_NE( m_patient->LoadPmBootstrap( "filename" ), 0 );
}

TEST_F( TestPmBootstrap, LoadConfigWillFailOnInvalidIdentifyUrl )
{
    m_fileUtil->MakeReadFileReturn( R"({"identify_url": 0})" );

    EXPECT_NE( m_patient->LoadPmBootstrap( "filename" ), 0 );
}
