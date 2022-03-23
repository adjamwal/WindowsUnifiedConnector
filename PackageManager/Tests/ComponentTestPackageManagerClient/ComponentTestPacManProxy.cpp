#include "ComponentTestPacManBase.h"
#include "StringUtil.h"

MATCHER_P( StrMatch, expected, "" ) {
    return std::string( arg )._Equal( std::string( expected ) );
}

class ComponentTestPacManProxy : public ComponentTestPacManBase
{
public:
    virtual ~ComponentTestPacManProxy() {}
};

std::string _ucNullReponse( R"(
{
  "packages": null
}
)" );

TEST_F( ComponentTestPacManProxy, PacManWillGetProxyDiscoveryInstanceFromPlatformDependencies )
{
    m_mockCloud->MakeCheckinReturn( true, _ucNullReponse, { 200, 0 } );
    ON_CALL( *m_mockProxyDiscovery, StartProxyDiscoveryAsync( _, _ ) ).WillByDefault( Invoke(
        [this]( const LPCTSTR testURL, const LPCTSTR urlPAC )
        {
            m_proxyDiscoverySubscriber->ProxiesDiscovered( m_proxyList );
        }
    ) );

    ON_CALL( *m_mockProxyDiscovery, ProxyDiscoverAndNotifySync( _, _, _ ) ).WillByDefault( Invoke(
        [this]( const LPCTSTR testURL, const LPCTSTR urlPAC, std::list<ProxyInfoModel>& proxyList )
        {
            m_proxyDiscoverySubscriber->ProxiesDiscovered( m_proxyList );
        }
    ) );

    m_mockConfig->MakeGetCloudCheckinUriReturn( "checkinuri" );
    m_mockConfig->MakeGetCloudEventUriReturn( "eventuri" );
    m_mockConfig->MakeGetCloudCatalogUriReturn( "cataloguri" );

    ON_CALL( *m_httpForProxyTesting, HttpGet( _, _, _ ) ).WillByDefault( Invoke(
        []( const std::string& url, std::string& responseContent, PmHttpExtendedResult& eResult )
        {
            responseContent = "";
            eResult.httpResponseCode = 0;
            eResult.subErrorType = "curl";
            eResult.subErrorCode = CURLE_PEER_FAILED_VERIFICATION;
            return true;
        }
    ) );

    m_mockPlatformConfiguration->ExpectGetProxyDiscoveryCalledOnce();

    StartPacManNoConfig();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();
}

TEST_F( ComponentTestPacManProxy, PacManWillDetectVerifyAndSetProxy )
{
    m_mockCloud->MakeCheckinReturn( true, _ucNullReponse, { 200, 0 } );
    ON_CALL( *m_mockProxyDiscovery, StartProxyDiscoveryAsync( _, _ ) ).WillByDefault( Invoke(
        [this]( const LPCTSTR testURL, const LPCTSTR urlPAC )
        {
            m_proxyDiscoverySubscriber->ProxiesDiscovered( m_proxyList );
        }
    ) );

    ON_CALL( *m_mockProxyDiscovery, ProxyDiscoverAndNotifySync( _, _, _ ) ).WillByDefault( Invoke(
        [this]( const LPCTSTR testURL, const LPCTSTR urlPAC, std::list<ProxyInfoModel>& proxyList )
        {
            m_proxyDiscoverySubscriber->ProxiesDiscovered( m_proxyList );
        }
    ) );

    m_mockConfig->MakeGetCloudCheckinUriReturn( "checkinuri" );
    m_mockConfig->MakeGetCloudEventUriReturn( "eventuri" );
    m_mockConfig->MakeGetCloudCatalogUriReturn( "cataloguri" );

    ON_CALL( *m_httpForProxyTesting, HttpGet( _, _, _ ) ).WillByDefault( Invoke(
        []( const std::string& url, std::string& responseContent, PmHttpExtendedResult& eResult )
        {
            responseContent = "";
            eResult.httpResponseCode = 0;
            eResult.subErrorType = "curl";
            eResult.subErrorCode = CURLE_PEER_FAILED_VERIFICATION;
            return true;
        }
    ) );

    EXPECT_CALL( *m_httpForProxyTesting,
        SetHttpProxy(
            StrMatch( "proxyServer:8080" ),
            StrMatch( "proxyUser" ),
            StrMatch( "proxyPass" ) )
    ).Times( 1 );
    EXPECT_CALL( *m_httpForProxyTesting, HttpGet( _, _, _ ) ).Times( 3 );
    EXPECT_CALL( *m_http,
        SetHttpProxy(
            StrMatch( "proxyServer:8080" ),
            StrMatch( "proxyUser" ),
            StrMatch( "proxyPass" ) )
    ).Times( 1 );

    StartPacManNoConfig();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();
}

TEST_F( ComponentTestPacManProxy, PacManWillDetectAndFailToVerifyIfConfigUrisMissing )
{
    m_mockCloud->MakeCheckinReturn( true, _ucNullReponse, { 200, 0 } );
    ON_CALL( *m_mockProxyDiscovery, StartProxyDiscoveryAsync( _, _ ) ).WillByDefault( Invoke(
        [this]( const LPCTSTR testURL, const LPCTSTR urlPAC )
        {
            m_proxyDiscoverySubscriber->ProxiesDiscovered( m_proxyList );
        }
    ) );

    ON_CALL( *m_mockProxyDiscovery, ProxyDiscoverAndNotifySync( _, _, _ ) ).WillByDefault( Invoke(
        [this]( const LPCTSTR testURL, const LPCTSTR urlPAC, std::list<ProxyInfoModel>& proxyList )
        {
            m_proxyDiscoverySubscriber->ProxiesDiscovered( m_proxyList );
        }
    ) );

    m_mockBootstrap->MakeGetIdentifyUriReturn( "" );
    m_mockConfig->MakeGetCloudCheckinUriReturn( "" );
    m_mockConfig->MakeGetCloudEventUriReturn( "" );
    m_mockConfig->MakeGetCloudCatalogUriReturn( "" );

    EXPECT_CALL( *m_httpForProxyTesting, SetHttpProxy( _, _, _ ) ).Times( 0 );
    EXPECT_CALL( *m_httpForProxyTesting, HttpGet( _, _, _ ) ).Times( 0 );
    EXPECT_CALL( *m_http, SetHttpProxy(
        StrMatch( "" ),
        StrMatch( "" ),
        StrMatch( "" ) )
    ).Times( 2 );

    StartPacManNoConfig();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();
}

TEST_F( ComponentTestPacManProxy, PacManWillDetectAndFailToVerifyIfResponseCodeUnexpected )
{
    m_mockCloud->MakeCheckinReturn( true, _ucNullReponse, { 200, 0 } );
    ON_CALL( *m_mockProxyDiscovery, StartProxyDiscoveryAsync( _, _ ) ).WillByDefault( Invoke(
        [this]( const LPCTSTR testURL, const LPCTSTR urlPAC )
        {
            m_proxyDiscoverySubscriber->ProxiesDiscovered( m_proxyList );
        }
    ) );

    ON_CALL( *m_mockProxyDiscovery, ProxyDiscoverAndNotifySync( _, _, _ ) ).WillByDefault( Invoke(
        [this]( const LPCTSTR testURL, const LPCTSTR urlPAC, std::list<ProxyInfoModel>& proxyList )
        {
            m_proxyDiscoverySubscriber->ProxiesDiscovered( m_proxyList );
        }
    ) );

    m_mockConfig->MakeGetCloudCheckinUriReturn( "checkinuri" );
    m_mockConfig->MakeGetCloudEventUriReturn( "eventuri" );
    m_mockConfig->MakeGetCloudCatalogUriReturn( "cataloguri" );

    ON_CALL( *m_httpForProxyTesting, HttpGet( _, _, _ ) ).WillByDefault( Invoke(
        []( const std::string& url, std::string& responseContent, PmHttpExtendedResult& eResult )
        {
            responseContent = "";
            eResult.httpResponseCode = 0;
            eResult.subErrorType = "";
            eResult.subErrorCode = 0;
            return true;
        }
    ) );

    EXPECT_CALL( *m_httpForProxyTesting,
        SetHttpProxy(
            StrMatch( "proxyServer:8080" ),
            StrMatch( "proxyUser" ),
            StrMatch( "proxyPass" ) )
    ).Times( 2 );
    EXPECT_CALL( *m_httpForProxyTesting, HttpGet( _, _, _ ) ).Times( 4 );
    EXPECT_CALL( *m_http,
        SetHttpProxy(
            StrMatch( "" ),
            StrMatch( "" ),
            StrMatch( "" ) )
    ).Times( 2 );

    StartPacManNoConfig();

    std::unique_lock<std::mutex> lock( m_mutex );
    m_cv.wait_for( lock, std::chrono::seconds( 2 ) );
    lock.unlock();
}
