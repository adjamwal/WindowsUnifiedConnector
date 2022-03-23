#include "pch.h"
#include "MockWindowsUtilities.h"
#include "PackageDiscoveryMethods.h"
#include "PmTypes.h"
#include "MockWinApiWrapper.h"
#include <memory>
#include <MockMsiApi.h>

class TestPackageDiscoveryMethods : public ::testing::Test
{
protected:
    void SetUp()
    {
        MockWindowsUtilities::Init();
        m_msiApi.reset( new NiceMock<MockMsiApi>() );
        m_patient = std::make_unique<PackageDiscoveryMethods>( *m_msiApi );
    }

    void TearDown()
    {
        m_patient.reset();
        m_msiApi.reset();
        MockWindowsUtilities::Deinit();
    }

    void SetupRegistryDiscoveryLookup( PmProductDiscoveryRules& lookupProduct, PmProductDiscoveryRegistryMethod& regRule )
    {
        SetupProductDiscoveryRules( lookupProduct );

        regRule.type = "registry";
        regRule.install.key = "HKLM\\SOFTWARE\\Immunet Protect\\InstallDir";
        regRule.install.type = "WOW6432";
        regRule.version.key = "HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Immunet Protect\\DisplayVersion";
        regRule.version.type = "WOW6432";

        ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryStringA( _, _, std::string( "InstallDir" ), _, _ ) )
            .WillByDefault( DoAll( SetArgReferee<4>( "data" ), Return( true ) ) );
        ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryStringA( _, _, std::string( "DisplayVersion" ), _, _ ) )
            .WillByDefault( DoAll( SetArgReferee<4>( "1" ), Return( true ) ) );
    }

    void SetupProductDiscoveryRules( PmProductDiscoveryRules& lookupProduct )
    {
        lookupProduct.product = "uc";
    }

    void SetupProductDiscoveryMsiMethod( PmProductDiscoveryMsiMethod& msiRule )
    {
        msiRule.type = "msi";
        msiRule.name = "testName";
        msiRule.vendor = "testVendor";
    }

    void SetupProductDiscoveryMsiUpgradeCodeMethod( PmProductDiscoveryMsiUpgradeCodeMethod& msiRule )
    {
        msiRule.type = "msi_upgrade_code";
        msiRule.upgradeCode = "{0010278}";
    }

    void SetupMsiApiReturnValue( std::tuple<int32_t, std::vector<MsiApiProductInfo>>& value )
    {
        MsiApiProductInfo msiApiProductInfo = {};
        msiApiProductInfo.Properties.VersionString = L"testVersion";

        std::vector<MsiApiProductInfo> msiReturnList;
        msiReturnList.push_back( msiApiProductInfo );

        value = std::make_tuple( 0, msiReturnList );
    }

    std::unique_ptr<MockMsiApi> m_msiApi;
    std::unique_ptr<PackageDiscoveryMethods> m_patient;
};

TEST_F( TestPackageDiscoveryMethods, DiscoverByRegistryWillGetUC )
{
    PmProductDiscoveryRules lookupProduct;
    PmProductDiscoveryRegistryMethod regRule;
    SetupRegistryDiscoveryLookup( lookupProduct, regRule );

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryStringA( _, _, _, _, _ ) ).Times( 2 );

    std::vector<PmInstalledPackage> detectedInstallations;
    m_patient->DiscoverByRegistry( lookupProduct, regRule, detectedInstallations );

    ASSERT_EQ( 1, detectedInstallations.size() );
    ASSERT_EQ( lookupProduct.product, detectedInstallations.front().product );
}

TEST_F( TestPackageDiscoveryMethods, DiscoverByRegistryWillDecodeRegistryLocationsCorrectly )
{
    PmProductDiscoveryRules lookupProduct;
    PmProductDiscoveryRegistryMethod regRule;
    SetupRegistryDiscoveryLookup( lookupProduct, regRule );

    InSequence S;
    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryStringA( _, "SOFTWARE\\Immunet Protect", "InstallDir", _, _ ) );
    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryStringA( _, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Immunet Protect", "DisplayVersion", _, _ ) );

    std::vector<PmInstalledPackage> detectedInstallations;
    m_patient->DiscoverByRegistry( lookupProduct, regRule, detectedInstallations );
}

TEST_F( TestPackageDiscoveryMethods, DiscoverByRegistryWillFailOnRegistryFailure )
{
    PmProductDiscoveryRules lookupProduct;
    PmProductDiscoveryRegistryMethod regRule;
    SetupRegistryDiscoveryLookup( lookupProduct, regRule );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeReadRegistryStringAReturn( false );
    
    std::vector<PmInstalledPackage> detectedInstallations;
    m_patient->DiscoverByRegistry( lookupProduct, regRule, detectedInstallations );

    ASSERT_EQ( 0, detectedInstallations.size() );
}

TEST_F( TestPackageDiscoveryMethods, DiscoverByRegistryWillPadVersionNumbers )
{
    PmProductDiscoveryRules lookupProduct;
    PmProductDiscoveryRegistryMethod regRule;
    SetupRegistryDiscoveryLookup( lookupProduct, regRule );

    std::vector<PmInstalledPackage> detectedInstallations;
    m_patient->DiscoverByRegistry( lookupProduct, regRule, detectedInstallations );

    ASSERT_EQ( 1, detectedInstallations.size() );
    ASSERT_EQ( "1.0.0.0", detectedInstallations[ 0 ].version );
}

TEST_F( TestPackageDiscoveryMethods, DiscoverByMsiNoneFound )
{
    std::vector<PmInstalledPackage> detectedInstallations;

    PmProductDiscoveryRules lookupProduct = {};
    SetupProductDiscoveryRules( lookupProduct );

    PmProductDiscoveryMsiMethod msiRule = {};
    SetupProductDiscoveryMsiMethod( msiRule );

    std::vector<MsiApiProductInfo> msiReturnList;
    auto methodReturnValue = std::make_tuple( 0, msiReturnList );

    ON_CALL( *m_msiApi, FindProductsByNameAndPublisher( _, _ ) )
        .WillByDefault( Return( methodReturnValue ) );

    m_patient->DiscoverByMsi( lookupProduct, msiRule, detectedInstallations );

    ASSERT_EQ( 0, detectedInstallations.size() );
}

TEST_F( TestPackageDiscoveryMethods, DiscoverByMsiProductFound )
{
    std::vector<PmInstalledPackage> detectedInstallations;
    
    PmProductDiscoveryRules lookupProduct = {};
    SetupProductDiscoveryRules( lookupProduct );
    
    PmProductDiscoveryMsiMethod msiRule = {};
    SetupProductDiscoveryMsiMethod( msiRule );

    std::tuple<int32_t, std::vector<MsiApiProductInfo>> methodReturnValue;
    SetupMsiApiReturnValue( methodReturnValue );

    ON_CALL( *m_msiApi, FindProductsByNameAndPublisher( _, _ ) )
        .WillByDefault( Return( methodReturnValue ) );

    m_patient->DiscoverByMsi( lookupProduct, msiRule, detectedInstallations );

    ASSERT_EQ( 1, detectedInstallations.size() );
    ASSERT_EQ( lookupProduct.product, detectedInstallations[ 0 ].product );
    ASSERT_EQ( "testVersion", detectedInstallations[ 0 ].version );
}

TEST_F( TestPackageDiscoveryMethods, DiscoverByUpgradeCodeProductNone )
{
    std::vector<PmInstalledPackage> detectedInstallations;

    PmProductDiscoveryRules lookupProduct = {};
    SetupProductDiscoveryRules( lookupProduct );

    PmProductDiscoveryMsiUpgradeCodeMethod msiRule = {};
    SetupProductDiscoveryMsiUpgradeCodeMethod( msiRule );

    std::vector<MsiApiProductInfo> msiReturnList;
    auto methodReturnValue = std::make_tuple( 0, msiReturnList );

    ON_CALL( *m_msiApi, FindProductsByNameAndPublisher( _, _ ) )
        .WillByDefault( Return( methodReturnValue ) );

    m_patient->DiscoverByMsiUpgradeCode( lookupProduct, msiRule, detectedInstallations );

    ASSERT_EQ( 0, detectedInstallations.size() );
}

TEST_F( TestPackageDiscoveryMethods, DiscoverByUpgradeCodeProductFound )
{
    std::vector<PmInstalledPackage> detectedInstallations;

    PmProductDiscoveryRules lookupProduct = {};
    SetupProductDiscoveryRules( lookupProduct );

    PmProductDiscoveryMsiUpgradeCodeMethod msiRule = {};
    SetupProductDiscoveryMsiUpgradeCodeMethod( msiRule );

    std::tuple<int32_t, std::vector<MsiApiProductInfo>> methodReturnValue;
    SetupMsiApiReturnValue( methodReturnValue );

    ON_CALL( *m_msiApi, FindRelatedProducts( _ ) )
        .WillByDefault( Return( methodReturnValue ) );

    m_patient->DiscoverByMsiUpgradeCode( lookupProduct, msiRule, detectedInstallations );

    ASSERT_EQ( 1, detectedInstallations.size() );
    ASSERT_EQ( lookupProduct.product, detectedInstallations[ 0 ].product );
    ASSERT_EQ( "testVersion", detectedInstallations[ 0 ].version );
}

TEST_F( TestPackageDiscoveryMethods, DiscoverByMsiRulesNoneFound )
{
    std::vector<PmInstalledPackage> detectedInstallations;

    PmProductDiscoveryRules lookupProduct = {};
    SetupProductDiscoveryRules( lookupProduct );

    PmProductDiscoveryMsiMethod msiRule = {};
    SetupProductDiscoveryMsiMethod( msiRule );

    std::vector<MsiApiProductInfo> msiReturnList;
    auto methodReturnValue = std::make_tuple( 0, msiReturnList );

    std::vector<MsiApiProductInfo> productCache;

    ON_CALL( *m_msiApi, FindProductsByNameAndPublisher( _, _ ) )
        .WillByDefault( Return( methodReturnValue ) );

    m_patient->DiscoverByMsiRules( lookupProduct, msiRule, detectedInstallations, productCache );

    ASSERT_EQ( 0, detectedInstallations.size() );
}

TEST_F( TestPackageDiscoveryMethods, DiscoverByMsiRulesProductFound )
{
    std::vector<PmInstalledPackage> detectedInstallations;

    PmProductDiscoveryRules lookupProduct = {};
    SetupProductDiscoveryRules( lookupProduct );

    PmProductDiscoveryMsiMethod msiRule = {};
    SetupProductDiscoveryMsiMethod( msiRule );

    std::vector<MsiApiProductInfo> productCache;

    MsiApiProductInfo msiApiProductInfo = {};
    msiApiProductInfo.Properties.Publisher = L"testVendor";
    msiApiProductInfo.Properties.InstalledProductName = L"testName";
    msiApiProductInfo.Properties.VersionString = L"testVersion";

    productCache.push_back( msiApiProductInfo );


    m_patient->DiscoverByMsiRules( lookupProduct, msiRule, detectedInstallations, productCache );

    ASSERT_EQ( 1, detectedInstallations.size() );
    ASSERT_EQ( lookupProduct.product, detectedInstallations[0].product );
    ASSERT_EQ( "testVersion", detectedInstallations[0].version );
}