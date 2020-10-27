#include "gtest/gtest.h"
#include "ComponentPackageProcessor.h"
#include "MockPmCloud.h"
#include "MockFileUtil.h"
#include "MockPmPlatformDependencies.h"
#include "MockPmPlatformComponentManager.h"
#include "MockSslUtil.h"

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
        m_patient.reset( new ComponentPackageProcessor( *m_cloud, *m_fileUtil, *m_sslUtil ) );

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

        m_expectedComponentPackage = {};
    }

    void SetupComponentPackage()
    {
        m_expectedComponentPackage = {
            "packageName",
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
            "configverifyPath"
        } );
    }

    void SetupComponentPackageWithConfig()
    {
        SetupComponentPackage();
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

TEST_F( TestComponentPackageProcessor, WillRemoteFileWhenDownloadIsSuccesful )
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

TEST_F( TestComponentPackageProcessor, WillDecodeConfigFile )
{
    SetupComponentPackageWithConfig();

    EXPECT_CALL( *m_sslUtil, DecodeBase64( _, _ ) );

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillFailIfDecodeConfigFileFails )
{
    SetupComponentPackageWithConfig();
    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( -1 );

    EXPECT_FALSE( m_patient->ProcessComponentPackage( m_expectedComponentPackage ) );
}

TEST_F( TestComponentPackageProcessor, WillNotCreateConfigWhenDecodeFails )
{
    SetupComponentPackageWithConfig();
    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( -1 );
    
    m_fileUtil->ExpectCloseFileNotCalled();

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillTempConfigFile )
{
    SetupComponentPackageWithConfig();
    std::string tempDir( "TempDir" );
       
    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakeGetTempDirReturn( tempDir );
    EXPECT_CALL( *m_fileUtil, PmCreateFile( _ ) ).WillOnce( Invoke( [tempDir]( const std::string& filename )
        {
            EXPECT_EQ( filename.find( tempDir ), 0 );
            return (FileUtilHandle*)1;
        } ) );

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillVerifyConfigFile )
{
    SetupComponentPackageWithConfig();

    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_fileUtil->MakeAppendFileReturn( 0 );

    EXPECT_CALL( *m_pmComponentManager, DeployConfiguration( _ ) );

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillMoveConfigFile )
{
    SetupComponentPackageWithConfig();

    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_fileUtil->MakeAppendFileReturn( 0 );
    m_pmComponentManager->MakeDeployConfigurationReturn( 0 );

    EXPECT_CALL( *m_fileUtil, Rename( _, _, _) );

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillMoveConfigWhenVerificationIsNotRequired )
{
    SetupComponentPackageWithConfig();
    m_expectedComponentPackage.configs.front().verifyBinPath = "";

    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_fileUtil->MakeAppendFileReturn( 0 );
    
    m_pmComponentManager->ExpectDeployConfigurationIsNotCalled();
    EXPECT_CALL( *m_fileUtil, Rename( _, _, _ ) );

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}

TEST_F( TestComponentPackageProcessor, WillRemoveTempConfig )
{
    SetupComponentPackageWithConfig();
    m_expectedComponentPackage.configs.front().verifyBinPath = "";

    m_patient->Initialize( m_dep.get() );

    m_sslUtil->MakeDecodeBase64Return( 0 );
    m_fileUtil->MakePmCreateFileReturn( ( FileUtilHandle* )1 );
    m_fileUtil->MakeAppendFileReturn( -1 );

    EXPECT_CALL( *m_fileUtil, DeleteFile( _ ) ).Times( 2 );

    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}