#include "MocksCommon.h"
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
        lookupProduct.product = "uc";

        regRule.type = "registry";
        regRule.install.key = "HKLM\\SOFTWARE\\Immunet Protect\\InstallDir";
        regRule.install.type = "WOW6432";
        regRule.version.key = "HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Immunet Protect\\DisplayVersion";
        regRule.version.type = "WOW6432";

        ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryStringA( _, _, std::string( "InstallDir" ), _ ) )
            .WillByDefault( DoAll( SetArgReferee<3>( "data" ), Return( true ) ) );
        ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryStringA( _, _, std::string( "DisplayVersion" ), _ ) )
            .WillByDefault( DoAll( SetArgReferee<3>( "1" ), Return( true ) ) );
    }

    std::unique_ptr<MockMsiApi> m_msiApi;
    std::unique_ptr<PackageDiscoveryMethods> m_patient;
};

TEST_F( TestPackageDiscoveryMethods, DiscoverByRegistryWillGetUC )
{
    PmProductDiscoveryRules lookupProduct;
    PmProductDiscoveryRegistryMethod regRule;
    SetupRegistryDiscoveryLookup( lookupProduct, regRule );

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryStringA( _, _, _, _ ) ).Times( 2 );

    std::vector<PmInstalledPackage> detectedInstallations;
    m_patient->DiscoverByRegistry( lookupProduct, regRule, detectedInstallations );

    ASSERT_EQ( 1, detectedInstallations.size() );
    EXPECT_EQ( detectedInstallations.front().product, lookupProduct.product );
    //EXPECT_EQ( detectedInstallations.front().configs.size(), 3 );
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
    EXPECT_EQ( detectedInstallations[ 0 ].version, "1.0.0.0" );
}
