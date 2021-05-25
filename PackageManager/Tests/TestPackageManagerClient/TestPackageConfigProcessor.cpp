#include "MocksCommon.h"
#include "PackageConfigProcessor.h"
#include "MockFileSysUtil.h"
#include "MockPmPlatformDependencies.h"
#include "MockPmPlatformComponentManager.h"
#include "MockSslUtil.h"
#include "MockCloudEventBuilder.h"
#include "MockCloudEventPublisher.h"
#include "MockUcidAdapter.h"

#include <memory>

class TestPackageConfigProcessor : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_fileUtil.reset( new NiceMock<MockFileSysUtil>() );
        m_pmComponentManager.reset( new NiceMock<MockPmPlatformComponentManager>() );
        m_dep.reset( new NiceMock<MockPmPlatformDependencies>() );
        m_sslUtil.reset( new NiceMock<MockSslUtil>() );
        m_ucidAdapter.reset( new NiceMock<MockUcidAdapter>() );
        m_eventBuilder.reset( new NiceMock<MockCloudEventBuilder>() );
        m_eventPublisher.reset( new NiceMock<MockCloudEventPublisher>() );

        m_patient.reset( new PackageConfigProcessor( *m_fileUtil, *m_sslUtil, *m_ucidAdapter, *m_eventBuilder, *m_eventPublisher ) );

        m_dep->MakeComponentManagerReturn( *m_pmComponentManager );
    }

    void TearDown()
    {
        m_patient.reset();

        m_fileUtil.reset();
        m_dep.reset();
        m_pmComponentManager.reset();
        m_sslUtil.reset();
        m_ucidAdapter.reset();
        m_eventBuilder.reset();
        m_eventPublisher.reset();

        m_configInfo = {};
    }

    void SetupConfig()
    {
        m_configInfo = {
            "configpath",
            "configsha256",
            "configcontents",
            "configverifyBinPath",
            "configverifyPath",
            "installLocation",
            "signerName",
            "test/1.0.0",
            false
        };
    }

    PackageConfigInfo m_configInfo;

    std::unique_ptr<MockFileSysUtil> m_fileUtil;
    std::unique_ptr<MockPmPlatformComponentManager> m_pmComponentManager;
    std::unique_ptr<MockPmPlatformDependencies> m_dep;
    std::unique_ptr<MockSslUtil> m_sslUtil;
    std::unique_ptr<MockUcidAdapter> m_ucidAdapter;
    std::unique_ptr<MockCloudEventBuilder> m_eventBuilder;
    std::unique_ptr<MockCloudEventPublisher> m_eventPublisher;

    std::unique_ptr<PackageConfigProcessor> m_patient;
};

TEST_F( TestPackageConfigProcessor, WillDecodeConfigFile )
{
    SetupConfig();

    EXPECT_CALL( *m_sslUtil, DecodeBase64( _, _ ) );

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, WillFailIfDecodeConfigFileFails )
{
    SetupConfig();
    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( -1 );

    EXPECT_EQ( false, m_patient->ProcessConfig( m_configInfo ) );
}

TEST_F( TestPackageConfigProcessor, WillNotCreateConfigWhenDecodeFails )
{
    SetupConfig();
    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( -1 );

    m_fileUtil->ExpectCloseFileNotCalled();

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, WillSendErrorEventIfDecodeConfigFileFails )
{
    SetupConfig();
    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( -1 );

    EXPECT_CALL( *m_eventBuilder, WithError( _, _ ) );
    EXPECT_CALL( *m_eventPublisher, Publish( _ ) );

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, WillCreateTempConfigFile )
{
    SetupConfig();
    std::string tempDir( "TempDir" );

    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakeGetTempDirReturn( tempDir );
    EXPECT_CALL( *m_fileUtil, PmCreateFile( _ ) ).WillOnce( Invoke( [tempDir]( const std::string& filename )
        {
            EXPECT_EQ( filename.find( tempDir ), 0 );
            return ( FileUtilHandle* )1;
        } ) );

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, WillVerifyConfigFileHash )
{
    SetupConfig();

    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_fileUtil->MakeAppendFileReturn( 1 );
    m_sslUtil->MakeCalculateSHA256Return( m_configInfo.sha256 );

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, WillVerifyConfigFile )
{
    SetupConfig();

    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_fileUtil->MakeAppendFileReturn( 1 );
    m_sslUtil->MakeCalculateSHA256Return( m_configInfo.sha256 );

    EXPECT_CALL( *m_pmComponentManager, DeployConfiguration( _ ) );

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, WillMoveConfigFile )
{
    SetupConfig();

    m_patient->Initialize( m_dep.get() );

    m_fileUtil->MakeAppendPathReturn( m_configInfo.path );
    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_fileUtil->MakeAppendFileReturn( 1 );
    m_pmComponentManager->MakeDeployConfigurationReturn( 0 );
    m_sslUtil->MakeCalculateSHA256Return( m_configInfo.sha256 );

    EXPECT_CALL( *m_fileUtil, Rename( _, _ ) );

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, AddFileWillSucceed )
{
    SetupConfig();

    m_patient->Initialize( m_dep.get() );

    m_fileUtil->MakeAppendPathReturn( m_configInfo.path );
    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_fileUtil->MakeAppendFileReturn( 1 );
    m_pmComponentManager->MakeDeployConfigurationReturn( 0 );
    m_sslUtil->MakeCalculateSHA256Return( m_configInfo.sha256 );
    m_fileUtil->MakeRenameReturn( 0 );

    EXPECT_TRUE( m_patient->ProcessConfig( m_configInfo ) );
}

TEST_F( TestPackageConfigProcessor, WillSendSuccessEventIfAddFileSucceeds )
{
    SetupConfig();

    m_patient->Initialize( m_dep.get() );

    m_fileUtil->MakeAppendPathReturn( m_configInfo.path );
    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_fileUtil->MakeAppendFileReturn( 1 );
    m_pmComponentManager->MakeDeployConfigurationReturn( 0 );
    m_sslUtil->MakeCalculateSHA256Return( m_configInfo.sha256 );
    m_fileUtil->MakeRenameReturn( 0 );

    EXPECT_CALL( *m_eventBuilder, WithError( _, _ ) ).Times( 0 );
    EXPECT_CALL( *m_eventPublisher, Publish( _ ) );

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, WillNotVerifyConfigHashIfNotProvided )
{
    SetupConfig();
    m_configInfo.sha256 = "";

    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_fileUtil->MakeAppendFileReturn( 0 );

    m_sslUtil->ExpectCalculateSHA256NotCalled();

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, WillRemoveTempConfig )
{
    SetupConfig();
    m_configInfo.verifyBinPath = "";

    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_fileUtil->MakeAppendFileReturn( 0 );

    EXPECT_CALL( *m_fileUtil, DeleteFile( _ ) );

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, WillDeleteConfig )
{
    SetupConfig();
    m_configInfo.deleteConfig = true;
    m_patient->Initialize( m_dep.get() );

    m_fileUtil->MakeAppendPathReturn( m_configInfo.path );
    m_fileUtil->MakeFileExistsReturn( true );

    EXPECT_CALL( *m_fileUtil, DeleteFile( m_configInfo.path ) );

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, RemoveConfigWillSucceed )
{
    SetupConfig();
    m_configInfo.deleteConfig = true;
    m_patient->Initialize( m_dep.get() );

    m_fileUtil->MakeAppendPathReturn( m_configInfo.path );
    m_fileUtil->MakeFileExistsReturn( true );

    EXPECT_TRUE( m_patient->ProcessConfig( m_configInfo ) );
}

TEST_F( TestPackageConfigProcessor, WillSendSuccessEventIfRemoveConfigSucceeds )
{
    SetupConfig();
    m_configInfo.deleteConfig = true;
    m_patient->Initialize( m_dep.get() );

    m_fileUtil->MakeAppendPathReturn( m_configInfo.path );
    m_fileUtil->MakeFileExistsReturn( true );

    EXPECT_CALL( *m_eventBuilder, WithError( _, _ ) ).Times( 0 );
    EXPECT_CALL( *m_eventPublisher, Publish( _ ) );

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, WillNotDeleteConfigWithoutValidPath )
{
    SetupConfig();
    m_configInfo.deleteConfig = true;
    m_patient->Initialize( m_dep.get() );

    m_fileUtil->MakeAppendPathReturn( "" );

    m_fileUtil->ExpectDeleteFileNotCalled();

    m_patient->ProcessConfig( m_configInfo );
}

TEST_F( TestPackageConfigProcessor, RemoveConfigWillFail )
{
    SetupConfig();
    m_configInfo.deleteConfig = true;
    m_patient->Initialize( m_dep.get() );

    m_fileUtil->MakeAppendPathReturn( "" );

    EXPECT_FALSE( m_patient->ProcessConfig( m_configInfo ) );
}

TEST_F( TestPackageConfigProcessor, WillSendErrorEventIfRemoveConfigFails )
{
    SetupConfig();
    m_configInfo.deleteConfig = true;
    m_patient->Initialize( m_dep.get() );

    m_fileUtil->MakeAppendPathReturn( "" );

    EXPECT_CALL( *m_eventBuilder, WithError( _, _ ) );
    EXPECT_CALL( *m_eventPublisher, Publish( _ ) );

    m_patient->ProcessConfig( m_configInfo );
}

