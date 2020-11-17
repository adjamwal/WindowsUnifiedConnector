#include "pch.h"

#include "UcConfig.h"
#include "MockWindowsUtilities.h"
#include <memory>

class TestUcConfig : public ::testing::Test
{
protected:
    void SetUp()
    {
        MockWindowsUtilities::Init();
        m_windowsUtils = MockWindowsUtilities::GetMockWindowUtilities();

        m_patient.reset( new UcConfig() );
    }

    void TearDown()
    {
        m_patient.reset();
        m_windowsUtils = nullptr;
        MockWindowsUtilities::Deinit();
    }

    MockWindowsUtilities* m_windowsUtils;
    std::unique_ptr<UcConfig> m_patient;
};

const std::string configData = R"(
{
	"uc": {
		"loglevel": 7
	}
}
)";

const std::string invalidConfigData = R"(
{
	"lmao": {
		"lol": 7
	}
}
)";

const std::string invalidConfigDataBrackets = R"(
{
	"uc": {
		"loglevel": 7
	}
)";

TEST_F( TestUcConfig, DefaultLogLevelIsError )
{
    EXPECT_EQ( m_patient->GetLogLevel(), ( uint32_t )IUcLogger::LOG_ERROR );
}

TEST_F( TestUcConfig, WillLoadLogLevel )
{
    m_windowsUtils->MakeFileExistsReturn( true );
    m_windowsUtils->MakeGetFileModifyTimeReturn( 1 );
    m_windowsUtils->MakeReadFileContentsReturn( configData );
    m_patient->LoadConfig();

    EXPECT_EQ( m_patient->GetLogLevel(), ( uint32_t )IUcLogger::LOG_DEBUG );
}

TEST_F( TestUcConfig, LoadWillFailOnInvalidJson )
{
    m_windowsUtils->MakeFileExistsReturn( true );
    m_windowsUtils->MakeGetFileModifyTimeReturn( 1 );
    m_windowsUtils->MakeReadFileContentsReturn( invalidConfigDataBrackets );
    
    EXPECT_FALSE( m_patient->LoadConfig() );
}

TEST_F( TestUcConfig, LoadInvalidConfigWillResetLogLevel )
{
    m_windowsUtils->MakeFileExistsReturn( true );
    m_windowsUtils->MakeGetFileModifyTimeReturn( 1 );
    m_windowsUtils->MakeReadFileContentsReturn( configData );
    m_patient->LoadConfig();

    m_windowsUtils->MakeGetFileModifyTimeReturn( 2 );
    m_windowsUtils->MakeReadFileContentsReturn( invalidConfigDataBrackets );
    m_patient->LoadConfig();

    EXPECT_EQ( m_patient->GetLogLevel(), ( uint32_t )IUcLogger::LOG_ERROR );
}

TEST_F( TestUcConfig, WillNotLoadIfFileNotModified )
{
    m_windowsUtils->MakeFileExistsReturn( true );
    m_windowsUtils->MakeGetFileModifyTimeReturn( 0 );

    m_windowsUtils->ExpectReadFileContentsIsNotCalled();

    m_patient->LoadConfig();
}

TEST_F( TestUcConfig, VerifyWillSucceed )
{
    m_windowsUtils->MakeReadFileContentsReturn( configData );

    EXPECT_TRUE( m_patient->VerifyConfig( L"file.json" ) );
}

TEST_F( TestUcConfig, VerifyWillFailOnInvalidJson )
{
    m_windowsUtils->MakeReadFileContentsReturn( invalidConfigDataBrackets );

    EXPECT_FALSE( m_patient->VerifyConfig( L"file.json" ) );
}

TEST_F( TestUcConfig, FailWhenConfigFileIsMissing )
{
    m_windowsUtils->MakeFileExistsReturn( false );

    EXPECT_FALSE( m_patient->LoadConfig() );
}
