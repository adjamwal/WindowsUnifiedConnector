#include "MocksCommon.h"
#include "PackageDiscovery.h"
#include "MockWindowsUtilities.h"
#include "MockPackageDiscoveryMethods.h"
#include "PmTypes.h"
#include "MockWinApiWrapper.h"
#include <memory>
#include <MockMsiApi.h>
#include <PmMocks/MockPmPlatformComponentManager.h>

class TestPackageDiscovery : public ::testing::Test
{
protected:
    void SetUp()
    {
        MockWindowsUtilities::Init();
        m_catalogRules.clear();
        m_detectedInstallations.clear();
        m_discoveryMethods.reset( new NiceMock<MockPackageDiscoveryMethods>() );
        m_pmPlatformComponentManager.reset( new NiceMock<MockPmPlatformComponentManager>());
        m_patient = std::make_unique<PackageDiscovery>( *m_discoveryMethods, *m_pmPlatformComponentManager );
    }

    void TearDown()
    {
        m_patient.reset();
        m_discoveryMethods.reset();
        m_detectedInstallations.clear();
        m_catalogRules.clear();
        MockWindowsUtilities::Deinit();
    }

    void SetupMsiUpgradeDiscovery( PmProductDiscoveryRules& testProduct )
    {
        PmProductDiscoveryMsiUpgradeCodeMethod upgMethod;
        testProduct.msiUpgradeCode_discovery.push_back( upgMethod );

        PmInstalledPackage detection;
        detection.version = "msiupg";
        m_detectedInstallations.push_back( detection );

        ON_CALL( *m_discoveryMethods, DiscoverByMsiUpgradeCode( _, _, _ ) )
            .WillByDefault( SetArgReferee<2>( m_detectedInstallations ) );
    }

    void SetupMsiDiscovery( PmProductDiscoveryRules& testProduct )
    {
        PmProductDiscoveryMsiMethod msiMethod;
        testProduct.msi_discovery.push_back( msiMethod );

        PmInstalledPackage detection;
        detection.version = "msi";
        m_detectedInstallations.push_back( detection );

        ON_CALL( *m_discoveryMethods, DiscoverByMsi( _, _, _ ) )
            .WillByDefault( SetArgReferee<2>( m_detectedInstallations ) );
    }

    void SetupRegistryDiscovery( PmProductDiscoveryRules& testProduct )
    {
        PmProductDiscoveryRegistryMethod regMethod;
        testProduct.reg_discovery.push_back( regMethod );

        PmInstalledPackage detection;
        detection.version = "reg";
        m_detectedInstallations.push_back( detection );

        ON_CALL( *m_discoveryMethods, DiscoverByRegistry( _, _, _ ) )
            .WillByDefault( SetArgReferee<2>( m_detectedInstallations ) );
    }

    std::vector<PmProductDiscoveryRules> m_catalogRules;
    std::vector<PmInstalledPackage> m_detectedInstallations;
    std::unique_ptr<MockPackageDiscoveryMethods> m_discoveryMethods;
    std::unique_ptr<PackageDiscovery> m_patient;
    std::unique_ptr<IPmPlatformComponentManager> m_pmPlatformComponentManager;
};

TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillSetOS )
{
    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    EXPECT_EQ( installedPackages.architecture, "x64" );
    EXPECT_EQ( installedPackages.platform, "win" );
}

TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillDetectByMsiUpgradeCode )
{
    PmProductDiscoveryRules productInCatalog;
    SetupMsiUpgradeDiscovery( productInCatalog );
    m_catalogRules.push_back( productInCatalog );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    ASSERT_EQ( 1, installedPackages.packages.size() );
    ASSERT_EQ( "msiupg", installedPackages.packages[ 0 ].version );
}

TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillDetectByMsi )
{
    PmProductDiscoveryRules productInCatalog;
    SetupMsiDiscovery( productInCatalog );
    m_catalogRules.push_back( productInCatalog );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    ASSERT_EQ( 1, installedPackages.packages.size() );
    ASSERT_EQ( "msi", installedPackages.packages[ 0 ].version );
}

TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillDetectByRegistry )
{
    PmProductDiscoveryRules productInCatalog;
    SetupRegistryDiscovery( productInCatalog );
    m_catalogRules.push_back( productInCatalog );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    ASSERT_EQ( 1, installedPackages.packages.size() );
    ASSERT_EQ( "reg", installedPackages.packages[ 0 ].version );
}

TEST_F( TestPackageDiscovery, DiscoveryWillCompleteAfterMsiUpgradeCodeMethod )
{
    PmProductDiscoveryRules productInCatalog;
    SetupMsiUpgradeDiscovery( productInCatalog );
    SetupMsiDiscovery( productInCatalog );
    SetupRegistryDiscovery( productInCatalog );
    m_catalogRules.push_back( productInCatalog );

    m_discoveryMethods->ExpectDiscoverByMsiIsNotCalled();
    m_discoveryMethods->ExpectDiscoverByRegistryIsNotCalled();

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    ASSERT_EQ( 1, installedPackages.packages.size() );
    ASSERT_EQ( "msiupg", installedPackages.packages[ 0 ].version );
}

TEST_F( TestPackageDiscovery, DiscoveryWillCompleteAfterMsiMethod )
{
    PmProductDiscoveryRules productInCatalog;
    SetupMsiDiscovery( productInCatalog );
    SetupRegistryDiscovery( productInCatalog );
    m_catalogRules.push_back( productInCatalog );

    m_discoveryMethods->ExpectDiscoverByMsiUpgradeCodeIsNotCalled();
    m_discoveryMethods->ExpectDiscoverByRegistryIsNotCalled();

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    ASSERT_EQ( 1, installedPackages.packages.size() );
    ASSERT_EQ( "msi", installedPackages.packages[ 0 ].version );
}

TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillDiscoverManyPrograms )
{
    PmProductDiscoveryRules productLookup;
    PmProductDiscoveryRegistryMethod regMethod;
    productLookup.reg_discovery.push_back( regMethod );
    for( int i = 0; i < 10; i++ ) {
        m_catalogRules.push_back( productLookup );
    }

    PmInstalledPackage detection;
    m_detectedInstallations.push_back( detection );
    ON_CALL( *m_discoveryMethods, DiscoverByRegistry( _, _, _ ) )
        .WillByDefault( SetArgReferee<2>( m_detectedInstallations ) );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    ASSERT_EQ( 10, installedPackages.packages.size() );
}

TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillResetTheCacheToLatestDiscoveryValues )
{
    PmProductDiscoveryRules productLookup;
    PmProductDiscoveryRegistryMethod regMethod;
    productLookup.reg_discovery.push_back( regMethod );
    for( int i = 0; i < 10; i++ ) {
        m_catalogRules.push_back( productLookup );
    }

    PmInstalledPackage detection;
    m_detectedInstallations.push_back( detection );
    ON_CALL( *m_discoveryMethods, DiscoverByRegistry( _, _, _ ) )
        .WillByDefault( SetArgReferee<2>( m_detectedInstallations ) );

    m_patient->DiscoverInstalledPackages( m_catalogRules );
    PackageInventory cache = m_patient->CachedInventory();

    ASSERT_EQ( m_catalogRules.size(), cache.packages.size() );
}

TEST_F( TestPackageDiscovery, ConfigurablesAreFound )
{
    PmProductDiscoveryRules lookupProduct;

    PmProductDiscoveryConfigurable configurable1 = {};
    configurable1.path = "Test\\Path";
    configurable1.max_instances = 7;
    configurable1.required = true;
    lookupProduct.configurables.push_back( configurable1 );

    PmProductDiscoveryConfigurable configurable2 = {};
    configurable2.path = "c:\\Test\\Path.config";
    configurable2.max_instances = 6;
    configurable2.required = false;
    lookupProduct.configurables.push_back( configurable2 );
}