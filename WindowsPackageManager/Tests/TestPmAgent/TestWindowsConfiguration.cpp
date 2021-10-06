#include "pch.h"
#include "WindowsConfiguration.h"
#include "MockWinCertLoader.h"
#include "MockCodesignVerifier.h"
#include "MockWindowsUtilities.h"
#include <memory>

class TestWindowsConfiguration: public ::testing::Test
{
protected:
    void SetUp()
    {
        MockWindowsUtilities::Init();
        m_winCertLoader.reset( new NiceMock<MockWinCertLoader>() );
        m_codeSignVerifier.reset( new NiceMock<MockCodesignVerifier>() );

        m_patient.reset( new WindowsConfiguration( *m_winCertLoader, *m_codeSignVerifier ) );
    }

    void TearDown()
    {
        m_patient.reset();

        m_winCertLoader.reset();
        m_codeSignVerifier.reset();

        MockWindowsUtilities::Deinit();
    }

    std::unique_ptr<MockWinCertLoader> m_winCertLoader;
    std::unique_ptr<MockCodesignVerifier> m_codeSignVerifier;

    std::unique_ptr<WindowsConfiguration> m_patient;
};

TEST_F( TestWindowsConfiguration, UpdateCertStoreForUrlWillSucceed )
{
    MockWindowsUtilities::GetMockWindowUtilities()->MakeWinHttpGetReturn( true );

    EXPECT_TRUE( m_patient->UpdateCertStoreForUrl( "http://SomeDomain/api" ) );
}

TEST_F( TestWindowsConfiguration, UpdateCertStoreForUrlWillOnlyQueryDomain )
{
    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), WinHttpGet( "SomeDomain" ) );

    m_patient->UpdateCertStoreForUrl( "http://SomeDomain/api" );
}

TEST_F( TestWindowsConfiguration, UpdateCertStoreForUrlWillQueryAllStoredDomains )
{
    //Store first domain
    m_patient->UpdateCertStoreForUrl( "http://SomeOtherDomain/api" );

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), WinHttpGet( _ ) ).Times( 2 );

    m_patient->UpdateCertStoreForUrl( "http://SomeDomain/api" );
}

TEST_F( TestWindowsConfiguration, UpdateCertStoreForUrlWillQueryOnlyUniqueDomains )
{
    //Store first domain
    m_patient->UpdateCertStoreForUrl( "http://SomeDomain/Otherapi" );

    EXPECT_CALL( *MockWindowsUtilities::GetMockWindowUtilities(), WinHttpGet( _ ) ).Times( 1 );

    m_patient->UpdateCertStoreForUrl( "http://SomeDomain/api" );
}

TEST_F( TestWindowsConfiguration, UpdateCertStoreForUrlWillFail)
{
    MockWindowsUtilities::GetMockWindowUtilities()->MakeWinHttpGetReturn( false );

    EXPECT_FALSE( m_patient->UpdateCertStoreForUrl( "http://SomeDomain/api" ) );
}