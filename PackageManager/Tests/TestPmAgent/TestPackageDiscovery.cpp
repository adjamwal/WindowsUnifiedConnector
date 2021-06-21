#include "MocksCommon.h"
#include "PackageDiscovery.h"
#include "MockWindowsUtilities.h"
#include "MockPackageDiscoveryMethods.h"
#include "PmTypes.h"
#include "MockWinApiWrapper.h"
#include <memory>
#include "MockMsiApi.h"
#include "MockUtf8PathVerifier.h"
#include "MockPmPlatformComponentManager.h"

class TestPackageDiscovery : public ::testing::Test
{
protected:
    void SetUp()
    {
        MockWindowsUtilities::Init();
        m_catalogRules.clear();
        m_detectedInstallations.clear();
        m_discoveryMethods.reset( new NiceMock<MockPackageDiscoveryMethods>() );
        m_msiApi.reset( new NiceMock<MockMsiApi>() );
        m_utf8PathVerifier.reset( new NiceMock<MockUtf8PathVerifier>() );

        m_patient = std::make_unique<PackageDiscovery>( *m_discoveryMethods, *m_msiApi, *m_utf8PathVerifier );
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

        ON_CALL( *m_discoveryMethods, DiscoverByMsiRules( _, _, _, _ ) )
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
    std::unique_ptr<MockMsiApi> m_msiApi;
    std::unique_ptr<MockUtf8PathVerifier> m_utf8PathVerifier;

    std::unique_ptr<PackageDiscovery> m_patient;
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
    m_catalogRules.push_back( productInCatalog );

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

TEST_F( TestPackageDiscovery, OneConfigurableIsFound )
{
    std::string path1 = "c:/test/one.xml";

    PmProductDiscoveryRules productDiscoveryRules;
    SetupMsiDiscovery( productDiscoveryRules );
    
    PmProductDiscoveryConfigurable configurable1 = {};
    configurable1.path = path1;
    configurable1.unresolvedPath = path1;
    configurable1.max_instances = 1;
    productDiscoveryRules.configurables.push_back( configurable1 );

    m_catalogRules.push_back( productDiscoveryRules );

    std::vector<std::filesystem::path> configs1;
    configs1.push_back( std::filesystem::path( path1 ) );
    
    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), FileSearchWithWildCard( _, _ ) )
        .WillOnce( DoAll( SetArgReferee<1>( configs1 ), Return( 0 ) ) );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    EXPECT_EQ( installedPackages.packages.size(), 1 );
    EXPECT_EQ( installedPackages.packages[0].configs.size(), 1 );
    EXPECT_EQ( std::filesystem::path( installedPackages.packages[0].configs[0].path ),
        std::filesystem::path( path1 ) );
}

TEST_F( TestPackageDiscovery, MultipleConfigurablesAreFound )
{
    std::string path1 = "c:/test/one.xml";
    std::string path2 = "c:/test/two.xml";

    PmProductDiscoveryRules productDiscoveryRules;
    SetupMsiDiscovery( productDiscoveryRules );

    PmProductDiscoveryConfigurable configurable1 = {};
    configurable1.path = path1;
    configurable1.unresolvedPath = path1;
    configurable1.max_instances = 1;
    productDiscoveryRules.configurables.push_back( configurable1 );

    PmProductDiscoveryConfigurable configurable2 = {};
    configurable2.path = path2;
    configurable2.unresolvedPath = path2;
    configurable2.max_instances = 1;
    configurable2.required = false;
    productDiscoveryRules.configurables.push_back( configurable2 );

    m_catalogRules.push_back( productDiscoveryRules );

    std::vector<std::filesystem::path> configs1;
    configs1.push_back( std::filesystem::path( path1 ) );

    std::vector<std::filesystem::path> configs2;
    configs2.push_back( std::filesystem::path( path2 ) );

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), FileSearchWithWildCard( _, _ ) )
        .WillOnce( DoAll( SetArgReferee<1>( configs1 ), Return( 0 ) ) )
        .WillOnce( DoAll( SetArgReferee<1>( configs2 ), Return( 0 ) ) );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    EXPECT_EQ( installedPackages.packages.size(), 1 );
    EXPECT_EQ( installedPackages.packages[0].configs.size(), 2 );
    EXPECT_EQ( std::filesystem::path( installedPackages.packages[0].configs[0].path ),
        std::filesystem::path( path1 ) );
    EXPECT_EQ( std::filesystem::path( installedPackages.packages[0].configs[1].path ),
        std::filesystem::path( path2 ) );
}

TEST_F( TestPackageDiscovery, OneConfigurableIsFoundMaxInstancesReachedOne )
{
    std::string path1 = "c:/test/one.xml";

    PmProductDiscoveryRules productDiscoveryRules;
    SetupMsiDiscovery( productDiscoveryRules );

    PmProductDiscoveryConfigurable configurable1 = {};
    configurable1.path = path1;
    configurable1.unresolvedPath = path1;
    configurable1.max_instances = 1;
    productDiscoveryRules.configurables.push_back( configurable1 );

    m_catalogRules.push_back( productDiscoveryRules );

    std::vector<std::filesystem::path> configs1;
    configs1.push_back( std::filesystem::path( "c:/test/one.xml" ) );
    configs1.push_back( std::filesystem::path( "c:/test/two.xml" ) );
    configs1.push_back( std::filesystem::path( "c:/test/three.xml" ) );

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), FileSearchWithWildCard( _, _ ) )
        .WillOnce( DoAll( SetArgReferee<1>( configs1 ), Return( 0 ) ) );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    EXPECT_EQ( installedPackages.packages.size(), 1 );
    EXPECT_EQ( installedPackages.packages[0].configs.size(), 1 );
    EXPECT_EQ( std::filesystem::path( installedPackages.packages[0].configs[0].path ),
        std::filesystem::path( path1 ) );
}

TEST_F( TestPackageDiscovery, OneConfigurableIsFoundMaxInstancesReachedTwo )
{
    std::string path1 = path1;

    PmProductDiscoveryRules productDiscoveryRules;
    SetupMsiDiscovery( productDiscoveryRules );

    PmProductDiscoveryConfigurable configurable1 = {};
    configurable1.path = path1;
    configurable1.unresolvedPath = path1;
    configurable1.max_instances = 2;
    productDiscoveryRules.configurables.push_back( configurable1 );

    m_catalogRules.push_back( productDiscoveryRules );

    std::vector<std::filesystem::path> configs1;
    configs1.push_back( std::filesystem::path( "c:/test/one.xml" ) );
    configs1.push_back( std::filesystem::path( "c:/test/two.xml" ) );
    configs1.push_back( std::filesystem::path( "c:/test/three.xml" ) );

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), FileSearchWithWildCard( _, _ ) )
        .WillOnce( DoAll( SetArgReferee<1>( configs1 ), Return( 0 ) ) );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    EXPECT_EQ( installedPackages.packages.size(), 1 );
    EXPECT_EQ( installedPackages.packages[0].configs.size(), 2 );
    EXPECT_EQ( std::filesystem::path( installedPackages.packages[0].configs[0].path ),
        std::filesystem::path( "c:/test/one.xml" ) );
    EXPECT_EQ( std::filesystem::path( installedPackages.packages[0].configs[1].path ),
        std::filesystem::path( "c:/test/two.xml" ) );
}

TEST_F( TestPackageDiscovery, OneConfigurableIsFoundDefaultMaxInstances )
{
    std::string path1 = "c:/test/one.xml";

    PmProductDiscoveryRules productDiscoveryRules;
    SetupMsiDiscovery( productDiscoveryRules );

    PmProductDiscoveryConfigurable configurable1 = {};
    configurable1.path = path1;
    configurable1.unresolvedPath = path1;
    productDiscoveryRules.configurables.push_back( configurable1 );

    m_catalogRules.push_back( productDiscoveryRules );

    std::vector<std::filesystem::path> configs1;
    configs1.push_back( std::filesystem::path( path1 ) );

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), FileSearchWithWildCard( _, _ ) )
        .WillOnce( DoAll( SetArgReferee<1>( configs1 ), Return( 0 ) ) );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    EXPECT_EQ( installedPackages.packages.size(), 1 );
    EXPECT_EQ( installedPackages.packages[0].configs.size(), 1 );
    EXPECT_EQ( std::filesystem::path( installedPackages.packages[0].configs[0].path ),
        std::filesystem::path( path1 ) );
}

TEST_F( TestPackageDiscovery, OneConfigurableIsFoundDefaultMaxInstancesReached )
{
    std::string path1 = "c:/test/one.xml";

    PmProductDiscoveryRules productDiscoveryRules;
    SetupMsiDiscovery( productDiscoveryRules );

    PmProductDiscoveryConfigurable configurable1 = {};
    configurable1.path = path1;
    configurable1.unresolvedPath = path1;
    productDiscoveryRules.configurables.push_back( configurable1 );

    m_catalogRules.push_back( productDiscoveryRules );

    std::vector<std::filesystem::path> configs1;
    configs1.push_back( std::filesystem::path( path1 ) );
    configs1.push_back( std::filesystem::path( "c:/test/two.xml" ) );

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), FileSearchWithWildCard( _, _ ) )
        .WillOnce( DoAll( SetArgReferee<1>( configs1 ), Return( 0 ) ) );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    EXPECT_EQ( installedPackages.packages.size(), 1 );
    EXPECT_EQ( installedPackages.packages[0].configs.size(), 1 );
    EXPECT_EQ( std::filesystem::path( installedPackages.packages[0].configs[0].path ),
        std::filesystem::path( path1 ) );
}

TEST_F( TestPackageDiscovery, OneConfigurableNoneAreFound )
{
    std::string path1 = "c:/test/one.xml";

    PmProductDiscoveryRules productDiscoveryRules;
    SetupMsiDiscovery( productDiscoveryRules );

    PmProductDiscoveryConfigurable configurable1 = {};
    configurable1.path = path1;
    configurable1.unresolvedPath = path1;
    productDiscoveryRules.configurables.push_back( configurable1 );

    m_catalogRules.push_back( productDiscoveryRules );

    std::vector<std::filesystem::path> configs1;

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), FileSearchWithWildCard( _, _ ) )
        .WillOnce( DoAll( SetArgReferee<1>( configs1 ), Return( 0 ) ) );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    EXPECT_EQ( installedPackages.packages.size(), 1 );
    EXPECT_EQ( installedPackages.packages[0].configs.size(), 0 );
}

TEST_F( TestPackageDiscovery, ConfigurablePathWillBeUnresolved )
{
    std::string path1 = "C:/ProgramData/test/one.xml";

    PmProductDiscoveryRules productDiscoveryRules;
    SetupMsiDiscovery( productDiscoveryRules );

    PmProductDiscoveryConfigurable configurable1 = {};
    configurable1.path = path1;
    configurable1.unresolvedPath = "<FOLDERID_ProgramData>/test/one.xml";
    productDiscoveryRules.configurables.push_back( configurable1 );

    m_catalogRules.push_back( productDiscoveryRules );

    std::vector<std::filesystem::path> configs1;
    configs1.push_back( std::filesystem::path( path1 ) );

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), FileSearchWithWildCard( _, _ ) )
        .WillOnce( DoAll( SetArgReferee<1>( configs1 ), Return( 0 ) ) );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( m_catalogRules );

    EXPECT_EQ( installedPackages.packages.size(), 1 );
    EXPECT_EQ( installedPackages.packages[0].configs.size(), 1 );
    EXPECT_EQ( std::filesystem::path( installedPackages.packages[0].configs[0].path ),
        std::filesystem::path( "<FOLDERID_ProgramData>/test/one.xml" ) );
}
