#pragma once

#include "gtest/gtest.h"
#include <memory>

#include "PackageManager.h"
#include "PackageInventoryProvider.h"
#include "PackageDiscoveryManager.h"
#include "CheckinFormatter.h"
#include "UcidAdapter.h"
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
#include "MockPmBootstrap.h"
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
#include "MockWatchdog.h"
#include "MockCertsAdapter.h"
#include "MockProxyConsumer.h"
#include "MockProxyDiscovery.h"
#include "MockPmHttp.h"
#include "PmProxyVerifier.h"
#include "PmProxyDiscoverySubscriber.h"
#include "IProxyConsumer.h"
#include "IProxyDiscoveryEngine.h"
#include "curl.h"

MATCHER_P( CloudEventBuilderMatch, expected, "" )
{
    return ( CloudEventBuilder& )arg == ( CloudEventBuilder& )expected;
}

class ComponentTestPacManBase : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_configUrl = "https://test.com";
        m_configIntervalCalledOnce = false;

        m_mockFileUtil.reset( new NiceMock<MockFileSysUtil>() );
        m_mockBootstrap.reset( new NiceMock<MockPmBootstrap>() );
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
        m_certsAdapter.reset( new NiceMock<MockCertsAdapter>() );

        m_eventBuilder.reset( new CloudEventBuilder() );
        m_eventPublisher.reset( new NiceMock<MockCloudEventPublisher>() );
        m_eventStorage.reset( new NiceMock<MockCloudEventStorage>() );
        m_ucUpgradeEventHandler.reset( new NiceMock<MockUcUpgradeEventHandler>() );

        m_checkinManifestRetriever.reset( new CheckinManifestRetriever( *m_mockCloud, *m_ucidAdapter, *m_certsAdapter, *m_mockConfig ) );
        m_catalogListRetriever.reset( new CatalogListRetriever( *m_mockCloud, *m_ucidAdapter, *m_certsAdapter, *m_mockConfig ) );
        m_catalogJsonParser.reset( new CatalogJsonParser() );
        m_packageDiscoveryManager.reset(
            new PackageDiscoveryManager( *m_catalogListRetriever, *m_packageInventoryProvider, *m_catalogJsonParser )
        );
        m_configProcesor.reset( new PackageConfigProcessor( *m_mockFileUtil, *m_mockSslUtil, *m_ucidAdapter, *m_eventBuilder, *m_eventPublisher ) );
        m_watchdog.reset( new NiceMock<MockWatchdog>() );
        m_componentPackageProcessor.reset( new ComponentPackageProcessor(
            *m_mockInstallerCacheMgr,
            *m_mockFileUtil,
            *m_mockSslUtil,
            *m_configProcesor,
            *m_ucidAdapter,
            *m_eventBuilder,
            *m_eventPublisher,
            *m_ucUpgradeEventHandler,
            *m_watchdog ) );
        m_manifestProcessor.reset( new ManifestProcessor( *m_manifest, *m_componentPackageProcessor ) );

        m_mockDeps->MakeConfigurationReturn( *m_mockPlatformConfiguration );
        m_mockDeps->MakeComponentManagerReturn( *m_mockPlatformComponentManager );
        ON_CALL( *m_mockPlatformConfiguration, GetIdentityToken( _ ) ).WillByDefault( DoAll( SetArgReferee<0>( "token" ), Return( true ) ) );
        ON_CALL( *m_mockConfig, GetCloudCheckinIntervalMs ).WillByDefault( Invoke( this, &ComponentTestPacManBase::GetCloudCheckinIntervalMs ) );
        ON_CALL( *m_mockPlatformComponentManager, ResolvePath( _ ) ).WillByDefault( Invoke(
            []( const std::string& basePath )
            {
                return basePath;
            }
        ) );

        m_http.reset( new NiceMock<MockPmHttp>() );
        m_httpForProxyTesting.reset( new NiceMock<MockPmHttp>() );

        m_proxyVerifier.reset( new PmProxyVerifier( *m_httpForProxyTesting, *m_mockConfig ) );
        m_proxyDiscoverySubscriber.reset( new PmProxyDiscoverySubscriber( *m_http, *m_proxyVerifier ) );

        ProxyInfoModel testProxy = {
            8080, //m_proxyPort
            0, //m_proxyAuthType
            0, //m_tunnel
            PROXY_FIND_REG, //m_proxyDiscoveryMode
            L"http_proxy", //m_proxyType
            L"proxyServer", //"m_proxyServer
            L"proxyUser", //m_proxyUname
            L"proxyPass", //m_proxyPassword
            L"http_proxy" //m_accessType
        };
        m_proxyList.push_back( testProxy );

        m_proxyDiscovery.reset( new NiceMock<MockProxyDiscovery>() );

        m_patient.reset( new PackageManager(
            *m_mockBootstrap,
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
            *m_thread,
            *m_watchdog,
            *m_proxyDiscoverySubscriber,
            *m_proxyDiscovery ) );
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
        m_mockBootstrap.reset();
        m_mockFileUtil.reset();

        m_proxyDiscovery.reset();
        m_proxyDiscoverySubscriber.reset();
        m_proxyVerifier.reset();
        m_http.reset();
        m_httpForProxyTesting.reset();
        m_mockConfig.reset();
        m_proxyList.clear();
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

    void SetupPacMacn()
    {
        m_mockPlatformConfiguration->MakeGetSslCertificatesReturn( 0 );
        m_mockConfig->MakeLoadPmConfigReturn( 0 );
        m_mockConfig->MakeGetCloudCheckinUriReturn( m_configUrl );
        m_mockConfig->MakeGetCloudEventUriReturn( m_configUrl );
        m_mockConfig->MakeGetCloudCatalogUriReturn( m_configUrl );
        m_mockConfig->MakeAllowPostInstallRebootsReturn( true );
        m_mockCloud->MakeGetReturn( true, "content", { 200, 0 } );
        m_mockInstallerCacheMgr->MakeDownloadOrUpdateInstallerReturn( "InstallerDownloadLocation" );

        m_patient->SetPlatformDependencies( m_mockDeps.get() );
    }

    void SetupPacMacNoConfig()
    {
        m_mockPlatformConfiguration->MakeGetSslCertificatesReturn( 0 );
        m_mockConfig->MakeLoadPmConfigReturn( 0 );
        m_mockConfig->MakeAllowPostInstallRebootsReturn( true );
        m_mockCloud->MakeGetReturn( true, "content", { 200, 0 } );
        m_mockInstallerCacheMgr->MakeDownloadOrUpdateInstallerReturn( "InstallerDownloadLocation" );

        m_patient->SetPlatformDependencies( m_mockDeps.get() );
    }

    void StartPacMan()
    {
        SetupPacMacn();
        m_patient->Start( "ConfigFile", "BootstrapFile" );
    }

    void StartPacManNoConfig()
    {
        SetupPacMacNoConfig();
        m_patient->Start( "ConfigFile", "BootstrapFile" );
    }

    void PublishedEventHasExpectedData(
        std::string ucid,
        CloudEventType evtype,
        std::string packageNameAndVersion,
        int errCode, std::string errMessage,
        int subErrCode, std::string subErrType,
        std::string oldPath, std::string oldHash, int oldSize,
        std::string newPath, std::string newHash, int newSize )
    {
        CloudEventBuilder expectedEventData {};
        expectedEventData.WithUCID( ucid );
        expectedEventData.WithPackageID( "uc/0.0.1" );
        expectedEventData.WithType( evtype );
        expectedEventData.WithError( errCode, errMessage );
        expectedEventData.WithSubError( subErrCode, subErrType );
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
    std::unique_ptr<MockPmBootstrap> m_mockBootstrap;
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
    std::unique_ptr<MockPmHttp> m_http;
    std::unique_ptr<MockPmHttp> m_httpForProxyTesting;
    std::unique_ptr<IProxyConsumer> m_proxyDiscoverySubscriber;
    std::unique_ptr<MockProxyDiscovery> m_proxyDiscovery;
    std::unique_ptr<IProxyVerifier> m_proxyVerifier;
    std::unique_ptr<IPackageInventoryProvider> m_packageInventoryProvider;
    std::unique_ptr<IPackageDiscoveryManager> m_packageDiscoveryManager;
    std::unique_ptr<ICheckinFormatter> m_checkinFormatter;
    std::unique_ptr<IUcidAdapter> m_ucidAdapter;
    std::unique_ptr<MockCertsAdapter> m_certsAdapter;
    std::unique_ptr<ICheckinManifestRetriever> m_checkinManifestRetriever;
    std::unique_ptr<ICatalogJsonParser> m_catalogJsonParser;
    std::unique_ptr<ICatalogListRetriever> m_catalogListRetriever;
    std::unique_ptr<CloudEventBuilder> m_eventBuilder;
    std::unique_ptr<MockCloudEventPublisher> m_eventPublisher;
    std::unique_ptr<MockCloudEventStorage> m_eventStorage;
    std::unique_ptr<MockUcUpgradeEventHandler> m_ucUpgradeEventHandler;

    std::unique_ptr<MockWatchdog> m_watchdog;
    std::unique_ptr<IComponentPackageProcessor> m_componentPackageProcessor;
    std::unique_ptr<IManifestProcessor> m_manifestProcessor;
    std::unique_ptr<IPackageConfigProcessor> m_configProcesor;
    PROXY_INFO_LIST m_proxyList;

    std::unique_ptr<IPackageManager> m_patient;
};
