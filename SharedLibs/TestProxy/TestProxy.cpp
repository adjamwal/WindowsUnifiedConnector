#include "stdafx.h"
#include "Proxy.h"
#include "MockWinHttpWrapper.h"

class TestProxy : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_Shutdown = false;
        m_ShutdownCallCount = 0;
        m_testUrl = L"";
        m_urlPAC = L"";
        m_winHttp.reset( new NiceMock<MockWinHttpWrapper>() );
        m_patient.reset( new Proxy( *m_winHttp ) );
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
    std::unique_ptr<Proxy> m_patient;

    std::wstring m_testUrl;
    std::wstring m_urlPAC;
    bool m_Shutdown;
    int m_ShutdownCallCount;
};

TEST_F( TestProxy, WillTerminateEarly )
{
    m_patient->Init( m_testUrl.c_str(), m_urlPAC.c_str(), [this]() -> bool { return ShutdownAfterFirstCall(); });
    EXPECT_EQ( m_ShutdownCallCount, 2 );
}

TEST_F( TestProxy, ProxyNoneReturnedWhenTerminatedEarly )
{
    m_testUrl = L"testUrl";
    m_urlPAC = L"urlPAC";
    int rtn = m_patient->Init( m_testUrl.c_str(), m_urlPAC.c_str(), [ this ]() -> bool { return ShutdownAfterFirstCall(); } );

    EXPECT_EQ( ( PROXY_INFO_SRC )rtn, PROXY_INFO_NONE );
}

TEST_F( TestProxy, ProxyListIsEmptyWhenTerminatedEarly )
{
    std::list<ProxyInfoModel> proxyList;
    m_testUrl = L"testUrl";
    m_urlPAC = L"urlPAC";

    m_patient->Init( m_testUrl.c_str(), m_urlPAC.c_str(), [ this ]() -> bool { return ShutdownAfterFirstCall(); } );
    m_patient->GetProxyInfo( &proxyList );

    EXPECT_TRUE( proxyList.empty() );
}