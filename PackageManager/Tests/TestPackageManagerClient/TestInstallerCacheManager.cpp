#include "gtest/gtest.h"
#include "InstallerCacheManager.h"
#include "MockPmCloud.h"
#include "MockFileSysUtil.h"
#include "MockSslUtil.h"
#include "MockPmPlatformDependencies.h"
#include "MockPmPlatformComponentManager.h"
#include "PmTypes.h"
#include "PackageException.h"
#include "CustomPathMatchers.h"

#include <memory>

class TestInstallerCacheManager : public ::testing::Test
{
protected:
    void SetUp()
    {
        SetupComponentPackage();

        m_cloud.reset( new NiceMock<MockPmCloud>() );
        m_fileUtil.reset( new NiceMock<MockFileSysUtil>() );
        m_sslUtil.reset( new NiceMock<MockSslUtil>() );

        m_componentMgr.reset( new NiceMock<MockPmPlatformComponentManager>() );
        m_deps.reset( new NiceMock<MockPmPlatformDependencies>() );

        m_deps->MakeComponentManagerReturn( *m_componentMgr );

        m_patient.reset( new InstallerCacheManager( *m_cloud, *m_fileUtil, *m_sslUtil ) );
    }

    void TearDown()
    {
        m_patient.reset();

        m_deps.reset();
        m_componentMgr.reset();

        m_cloud.reset();
        m_fileUtil.reset();
        m_sslUtil.reset();
    }

    void SetupComponentPackage()
    {
        m_component = {
            "test/1.0.0",
            "installerUrl",
            "msi",
            "installerArgs",
            "installLocation",
            "signerName",
            "installerHash",
            "downloadedInstallerPath",
            "", //downloadErrorMsg
            {}, //downloadSubError
            false,
            {}
        };
    }

    PmComponent m_component;

    std::unique_ptr<MockPmCloud> m_cloud;
    std::unique_ptr<MockFileSysUtil> m_fileUtil;
    std::unique_ptr<MockSslUtil> m_sslUtil;
    std::unique_ptr<MockPmPlatformComponentManager> m_componentMgr;
    std::unique_ptr<MockPmPlatformDependencies> m_deps;

    std::unique_ptr<InstallerCacheManager> m_patient;
};

TEST_F( TestInstallerCacheManager, CanDeleteInstaller )
{
    std::filesystem::path installerPath = "SomePath";

    m_fileUtil->MakeFileExistsReturn( true );
    EXPECT_CALL( *m_fileUtil, EraseFile( installerPath ) );

    m_patient->DeleteInstaller( installerPath );
}

TEST_F( TestInstallerCacheManager, WontDeleteEmptyInstaller )
{
    std::string installerPath = "";

    m_fileUtil->ExpectEraseFileNotCalled();

    m_patient->DeleteInstaller( installerPath );
}

TEST_F( TestInstallerCacheManager, WontDeleteNonExistantInstaller )
{
    std::string installerPath = "SomePath";

    m_fileUtil->MakeFileExistsReturn( false );
    m_fileUtil->ExpectEraseFileNotCalled();

    m_patient->DeleteInstaller( installerPath );
}

TEST_F( TestInstallerCacheManager, QaScenarioDeleteInstallerWhenHashIsEmpty )
{
    m_component.installerHash = "";

    m_cloud->MakeDownloadFileReturn( true, { 200, 0 } );
    m_fileUtil->MakeFileExistsReturn( true );
    EXPECT_CALL( *m_fileUtil, EraseFile( _ ) );

    m_patient->DownloadOrUpdateInstaller( m_component );
}

TEST_F( TestInstallerCacheManager, QaScenarioWillSanitizeDownloadPath )
{
    m_component.installerHash = "";

    m_cloud->MakeDownloadFileReturn( true, { 200, 0 } );
    m_fileUtil->MakeFileExistsReturn( true );
    EXPECT_CALL( *m_cloud, DownloadFile( _, PathContains( "test-1.0.0" ), _ ) );

    m_patient->DownloadOrUpdateInstaller( m_component );
}

TEST_F( TestInstallerCacheManager, QaScenarioCanDownloadSuccessfully )
{
    m_component.installerHash = "";

    m_cloud->MakeDownloadFileReturn( true, { 200, 0 } );
    m_fileUtil->MakeFileExistsReturn( true );

    EXPECT_FALSE( m_patient->DownloadOrUpdateInstaller( m_component ).empty() );
}

TEST_F( TestInstallerCacheManager, WillNotDownloadInstallerIfValidInstallerExists )
{
    std::optional<std::string> sha;
    sha.emplace( m_component.installerHash );

    ON_CALL( *m_fileUtil, FileExists( PathContains( m_component.installerHash ) ) ).WillByDefault( Return( true ) );
    ON_CALL( *m_sslUtil, CalculateSHA256( PathContains( m_component.installerHash ) ) ).WillByDefault( Return( sha ) );

    m_cloud->ExpectDownloadFileIsNotCalled();

    m_patient->DownloadOrUpdateInstaller( m_component );
}

TEST_F( TestInstallerCacheManager, WillDeleteInstallerIfValidationFails )
{
    std::optional<std::string> invalidSha;
    invalidSha.emplace( "Invalid Sha" );

    ON_CALL( *m_fileUtil, FileExists( PathContains( m_component.installerHash ) ) ).WillByDefault( Return( true ) );
    ON_CALL( *m_sslUtil, CalculateSHA256( PathContains( m_component.installerHash ) ) ).WillByDefault( Return( invalidSha ) );

    EXPECT_CALL( *m_fileUtil, EraseFile( PathContains( m_component.installerHash ) ) );

    //Expected but uninteresting exception
    EXPECT_THROW( m_patient->DownloadOrUpdateInstaller( m_component ), PackageException );
}

TEST_F( TestInstallerCacheManager, WillDeleteInstallerIfDownloadFails )
{
    std::optional<std::string> invalidSha;
    invalidSha.emplace( "Invalid Sha" );

    m_cloud->MakeDownloadFileReturn( true, { 200, 0 } );
    EXPECT_CALL( *m_fileUtil, FileExists( PathContains( m_component.installerHash ) ) )
        .WillOnce( Return( false ) )
        .WillOnce( Return( false ) )
        .WillOnce( Return( true ) )
        .WillOnce( Return( true ) );
    ON_CALL( *m_sslUtil, CalculateSHA256( PathContains( m_component.installerHash ) ) ).WillByDefault( Return( invalidSha ) );

    EXPECT_CALL( *m_fileUtil, EraseFile( PathContains( m_component.installerHash ) ) );

    //Expected but uninteresting exception
    EXPECT_THROW( m_patient->DownloadOrUpdateInstaller( m_component ), PackageException );
}

TEST_F( TestInstallerCacheManager, CanDownloadInstallerSuccessfully )
{
    std::optional<std::string> sha;
    sha.emplace( m_component.installerHash );
    m_cloud->MakeDownloadFileReturn( true, { 200, 0 } );

    EXPECT_CALL( *m_fileUtil, FileExists( PathContains( m_component.installerHash ) ) )
        .WillOnce( Return( false ) )
        .WillOnce( Return( false ) )
        .WillOnce( Return( true ) );
    ON_CALL( *m_sslUtil, CalculateSHA256( PathContains( m_component.installerHash ) ) ).WillByDefault( Return( sha ) );

    EXPECT_CALL( *m_cloud, DownloadFile( _, PathContains( m_component.installerHash ), _ ) );

    m_patient->DownloadOrUpdateInstaller( m_component );
}
TEST_F( TestInstallerCacheManager, DownloadingSuccessfullyWillReturnPath )
{
    std::optional<std::string> sha;
    sha.emplace( m_component.installerHash );

    m_cloud->MakeDownloadFileReturn( true, { 200, 0 } );
    EXPECT_CALL( *m_fileUtil, FileExists( PathContains( m_component.installerHash ) ) )
        .WillOnce( Return( false ) )
        .WillOnce( Return( false ) )
        .WillOnce( Return( true ) );
    ON_CALL( *m_sslUtil, CalculateSHA256( PathContains( m_component.installerHash ) ) ).WillByDefault( Return( sha ) );

    EXPECT_FALSE( m_patient->DownloadOrUpdateInstaller( m_component ).empty() );
}

TEST_F( TestInstallerCacheManager, PruneInstallersCannotBeRunUninitialied )
{
    m_componentMgr->ExpectFileSearchWithWildCardNotCalled();

    EXPECT_NO_THROW( m_patient->PruneInstallers( 0 ) );
}

TEST_F( TestInstallerCacheManager, PruneInstallersWillDeleteFile )
{
    uint32_t age = 100;
    uint32_t lastWriteTime = ( uint32_t )time( NULL ) - ( age * 2 );
    std::vector<std::filesystem::path> searchResults = { "Path1.exe" };

    m_patient->Initialize( m_deps.get() );
    ON_CALL( *m_componentMgr, FileSearchWithWildCard ).WillByDefault( DoAll(
        SetArgReferee<1>( searchResults ),
        Return( 0 )
    ) );
    m_fileUtil->MakeLastWriteTimeReturn( lastWriteTime );

    EXPECT_CALL( *m_fileUtil, EraseFile( searchResults[ 0 ] ) );

    m_patient->PruneInstallers( age );
}

TEST_F( TestInstallerCacheManager, PruneInstallersWillIgnoreNonExpiredFile )
{
    uint32_t age = 100;
    uint32_t lastWriteTime = ( uint32_t )time( NULL ) - ( age / 2 );
    std::vector<std::filesystem::path> searchResults = { "Path1.exe" };

    m_patient->Initialize( m_deps.get() );
    ON_CALL( *m_componentMgr, FileSearchWithWildCard ).WillByDefault( DoAll(
        SetArgReferee<1>( searchResults ),
        Return( 0 )
    ) );
    m_fileUtil->MakeLastWriteTimeReturn( lastWriteTime );

    m_fileUtil->ExpectEraseFileNotCalled();

    m_patient->PruneInstallers( age );
}

TEST_F( TestInstallerCacheManager, PruneInstallersWillDeleteMultipleFile )
{
    uint32_t age = 100;
    uint32_t lastWriteTime = ( uint32_t )time( NULL ) - ( age * 2 );
    std::vector<std::filesystem::path> searchResults = { "Path1.exe", "Path2.exe", "Path3.exe" };

    m_patient->Initialize( m_deps.get() );
    ON_CALL( *m_componentMgr, FileSearchWithWildCard ).WillByDefault( DoAll(
        SetArgReferee<1>( searchResults ),
        Return( 0 )
    ) );
    m_fileUtil->MakeLastWriteTimeReturn( lastWriteTime );

    EXPECT_CALL( *m_fileUtil, EraseFile( _ ) ).Times( ( int )searchResults.size() );

    m_patient->PruneInstallers( age );
}

TEST_F( TestInstallerCacheManager, ResultCode200WillNotThrow )
{
    m_component.installerHash = "";

    m_cloud->MakeDownloadFileReturn( true, { 200, 0 } );
    m_fileUtil->MakeFileExistsReturn( true );

    EXPECT_NO_THROW( m_patient->DownloadOrUpdateInstaller( m_component ) );
}

TEST_F( TestInstallerCacheManager, ResultCodeOtherThan200WillThrow )
{
    m_component.installerHash = "";

    m_cloud->MakeDownloadFileReturn( false, { 400, 0 } );
    m_fileUtil->MakeFileExistsReturn( true );

    EXPECT_THROW( m_patient->DownloadOrUpdateInstaller( m_component ), PackageException );
}

TEST_F( TestInstallerCacheManager, ResultCodeLessThan200WillBeReportedAsGenericErrorInExceptionMessage )
{
    m_component.installerHash = "";

    m_cloud->MakeDownloadFileReturn( false, { 0, 0 } );
    m_fileUtil->MakeFileExistsReturn( true );

    std::string errorMessage = "";
    try
    {
        m_patient->DownloadOrUpdateInstaller( m_component );
    }
    catch( PackageException& ex )
    {
        errorMessage = ex.what();
    }

    EXPECT_TRUE( errorMessage.find( "Request timed out or unknown error." ) != std::string::npos );
}

TEST_F( TestInstallerCacheManager, ResultCodeOver200WillBeReportedAsHttpErrorInExceptionMessage )
{
    m_component.installerHash = "";

    m_cloud->MakeDownloadFileReturn( false, { 300, 123, "subtype" } );
    m_fileUtil->MakeFileExistsReturn( true );

    std::string errorMessage = "";
    PmHttpExtendedResult extendedError = {};
    try
    {
        m_patient->DownloadOrUpdateInstaller( m_component );
    }
    catch( PackageException& ex )
    {
        errorMessage = ex.what();
        extendedError = ex.whatSubError();
    }

    EXPECT_TRUE( errorMessage.find( "HTTP response: 300" ) != std::string::npos );
    EXPECT_EQ( 123, extendedError.subErrorCode );
    EXPECT_STREQ( "subtype", extendedError.subErrorType.c_str() );
}

