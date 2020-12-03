#include "gtest/gtest.h"
#include "ComponentPackageProcessor.h"
#include "MockPmCloud.h"
#include "MockFileUtil.h"
#include "MockPmPlatformDependencies.h"
#include "MockPmPlatformComponentManager.h"
#include "MockSslUtil.h"
#include "MockPackageConfigProcessor.h"
#include "MockCloudEventBuilder.h"
#include "MockCloudEventPublisher.h"
#include "MockUcidAdapter.h"

#include <memory>

class TestComponentPackageProcessor : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_cloud.reset( new NiceMock<MockPmCloud>() );
        m_fileUtil.reset( new NiceMock<MockFileUtil>() );
        m_pmComponentManager.reset( new NiceMock<MockPmPlatformComponentManager>() );
        m_dep.reset( new NiceMock<MockPmPlatformDependencies>() );
        m_sslUtil.reset( new NiceMock<MockSslUtil>() );
        m_configProcessor.reset( new NiceMock<MockPackageConfigProcessor>() );
        m_ucidAdapter.reset( new NiceMock<MockUcidAdapter>() );
        m_eventBuilder.reset( new NiceMock<MockCloudEventBuilder>() );
        m_eventPublisher.reset( new NiceMock<MockCloudEventPublisher>() );

        m_patient.reset( new ComponentPackageProcessor( *m_cloud, *m_fileUtil, *m_sslUtil, *m_configProcessor, *m_ucidAdapter, *m_eventBuilder, *m_eventPublisher ) );

        m_dep->MakeComponentManagerReturn( *m_pmComponentManager );
    }

    void TearDown()
    {
        m_patient.reset();

        m_cloud.reset();
        m_fileUtil.reset();
        m_dep.reset();
        m_pmComponentManager.reset();
        m_sslUtil.reset();
        m_configProcessor.reset();
        m_ucidAdapter.reset();
        m_eventBuilder.reset();
        m_eventPublisher.reset();

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

    void SetupComponentPackageWithConfig()
    {
        SetupComponentPackage();
        m_expectedComponentPackage.installerHash = "";
        m_patient->Initialize( m_dep.get() );
        m_cloud->MakeDownloadFileReturn( 200 );
        m_sslUtil->MakeCalculateSHA256Return( "installerHash" );
        m_pmComponentManager->MakeUpdateComponentReturn( 0 );
    }

    PmComponent m_expectedComponentPackage;
    std::unique_ptr<MockPmCloud> m_cloud;
    std::unique_ptr<MockFileUtil> m_fileUtil;
    std::unique_ptr<MockPmPlatformComponentManager> m_pmComponentManager;
    std::unique_ptr<MockPmPlatformDependencies> m_dep;
    std::unique_ptr<MockSslUtil> m_sslUtil;
    std::unique_ptr<MockPackageConfigProcessor> m_configProcessor;
    std::unique_ptr<MockUcidAdapter> m_ucidAdapter;
    std::unique_ptr<MockCloudEventBuilder> m_eventBuilder;
    std::unique_ptr<MockCloudEventPublisher> m_eventPublisher;

    std::unique_ptr<ComponentPackageProcessor> m_patient;
};

TEST_F( TestComponentPackageProcessor, WillTryToDownloadIfInitialized )
{
    SetupComponentPackage();
    m_patient->Initialize( m_dep.get() );

    EXPECT_CALL( *m_cloud, DownloadFile( m_expectedComponentPackage.installerUrl, _ ) ).Times( 1 );

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillUpdateWhenDownloadIsSuccesful )
{
    SetupComponentPackageWithConfig();

    EXPECT_CALL( *m_pmComponentManager, UpdateComponent( _, _ ) );

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillRemoveFileWhenDownloadIsSuccesful )
{
    SetupComponentPackage();
    m_patient->Initialize( m_dep.get() );

    m_cloud->MakeDownloadFileReturn( 200 );
    EXPECT_CALL( *m_fileUtil, DeleteFile( _ ) );

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillNotProcessComponentPackageIfNotInitialized )
{
    SetupComponentPackage();
    m_cloud->ExpectDownloadFileIsNotCalled();
    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillProcessConfig )
{
    SetupComponentPackageWithConfig();

    EXPECT_CALL( *m_configProcessor, ProcessConfig( _ ) );

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillSendSuccessEventIfProcessComponentPackageSucceeds )
{
    SetupComponentPackageWithConfig();

    EXPECT_CALL( *m_eventBuilder, WithError( _, _ ) ).Times( 0 );
    EXPECT_CALL( *m_eventPublisher, Publish( _ ) );

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillSendFailureEventIfProcessComponentPackageFails )
{
    SetupComponentPackageWithConfig();
    m_pmComponentManager->MakeUpdateComponentReturn( -1 );

    EXPECT_CALL( *m_eventBuilder, WithError( _, _ ) );
    EXPECT_CALL( *m_eventPublisher, Publish( _ ) );

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

