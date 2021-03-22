#include "MocksCommon.h"
#include "PackageDiscovery.h"
#include "MockWindowsUtilities.h"
#include "PmTypes.h"
#include <memory>

class TestPackageDiscovery : public ::testing::Test
{
protected:
    void SetUp()
    {
        MockWindowsUtilities::Init();
        m_patient = std::make_unique<PackageDiscovery>();
    }

    void TearDown()
    {
        m_patient.reset();
        MockWindowsUtilities::Deinit();
    }

    std::unique_ptr<PackageDiscovery> m_patient;
};

TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillSetOS )
{
    std::vector<PmProductDiscoveryRules> catalogRules;

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( catalogRules );

    EXPECT_EQ( installedPackages.architecture, "x64" );
    EXPECT_EQ( installedPackages.platform, "win" );
}

//TODO: re-enable & fix once msi & registry discovery methods are implemented
TEST_F( TestPackageDiscovery, DISABLED_DiscoverInstalledPackagesWillGetUC )
{
    std::vector<PmProductDiscoveryRules> catalogRules;
    PmProductDiscoveryRules interestedPrograms;
    interestedPrograms.product = "uc";
    catalogRules.push_back( interestedPrograms );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeReadRegistryStringReturn( true );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( catalogRules );

    ASSERT_TRUE( installedPackages.packages.size() > 0 );

    EXPECT_EQ( installedPackages.packages.front().product, interestedPrograms.product );
    EXPECT_EQ( installedPackages.packages.front().configs.size(), 3 );
}

//TODO: re-enable & fix once msi & registry discovery methods are implemented
TEST_F( TestPackageDiscovery, DISABLED_DiscoverInstalledPackagesWillGetImmunet )
{
    std::vector<PmProductDiscoveryRules> catalogRules;
    PmProductDiscoveryRules interestedPrograms;
    interestedPrograms.product = "Immunet";
    catalogRules.push_back( interestedPrograms );
    
    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryString( _, _, std::wstring( L"DisplayName" ), _ ) )
        .WillByDefault( DoAll( SetArgReferee<3>( L"Immunet" ), Return( true ) ) );
    ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryString( _, _, std::wstring( L"DisplayVersion" ), _ ) )
        .WillByDefault( Return( true ) );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( catalogRules );

    ASSERT_TRUE( installedPackages.packages.size() > 0 );

    EXPECT_EQ( installedPackages.packages.front().product, interestedPrograms.product );
}

//TODO: re-enable & fix once msi & registry discovery methods are implemented
TEST_F( TestPackageDiscovery, DISABLED_DiscoverInstalledPackagesWillGetAmp )
{
    std::vector<PmProductDiscoveryRules> catalogRules;
    PmProductDiscoveryRules interestedPrograms;
    interestedPrograms.product = "Immunet";
    catalogRules.push_back( interestedPrograms );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryString( _, _, std::wstring( L"DisplayName" ), _ ) )
        .WillByDefault( DoAll( SetArgReferee<3>( L"Cisco AMP for Endpoints Connector" ), Return( true ) ) );
    ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryString( _, _, std::wstring( L"DisplayVersion" ), _ ) )
        .WillByDefault( Return( true ) );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( catalogRules );

    ASSERT_TRUE( installedPackages.packages.size() > 0 );

    EXPECT_EQ( installedPackages.packages.front().product, interestedPrograms.product );
}

TEST_F( TestPackageDiscovery, BuildAmpWillFailOnRegistryFailure )
{
    std::vector<PmProductDiscoveryRules> catalogRules;
    PmProductDiscoveryRules interestedPrograms;
    interestedPrograms.product = "Immunet";
    catalogRules.push_back( interestedPrograms );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeReadRegistryStringReturn( false );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( catalogRules );

    EXPECT_EQ( installedPackages.packages.size(), 0 );
}

//TODO: re-enable & fix once msi & registry discovery methods are implemented
TEST_F( TestPackageDiscovery, DISABLED_DiscoverInstalledPackagesWillDiscoverPrograms )
{
    std::vector<PmProductDiscoveryRules> catalogRules;
    std::vector<WindowsUtilities::WindowsInstallProgram> installedList;

    PmProductDiscoveryRules interestedPrograms;
    interestedPrograms.product = "p1";
    catalogRules.push_back( interestedPrograms );

    WindowsUtilities::WindowsInstallProgram installedProgram;
    installedProgram.name = interestedPrograms.product;
    installedProgram.version = "1.0.0.0";
    installedList.push_back( installedProgram );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetInstalledProgramsReturn( installedList );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( catalogRules );

    ASSERT_TRUE( installedPackages.packages.size() > 0 );

    EXPECT_EQ( installedPackages.packages[ 0 ].product, interestedPrograms.product );
    EXPECT_EQ( installedPackages.packages[ 0 ].version, installedProgram.version );
}

//TODO: re-enable & fix once msi & registry discovery methods are implemented
TEST_F( TestPackageDiscovery, DISABLED_DiscoverInstalledPackagesWillDiscoverManyPrograms )
{
    std::vector<PmProductDiscoveryRules> catalogRules;
    std::vector<WindowsUtilities::WindowsInstallProgram> installedList;

    PmProductDiscoveryRules interestedPrograms;
    interestedPrograms.product = "p1";
    catalogRules.push_back( interestedPrograms );

    interestedPrograms.product = "p2";
    catalogRules.push_back( interestedPrograms );

    WindowsUtilities::WindowsInstallProgram installedProgram;
    installedProgram.name = interestedPrograms.product;
    installedProgram.version = "1.0.0.0";
    installedList.push_back( installedProgram );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetInstalledProgramsReturn( installedList );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( catalogRules );

    ASSERT_TRUE( installedPackages.packages.size() > 0 );

    EXPECT_EQ( installedPackages.packages[ 0 ].product, "p1" );
    EXPECT_EQ( installedPackages.packages[ 0 ].version, installedProgram.version );

    EXPECT_EQ( installedPackages.packages[ 1 ].product, "p2" );
    EXPECT_EQ( installedPackages.packages[ 1 ].version, installedProgram.version );
}

TEST_F( TestPackageDiscovery, DISABLED_DiscoverInstalledPackagesWillPadVersionNumbers )
{
    std::vector<PmProductDiscoveryRules> catalogRules;
    std::vector<WindowsUtilities::WindowsInstallProgram> installedList;

    PmProductDiscoveryRules interestedPrograms;
    interestedPrograms.product = "p1";
    catalogRules.push_back( interestedPrograms );

    WindowsUtilities::WindowsInstallProgram installedProgram;
    installedProgram.name = interestedPrograms.product;
    installedProgram.version = "1";
    installedList.push_back( installedProgram );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetInstalledProgramsReturn( installedList );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( catalogRules );

    ASSERT_TRUE( installedPackages.packages.size() > 0 );

    EXPECT_EQ( installedPackages.packages[ 0 ].version, "1.0.0.0" );
}
