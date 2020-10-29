#include "pch.h"
#include "IPmPlatformComponentManager.h"
#include "WindowsComponentManager.h"
#include "IUcLogger.h"
#include "PmTypes.h"
#include "MockWinApiWrapper.h"
#include "MockCodesignVerifier.h"
#include "MockWindowsUtilities.h"
#include <memory>

using ::testing::StrEq;

class ComponentTestWindowsPackageManager : public ::testing::Test
{
protected:
    void SetUp()
    {
        MockWindowsUtilities::InitMock();
        m_mockCodesignVerifier = std::make_unique<NiceMock<MockCodesignVerifier>>();
        m_mockWinApiWrapper = std::make_unique<NiceMock<MockWinApiWrapper>>();
        m_patient = std::make_unique<WindowsComponentManager>( *m_mockWinApiWrapper, *m_mockCodesignVerifier );
    }

    void TearDown()
    {
        m_patient.reset();
        m_mockWinApiWrapper.reset();
        m_mockCodesignVerifier.reset();
        MockWindowsUtilities::DeinitMock();
    }

    std::unique_ptr<MockCodesignVerifier> m_mockCodesignVerifier;
    std::unique_ptr<MockWinApiWrapper> m_mockWinApiWrapper;
    std::unique_ptr<WindowsComponentManager> m_patient;
};

TEST_F( ComponentTestWindowsPackageManager, UpdateComponentSuccess )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    MockWindowsUtilities::GetMockWindowUtilities().MakeGetSysDirectoryReturn( true );
    m_mockWinApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_mockWinApiWrapper->MakeGetExitCodeProcessReturn( TRUE );
    m_mockWinApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_mockWinApiWrapper->MakeGetLastErrorReturn( 0 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ret, 0 );
    EXPECT_EQ( error, "" );
}

TEST_F( ComponentTestWindowsPackageManager, UpdateComponentInvalidPackageType )
{
    std::string error;
    PmComponent c;
    c.installerType = "INVALID";

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_NE( ret, 0 );
    EXPECT_NE( error, "" );
}

TEST_F( ComponentTestWindowsPackageManager, UpdateComponentFailureToGetSystemDirectory )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    MockWindowsUtilities::GetMockWindowUtilities().MakeGetSysDirectoryReturn( false );
    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_NE( ret, 0 );
    EXPECT_NE( error, "" );
}


TEST_F( ComponentTestWindowsPackageManager, UpdateComponentCreateProcessFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    MockWindowsUtilities::GetMockWindowUtilities().MakeGetSysDirectoryReturn( true );
    m_mockWinApiWrapper->MakeCreateProcessWReturn( FALSE );
    m_mockWinApiWrapper->MakeGetLastErrorReturn( 5 );

    int32_t ret = m_patient->UpdateComponent( c, error );
    
    EXPECT_EQ( ret, 5 );
    EXPECT_NE( error, "" );
}

TEST_F( ComponentTestWindowsPackageManager, UpdateWaitForSingleObjectFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    m_mockWinApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_mockWinApiWrapper->MakeWaitForSingleObjectReturn( -1 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ret, -1 );
    EXPECT_NE( error, "" );
}

TEST_F( ComponentTestWindowsPackageManager, UpdateComponentExitCodeProcessFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    MockWindowsUtilities::GetMockWindowUtilities().MakeGetSysDirectoryReturn( true );
    m_mockWinApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_mockWinApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_mockWinApiWrapper->MakeGetExitCodeProcessReturn( FALSE );
    m_mockWinApiWrapper->MakeGetLastErrorReturn( 5 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ret, 5 );
    EXPECT_NE( error, "" );
}

TEST_F( ComponentTestWindowsPackageManager, UpdateComponentVerifyPackageFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    m_mockCodesignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_ERROR );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ( CodesignStatus )ret, CodesignStatus::CODE_SIGNER_ERROR );
    EXPECT_NE( error, "" );
}

#define TEST_INSTALL_PATH "INSTALL PATH"
#define TEST_VERIFY_BIN_PATH "TestVerifyBin.exe"
#define TEST_VERIFY_SIGNER "TEST SIGNER"
#define TEST_VERIFY_PATH "TestVerifyPath"
TEST_F( ComponentTestWindowsPackageManager, DeployConfigurationWillVerifySigner )
{
    std::string error;
    PackageConfigInfo c;
    c.installLocation = TEST_INSTALL_PATH;
    c.signerName = TEST_VERIFY_SIGNER;
    c.verifyBinPath = TEST_VERIFY_BIN_PATH;

    EXPECT_CALL( *m_mockCodesignVerifier, Verify( 
        std::wstring( _T( TEST_INSTALL_PATH "\\" TEST_VERIFY_BIN_PATH ) ),
        std::wstring( _T( TEST_VERIFY_SIGNER ) ), 
        _ ) );

    m_patient->DeployConfiguration( c  );
}

TEST_F( ComponentTestWindowsPackageManager, DeployConfigurationWillReturnVerifyFailure )
{
    PackageConfigInfo c;

    m_mockCodesignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_ERROR );

    int32_t ret = m_patient->DeployConfiguration( c );

    EXPECT_EQ( ( CodesignStatus )ret, CodesignStatus::CODE_SIGNER_ERROR );
}

TEST_F( ComponentTestWindowsPackageManager, DeployConfigurationWillBuildCommandLine )
{
    PackageConfigInfo c;

    c.verifyPath = TEST_VERIFY_PATH;
    std::wstring expectedArg = L"--config-path " + std::wstring(_T( TEST_VERIFY_PATH ) );
    m_mockCodesignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_SUCCESS );

    EXPECT_CALL( *m_mockWinApiWrapper, CreateProcessW( _,
        StrEq( expectedArg.c_str() ),
        _, _, _, _, _, _, _, _ ) );

    m_patient->DeployConfiguration( c );
}

TEST_F( ComponentTestWindowsPackageManager, DeployConfigurationSuccess )
{
    PackageConfigInfo c;

    m_mockCodesignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_SUCCESS );
    MockWindowsUtilities::GetMockWindowUtilities().MakeGetSysDirectoryReturn( true );
    m_mockWinApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_mockWinApiWrapper->MakeGetExitCodeProcessReturn( TRUE );
    m_mockWinApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_mockWinApiWrapper->MakeGetLastErrorReturn( 0 );

    int32_t ret = m_patient->DeployConfiguration( c );

    EXPECT_EQ( ret, 0 );
}