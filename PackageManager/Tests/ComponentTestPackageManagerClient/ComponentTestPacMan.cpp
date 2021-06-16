#include "gtest/gtest.h"
#include <memory>

#include "PackageManager.h"
#include "PackageInventoryProvider.h"
#include "PackageDiscoveryManager.h"
#include "CheckinFormatter.h"
#include "UcidAdapter.h"
#include "CertsAdapter.h"
#include "CheckinManifestRetriever.h"
#include "CatalogListRetriever.h"
#include "CatalogJsonParser.h"
#include "ComponentPackageProcessor.h"
#include "PackageConfigProcessor.h"
#include "ManifestProcessor.h"
#include "WorkerThread.h"
#include "PmManifest.h"
#include "PmConstants.h"
#include "SslUtil.h"

#include "MockFileSysUtil.h"
#include "MockPmConfig.h"
#include "MockPmCloud.h"
#include "MockPmPlatformConfiguration.h"
#include "MockPmPlatformComponentManager.h"
#include "MockPmPlatformDependencies.h"
#include "MockSslUtil.h"
#include "CloudEventBuilder.h"
#include "MockCloudEventPublisher.h"
#include "MockCloudEventStorage.h"
#include "MockUcUpgradeEventHandler.h"
#include "MockInstallerCacheManager.h"
#include "MockRebootHandler.h"
#include "CustomPathMatchers.h"

MATCHER_P( CloudEventBuilderMatch, expected, "" )
{
    return ( CloudEventBuilder& )arg == ( CloudEventBuilder& )expected;
}

class ComponentTestPacMan : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_configUrl = "https://test.com";
        m_configIntervalCalledOnce = false;

        m_mockFileUtil.reset( new NiceMock<MockFileSysUtil>() );
        m_mockConfig.reset( new NiceMock<MockPmConfig>() );
        m_mockCloud.reset( new NiceMock<MockPmCloud>() );
        m_mockPlatformConfiguration.reset( new NiceMock<MockPmPlatformConfiguration>() );
        m_mockPlatformComponentManager.reset( new NiceMock<MockPmPlatformComponentManager>() );
        m_mockDeps.reset( new NiceMock<MockPmPlatformDependencies>() );
        m_mockSslUtil.reset( new NiceMock<MockSslUtil>() );
        m_mockInstallerCacheMgr.reset( new NiceMock<MockInstallerCacheManager>() );
        m_mockRebootHandler.reset( new NiceMock<MockRebootHandler>() );

        m_manifest.reset( new PmManifest() );
        m_thread.reset( new WorkerThread() );
        m_packageInventoryProvider.reset( new PackageInventoryProvider( *m_mockFileUtil, *m_mockSslUtil ) );
        m_checkinFormatter.reset( new CheckinFormatter() );
        m_ucidAdapter.reset( new UcidAdapter() );
        m_certsAdapter.reset( new CertsAdapter() );

        m_eventBuilder.reset( new CloudEventBuilder() );
        m_eventPublisher.reset( new NiceMock<MockCloudEventPublisher>() );
        m_eventStorage.reset( new NiceMock<MockCloudEventStorage>() );
        m_ucUpgradeEventHandler.reset( new NiceMock<MockUcUpgradeEventHandler>() );

        m_checkinManifestRetriever.reset( new CheckinManifestRetriever( *m_mockCloud, *m_ucidAdapter, *m_certsAdapter ) );
        m_catalogListRetriever.reset( new CatalogListRetriever( *m_mockCloud, *m_ucidAdapter, *m_certsAdapter, *m_mockConfig ) );
        m_catalogJsonParser.reset( new CatalogJsonParser() );
        m_packageDiscoveryManager.reset( 
            new PackageDiscoveryManager( *m_catalogListRetriever, *m_packageInventoryProvider, *m_catalogJsonParser ) 
        );
        m_configProcesor.reset( new PackageConfigProcessor( *m_mockFileUtil, *m_mockSslUtil, *m_ucidAdapter, *m_eventBuilder, *m_eventPublisher ) );
        m_componentPackageProcessor.reset( new ComponentPackageProcessor(
            *m_mockInstallerCacheMgr,
            *m_mockFileUtil,
            *m_mockSslUtil,
            *m_configProcesor,
            *m_ucidAdapter,
            *m_eventBuilder,
            *m_eventPublisher,
            *m_ucUpgradeEventHandler ) );
        m_manifestProcessor.reset( new ManifestProcessor( *m_manifest, *m_componentPackageProcessor ) );

        m_mockDeps->MakeConfigurationReturn( *m_mockPlatformConfiguration );
        m_mockDeps->MakeComponentManagerReturn( *m_mockPlatformComponentManager );
        ON_CALL( *m_mockPlatformConfiguration, GetIdentityToken( _ ) ).WillByDefault( DoAll( SetArgReferee<0>( "token" ), Return( true ) ) );
        ON_CALL( *m_mockConfig, GetCloudCheckinIntervalMs ).WillByDefault( Invoke( this, &ComponentTestPacMan::GetCloudCheckinIntervalMs ) );
        ON_CALL( *m_mockPlatformComponentManager, ResolvePath( _ ) ).WillByDefault( Invoke(
            []( const std::string& basePath )
            {
                return basePath;
            }
        ) );

        m_patient.reset( new PackageManager(
            *m_mockConfig,
            *m_mockCloud,
            *m_mockInstallerCacheMgr,
            *m_packageDiscoveryManager,
            *m_checkinFormatter,
            *m_catalogJsonParser,
            *m_ucidAdapter,
            *m_certsAdapter,
            *m_checkinManifestRetriever,
            *m_manifestProcessor,
            *m_eventPublisher,
            *m_eventStorage,
            *m_ucUpgradeEventHandler,
            *m_mockRebootHandler,
            *m_thread ) );
    }

    void TearDown()
    {
        m_patient->Stop();
        m_patient.reset();

        m_ucUpgradeEventHandler.reset();
        m_manifestProcessor.reset();
        m_componentPackageProcessor.reset();
        m_configProcesor.reset();
        m_checkinManifestRetriever.reset();
        m_catalogListRetriever.reset();
        m_catalogJsonParser.reset();
        m_eventBuilder.reset();
        m_eventPublisher.reset();
        m_eventStorage.reset();
        m_ucUpgradeEventHandler.reset();
        m_certsAdapter.reset();
        m_ucidAdapter.reset();
        m_checkinFormatter.reset();
        m_packageInventoryProvider.reset();
        m_thread.reset();
        m_manifest.reset();
        m_mockInstallerCacheMgr.reset();
        m_mockRebootHandler.reset();
        m_mockSslUtil.reset();

        m_mockDeps.reset();
        m_mockPlatformComponentManager.reset();
        m_mockPlatformConfiguration.reset();
        m_mockCloud.reset();
        m_mockConfig.reset();
        m_mockFileUtil.reset();
    }

    uint32_t GetCloudCheckinIntervalMs()
    {
        std::unique_lock<std::mutex> lock( m_configMutex );

        // Addresses random failures. If config always returns 1 as the interval, then sometimes a second
        // run of the workflow thread is run and causes the expectations to fail
        uint32_t interval = 10000;
        if( !m_configIntervalCalledOnce ) {
            interval = 1;
            m_configIntervalCalledOnce = true;
        }

        return interval;
    }

    void StartPacMan()
    {
        m_mockPlatformConfiguration->MakeGetSslCertificatesReturn( 0 );
        m_mockConfig->MakeLoadBsConfigReturn( 0 );
        m_mockConfig->MakeLoadPmConfigReturn( 0 );
        m_mockConfig->MakeGetCloudCheckinUriReturn( m_configUrl );
        m_mockConfig->MakeGetCloudCatalogUriReturn( m_configUrl );
        m_mockConfig->MakeAllowPostInstallRebootsReturn( true );
        m_mockCloud->MakeGetReturn( 200 );
        m_mockInstallerCacheMgr->MakeDownloadOrUpdateInstallerReturn( "InstallerDownloadLocation" );

        m_patient->SetPlatformDependencies( m_mockDeps.get() );
        m_patient->Start( "ConfigFile", "ConfigFile" );
    }

    void PublishedEventHasExpectedData(
        std::string ucid,
        CloudEventType evtype,
        std::string packageNameAndVersion,
        int errCode, std::string errMessage,
        std::string oldPath, std::string oldHash, int oldSize,
        std::string newPath, std::string newHash, int newSize )
    {
        CloudEventBuilder expectedEventData {};
        expectedEventData.WithUCID( ucid );
        expectedEventData.WithPackageID( "uc/0.0.1" );
        expectedEventData.WithType( evtype );
        expectedEventData.WithError( errCode, errMessage );
        expectedEventData.WithOldFile( oldPath, oldHash, oldSize );
        expectedEventData.WithNewFile( newPath, newHash, newSize );

        EXPECT_EQ( expectedEventData, *m_eventBuilder );
    }

    bool m_configIntervalCalledOnce;
    std::string m_configUrl;
    std::mutex m_mutex;
    std::mutex m_configMutex;
    std::condition_variable m_cv;

    std::unique_ptr<MockFileSysUtil> m_mockFileUtil;
    std::unique_ptr<MockPmConfig> m_mockConfig;
    std::unique_ptr<MockPmCloud> m_mockCloud;
    std::unique_ptr<MockPmPlatformConfiguration> m_mockPlatformConfiguration;
    std::unique_ptr<MockPmPlatformComponentManager> m_mockPlatformComponentManager;
    std::unique_ptr<MockPmPlatformDependencies> m_mockDeps;
    std::unique_ptr<MockSslUtil> m_mockSslUtil;
    std::unique_ptr<MockInstallerCacheManager> m_mockInstallerCacheMgr;
    std::unique_ptr<MockRebootHandler> m_mockRebootHandler;

    std::unique_ptr<IPmManifest> m_manifest;
    std::unique_ptr<IWorkerThread> m_thread;
    std::unique_ptr<IPackageInventoryProvider> m_packageInventoryProvider;
    std::unique_ptr<IPackageDiscoveryManager> m_packageDiscoveryManager;
    std::unique_ptr<ICheckinFormatter> m_checkinFormatter;
    std::unique_ptr<IUcidAdapter> m_ucidAdapter;
    std::unique_ptr<ICertsAdapter> m_certsAdapter;
    std::unique_ptr<ICheckinManifestRetriever> m_checkinManifestRetriever;
    std::unique_ptr<ICatalogJsonParser> m_catalogJsonParser;
    std::unique_ptr<ICatalogListRetriever> m_catalogListRetriever;
    std::unique_ptr<CloudEventBuilder> m_eventBuilder;
    std::unique_ptr<MockCloudEventPublisher> m_eventPublisher;
    std::unique_ptr<MockCloudEventStorage> m_eventStorage;
    std::unique_ptr<MockUcUpgradeEventHandler> m_ucUpgradeEventHandler;

    std::unique_ptr<IComponentPackageProcessor> m_componentPackageProcessor;
    std::unique_ptr<IManifestProcessor> m_manifestProcessor;
    std::unique_ptr<IPackageConfigProcessor> m_configProcesor;

    std::unique_ptr<IPackageManager> m_patient;
};

std::string _ucReponseNoConfig( R"(
{
  "packages": [
    {
      "installer_args": [
        "/S",
        "/Q"
      ],
      "install_location": "/install/location",
      "installer_signer_name": "Cisco Systems, Inc.",
      "installer_sha256": "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3",
      "installer_type": "msi",
      "installer_uri": "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi",
      "package": "uc/0.0.1"
    }
  ] 
}
)" );

TEST_F( ComponentTestPacMan, PacManWillUpdatePackage )
{
    bool pass = false;
    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).WillByDefault( DoAll( SetArgReferee<1>( _ucReponseNoConfig ), Return( 200 ) ) );

    m_mockFileUtil->MakeFileExistsReturn( true );
    m_mockFileUtil->MakeFileSizeReturn( 100 );
    m_mockFileUtil->MakeDeleteFileReturn( 0 );
    m_mockSslUtil->MakeCalculateSHA256Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" );

    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) ).WillOnce( Invoke(
        [this, &pass]( const PmComponent& package, std::string& error )
        {
            EXPECT_EQ( "/S /Q ", package.installerArgs );
            EXPECT_EQ( "/install/location", package.installLocation );
            EXPECT_EQ( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3", package.installerHash );
            EXPECT_EQ( "Cisco Systems, Inc.", package.signerName );
            EXPECT_EQ( "msi", package.installerType );
            EXPECT_EQ( "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi", package.installerUrl );
            EXPECT_EQ( "uc/0.0.1", package.productAndVersion );

            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    //this fails (matcher gets correct params but passes them wrongly to the == operator)
    //EXPECT_CALL( *m_eventPublisher, Publish( CloudEventBuilderMatch( m_eventBuilder.get() ) ) ).Times( 1 );
    
    EXPECT_CALL( *m_eventPublisher, Publish( _ ) ).Times( 1 );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );

    PublishedEventHasExpectedData(
        "",
        pkginstall,
        "uc/0.0.1",
        0,
        "",
        "",
        "",
        0,
        "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi",
        "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3",
        100
    );
}

TEST_F( ComponentTestPacMan, PacManWillRebootWhenPackageUpdateSetsRequiredFlag )
{
    bool pass = false;
    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).WillByDefault( DoAll( SetArgReferee<1>( _ucReponseNoConfig ), Return( 200 ) ) );

    m_mockFileUtil->MakeFileExistsReturn( true );
    m_mockFileUtil->MakeFileSizeReturn( 100 );
    m_mockFileUtil->MakeDeleteFileReturn( 0 );
    m_mockSslUtil->MakeCalculateSHA256Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" );

    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) ).WillOnce( Invoke(
        [this, &pass]( const PmComponent& package, std::string& error )
        {
            return ERROR_SUCCESS_REBOOT_REQUIRED;
        } ) );

    EXPECT_CALL( *m_mockRebootHandler, HandleReboot( _ ) ).WillOnce( Invoke(
        [this, &pass]( bool rebootRequired )
        {
            pass = true;
            m_cv.notify_one();

            return true;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );
}

TEST_F( ComponentTestPacMan, PacManWillSendRebootEventWhenRebootIsFlagged )
{
    bool pass = false;
    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).
        WillByDefault( DoAll( SetArgReferee<1>( _ucReponseNoConfig ), Return( 200 ) ) );

    m_mockFileUtil->MakeFileExistsReturn( true );
    m_mockFileUtil->MakeFileSizeReturn( 100 );
    m_mockFileUtil->MakeDeleteFileReturn( 0 );
    m_mockSslUtil->MakeCalculateSHA256Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" );

    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) ).WillOnce( Invoke(
        [this, &pass]( const PmComponent& package, std::string& error )
        {
            return ERROR_SUCCESS_REBOOT_REQUIRED;
        } ) );

    EXPECT_CALL( *m_mockRebootHandler, HandleReboot( _ ) ).WillOnce( Invoke(
        [this, &pass]( bool rebootRequired )
        {
            pass = true;
            m_cv.notify_one();

            return true;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );

    PublishedEventHasExpectedData(
        "",
        pkginstall,
        "uc/0.0.1",
        UCPM_EVENT_SUCCESS_REBOOT_REQ,
        "Reboot required event",
        "",
        "",
        0,
        "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi",
        "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3",
        100
    );
}

std::string _ucReponseConfigOnly( R"(
{
  "packages": [
    {
      "package": "uc/0.0.1",
      "install_location": "/install/location",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "config.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
          "verify_path": "verify.exe"
        }
      ]
    }
  ]
}
)" );

std::string _decodedConfig( R"({
  "path": "config.json",
  "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
  "verify_path": "verify.exe",
})" );

TEST_F( ComponentTestPacMan, PacManWillDecodeConfig )
{
    bool pass = false;

    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).WillByDefault( DoAll( SetArgReferee<1>( _ucReponseConfigOnly ), Return( 200 ) ) );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );

    ON_CALL( *m_mockSslUtil, DecodeBase64( _, _ ) ).WillByDefault( Invoke(
        []( const std::string& base64Str, std::vector<uint8_t>& output )
        {
            SslUtil sslUtil;
            return sslUtil.DecodeBase64( base64Str, output );
        }
    ) );

    EXPECT_CALL( *m_mockFileUtil, AppendFile( _, _, _ ) ).WillOnce( Invoke(
        [this, &pass]( FileUtilHandle* handle, void* data, size_t dataLen )
        {
            std::string strData( ( char* )data, dataLen );
            EXPECT_EQ( _decodedConfig, strData );

            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );
}

TEST_F( ComponentTestPacMan, PacManWillVerifyConfig )
{
    bool pass = false;
    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).WillByDefault( DoAll( SetArgReferee<1>( _ucReponseConfigOnly ), Return( 200 ) ) );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockSslUtil->MakeCalculateSHA256Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" );

    EXPECT_CALL( *m_mockPlatformComponentManager, DeployConfiguration( _ ) ).WillOnce( Invoke(
        [this, &pass]( const PackageConfigInfo& config )
        {
            EXPECT_EQ( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4", config.sha256 );
            EXPECT_EQ( "verify.exe", config.verifyBinPath );
            EXPECT_NE( std::string::npos, config.verifyPath.find( "tmpPmConf_" ) );

            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );
}

TEST_F( ComponentTestPacMan, PacManWillMoveConfig )
{
    bool pass = false;
    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).WillByDefault( DoAll( SetArgReferee<1>( _ucReponseConfigOnly ), Return( 200 ) ) );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockPlatformComponentManager->MakeDeployConfigurationReturn( 0 );
    m_mockSslUtil->MakeCalculateSHA256Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" );

    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) ).WillOnce( Invoke(
        [this, &pass]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
        {
            EXPECT_EQ( std::filesystem::path( "/install/location/config.json" ), newName );
            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );
}

std::string _ucReponseConfigWithoutVerify( R"(
{
  "packages": [
    {
      "package": "uc/0.0.1",
      "install_location": "/install/location",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "config.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",         
        }
      ]
    }
  ]
}
)" );

TEST_F( ComponentTestPacMan, PacManWillMoveConfigWithoutVerification )
{
    bool pass = false;
    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).WillByDefault( DoAll( SetArgReferee<1>( _ucReponseConfigWithoutVerify ), Return( 200 ) ) );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockSslUtil->MakeCalculateSHA256Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" );
    m_mockFileUtil->MakeFileExistsReturn( true );
    m_mockPlatformComponentManager->ExpectDeployConfigurationIsNotCalled();
    
    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) ).WillOnce( Invoke(
        [this, &pass]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
        {
            EXPECT_EQ( std::filesystem::path( "/install/location/config.json" ), newName );
            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );
}

std::string _ucReponseWithConfig( R"(
{
  "packages": [
    {
      "installer_args": [
        "/S",
        "/Q"
      ],
      "install_location": "/install/location",
      "installer_signer_name": "Cisco Systems, Inc.",
      "installer_sha256": "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3",
      "installer_type": "msi",
      "installer_uri": "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi",
      "package": "uc/0.0.1",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "config.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
          "verify_path": "verify.exe"
        }
      ]
    }
  ]
}
)" );

TEST_F( ComponentTestPacMan, PacManWillUpdatePackageAndConfig )
{
    bool packageUpdated = false;
    bool configUpdated = false;
    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).WillByDefault( DoAll( SetArgReferee<1>( _ucReponseWithConfig ), Return( 200 ) ) );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockPlatformComponentManager->MakeDeployConfigurationReturn( 0 );

    ON_CALL( *m_mockFileUtil, FileExists( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( true ) );
    ON_CALL( *m_mockFileUtil, FileSize( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 100 ) );
    ON_CALL( *m_mockFileUtil, DeleteFile( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 0 ) );

    ON_CALL( *m_mockSslUtil, CalculateSHA256( HasSubstr( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" ) );
    ON_CALL( *m_mockSslUtil, CalculateSHA256( HasSubstr( "tmpPmConf_" ) ) ).WillByDefault( Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" ) );

    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) ).WillOnce( Invoke(
        [this, &packageUpdated]( const PmComponent& package, std::string& error )
        {
            EXPECT_EQ( "/S /Q ", package.installerArgs );
            EXPECT_EQ( std::filesystem::path( "/install/location" ), package.installLocation );
            EXPECT_EQ( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3", package.installerHash );
            EXPECT_EQ( "Cisco Systems, Inc.", package.signerName );
            EXPECT_EQ( "msi", package.installerType );
            EXPECT_EQ( "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi", package.installerUrl );
            EXPECT_EQ( "uc/0.0.1", package.productAndVersion );

            packageUpdated = true;
            return 0;
        } ) );

    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) ).WillOnce( Invoke(
        [this, &configUpdated]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
        {
            configUpdated = true;
            m_cv.notify_one();

            return 0;
        } ) );

    EXPECT_CALL( *m_eventPublisher, Publish( _ ) ).Times( 2 );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( packageUpdated && configUpdated );

    PublishedEventHasExpectedData(
        "",
        pkgreconfig,
        "uc/0.0.1",
        0,
        "",
        "",
        "",
        0,
        "config.json",
        "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",
        0
    );
}

std::string _ucReponseMultiPackageAndConfig( R"(
{
  "packages": [
    {
      "installer_args": [
        "/S",
        "/Q"
      ],
      "install_location": "/install/location",
      "installer_signer_name": "Cisco Systems, Inc.",
      "installer_sha256": "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3",
      "installer_type": "msi",
      "installer_uri": "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi",
      "package": "uc/0.0.1",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "p1_config1.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
          "verify_path": "verify.exe"
        },
        {
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "p1_config2.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
          "verify_path": "verify.exe"
        }
      ]
    },
    {
      "installer_args": [
        "/S",
        "/Q"
      ],
      "install_location": "/install/location",
      "installer_signer_name": "Cisco Systems, Inc.",
      "installer_sha256": "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3",
      "installer_type": "exe",
      "installer_uri": "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi",
      "package": "uc2/0.0.1",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "p2_config1.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
          "verify_path": "verify.exe"
        },
        {
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "p2_config2.json",
          "sha256": "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4",          
          "verify_path": "verify.exe"
        }
      ]
    }
  ]
}
)" );

TEST_F( ComponentTestPacMan, PacManWillUpdateMultiplePackageAndConfig )
{
    int packageUpdated = 0;
    int configUpdated = 0;
    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).WillByDefault( DoAll( SetArgReferee<1>( _ucReponseMultiPackageAndConfig ), Return( 200 ) ) );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockPlatformComponentManager->MakeDeployConfigurationReturn( 0 );

    ON_CALL( *m_mockFileUtil, FileExists( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( true ) );
    ON_CALL( *m_mockFileUtil, FileSize( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 100 ) );
    ON_CALL( *m_mockFileUtil, DeleteFile( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 0 ) );

    ON_CALL( *m_mockSslUtil, CalculateSHA256( HasSubstr( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" ) );
    ON_CALL( *m_mockSslUtil, CalculateSHA256( HasSubstr( "tmpPmConf_" ) ) ).WillByDefault( Return( "2927db35b1875ef3a426d05283609b2d95d429c091ee1a82f0671423a64d83a4" ) );

    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) )
        .WillOnce( Invoke(
            [this, &packageUpdated]( const PmComponent& package, std::string& error )
            {
                EXPECT_EQ( "/S /Q ", package.installerArgs );
                EXPECT_EQ( "/install/location", package.installLocation );
                EXPECT_EQ( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3", package.installerHash );
                EXPECT_EQ( "Cisco Systems, Inc.", package.signerName );
                EXPECT_EQ( "msi", package.installerType );
                EXPECT_EQ( "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi", package.installerUrl );
                EXPECT_EQ( "uc/0.0.1", package.productAndVersion );

                packageUpdated++;
                return 0;
            } ) )
        .WillOnce( Invoke(
            [this, &packageUpdated]( const PmComponent& package, std::string& error )
            {
                EXPECT_EQ( "/S /Q ", package.installerArgs );
                EXPECT_EQ( "/install/location", package.installLocation );
                EXPECT_EQ( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3", package.installerHash );
                EXPECT_EQ( "Cisco Systems, Inc.", package.signerName );
                EXPECT_EQ( "exe", package.installerType );
                EXPECT_EQ( "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi", package.installerUrl );
                EXPECT_EQ( "uc2/0.0.1", package.productAndVersion );

                packageUpdated++;
                return 0;
            } ) );
            EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) )
                .WillOnce( Invoke(
                    [this, &configUpdated]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
                    {
                        EXPECT_EQ( std::filesystem::path( "/install/location/p1_config1.json" ), newName );
                        configUpdated++;
                        return 0;
                    } ) )
                .WillOnce( Invoke(
                    [this, &configUpdated]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
                    {
                        EXPECT_EQ( std::filesystem::path( "/install/location/p1_config2.json" ), newName );
                        configUpdated++;
                        return 0;
                    } ) )
                        .WillOnce( Invoke(
                            [this, &configUpdated]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
                            {
                                EXPECT_EQ( std::filesystem::path( "/install/location/p2_config1.json" ), newName );
                                configUpdated++;
                                return 0;
                            } ) )
                        .WillOnce( Invoke(
                            [this, &configUpdated]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
                            {
                                EXPECT_EQ( std::filesystem::path(  "/install/location/p2_config2.json" ), newName );
                                configUpdated++;
                                m_cv.notify_one();

                                return 0;
                            } ) );


                            StartPacMan();

                            std::unique_lock<std::mutex> lock( m_mutex );
                            m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
                            lock.unlock();

                            EXPECT_EQ( 2, packageUpdated );
                            EXPECT_EQ( 4, configUpdated );
}

std::string _ucReponseWithConfigCloudData( R"(
{
  "packages": [
    {
      "installer_args": [
        "/S",
        "/Q"
      ],
      "installer_signer_name": "Cisco Systems, Inc.",
      "installer_sha256": "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3",
      "installer_type": "msi",
      "installer_uri": "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi",
      "package": "uc/0.0.1",
      "files": [
        {
          "contents": "ewogICJwYXRoIjogImNvbmZpZy5qc29uIiwKICAic2hhMjU2IjogIjI5MjdkYjM1YjE4NzVlZjNhNDI2ZDA1MjgzNjA5YjJkOTVkNDI5YzA5MWVlMWE4MmYwNjcxNDIzYTY0ZDgzYTQiLCAgICAgICAgICAKICAidmVyaWZ5X3BhdGgiOiAidmVyaWZ5LmV4ZSIsCn0=",
          "path": "C:/Program Files/Cisco/SecureClient/UnifiedConnector/Configuration/uc.json"
        }
      ]
    }
  ]
}
)" );

TEST_F( ComponentTestPacMan, PacManWillUpdatePackageAndConfigCloudData )
{
    bool packageUpdated = false;
    bool configUpdated = false;
    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).WillByDefault( DoAll( SetArgReferee<1>( _ucReponseWithConfigCloudData ), Return( 200 ) ) );
    m_mockFileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_mockFileUtil->MakeAppendFileReturn( 1 );
    m_mockPlatformComponentManager->MakeDeployConfigurationReturn( 0 );

    ON_CALL( *m_mockFileUtil, FileExists( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( true ) );
    ON_CALL( *m_mockFileUtil, FileSize( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 100 ) );
    ON_CALL( *m_mockFileUtil, DeleteFile( PathContains( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( 0 ) );

    ON_CALL( *m_mockSslUtil, CalculateSHA256( HasSubstr( "InstallerDownloadLocation" ) ) ).WillByDefault( Return( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3" ) );
    
    EXPECT_CALL( *m_mockPlatformComponentManager, UpdateComponent( _, _ ) ).WillOnce( Invoke(
        [this, &packageUpdated]( const PmComponent& package, std::string& error )
        {
            EXPECT_EQ( "/S /Q ", package.installerArgs );
            EXPECT_EQ( "ec9b9dc8cb017a5e0096f79e429efa924cc1bfb61ca177c1c04625c1a9d054c3", package.installerHash );
            EXPECT_EQ( "Cisco Systems, Inc.", package.signerName );
            EXPECT_EQ( "msi", package.installerType );
            EXPECT_EQ( "https://nexus.engine.sourcefire.com/repository/raw/UnifiedConnector/Windows/Pub/x64/uc-0.0.1-alpha.msi", package.installerUrl );
            EXPECT_EQ( "uc/0.0.1", package.productAndVersion );

            packageUpdated = true;
            return 0;
        } ) );
    EXPECT_CALL( *m_mockFileUtil, Rename( _, _ ) ).WillOnce( Invoke(
        [this, &configUpdated]( const std::filesystem::path& oldFilename, const std::filesystem::path& newName )
        {
            EXPECT_EQ( std::filesystem::path( "C:/Program Files/Cisco/SecureClient/UnifiedConnector/Configuration/uc.json" ), newName );
            configUpdated = true;
            m_cv.notify_one();

            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( packageUpdated && configUpdated );
}

std::string _ucReponseNoPackages( R"(
{
  "packages": null
}
)" );

TEST_F( ComponentTestPacMan, PacManWillSendDicoveryList )
{
    bool pass = false;
    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).WillByDefault( DoAll( SetArgReferee<1>( _ucReponseNoPackages ), Return( 200 ) ) );

    EXPECT_CALL( *m_mockPlatformComponentManager, GetInstalledPackages( _, _ ) )
        .WillOnce( Invoke(
        [this, &pass]( const std::vector<PmProductDiscoveryRules>& catalogRules, PackageInventory& packagesDiscovered )
        {
            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

//    std::this_thread::sleep_for( std::chrono::microseconds( 4000 ) );

    EXPECT_TRUE( pass );
}

TEST_F( ComponentTestPacMan, PacManWillSendFailedEvents )
{
    bool pass = false;
    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).WillByDefault( DoAll( SetArgReferee<1>( _ucReponseNoPackages ), Return( 200 ) ) );

    EXPECT_CALL( *m_eventPublisher, PublishFailedEvents() ).WillOnce( Invoke(
        [this, &pass]()
        {
            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );
}

TEST_F( ComponentTestPacMan, PacManWillPruneInstallers )
{
    bool pass = false;
    ON_CALL( *m_mockCloud, Checkin( _, _ ) ).WillByDefault( DoAll( SetArgReferee<1>( _ucReponseNoPackages ), Return( 200 ) ) );
    EXPECT_CALL( *m_mockInstallerCacheMgr, PruneInstallers( _ ) ).WillOnce( Invoke(
        [this, &pass]( uint32_t ageInSeconds )
        {
            pass = true;
            m_cv.notify_one();
            return 0;
        } ) );

    StartPacMan();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();

    EXPECT_TRUE( pass );
}
