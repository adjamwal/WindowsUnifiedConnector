#include "pch.h"
#include "IPmPlatformComponentManager.h"
#include "WindowsComponentManager.h"
#include "IUcLogger.h"
#include "PmTypes.h"
#include "MockWinApiWrapper.h"
#include <memory>

class ComponentTestWindowsPackageManager : public ::testing::Test
{
protected:
    void SetUp()
    {
        m_mockWinApiWrapper = std::make_unique<NiceMock<MockWinApiWrapper>>();
        m_patient = std::make_unique<WindowsComponentManager>( *m_mockWinApiWrapper );
    }

    void TearDown()
    {
        m_patient.reset();
        m_mockWinApiWrapper.reset();
    }

    std::unique_ptr<MockWinApiWrapper> m_mockWinApiWrapper;
    std::unique_ptr<WindowsComponentManager> m_patient;
};

TEST_F( ComponentTestWindowsPackageManager, UpdateComponentSuccess )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    m_mockWinApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_mockWinApiWrapper->MakeGetExitCodeProcessReturn( TRUE );
    m_mockWinApiWrapper->MakeSHGetKnownFolderPathReturn( 0 );
    m_mockWinApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_mockWinApiWrapper->MakeGetLastErrorReturn( 0 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ret, 0 );
    EXPECT_EQ( error, "" );
}

TEST_F( ComponentTestWindowsPackageManager, UpdateComponentInvalidPackageType )
{
    std::string error;
    PmComponent c;
    c.installerType = "INVALID";

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_NE( ret, 0 );
    EXPECT_NE( error, "" );
}

TEST_F( ComponentTestWindowsPackageManager, UpdateComponentFailureToGetSystemDirectory )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    m_mockWinApiWrapper->MakeSHGetKnownFolderPathReturn( -1 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_NE( ret, 0 );
    EXPECT_NE( error, "" );
}


TEST_F( ComponentTestWindowsPackageManager, UpdateComponentCreateProcessFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    m_mockWinApiWrapper->MakeCreateProcessWReturn( FALSE );
    m_mockWinApiWrapper->MakeGetLastErrorReturn( 5 );

    int32_t ret = m_patient->UpdateComponent( c, error );
    
    EXPECT_EQ( ret, 5 );
    EXPECT_NE( error, "" );
}

TEST_F( ComponentTestWindowsPackageManager, UpdateWaitForSingleObjectFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    m_mockWinApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_mockWinApiWrapper->MakeWaitForSingleObjectReturn( -1 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ret, -1 );
    EXPECT_NE( error, "" );
}

TEST_F( ComponentTestWindowsPackageManager, UpdateComponentExitCodeProcessFailure )
{
    std::string error;
    PmComponent c;
    c.installerType = "msi";

    m_mockWinApiWrapper->MakeCreateProcessWReturn( TRUE );
    m_mockWinApiWrapper->MakeWaitForSingleObjectReturn( 0 );
    m_mockWinApiWrapper->MakeGetExitCodeProcessReturn( FALSE );
    m_mockWinApiWrapper->MakeGetLastErrorReturn( 5 );

    int32_t ret = m_patient->UpdateComponent( c, error );

    EXPECT_EQ( ret, 5 );
    EXPECT_NE( error, "" );
}
