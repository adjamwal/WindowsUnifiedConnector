#include "stdafx.h"
#include "ThreadedProxyDiscovery.h"
#include "MockProxyDiscovery.h"
#include "MockProxyConsumer.h"

void WaitUntilTrueOrTimeout( bool& waitBool, int timeoutMs = 3000 )
{
    int elapsedTimeMs = 0;
    while( !waitBool && elapsedTimeMs < timeoutMs ) {
        Sleep( 10 );
        elapsedTimeMs += 10;
    }
}

class TestThreadedProxyDiscovery : public ::testing::Test
{
protected:

    virtual void SetUp()
    {
        m_proxyDiscovery.reset( new NiceMock<MockProxyDiscovery>() );
        m_patient.reset( new ThreadedProxyDiscovery( *m_proxyDiscovery ) );
    };

    virtual void TearDown()
    {
        m_patient.reset();
        m_proxyDiscovery.reset();
    }

protected:
    std::wstring m_testUrl;
    std::wstring m_urlPAC;
    std::unique_ptr<MockProxyDiscovery> m_proxyDiscovery;
    std::unique_ptr<ThreadedProxyDiscovery> m_patient;
};

TEST_F( TestThreadedProxyDiscovery, WillForwardRegisterForProxyNotifications )
{
    MockProxyConsumer proxyConsumer;

    EXPECT_CALL( *m_proxyDiscovery, RegisterForProxyNotifications( &proxyConsumer ) );

    m_patient->RegisterForProxyNotifications( &proxyConsumer );
}

TEST_F( TestThreadedProxyDiscovery, WillForwardUnRegisterForProxyNotifications )
{
    MockProxyConsumer proxyConsumer;

    EXPECT_CALL( *m_proxyDiscovery, UnregisterForProxyNotifications( &proxyConsumer ) );

    m_patient->UnregisterForProxyNotifications( &proxyConsumer );
}

TEST_F( TestThreadedProxyDiscovery, WillForwardProxyDiscoverySync )
{
    std::list<ProxyInfoModel> proxyList;

    EXPECT_CALL( *m_proxyDiscovery, ProxyDiscoverySync( m_testUrl.c_str(), m_urlPAC.c_str(), proxyList ) );

    m_patient->ProxyDiscoverySync( m_testUrl.c_str(), m_urlPAC.c_str(), proxyList );
}

/*
TEST_F( TestThreadedProxyDiscovery, WillCallStartProxyDiscoveryFromANewThread )
{
    static int proxyDiscoveryThreadId = GetCurrentThreadId();
    DWORD testThreadId = GetCurrentThreadId();
    ON_CALL( *m_proxyDiscovery, StartProxyDiscovery( _, _ ) ).WillByDefault( testing::Invoke( []( LPCTSTR a, LPCTSTR b )
        {
            proxyDiscoveryThreadId = GetCurrentThreadId();
        } ) );

    m_patient->StartProxyDiscovery( m_testUrl.c_str(), m_urlPAC.c_str() );

    TIMED_EXPECT_NE( 1000, proxyDiscoveryThreadId, testThreadId );
}
*/

TEST_F( TestThreadedProxyDiscovery, WillNotStartTwoProxyDiscoveryThread )
{
    static bool exitThread = false;
    static bool attemptSecondDiscoveryStart = false;
    ON_CALL( *m_proxyDiscovery, StartProxyDiscoveryAsync( _, _ ) ).WillByDefault( testing::Invoke( []( LPCTSTR a, LPCTSTR b )
        {
            attemptSecondDiscoveryStart = true;
            WaitUntilTrueOrTimeout( exitThread );
        } ) );
    m_patient->StartProxyDiscoveryAsync( m_testUrl.c_str(), m_urlPAC.c_str() );
    WaitUntilTrueOrTimeout( attemptSecondDiscoveryStart );

    EXPECT_CALL( *m_proxyDiscovery, StartProxyDiscoveryAsync( m_testUrl.c_str(), m_urlPAC.c_str() ) ).Times( 0 );

    m_patient->StartProxyDiscoveryAsync( m_testUrl.c_str(), m_urlPAC.c_str() );
    exitThread = true;
}

TEST_F( TestThreadedProxyDiscovery, WillStartProxyDiscoveryAgainAfterFirstAttempt )
{
    EXPECT_CALL( *m_proxyDiscovery, StartProxyDiscoveryAsync( m_testUrl.c_str(), m_urlPAC.c_str() ) ).Times( 2 );

    m_patient->StartProxyDiscoveryAsync( m_testUrl.c_str(), m_urlPAC.c_str() );
    Sleep( 100 );
    m_patient->StartProxyDiscoveryAsync( m_testUrl.c_str(), m_urlPAC.c_str() );
}

TEST_F( TestThreadedProxyDiscovery, WillNotCrashWhenClassIsCleanedUpBeforeThreadExit )
{
    static bool exitThread = false;
    static bool startClassCleanup = false;
    ON_CALL( *m_proxyDiscovery, StartProxyDiscoveryAsync( _, _ ) ).WillByDefault( testing::Invoke( []( LPCTSTR a, LPCTSTR b )
        {
            startClassCleanup = true;
            WaitUntilTrueOrTimeout( exitThread, 100 );
        } ) );
    m_patient->StartProxyDiscoveryAsync( m_testUrl.c_str(), m_urlPAC.c_str() );
    WaitUntilTrueOrTimeout( startClassCleanup );

    m_patient.reset();
}

TEST_F( TestThreadedProxyDiscovery, WillIgnoreShutdownCallback )
{
    EXPECT_CALL( *m_proxyDiscovery, SetShutdownCallback( _ ) ).Times( 0 );

    m_patient->SetShutdownCallback( []() -> bool { return false; } );
}
