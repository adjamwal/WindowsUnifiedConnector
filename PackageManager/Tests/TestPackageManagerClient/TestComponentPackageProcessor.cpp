#include "gtest/gtest.h"
#include "ComponentPackageProcessor.h"
#include "MockInstallerCacheManager.h"
#include "MockFileSysUtil.h"
#include "MockPmPlatformDependencies.h"
#include "MockPmPlatformComponentManager.h"
#include "MockSslUtil.h"
#include "MockPackageConfigProcessor.h"
#include "MockCloudEventBuilder.h"
#include "MockCloudEventPublisher.h"
#include "MockUcidAdapter.h"
#include "MockUcUpgradeEventHandler.h"
#include "MockWatchdog.h"
#include "WinError.h"

#include <memory>

class TestComponentPackageProcessor : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_installerCacheMgr.reset( new NiceMock<MockInstallerCacheManager>() );
        m_fileUtil.reset( new NiceMock<MockFileSysUtil>() );
        m_pmComponentManager.reset( new NiceMock<MockPmPlatformComponentManager>() );
        m_dep.reset( new NiceMock<MockPmPlatformDependencies>() );
        m_sslUtil.reset( new NiceMock<MockSslUtil>() );
        m_configProcessor.reset( new NiceMock<MockPackageConfigProcessor>() );
        m_ucidAdapter.reset( new NiceMock<MockUcidAdapter>() );
        m_eventBuilder.reset( new NiceMock<MockCloudEventBuilder>() );
        m_eventPublisher.reset( new NiceMock<MockCloudEventPublisher>() );
        m_ucUpgradeEventHandler.reset( new NiceMock<MockUcUpgradeEventHandler>() );
        m_watchdog.reset( new NiceMock<MockWatchdog>() );

        m_patient.reset( new ComponentPackageProcessor( *m_installerCacheMgr,
            *m_fileUtil,
            *m_sslUtil,
            *m_configProcessor,
            *m_ucidAdapter,
            *m_eventBuilder,
            *m_eventPublisher,
            *m_ucUpgradeEventHandler,
            *m_watchdog ) );

        m_dep->MakeComponentManagerReturn( *m_pmComponentManager );
    }

    void TearDown()
    {
        m_patient.reset();

        m_watchdog.reset();
        m_installerCacheMgr.reset();
        m_fileUtil.reset();
        m_dep.reset();
        m_pmComponentManager.reset();
        m_sslUtil.reset();
        m_configProcessor.reset();
        m_ucidAdapter.reset();
        m_eventBuilder.reset();
        m_eventPublisher.reset();
        m_ucUpgradeEventHandler.reset();

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
            "downloadedInstallerPath",
            "", //downloadErrorMsg
            false,
            {}
        };

        m_expectedComponentPackage.configs.push_back( {
            "configpath",
            "configpath",
            "configsha256",
            "configcontents",
            "configverifyBinPath",
            "configverifyPath",
            "installLocation",
            "signerName",
            "test/1.0.0",
            false,
            } );
    }

    void SetupComponentPackageWithConfig()
    {
        SetupComponentPackage();
        m_expectedComponentPackage.installerHash = "";
        m_patient->Initialize( m_dep.get() );
        m_sslUtil->MakeCalculateSHA256Return( "installerHash" );
        m_pmComponentManager->MakeUpdateComponentReturn( 0 );
        m_fileUtil->MakeFileSizeReturn( 1 );
        m_fileUtil->MakeFileExistsReturn( true );
    }

    void SetupComponentPackageWithMissingBinary()
    {
        SetupComponentPackage();
        m_expectedComponentPackage.installerHash = "";
        m_patient->Initialize( m_dep.get() );
        m_fileUtil->MakeFileSizeReturn( -1 );
        m_fileUtil->MakeFileExistsReturn( false );
    }

    void SetupInventoryCacheWithPackageVer( const std::string& pkgName, const std::string& pkgVer )
    {
        PackageInventory testCache = {};
        PmInstalledPackage testPackage = {};
        testPackage.product = pkgName;
        testPackage.version = pkgVer;
        testCache.packages.push_back( testPackage );
        m_pmComponentManager->MakeGetCachedInventoryReturn( 0, testCache );

        ON_CALL( *m_eventBuilder, GetPackageName() ).WillByDefault( Return( pkgName ) );
        ON_CALL( *m_eventBuilder, GetPackageVersion() ).WillByDefault( Return( pkgVer ) );
    }

    PmComponent m_expectedComponentPackage;
    std::unique_ptr<MockInstallerCacheManager> m_installerCacheMgr;
    std::unique_ptr<MockFileSysUtil> m_fileUtil;
    std::unique_ptr<MockPmPlatformComponentManager> m_pmComponentManager;
    std::unique_ptr<MockPmPlatformDependencies> m_dep;
    std::unique_ptr<MockSslUtil> m_sslUtil;
    std::unique_ptr<MockPackageConfigProcessor> m_configProcessor;
    std::unique_ptr<MockUcidAdapter> m_ucidAdapter;
    std::unique_ptr<MockCloudEventBuilder> m_eventBuilder;
    std::unique_ptr<MockCloudEventPublisher> m_eventPublisher;
    std::unique_ptr<MockUcUpgradeEventHandler> m_ucUpgradeEventHandler;
    std::unique_ptr<MockWatchdog> m_watchdog;

    std::unique_ptr<ComponentPackageProcessor> m_patient;
};

TEST_F( TestComponentPackageProcessor, DownloadPackageBinaryWillTryToDownload )
{
    SetupComponentPackageWithConfig();
    m_patient->Initialize( m_dep.get() );

    EXPECT_CALL( *m_installerCacheMgr, DownloadOrUpdateInstaller( _ ) ).Times( 1 );

    m_patient->DownloadPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillUpdateWhenDownloadIsSuccesful )
{
    SetupComponentPackageWithConfig();

    EXPECT_CALL( *m_pmComponentManager, UpdateComponent( _, _ ) );

    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillFlagForRebootWhenMsiReturnCode3010 )
{
    SetupComponentPackageWithConfig();

    m_pmComponentManager->MakeUpdateComponentReturn( ERROR_SUCCESS_REBOOT_REQUIRED );

    EXPECT_TRUE( m_patient->ProcessPackageBinary( m_expectedComponentPackage ) );
    EXPECT_TRUE( m_expectedComponentPackage.postInstallRebootRequired );
}

TEST_F( TestComponentPackageProcessor, WillFlagForRebootWhenMsiReturnCode3011 )
{
    SetupComponentPackageWithConfig();

    m_pmComponentManager->MakeUpdateComponentReturn( ERROR_SUCCESS_RESTART_REQUIRED );

    EXPECT_TRUE( m_patient->ProcessPackageBinary( m_expectedComponentPackage ) );
    EXPECT_TRUE( m_expectedComponentPackage.postInstallRebootRequired );
}

TEST_F( TestComponentPackageProcessor, WillSucceedWhenMsiReturnCode1641 )
{
    SetupComponentPackageWithConfig();

    m_pmComponentManager->MakeUpdateComponentReturn( ERROR_SUCCESS_REBOOT_INITIATED );

    EXPECT_TRUE( m_patient->ProcessPackageBinary( m_expectedComponentPackage ) );
    EXPECT_FALSE( m_expectedComponentPackage.postInstallRebootRequired );
}

TEST_F( TestComponentPackageProcessor, WillStoreUcUpgradeEvent )
{
    SetupComponentPackageWithConfig();

    EXPECT_CALL( *m_ucUpgradeEventHandler, StoreUcUpgradeEvent( _ ) );

    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillRemoveFileWhenInstallIsSuccesful )
{
    SetupComponentPackageWithConfig();
    m_patient->Initialize( m_dep.get() );

    EXPECT_CALL( *m_installerCacheMgr, DeleteInstaller( _ ) );

    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillNotRemoveFileWhenInstallFails )
{
    SetupComponentPackageWithConfig();
    m_patient->Initialize( m_dep.get() );

    m_pmComponentManager->MakeUpdateComponentReturn( 1 );
    m_installerCacheMgr->ExpectDeleteInstallerIsNotCalled();

    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillNotProcessComponentPackageIfNotInitialized )
{
    SetupComponentPackage();
    m_eventPublisher->ExpectSetTokenNotCalled();
    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillProcessConfig )
{
    SetupComponentPackageWithConfig();

    EXPECT_CALL( *m_configProcessor, ProcessConfig( _ ) );

    m_patient->ProcessConfigsForPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillSendSuccessEventIfProcessComponentPackageSucceeds )
{
    SetupComponentPackageWithConfig();

    EXPECT_CALL( *m_eventBuilder, WithError( _, _ ) ).Times( 0 );
    EXPECT_CALL( *m_eventPublisher, Publish( _ ) );

    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillSendFailureEventIfProcessComponentPackageFails )
{
    SetupComponentPackageWithConfig();
    m_pmComponentManager->MakeUpdateComponentReturn( -1 );

    EXPECT_CALL( *m_eventBuilder, WithError( _, _ ) );
    EXPECT_CALL( *m_eventPublisher, Publish( _ ) );

    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillSendFailureEventIfProcessComponentPackageIsMissingBinary )
{
    SetupComponentPackageWithMissingBinary();

    EXPECT_CALL( *m_eventBuilder, WithError( _, _ ) );
    EXPECT_CALL( *m_eventPublisher, Publish( _ ) );

    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, DownloadPackageBinaryWillCacheDownloadError )
{
    SetupComponentPackageWithMissingBinary();
    m_installerCacheMgr->MakeDownloadOrUpdateInstallerThrow( "http 1234", 1 );

    m_patient->DownloadPackageBinary( m_expectedComponentPackage );

    EXPECT_TRUE( m_expectedComponentPackage.downloadErrorMsg.find( "http 1234" ) != std::string::npos );
}

TEST_F( TestComponentPackageProcessor, WillSetEventFromFieldIfVersionedPackageFoundLocally )
{
    SetupComponentPackageWithConfig();
    SetupInventoryCacheWithPackageVer( "test", "1.0.0" );

    EXPECT_CALL( *m_eventBuilder, WithFrom( StrEq( "1.0.0" ) ) ).Times( 1 );

    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillSetEventFromFieldIfVersionWithRevFoundLocally )
{
    SetupComponentPackageWithConfig();
    SetupInventoryCacheWithPackageVer( "test", "1.0.0.1234" );

    EXPECT_CALL( *m_eventBuilder, WithFrom( StrEq( "1.0.0.1234" ) ) ).Times( 1 );

    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillSetEventFromFieldIfNonVersionedPackageFoundLocally )
{
    SetupComponentPackageWithConfig();
    SetupInventoryCacheWithPackageVer( "test", "" );

    EXPECT_CALL( *m_eventBuilder, WithFrom( StrEq( "") ) ).Times( 1 );

    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}


TEST_F( TestComponentPackageProcessor, WillNetSetEventFromField )
{
    SetupComponentPackageWithConfig();
    SetupInventoryCacheWithPackageVer( "NotTest", "1.0.0" );
    ON_CALL( *m_eventBuilder, GetPackageName() ).WillByDefault( Return( "test" ) );

    EXPECT_CALL( *m_eventBuilder, WithFrom( _ ) ).Times( 0 );

    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, DownloadPackageBinaryWillKickWatchdog )
{
    EXPECT_CALL( *m_watchdog, Kick() );

    m_patient->DownloadPackageBinary( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, ProcessPackageBinaryWillKickWatchdog )
{
    EXPECT_CALL( *m_watchdog, Kick() );

    m_patient->ProcessPackageBinary( m_expectedComponentPackage );
}