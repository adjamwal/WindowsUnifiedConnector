#include "stdafx.h"
#include "ProxyDiscovery.h"
#include "MockProxy.h"
#include "MockProxyConsumer.h"

class TestProxyDiscovery : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_testUrl = L"";
        m_urlPAC = L"";
        m_proxy.reset( new NiceMock<MockProxy>() );
        m_patient.reset( new ProxyDiscovery( m_proxy.get() ) );
    };

    virtual void TearDown()
    {
        m_testUrl = L"";
        m_urlPAC = L"";
        m_patient.reset();
        m_proxy.reset();
    }

    PROXY_INFO_LIST SomeProxyInfos()
    {
        PROXY_INFO_LIST proxies;
        ProxyInfoModel proxy1;
        proxy1.SetProxyServer( L"Server1" );
        proxies.push_back( proxy1 );
        ProxyInfoModel proxy2;
        proxy2.SetProxyServer( L"Server2" );
        proxies.push_back( proxy2 );
        ProxyInfoModel proxy3;
        proxy3.SetProxyServer( L"Server3" );
        proxies.push_back( proxy3 );

        return proxies;
    }

protected:
    std::wstring m_testUrl;
    std::wstring m_urlPAC;
    std::unique_ptr<MockProxy> m_proxy;
    std::unique_ptr<ProxyDiscovery> m_patient;
};

TEST_F( TestProxyDiscovery, WillFailToAddNullConsumer )
{
    EXPECT_FALSE( m_patient->RegisterForProxyNotifications( NULL ) );
}

TEST_F( TestProxyDiscovery, CanAddConsumerForProxyNotifications )
{
    MockProxyConsumer consumer;
    testing::internal::CaptureStdout();

    EXPECT_TRUE( m_patient->RegisterForProxyNotifications( &consumer ) );

    std::string testOutput = testing::internal::GetCapturedStdout();
    EXPECT_NE( testOutput.find( "Added consumer" ), std::string::npos );
}

TEST_F( TestProxyDiscovery, CanNotAddSameConsumerTwice )
{
    MockProxyConsumer consumer;
    testing::internal::CaptureStdout();

    m_patient->RegisterForProxyNotifications( &consumer );
    EXPECT_FALSE( m_patient->RegisterForProxyNotifications( &consumer ) );

    std::string testOutput = testing::internal::GetCapturedStdout();
    EXPECT_NE( testOutput.find( "Consumer is already registered" ), std::string::npos );
}

TEST_F( TestProxyDiscovery, CanAddMultipleConsumers )
{
    MockProxyConsumer consumer1;
    MockProxyConsumer consumer2;

    EXPECT_TRUE( m_patient->RegisterForProxyNotifications( &consumer1 ) );
    EXPECT_TRUE( m_patient->RegisterForProxyNotifications( &consumer2 ) );
}

TEST_F( TestProxyDiscovery, WillFailToUnregisterNullConsumer )
{
    EXPECT_FALSE( m_patient->UnregisterForProxyNotifications( NULL ) );
}

TEST_F( TestProxyDiscovery, WillFailToUnregisterUnknownConsumer )
{
    MockProxyConsumer consumer;

    EXPECT_FALSE( m_patient->UnregisterForProxyNotifications( &consumer ) );
}

TEST_F( TestProxyDiscovery, WillRemoveKnownConsumer )
{
    MockProxyConsumer consumer;
    m_patient->RegisterForProxyNotifications( &consumer );

    EXPECT_TRUE( m_patient->UnregisterForProxyNotifications( &consumer ) );
}

TEST_F( TestProxyDiscovery, WillGetProxyinfoInSequence )
{
    InSequence s;
    EXPECT_CALL( *m_proxy, Init( _, _, _ ) );
    EXPECT_CALL( *m_proxy, GetProxyInfo(_) );

    m_patient->StartProxyDiscovery( m_testUrl.c_str(), m_urlPAC.c_str() );
}

TEST_F( TestProxyDiscovery, WillNotifyConsumerOfDiscoveredProxies )
{
    MockProxyConsumer consumer;
    m_patient->RegisterForProxyNotifications( &consumer );
    PROXY_INFO_LIST proxies = SomeProxyInfos();
    ON_CALL( *m_proxy, GetProxyInfo( _ ) ).WillByDefault(DoAll( 
        SetArgPointee<0>( proxies ),
        Return(TRUE)));

    EXPECT_CALL( consumer, ProxiesDiscovered( proxies ) );

    m_patient->StartProxyDiscovery( m_testUrl.c_str(), m_urlPAC.c_str() );
}

TEST_F( TestProxyDiscovery, WillNotifyConsumersOfProxiesWhenDoneDiscovery )
{
    MockProxyConsumer consumer1;
    MockProxyConsumer consumer2;
    m_patient->RegisterForProxyNotifications( &consumer1 );
    m_patient->RegisterForProxyNotifications( &consumer2 );

    EXPECT_CALL( consumer1, ProxiesDiscovered( _ ) );
    EXPECT_CALL( consumer2, ProxiesDiscovered( _ ) );

    m_patient->StartProxyDiscovery( m_testUrl.c_str(), m_urlPAC.c_str() );
}

TEST_F( TestProxyDiscovery, WillDiscoveryProxiesWithoutCallback )
{
    PROXY_INFO_LIST proxyList;
    PROXY_INFO_LIST expectedProxies = SomeProxyInfos();
    ON_CALL( *m_proxy, GetProxyInfo( _ ) ).WillByDefault( DoAll(
        SetArgPointee<0>( expectedProxies ),
        Return( TRUE ) ) );

    m_patient->StartProxyDiscoverySync( m_testUrl.c_str(), m_urlPAC.c_str(), proxyList );

    EXPECT_EQ( proxyList, expectedProxies );
}

TEST_F( TestProxyDiscovery, WillGetProxyinfoInSequenceSynchronous )
{
    PROXY_INFO_LIST proxyList;

    InSequence s;
    EXPECT_CALL( *m_proxy, Init( _, _, _ ) );
    EXPECT_CALL( *m_proxy, GetProxyInfo( _ ) );

    m_patient->StartProxyDiscoverySync( m_testUrl.c_str(), m_urlPAC.c_str(), proxyList);
}
