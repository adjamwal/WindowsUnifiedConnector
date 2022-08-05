#include "pch.h"
#include "IPmPlatformComponentManager.h"
#include "WindowsComponentManager.h"
#include "IUcLogger.h"
#include "PmTypes.h"
#include "MockWinApiWrapper.h"
#include "MockCodesignVerifier.h"
#include "MockPackageDiscovery.h"
#include "MockWindowsUtilities.h"
#include "MockUserImpersonator.h"
#include "MockMsiApi.h"
#include <memory>
#include <codecvt>

class TestWindowsComponentManager : public ::testing::Test
{
protected:
    void SetUp()
    {
        MockWindowsUtilities::Init();
        m_winApiWrapper.reset( new NiceMock<MockWinApiWrapper>() );
        m_codeSignVerifier.reset( new NiceMock<MockCodesignVerifier>() );
        m_packageDiscovery.reset( new NiceMock<MockPackageDiscovery>() );
        m_userImpersonator.reset( new NiceMock<MockUserImpersonator>() );
        m_msiApi.reset( new NiceMock<MockMsiApi>() );

        m_patient.reset( new WindowsComponentManager( *m_winApiWrapper, *m_codeSignVerifier, *m_packageDiscovery, *m_userImpersonator, *m_msiApi ) );
    }

    void TearDown()
    {
        m_patient.reset();

        m_winApiWrapper.reset();
        m_codeSignVerifier.reset();
        m_packageDiscovery.reset();
        m_userImpersonator.reset();
        m_msiApi.reset();

        MockWindowsUtilities::Deinit();
        m_expectedComponentPackage = {};
    }

    void SetupComponentPackage()
    {
        m_expectedComponentPackage = {
            "test/1.0.0",
            "installerUrl",
            "msi",
            "installerArgs",
            "installLocation",
            "signerName",
            "installerHash",
            "installerPath",
            {}
        };

        m_expectedComponentPackage.configs.push_back( {
            "cfgPath",
            "unresolvedCfgPath",
            "", //deployPath
            "", //unresolvedDeployPath
            false, //isDiscoveredAtDeployPath
            "configsha256",
            "configcontents",
            "configverifyBinPath",
            "configverifyPath",
            "installLocation",
            "signerName",
            "test/1.0.0",
            false //deleteFile
            } );
    }

    PmComponent m_expectedComponentPackage;
    std::unique_ptr<MockWinApiWrapper> m_winApiWrapper;
    std::unique_ptr<MockCodesignVerifier> m_codeSignVerifier;
    std::unique_ptr<MockPackageDiscovery> m_packageDiscovery;
    std::unique_ptr<MockUserImpersonator> m_userImpersonator;
    std::unique_ptr<MockMsiApi> m_msiApi;

    std::unique_ptr<WindowsComponentManager> m_patient;
};

TEST_F( TestWindowsComponentManager, WillGetInstalledPackages )
{
    EXPECT_CALL( *m_packageDiscovery, DiscoverInstalledPackages( _ ) );

    std::vector<PmProductDiscoveryRules> catalogRules;
    PackageInventory foundPackages;
    m_patient->GetInstalledPackages( catalogRules, foundPackages );
}

TEST_F( TestWindowsComponentManager, WillCodeSignVerifyOnUpdateComponent )
{
    SetupComponentPackage();
    EXPECT_CALL( *m_codeSignVerifier, Verify( _, Matcher<const std::wstring&>( _ ), _ ) );

    std::string error;
    m_patient->UpdateComponent( m_expectedComponentPackage, error );
}

TEST_F(TestWindowsComponentManager, UpdateComponentWontVerifyBinaryWhenSignerIsEmpty)
{
    SetupComponentPackage();
    m_expectedComponentPackage.signerName = "";

    m_codeSignVerifier->ExpectVerifyIsNotCalled();
    m_codeSignVerifier->ExpectVerifyWithKilldateIsNotCalled();

    std::string error;
    m_patient->UpdateComponent(m_expectedComponentPackage, error);
}

TEST_F( TestWindowsComponentManager, WillCodeSignVerifyOnDeployConfiguration )
{
    EXPECT_CALL( *m_codeSignVerifier, Verify( _, Matcher<const std::wstring&>( _ ), _ ) );

    SetupComponentPackage();
    m_patient->DeployConfiguration( m_expectedComponentPackage.configs[0] );
}

TEST_F( TestWindowsComponentManager, UpdateComponentSuccess )
{
    std::string error;
    SetupComponentPackage();

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    m_winApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_winApiWrapper->MakeGetExitCodeProcessReturn( TRUE );
    m_winApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_winApiWrapper->MakeGetLastErrorReturn( 0 );

    int32_t ret = m_patient->UpdateComponent( m_expectedComponentPackage, error );

    EXPECT_EQ( ret, 0 );
    EXPECT_EQ( error, "" );
}

TEST_F( TestWindowsComponentManager, UpdateExeWillAddExeToCmdLine )
{
    std::string error;
    SetupComponentPackage();
    m_expectedComponentPackage.installerType = "exe";
    m_expectedComponentPackage.installerArgs = " /args";
    m_expectedComponentPackage.downloadedInstallerPath = "update.exe";
    std::string expectedCmdLine = m_expectedComponentPackage.downloadedInstallerPath.generic_u8string() + " " + m_expectedComponentPackage.installerArgs;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wExpectedCmdLine = converter.from_bytes( expectedCmdLine );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );

    EXPECT_CALL( *m_winApiWrapper, CreateProcessW( _, StrEq( wExpectedCmdLine ), _, _, _, _, _, _, _, _ ) );

    m_patient->UpdateComponent( m_expectedComponentPackage, error );
}

TEST_F( TestWindowsComponentManager, UpdateExeWillAddExeAndDropPath )
{
    std::string error;
    SetupComponentPackage();
    m_expectedComponentPackage.installerType = "exe";
    m_expectedComponentPackage.installerArgs = " /args";
    std::string updateExe = "update.exe";
    m_expectedComponentPackage.downloadedInstallerPath = "path\\" + updateExe;
    std::string expectedCmdLine = updateExe + " " + m_expectedComponentPackage.installerArgs;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wExpectedCmdLine = converter.from_bytes( expectedCmdLine );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );

    EXPECT_CALL( *m_winApiWrapper, CreateProcessW( _, StrEq( wExpectedCmdLine ), _, _, _, _, _, _, _, _ ) );

    m_patient->UpdateComponent( m_expectedComponentPackage, error );
}

TEST_F( TestWindowsComponentManager, UpdateComponentInvalidPackageType )
{
    std::string error;
    SetupComponentPackage();
    m_expectedComponentPackage.installerType = "INVALID";

    int32_t ret = m_patient->UpdateComponent( m_expectedComponentPackage, error );

    EXPECT_NE( ret, 0 );
    EXPECT_NE( error, "" );
}

TEST_F( TestWindowsComponentManager, UpdateComponentFailureToGetSystemDirectory )
{
    std::string error;
    SetupComponentPackage();

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( false );
    int32_t ret = m_patient->UpdateComponent( m_expectedComponentPackage, error );

    EXPECT_NE( ret, 0 );
    EXPECT_NE( error, "" );
}


TEST_F( TestWindowsComponentManager, UpdateComponentCreateProcessFailure )
{
    std::string error;
    SetupComponentPackage();

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    m_winApiWrapper->MakeCreateProcessWReturn( FALSE );
    m_winApiWrapper->MakeGetLastErrorReturn( 5 );

    int32_t ret = m_patient->UpdateComponent( m_expectedComponentPackage, error );

    EXPECT_EQ( ret, 5 );
    EXPECT_NE( error, "" );
}

TEST_F( TestWindowsComponentManager, UpdateWaitForSingleObjectFailure )
{
    std::string error;
    SetupComponentPackage();

    m_winApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_winApiWrapper->MakeWaitForSingleObjectReturn( -1 );

    int32_t ret = m_patient->UpdateComponent( m_expectedComponentPackage, error );

    EXPECT_EQ( ret, -1 );
    EXPECT_NE( error, "" );
}

TEST_F( TestWindowsComponentManager, UpdateComponentExitCodeProcessFailure )
{
    std::string error;
    SetupComponentPackage();

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    m_winApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_winApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_winApiWrapper->MakeGetExitCodeProcessReturn( FALSE );
    m_winApiWrapper->MakeGetLastErrorReturn( 5 );

    int32_t ret = m_patient->UpdateComponent( m_expectedComponentPackage, error );

    EXPECT_EQ( ret, 5 );
    EXPECT_NE( error, "" );
}

TEST_F( TestWindowsComponentManager, UpdateComponentVerifyPackageFailure )
{
    std::string error;
    SetupComponentPackage();

    m_codeSignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_ERROR );

    int32_t ret = m_patient->UpdateComponent( m_expectedComponentPackage, error );

    EXPECT_EQ( ( CodesignStatus )ret, CodesignStatus::CODE_SIGNER_ERROR );
    EXPECT_NE( error, "" );
}

#define TEST_INSTALL_PATH "INSTALL PATH"
#define TEST_VERIFY_BIN_PATH "TestVerifyBin.exe"
#define TEST_VERIFY_SIGNER "TEST SIGNER"
#define TEST_VERIFY_PATH "TestVerifyPath"
TEST_F( TestWindowsComponentManager, DeployConfigurationWillVerifySigner )
{
    std::string error;
    SetupComponentPackage();
    m_expectedComponentPackage.configs.front().installLocation = TEST_INSTALL_PATH;
    m_expectedComponentPackage.configs.front().signerName = TEST_VERIFY_SIGNER;
    m_expectedComponentPackage.configs.front().verifyBinPath = TEST_VERIFY_BIN_PATH;

    EXPECT_CALL( *m_codeSignVerifier, Verify(
        std::wstring( _T( TEST_INSTALL_PATH "\\" TEST_VERIFY_BIN_PATH ) ),
        std::wstring( _T( TEST_VERIFY_SIGNER ) ),
        _ ) );

    m_patient->DeployConfiguration( m_expectedComponentPackage.configs.front() );
}

TEST_F(TestWindowsComponentManager, DeployConfigurationWontVerifyWhenSignerIsEmpty )
{
    std::string error;
    SetupComponentPackage();
    m_expectedComponentPackage.configs.front().installLocation = TEST_INSTALL_PATH;
    m_expectedComponentPackage.configs.front().signerName = "";
    m_expectedComponentPackage.configs.front().verifyBinPath = TEST_VERIFY_BIN_PATH;

    m_codeSignVerifier->ExpectVerifyIsNotCalled();
    m_codeSignVerifier->ExpectVerifyWithKilldateIsNotCalled();

    m_patient->DeployConfiguration( m_expectedComponentPackage.configs.front() );
}

TEST_F( TestWindowsComponentManager, DeployConfigurationWillReturnVerifyFailure )
{
    SetupComponentPackage();

    m_codeSignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_ERROR );

    int32_t ret = m_patient->DeployConfiguration( m_expectedComponentPackage.configs.front() );

    EXPECT_EQ( ( CodesignStatus )ret, CodesignStatus::CODE_SIGNER_ERROR );
}

TEST_F( TestWindowsComponentManager, DeployConfigurationWillBuildCommandLine )
{
    SetupComponentPackage();

    m_expectedComponentPackage.configs.front().verifyPath = TEST_VERIFY_PATH;
    std::wstring expectedArg = L"--config-path " + std::wstring( _T( TEST_VERIFY_PATH ) );
    m_codeSignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_SUCCESS );

    EXPECT_CALL( *m_winApiWrapper, CreateProcessW( _,
        StrEq( expectedArg.c_str() ),
        _, _, _, _, _, _, _, _ ) );

    m_patient->DeployConfiguration( m_expectedComponentPackage.configs.front() );
}

TEST_F( TestWindowsComponentManager, DeployConfigurationSuccess )
{
    SetupComponentPackage();

    m_codeSignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_SUCCESS );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    m_winApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_winApiWrapper->MakeGetExitCodeProcessReturn( TRUE );
    m_winApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_winApiWrapper->MakeGetLastErrorReturn( 0 );

    int32_t ret = m_patient->DeployConfiguration( m_expectedComponentPackage.configs.front() );

    EXPECT_EQ( ret, 0 );
}

TEST_F( TestWindowsComponentManager, GetInstalledPackagesSucceed )
{
    std::vector<PmProductDiscoveryRules> catalogRules;
    PackageInventory installedPackages;

    int32_t ret = m_patient->GetInstalledPackages( catalogRules, installedPackages );

    EXPECT_EQ( ret, 0 );
}

TEST_F( TestWindowsComponentManager, GetInstalledPackagesWillSearchForPackages )
{
    std::vector<PmProductDiscoveryRules> catalogRules;
    PackageInventory installedPackages;

    EXPECT_CALL( *m_packageDiscovery, DiscoverInstalledPackages( _ ) );

    int32_t ret = m_patient->GetInstalledPackages( catalogRules, installedPackages );
}

TEST_F( TestWindowsComponentManager, NotifySystemRestartWillImpersonateUser )
{
    std::wstring diagToolDir = L"SomeDir";
    std::vector<ULONG> sessionList;

    sessionList.push_back( 0 );

    ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryStringW( _, _, _, _ ) ).WillByDefault( DoAll(
        SetArgReferee<3>( diagToolDir ),
        Return( true )
    ) );

    ON_CALL( *m_userImpersonator, GetActiveUserSessions( _ ) ).WillByDefault( DoAll(
        SetArgReferee<0>( sessionList ),
        Return( true )
    ) );

    EXPECT_CALL( *m_userImpersonator, RunProcessInSession( _, _, _, _ ) );

    m_patient->NotifySystemRestart();
}

TEST_F( TestWindowsComponentManager, NotifySystemRestartWillAbortWithoutUCPath )
{
    MockWindowsUtilities::GetMockWindowUtilities()->MakeReadRegistryStringReturn( false );

    m_userImpersonator->ExpectGetActiveUserSessionsNotCalled();
    m_userImpersonator->ExpectRunProcessInSessionNotCalled();

    m_patient->NotifySystemRestart();
}

TEST_F( TestWindowsComponentManager, NotifySystemRestartWillAbortWithoutSessionList )
{
    MockWindowsUtilities::GetMockWindowUtilities()->MakeReadRegistryStringReturn( false );
    m_userImpersonator->MakeGetActiveUserSessionsReturn( false );

    m_userImpersonator->ExpectRunProcessInSessionNotCalled();

    m_patient->NotifySystemRestart();
}

TEST_F( TestWindowsComponentManager, UpdateComponentWillUseBackSlashesForExePackage )
{
    std::string error;
    SetupComponentPackage();
    m_expectedComponentPackage.installerType = "exe";
    m_expectedComponentPackage.installerArgs = " /args";
    m_expectedComponentPackage.downloadedInstallerPath = "C:/test/update.exe";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );

    EXPECT_CALL( *m_winApiWrapper, CreateProcessW( StrEq( L"C:\\test\\update.exe" ), _, _, _, _, _, _, _, _, _ ) );

    m_patient->UpdateComponent( m_expectedComponentPackage, error );
}

TEST_F( TestWindowsComponentManager, UpdateComponentWillUseBackSlashesForMsiPackage )
{
    std::string error;
    SetupComponentPackage();
    m_expectedComponentPackage.installerType = "msi";
    m_expectedComponentPackage.installerArgs = " /args";
    m_expectedComponentPackage.downloadedInstallerPath = "C:/test/update.msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );

    EXPECT_CALL( *m_winApiWrapper, CreateProcessW( _, HasSubstr( L"C:\\test\\update.msi" ), _, _, _, _, _, _, _, _ ) );

    m_patient->UpdateComponent( m_expectedComponentPackage, error );
}

TEST_F( TestWindowsComponentManager, UpdateComponentWillCheckIfMsiServiceIsReady )
{
    std::string error;
    SetupComponentPackage();
    m_expectedComponentPackage.installerType = "msi";
    m_expectedComponentPackage.installerArgs = " /args";
    m_expectedComponentPackage.downloadedInstallerPath = "C:/test/update.msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    ON_CALL( *m_msiApi, IsMsiServiceReadyforInstall() ).WillByDefault( Return( true ) );

    EXPECT_CALL( *m_msiApi, IsMsiServiceReadyforInstall() ).Times( 1 );

    m_patient->UpdateComponent( m_expectedComponentPackage, error );
}

TEST_F( TestWindowsComponentManager, UpdateComponentWillRetryIffMsiServiceIsReadyIsNotReady )
{
    std::string error;
    SetupComponentPackage();
    m_expectedComponentPackage.installerType = "msi";
    m_expectedComponentPackage.installerArgs = " /args";
    m_expectedComponentPackage.downloadedInstallerPath = "C:/test/update.msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );

    EXPECT_CALL( *m_msiApi, IsMsiServiceReadyforInstall() )
        .WillOnce( Return( false ) )
        .WillOnce( Return( true ) );

    m_patient->UpdateComponent( m_expectedComponentPackage, error );
}

TEST_F( TestWindowsComponentManager, UpdateComponentWillCheckIfMsiServiceIsReadyUpTo30Times )
{
    std::string error;
    SetupComponentPackage();
    m_expectedComponentPackage.installerType = "msi";
    m_expectedComponentPackage.installerArgs = " /args";
    m_expectedComponentPackage.downloadedInstallerPath = "C:/test/update.msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    ON_CALL( *m_msiApi, IsMsiServiceReadyforInstall() ).WillByDefault( Return( false ) );

    EXPECT_CALL( *m_msiApi, IsMsiServiceReadyforInstall() ).Times( 30 );

    m_patient->UpdateComponent( m_expectedComponentPackage, error );
}

TEST_F( TestWindowsComponentManager, UpdateComponentWillUpdateEvenIfSerivceIsNotReady )
{
    std::string error;
    SetupComponentPackage();
    m_expectedComponentPackage.installerType = "msi";
    m_expectedComponentPackage.installerArgs = " /args";
    m_expectedComponentPackage.downloadedInstallerPath = "C:/test/update.msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    ON_CALL( *m_msiApi, IsMsiServiceReadyforInstall() ).WillByDefault( Return( false ) );

    EXPECT_CALL( *m_winApiWrapper, CreateProcessW( _, HasSubstr( L"C:\\test\\update.msi" ), _, _, _, _, _, _, _, _ ) );

    m_patient->UpdateComponent( m_expectedComponentPackage, error );
}