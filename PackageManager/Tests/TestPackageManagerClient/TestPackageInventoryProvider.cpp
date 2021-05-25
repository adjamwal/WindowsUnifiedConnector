#include "gtest/gtest.h"
#include "PackageInventoryProvider.h"
#include "MockPmCloud.h"
#include "MockFileSysUtil.h"
#include "MockSslUtil.h"
#include "MockPmPlatformDependencies.h"
#include "MockPmPlatformComponentManager.h"
#include <memory>

class TestPackageInventoryProvider : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_fileUtil.reset( new NiceMock<MockFileSysUtil>() );
        m_sslUtil.reset( new NiceMock<MockSslUtil>() );
        m_pmComponentManager.reset( new NiceMock<MockPmPlatformComponentManager>() );
        m_dep.reset( new NiceMock<MockPmPlatformDependencies>() );
        m_dep->MakeComponentManagerReturn( *m_pmComponentManager );

        m_patient.reset( new PackageInventoryProvider( *m_fileUtil, *m_sslUtil ) );
    }

    void TearDown()
    {
        m_patient.reset();
        m_fileUtil.reset();
        m_sslUtil.reset();
        m_pmComponentManager.reset();
        m_dep.reset();
    }

    std::unique_ptr<MockFileSysUtil> m_fileUtil;
    std::unique_ptr<MockSslUtil> m_sslUtil;
    std::unique_ptr<MockPmPlatformComponentManager> m_pmComponentManager;
    std::unique_ptr<MockPmPlatformDependencies> m_dep;
    std::unique_ptr<PackageInventoryProvider> m_patient;
};

TEST_F( TestPackageInventoryProvider, WillFailWhenNotInitialied )
{
    PackageInventory inventory;

    EXPECT_FALSE( m_patient->GetInventory( inventory ) );
}

TEST_F( TestPackageInventoryProvider, GetInventorySucceeds )
{
    PackageInventory inventory;
    m_patient->Initialize( m_dep.get() );

    EXPECT_TRUE( m_patient->GetInventory( inventory ) );
}

TEST_F( TestPackageInventoryProvider, GetInventoryWillRetrieveConfigSha )
{
    PackageConfigInfo config;
    PmInstalledPackage package;
    PackageInventory inventory;
    PackageInventory detectedPackages;
    std::string sha256 = "Sha256";

    package.configs.push_back( config );
    detectedPackages.packages.push_back( package );
    ON_CALL( *m_pmComponentManager, GetInstalledPackages( _, _ ) )
        .WillByDefault( DoAll( SetArgReferee<1>( detectedPackages ), Return( 0 ) ) );
    m_fileUtil->MakeFileExistsReturn( true );
    m_sslUtil->MakeCalculateSHA256Return( sha256 );
    m_patient->Initialize( m_dep.get() );

    m_patient->GetInventory( inventory );

    EXPECT_EQ( inventory.packages[ 0 ].configs[ 0 ].sha256, sha256 );
}

TEST_F( TestPackageInventoryProvider, GetInventoryWillFailTooRetrieveConfigSha )
{
    PackageConfigInfo config;
    PmInstalledPackage package;
    PackageInventory inventory;
    PackageInventory detectedPackages;
    std::optional<std::string> sha256;

    package.configs.push_back( config );
    detectedPackages.packages.push_back( package );
    ON_CALL( *m_pmComponentManager, GetInstalledPackages( _, _ ) )
        .WillByDefault( DoAll( SetArgReferee<1>( detectedPackages ), Return( 0 ) ) );
    m_fileUtil->MakeFileExistsReturn( true );
    m_sslUtil->MakeCalculateSHA256Return( sha256 );
    m_patient->Initialize( m_dep.get() );

    m_patient->GetInventory( inventory );

    EXPECT_EQ( inventory.packages[0].configs[0].sha256, "" );
}