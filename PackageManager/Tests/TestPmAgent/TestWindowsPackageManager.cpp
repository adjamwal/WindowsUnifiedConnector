#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include "MocksCommon.h"
#include "IPmPlatformComponentManager.h"
#include "WindowsComponentManager.h"
#include "IUcLogger.h"
#include "PmTypes.h"
#include "MockWinApiWrapper.h"
#include "MockCodesignVerifier.h"
#include "MockWindowsUtilities.h"
#include "MockPackageDiscovery.h"
#include <memory>

#include <codecvt>

using ::testing::StrEq;

class TestWindowsPackageManager : public ::testing::Test
{
protected:
    void SetUp()
    {
        MockWindowsUtilities::Init();
        m_mockCodesignVerifier = std::make_unique<NiceMock<MockCodesignVerifier>>();
        m_mockWinApiWrapper = std::make_unique<NiceMock<MockWinApiWrapper>>();
        m_mockPackageDiscovery = std::make_unique<NiceMock<MockPackageDiscovery>>();
        m_patient = std::make_unique<WindowsComponentManager>( *m_mockWinApiWrapper, *m_mockCodesignVerifier, *m_mockPackageDiscovery );
    }

    void TearDown()
    {
        m_patient.reset();
        m_mockWinApiWrapper.reset();
        m_mockCodesignVerifier.reset();
        m_mockPackageDiscovery.reset();

        MockWindowsUtilities::Deinit();
    }

    std::unique_ptr<MockCodesignVerifier> m_mockCodesignVerifier;
    std::unique_ptr<MockWinApiWrapper> m_mockWinApiWrapper;
    std::unique_ptr<MockPackageDiscovery> m_mockPackageDiscovery;
    std::unique_ptr<WindowsComponentManager> m_patient;
};

TEST_F( TestWindowsPackageManager, UpdateComponentSuccess )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    m_mockWinApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_mockWinApiWrapper->MakeGetExitCodeProcessReturn( TRUE );
    m_mockWinApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_mockWinApiWrapper->MakeGetLastErrorReturn( 0 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ret, 0 );
    EXPECT_EQ( error, "" );
}

TEST_F( TestWindowsPackageManager, UpdateExeWillAddExeToCmdLine )
{
    std::string error;
    PmComponent c;
    c.installerType = "exe";
    c.installerArgs = " /args";
    c.installerPath = "update.exe";
    std::string expectedCmdLine = c.installerPath + " " + c.installerArgs;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wExpectedCmdLine = converter.from_bytes( expectedCmdLine );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );

    EXPECT_CALL( *m_mockWinApiWrapper, CreateProcessW( _, StrEq( wExpectedCmdLine ), _, _, _, _, _, _, _, _ ) );

    m_patient->UpdateComponent( c, error );
}

TEST_F( TestWindowsPackageManager, UpdateExeWillAddExeAndDropPath )
{
    std::string error;
    PmComponent c;
    c.installerType = "exe";
    c.installerArgs = " /args";
    std::string updateExe = "update.exe";
    c.installerPath = "path\\" + updateExe;
    std::string expectedCmdLine = updateExe + " " + c.installerArgs;
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wExpectedCmdLine = converter.from_bytes( expectedCmdLine );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );

    EXPECT_CALL( *m_mockWinApiWrapper, CreateProcessW( _, StrEq( wExpectedCmdLine ), _, _, _, _, _, _, _, _ ) );

    m_patient->UpdateComponent( c, error );
}

TEST_F( TestWindowsPackageManager, UpdateComponentInvalidPackageType )
{
    std::string error;
    PmComponent c;
    c.installerType = "INVALID";

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_NE( ret, 0 );
    EXPECT_NE( error, "" );
}

TEST_F( TestWindowsPackageManager, UpdateComponentFailureToGetSystemDirectory )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( false );
    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_NE( ret, 0 );
    EXPECT_NE( error, "" );
}


TEST_F( TestWindowsPackageManager, UpdateComponentCreateProcessFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    m_mockWinApiWrapper->MakeCreateProcessWReturn( FALSE );
    m_mockWinApiWrapper->MakeGetLastErrorReturn( 5 );

    int32_t ret = m_patient->UpdateComponent( c, error );
    
    EXPECT_EQ( ret, 5 );
    EXPECT_NE( error, "" );
}

TEST_F( TestWindowsPackageManager, UpdateWaitForSingleObjectFailure )
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

TEST_F( TestWindowsPackageManager, UpdateComponentExitCodeProcessFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    m_mockWinApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_mockWinApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_mockWinApiWrapper->MakeGetExitCodeProcessReturn( FALSE );
    m_mockWinApiWrapper->MakeGetLastErrorReturn( 5 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ret, 5 );
    EXPECT_NE( error, "" );
}

TEST_F( TestWindowsPackageManager, UpdateComponentVerifyPackageFailure )
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
TEST_F( TestWindowsPackageManager, DeployConfigurationWillVerifySigner )
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

TEST_F( TestWindowsPackageManager, DeployConfigurationWillReturnVerifyFailure )
{
    PackageConfigInfo c;

    m_mockCodesignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_ERROR );

    int32_t ret = m_patient->DeployConfiguration( c );

    EXPECT_EQ( ( CodesignStatus )ret, CodesignStatus::CODE_SIGNER_ERROR );
}

TEST_F( TestWindowsPackageManager, DeployConfigurationWillBuildCommandLine )
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

TEST_F( TestWindowsPackageManager, DeployConfigurationSuccess )
{
    PackageConfigInfo c;

    m_mockCodesignVerifier->MakeVerifyReturn( CodesignStatus::CODE_SIGNER_SUCCESS );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetSysDirectoryReturn( true );
    m_mockWinApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_mockWinApiWrapper->MakeGetExitCodeProcessReturn( TRUE );
    m_mockWinApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_mockWinApiWrapper->MakeGetLastErrorReturn( 0 );

    int32_t ret = m_patient->DeployConfiguration( c );

    EXPECT_EQ( ret, 0 );
}

TEST_F( TestWindowsPackageManager, GetInstalledPackagesSucceed )
{
    PackageInventory installedPackages;
    std::vector<PmDiscoveryComponent> discoveryList;

    int32_t ret = m_patient->GetInstalledPackages( discoveryList, installedPackages );

    EXPECT_EQ( ret, 0 );
}

TEST_F( TestWindowsPackageManager, GetInstalledPackagesWillSearchForPackages )
{
    PackageInventory installedPackages;
    std::vector<PmDiscoveryComponent> discoveryList;

    EXPECT_CALL( *m_mockPackageDiscovery, GetInstalledPackages( _ ) );

    int32_t ret = m_patient->GetInstalledPackages( discoveryList, installedPackages );
}

TEST_F( TestWindowsPackageManager, WillResolveKnownFolderID )
{
    std::string knownFolderString = "_My_KNOWN_FOLDER_";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeResolveKnownFolderIdReturn( knownFolderString );

    std::string rtn = m_patient->ResolvePath( "<FOLDERID_SomeKnownFolder>" );
    EXPECT_EQ( rtn, knownFolderString);
}

TEST_F( TestWindowsPackageManager, WillResolveKnownFolderIDWithPrefix )
{
    std::string prefix = "prefix";
    std::string knownFolderString = "_My_KNOWN_FOLDER_";
    std::string suffix = "suffix";

    MockWindowsUtilities::GetMockWindowUtilities()->MakeResolveKnownFolderIdReturn( knownFolderString );

    std::string rtn = m_patient->ResolvePath( prefix + "<FOLDERID_SomeKnownFolder>" + suffix );
    EXPECT_EQ( rtn, prefix + knownFolderString + suffix );
}

TEST_F( TestWindowsPackageManager, WillNotModifyPathWhenKnownFolderIsEmpty )
{
    std::string folder = "prefix<FOLDERID_SomeKnownFolder>suffix";

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ResolveKnownFolderId( _ ) ).WillOnce( Return( "" ) );

    std::string rtn = m_patient->ResolvePath( folder );

    EXPECT_EQ( rtn, folder );
}


TEST_F( TestWindowsPackageManager, WillNotResolveKnownFolderWhenTagNotFound )
{
    MockWindowsUtilities::GetMockWindowUtilities()->ExpectResolveKnownFolderIdIsNotCalled();

    m_patient->ResolvePath( "C:\\Windows\\Somthing");
}
