#include "stdafx.h"
#include "ProxyDiscoveryEngine.h"
#include "MockWinHttpWrapper.h"

class TestProxyDiscoveryEngine : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_Shutdown = false;
        m_ShutdownCallCount = 0;
        m_testUrl = L"";
        m_urlPAC = L"";
        m_winHttp.reset( new NiceMock<MockWinHttpWrapper>() );
        m_patient.reset( new ProxyDiscoveryEngine( *m_winHttp ) );
    };

    virtual void TearDown()
    {
        m_testUrl = L"";
        m_urlPAC = L"";
        m_patient.reset();
        m_winHttp.reset();
    }

    bool ShutdownAfterFirstCall() {
        bool shutdown = m_Shutdown;
        ++m_ShutdownCallCount;
        if( !m_Shutdown ) {
            m_Shutdown = true;
        }

        return shutdown;
    }
protected:
    std::unique_ptr<MockWinHttpWrapper> m_winHttp;
    std::unique_ptr<ProxyDiscoveryEngine> m_patient;

    std::wstring m_testUrl;
    std::wstring m_urlPAC;
    bool m_Shutdown;
    int m_ShutdownCallCount;
};

TEST_F( TestProxyDiscoveryEngine, WillTerminateEarly )
{
    m_patient->Init( m_testUrl.c_str(), m_urlPAC.c_str(), [this]() -> bool { return ShutdownAfterFirstCall(); });
    EXPECT_EQ( m_ShutdownCallCount, 2 );
}

TEST_F( TestProxyDiscoveryEngine, ProxyNoneReturnedWhenTerminatedEarly )
{
    m_testUrl = L"testUrl";
    m_urlPAC = L"urlPAC";
    int rtn = m_patient->Init( m_testUrl.c_str(), m_urlPAC.c_str(), [ this ]() -> bool { return ShutdownAfterFirstCall(); } );

    EXPECT_EQ( ( PROXY_FIND_METHOD )rtn, PROXY_FIND_NONE );
}

TEST_F( TestProxyDiscoveryEngine, ProxyListIsEmptyWhenTerminatedEarly )
{
    std::list<ProxyInfoModel> proxyList;
    m_testUrl = L"testUrl";
    m_urlPAC = L"urlPAC";

    m_patient->Init( m_testUrl.c_str(), m_urlPAC.c_str(), [ this ]() -> bool { return ShutdownAfterFirstCall(); } );
    m_patient->GetProxyInfo( &proxyList );

    EXPECT_TRUE( proxyList.empty() );
}