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


TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillGetUC )
{
    std::vector<PmProductDiscoveryRules> catalogRules;
    PmProductDiscoveryRules interestedPrograms;
    interestedPrograms.product = "uc";
    catalogRules.push_back( interestedPrograms );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeReadRegistryStringReturn( true );

    PackageInventory installedPackages = m_patient->DiscoverInstalledPackages( catalogRules );

    EXPECT_EQ( installedPackages.packages.front().packageName, interestedPrograms.product );
    EXPECT_EQ( installedPackages.packages.front().configs.size(), 3 );
}

TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillGetImmunet )
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

    EXPECT_EQ( installedPackages.packages.front().packageName, interestedPrograms.product );
}

TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillGetAmp )
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

    EXPECT_EQ( installedPackages.packages.front().packageName, interestedPrograms.product );
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


TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillDiscoverPrograms )
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

    EXPECT_EQ( installedPackages.packages[ 0 ].packageName, interestedPrograms.product );
    EXPECT_EQ( installedPackages.packages[ 0 ].packageVersion, installedProgram.version );
}

TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillDiscoverManyPrograms )
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

    EXPECT_EQ( installedPackages.packages[ 0 ].packageName, "p1" );
    EXPECT_EQ( installedPackages.packages[ 0 ].packageVersion, installedProgram.version );

    EXPECT_EQ( installedPackages.packages[ 1 ].packageName, "p2" );
    EXPECT_EQ( installedPackages.packages[ 1 ].packageVersion, installedProgram.version );
}

TEST_F( TestPackageDiscovery, DiscoverInstalledPackagesWillPadVersionNumbers )
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

    EXPECT_EQ( installedPackages.packages[ 0 ].packageVersion, "1.0.0.0" );
}
