#include "pch.h"
#include "WindowsConfiguration.h"
#include "MockWinCertLoader.h"
#include "MockCodesignVerifier.h"
#include "MockWindowsUtilities.h"
#include "MockProxyDiscovery.h"
#include <memory>

class TestWindowsConfiguration: public ::testing::Test
{
protected:
    void SetUp()
    {
        MockWindowsUtilities::Init();
        m_winCertLoader.reset( new NiceMock<MockWinCertLoader>() );
        m_codeSignVerifier.reset( new NiceMock<MockCodesignVerifier>() );
        m_proxyDiscovery.reset( new NiceMock<MockProxyDiscovery>() );

        m_patient.reset( new WindowsConfiguration( *m_winCertLoader, *m_codeSignVerifier, *m_proxyDiscovery ) );
    }

    void TearDown()
    {
        m_patient.reset();

        m_winCertLoader.reset();
        m_codeSignVerifier.reset();
        m_proxyDiscovery.reset();

        MockWindowsUtilities::Deinit();
    }

    std::unique_ptr<MockWinCertLoader> m_winCertLoader;
    std::unique_ptr<MockCodesignVerifier> m_codeSignVerifier;
    std::unique_ptr<MockProxyDiscovery> m_proxyDiscovery;

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

void ProxyCallback(void* context, const std::list<PmProxy>& proxyList)
{
    *static_cast<bool*>(context) = true;
}

TEST_F(TestWindowsConfiguration, StartProxyDiscoveryAsyncWillGetNotification)
{
    bool notified = false;

    ON_CALL(*m_proxyDiscovery, StartProxyDiscoveryAsync(_, _)).WillByDefault(Invoke([this](const LPCTSTR, const LPCTSTR) {
        // Need to simulate the proxy discovery thread
        std::thread discoverThread([this] { m_patient->ProxiesDiscovered({}); } );
        discoverThread.detach();
        }));

    EXPECT_TRUE(m_patient->StartProxyDiscoveryAsync("TestUrl", "TestPacUrl", ProxyCallback, &notified));

    Sleep( 10 ); // RD: I'm lazy
    EXPECT_TRUE(notified);
}