#include "gtest/gtest.h"
#include "ComponentPackageProcessor.h"
#include "MockPmCloud.h"
#include "MockFileUtil.h"
#include "MockPmPlatformDependencies.h"
#include "MockSslUtil.h"

#include <memory>

class TestComponentPackageProcessor : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_cloud.reset( new NiceMock<MockPmCloud>() );
        m_fileUtil.reset( new NiceMock<MockFileUtil>() );
        m_dep.reset( new NiceMock<MockPmPlatformDependencies>() );
        m_sslUtil.reset( new NiceMock<MockSslUtil>() );
        m_patient.reset( new ComponentPackageProcessor( *m_cloud, *m_fileUtil, *m_sslUtil ) );
    }

    void TearDown()
    {
        m_patient.reset();
        m_cloud.reset();
        m_fileUtil.reset();
        m_dep.reset();
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
                "installerPath"
        };
    }

    PmComponent m_expectedComponentPackage;
    std::unique_ptr<MockPmCloud> m_cloud;
    std::unique_ptr<MockFileUtil> m_fileUtil;
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

TEST_F( TestComponentPackageProcessor, WillNotProcessComponentPackageIfNotInitialized )
{
    SetupComponentPackage();
    m_cloud->ExpectDownloadFileIsNotCalled();
    m_patient->ProcessComponentPackage( m_expectedComponentPackage );
}
