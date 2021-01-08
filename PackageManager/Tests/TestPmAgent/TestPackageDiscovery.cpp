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

TEST_F( TestPackageDiscovery, GetInstalledPackagesWillSetOS )
{
    std::vector<PmDiscoveryComponent> discoveryList;

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );

    PackageInventory installedPackages = m_patient->GetInstalledPackages( discoveryList );

    EXPECT_EQ( installedPackages.architecture, "x64" );
    EXPECT_EQ( installedPackages.platform, "win" );
}


TEST_F( TestPackageDiscovery, GetInstalledPackagesWillGetUC )
{
    std::vector<PmDiscoveryComponent> discoveryList;
    PmDiscoveryComponent interestedPrograms;
    interestedPrograms.packageId = "uc";
    interestedPrograms.packageName = "Unified Connector";
    discoveryList.push_back( interestedPrograms );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );

    PackageInventory installedPackages = m_patient->GetInstalledPackages( discoveryList );

    EXPECT_EQ( installedPackages.packages.front().packageName, interestedPrograms.packageId );
    EXPECT_EQ( installedPackages.packages.front().configs.size(), 3 );
}

TEST_F( TestPackageDiscovery, GetInstalledPackagesWillGetImmuent )
{
    std::vector<PmDiscoveryComponent> discoveryList;
    PmDiscoveryComponent interestedPrograms;
    interestedPrograms.packageId = "amp";
    interestedPrograms.packageName = "Immunet";
    discoveryList.push_back( interestedPrograms );
    
    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryString( _, _, std::wstring( L"DisplayName" ), _ ) )
        .WillByDefault( DoAll( SetArgReferee<3>( L"Immunet" ), Return( true ) ) );
    ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryString( _, _, std::wstring( L"DisplayVersion" ), _ ) )
        .WillByDefault( Return( true ) );

    PackageInventory installedPackages = m_patient->GetInstalledPackages( discoveryList );

    EXPECT_EQ( installedPackages.packages.front().packageName, interestedPrograms.packageId );
}

TEST_F( TestPackageDiscovery, GetInstalledPackagesWillGetAmp )
{
    std::vector<PmDiscoveryComponent> discoveryList;
    PmDiscoveryComponent interestedPrograms;
    interestedPrograms.packageId = "amp";
    interestedPrograms.packageName = "Cisco AMP for Endpoints Connector";
    discoveryList.push_back( interestedPrograms );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryString( _, _, std::wstring( L"DisplayName" ), _ ) )
        .WillByDefault( DoAll( SetArgReferee<3>( L"Cisco AMP for Endpoints Connector" ), Return( true ) ) );
    ON_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), ReadRegistryString( _, _, std::wstring( L"DisplayVersion" ), _ ) )
        .WillByDefault( Return( true ) );

    PackageInventory installedPackages = m_patient->GetInstalledPackages( discoveryList );

    EXPECT_EQ( installedPackages.packages.front().packageName, interestedPrograms.packageId );
}

TEST_F( TestPackageDiscovery, BuildAmpWillFailOnRegistryFailure )
{
    std::vector<PmDiscoveryComponent> discoveryList;
    PmDiscoveryComponent interestedPrograms;
    interestedPrograms.packageId = "amp";
    interestedPrograms.packageName = "Cisco AMP for Endpoints Connector";
    discoveryList.push_back( interestedPrograms );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeReadRegistryStringReturn( false );

    PackageInventory installedPackages = m_patient->GetInstalledPackages( discoveryList );

    EXPECT_EQ( installedPackages.packages.size(), 0 );
}


TEST_F( TestPackageDiscovery, GetInstalledPackagesWillDiscoverPrograms )
{
    std::vector<PmDiscoveryComponent> discoveryList;
    std::vector<WindowsUtilities::WindowsInstallProgram> installedList;

    PmDiscoveryComponent interestedPrograms;
    interestedPrograms.packageId = "p1";
    interestedPrograms.packageName = "Package1";
    discoveryList.push_back( interestedPrograms );

    WindowsUtilities::WindowsInstallProgram installedProgram;
    installedProgram.name = interestedPrograms.packageName;
    installedProgram.version = "1.0.0.0";
    installedList.push_back( installedProgram );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetInstalledProgramsReturn( installedList );

    PackageInventory installedPackages = m_patient->GetInstalledPackages( discoveryList );

    EXPECT_EQ( installedPackages.packages[ 0 ].packageName, interestedPrograms.packageId );
    EXPECT_EQ( installedPackages.packages[ 0 ].packageVersion, installedProgram.version );
}

TEST_F( TestPackageDiscovery, GetInstalledPackagesWillDiscoverManyPrograms )
{
    std::vector<PmDiscoveryComponent> discoveryList;
    std::vector<WindowsUtilities::WindowsInstallProgram> installedList;

    PmDiscoveryComponent interestedPrograms;
    interestedPrograms.packageId = "p1";
    interestedPrograms.packageName = "Package";
    discoveryList.push_back( interestedPrograms );

    interestedPrograms.packageId = "p2";
    interestedPrograms.packageName = "Package";
    discoveryList.push_back( interestedPrograms );

    WindowsUtilities::WindowsInstallProgram installedProgram;
    installedProgram.name = interestedPrograms.packageName;
    installedProgram.version = "1.0.0.0";
    installedList.push_back( installedProgram );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetInstalledProgramsReturn( installedList );

    PackageInventory installedPackages = m_patient->GetInstalledPackages( discoveryList );

    EXPECT_EQ( installedPackages.packages[ 0 ].packageName, "p1" );
    EXPECT_EQ( installedPackages.packages[ 0 ].packageVersion, installedProgram.version );

    EXPECT_EQ( installedPackages.packages[ 1 ].packageName, "p2" );
    EXPECT_EQ( installedPackages.packages[ 1 ].packageVersion, installedProgram.version );
}

TEST_F( TestPackageDiscovery, GetInstalledPackagesWillPadVersionNumbers )
{
    std::vector<PmDiscoveryComponent> discoveryList;
    std::vector<WindowsUtilities::WindowsInstallProgram> installedList;

    PmDiscoveryComponent interestedPrograms;
    interestedPrograms.packageId = "p1";
    interestedPrograms.packageName = "Package1";
    discoveryList.push_back( interestedPrograms );

    WindowsUtilities::WindowsInstallProgram installedProgram;
    installedProgram.name = interestedPrograms.packageName;
    installedProgram.version = "1";
    installedList.push_back( installedProgram );

    MockWindowsUtilities::GetMockWindowUtilities()->MakeIs64BitWindowsReturn( true );
    MockWindowsUtilities::GetMockWindowUtilities()->MakeGetInstalledProgramsReturn( installedList );

    PackageInventory installedPackages = m_patient->GetInstalledPackages( discoveryList );

    EXPECT_EQ( installedPackages.packages[ 0 ].packageVersion, "1.0.0.0" );
}
