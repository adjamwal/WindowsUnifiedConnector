#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "MocksCommon.h"
#include "IPmPlatformComponentManager.h"
#include "WindowsComponentManager.h"
#include "IUcLogger.h"
#include "PmTypes.h"
#include "MockWinApiWrapper.h"
#include "MockCodesignVerifier.h"
#include "MockPackageDiscovery.h"
#include "MockWindowsUtilities.h"
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

        m_patient.reset( new WindowsComponentManager( *m_winApiWrapper, *m_codeSignVerifier, *m_packageDiscovery ) );
    }

    void TearDown()
    {
        m_patient.reset();

        m_winApiWrapper.reset();
        m_codeSignVerifier.reset();
        m_packageDiscovery.reset();

        MockWindowsUtilities::Deinit();
        m_expectedComponentPackage = {};
    }

    void SetupComponentPackage()
    {
        m_expectedComponentPackage = {
            "test/1.0.0",
            "installerUrl",
            "installerType",
            "installerArgs",
            "installLocation",
            "signerName",
            "installerHash",
            "installerPath",
            {}
        };

        m_expectedComponentPackage.configs.push_back( {
            "configpath",
            "configsha256",
            "configcontents",
            "configverifyBinPath",
            "configverifyPath",
            "installLocation",
            "signerName",
            "test/1.0.0",
            false
            } );
    }

    PmComponent m_expectedComponentPackage;
    std::unique_ptr<MockWinApiWrapper> m_winApiWrapper;
    std::unique_ptr<MockCodesignVerifier> m_codeSignVerifier;
    std::unique_ptr<MockPackageDiscovery> m_packageDiscovery;

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
    EXPECT_CALL( *m_codeSignVerifier, Verify( _, _, _ ) );

    std::string error;
    m_patient->UpdateComponent( m_expectedComponentPackage, error );
}

TEST_F( TestWindowsComponentManager, WillCodeSignVerifyOnDeployConfiguration )
{
    EXPECT_CALL( *m_codeSignVerifier, Verify( _, _, _ ) );

    PackageConfigInfo config;
    m_patient->DeployConfiguration( config );
}

TEST_F( TestWindowsComponentManager, UpdateComponentSuccess )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    m_winApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_winApiWrapper->MakeGetExitCodeProcessReturn( TRUE );
    m_winApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_winApiWrapper->MakeGetLastErrorReturn( 0 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ret, 0 );
    EXPECT_EQ( error, "" );
}

TEST_F( TestWindowsComponentManager, UpdateExeWillAddExeToCmdLine )
{
    std::string error;
    PmComponent c;
    c.installerType = "exe";
    c.installerArgs = " /args";
    c.downloadedInstallerPath = "update.exe";
    std::string expectedCmdLine = c.downloadedInstallerPath + " " + c.installerArgs;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wExpectedCmdLine = converter.from_bytes( expectedCmdLine );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );

    EXPECT_CALL( *m_winApiWrapper, CreateProcessW( _, StrEq( wExpectedCmdLine ), _, _, _, _, _, _, _, _ ) );

    m_patient->UpdateComponent( c, error );
}

TEST_F( TestWindowsComponentManager, UpdateExeWillAddExeAndDropPath )
{
    std::string error;
    PmComponent c;
    c.installerType = "exe";
    c.installerArgs = " /args";
    std::string updateExe = "update.exe";
    c.downloadedInstallerPath = "path\\" + updateExe;
    std::string expectedCmdLine = updateExe + " " + c.installerArgs;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wExpectedCmdLine = converter.from_bytes( expectedCmdLine );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );

    EXPECT_CALL( *m_winApiWrapper, CreateProcessW( _, StrEq( wExpectedCmdLine ), _, _, _, _, _, _, _, _ ) );

    m_patient->UpdateComponent( c, error );
}

TEST_F( TestWindowsComponentManager, UpdateComponentInvalidPackageType )
{
    std::string error;
    PmComponent c;
    c.installerType = "INVALID";

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_NE( ret, 0 );
    EXPECT_NE( error, "" );
}

TEST_F( TestWindowsComponentManager, UpdateComponentFailureToGetSystemDirectory )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( false );
    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_NE( ret, 0 );
    EXPECT_NE( error, "" );
}


TEST_F( TestWindowsComponentManager, UpdateComponentCreateProcessFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    m_winApiWrapper->MakeCreateProcessWReturn( FALSE );
    m_winApiWrapper->MakeGetLastErrorReturn( 5 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ret, 5 );
    EXPECT_NE( error, "" );
}

TEST_F( TestWindowsComponentManager, UpdateWaitForSingleObjectFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    m_winApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_winApiWrapper->MakeWaitForSingleObjectReturn( -1 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ret, -1 );
    EXPECT_NE( error, "" );
}

TEST_F( TestWindowsComponentManager, UpdateComponentExitCodeProcessFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    m_winApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_winApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_winApiWrapper->MakeGetExitCodeProcessReturn( FALSE );
    m_winApiWrapper->MakeGetLastErrorReturn( 5 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ret, 5 );
    EXPECT_NE( error, "" );
}

TEST_F( TestWindowsComponentManager, UpdateComponentVerifyPackageFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    m_codeSignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_ERROR );

    int32_t ret = m_patient->UpdateComponent( c, error );

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
    PackageConfigInfo c;
    c.installLocation = TEST_INSTALL_PATH;
    c.signerName = TEST_VERIFY_SIGNER;
    c.verifyBinPath = TEST_VERIFY_BIN_PATH;

    EXPECT_CALL( *m_codeSignVerifier, Verify(
        std::wstring( _T( TEST_INSTALL_PATH "\\" TEST_VERIFY_BIN_PATH ) ),
        std::wstring( _T( TEST_VERIFY_SIGNER ) ),
        _ ) );

    m_patient->DeployConfiguration( c );
}

TEST_F( TestWindowsComponentManager, DeployConfigurationWillReturnVerifyFailure )
{
    PackageConfigInfo c;

    m_codeSignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_ERROR );

    int32_t ret = m_patient->DeployConfiguration( c );

    EXPECT_EQ( ( CodesignStatus )ret, CodesignStatus::CODE_SIGNER_ERROR );
}

TEST_F( TestWindowsComponentManager, DeployConfigurationWillBuildCommandLine )
{
    PackageConfigInfo c;

    c.verifyPath = TEST_VERIFY_PATH;
    std::wstring expectedArg = L"--config-path " + std::wstring( _T( TEST_VERIFY_PATH ) );
    m_codeSignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_SUCCESS );

    EXPECT_CALL( *m_winApiWrapper, CreateProcessW( _,
        StrEq( expectedArg.c_str() ),
        _, _, _, _, _, _, _, _ ) );

    m_patient->DeployConfiguration( c );
}

TEST_F( TestWindowsComponentManager, DeployConfigurationSuccess )
{
    PackageConfigInfo c;

    m_codeSignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_SUCCESS );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    m_winApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_winApiWrapper->MakeGetExitCodeProcessReturn( TRUE );
    m_winApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_winApiWrapper->MakeGetLastErrorReturn( 0 );

    int32_t ret = m_patient->DeployConfiguration( c );

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

TEST_F( TestWindowsComponentManager, WillResolveKnownFolderID )
{
    std::string knownFolderString = "_My_KNOWN_FOLDER_";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeResolveKnownFolderIdReturn( knownFolderString );

    std::string rtn = m_patient->ResolvePath( "<FOLDERID_SomeKnownFolder>" );
    EXPECT_EQ( rtn, knownFolderString );
}

TEST_F( TestWindowsComponentManager, WillResolveKnownFolderIDWithPrefix )
{
    std::string prefix = "prefix";
    std::string knownFolderString = "_My_KNOWN_FOLDER_";
    std::string suffix = "suffix";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeResolveKnownFolderIdReturn( knownFolderString );

    std::string rtn = m_patient->ResolvePath( prefix + "<FOLDERID_SomeKnownFolder>" + suffix );
    EXPECT_EQ( rtn, prefix + knownFolderString + suffix );
}

TEST_F( TestWindowsComponentManager, WillNotModifyPathWhenKnownFolderIsEmpty )
{
    std::string folder = "prefix<FOLDERID_SomeKnownFolder>suffix";

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ResolveKnownFolderId( _ ) ).WillOnce( Return( "" ) );

    std::string rtn = m_patient->ResolvePath( folder );

    EXPECT_EQ( rtn, folder );
}


TEST_F( TestWindowsComponentManager, WillNotResolveKnownFolderWhenTagNotFound )
{
    MockWindowsUtilities::GetMockWindowUtilities()->ExpectResolveKnownFolderIdIsNotCalled();

    m_patient->ResolvePath( "C:\\Windows\\Somthing" );
}
