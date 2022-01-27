#include "PmProxyVerifier.h"
#include "IProxyDiscoveryEngine.h"
#include "IPmHttp.h"
#include "PmConfig.h"
#include "StringUtil.h"
#include "PmLogger.h"
#include "curl.h"

PmProxyVerifier::PmProxyVerifier( IPmHttp& testHttp, IPmConfig& pmConfig )
    : m_testHttp( testHttp )
    , m_pmConfig( pmConfig )
    , m_isBusy( false )
{
    PmHttpExtendedResult eResult;
    if( !m_testHttp.Init( NULL, NULL, "PmProxyVerifier", eResult ) )
    {
        LOG_ERROR( "ProxyVerifierPmHttp Init Failed: subErrorCode:%d, subErrorDesc:%s",
            eResult.subErrorCode,
            m_testHttp.GetErrorDescription( eResult.subErrorCode ).c_str() );
    }
}

PmProxyVerifier::~PmProxyVerifier()
{
    m_testHttp.Deinit();
}

bool PmProxyVerifier::IsBusy()
{
    return m_isBusy;
}

bool PmProxyVerifier::IsValidProxy( const ProxyInfoModel& testProxy )
{
    if( IsBusy() ) return false; //prevent re-entry using an atomic bool, without engaging the mutex

    std::lock_guard<std::mutex> lock( m_mutex );
    m_isBusy = true;

    bool isValid = false;

    try
    {
        if( !StringUtil::Trim( testProxy.GetProxyServer() ).empty() &&
            testProxy.GetProxyPort() > 0 &&
            ( testProxy.GetProxyType() == L"http_proxy" || testProxy.GetProxyType() == L"http" ) )
        {
            if( m_pmConfig.GetCloudCheckinUri().empty() )
            {
                WLOG_DEBUG( L"Can't validate proxy '%s:%d', type '%s': CloudCheckinUri is empty",
                    testProxy.GetProxyServer().c_str(), testProxy.GetProxyPort(), testProxy.GetProxyType().c_str() );
            }
            else if( m_pmConfig.GetCloudEventUri().empty() )
            {
                WLOG_DEBUG( L"Can't validate proxy '%s:%d', type '%s': CloudEventUri is empty",
                    testProxy.GetProxyServer().c_str(), testProxy.GetProxyPort(), testProxy.GetProxyType().c_str() );
            }
            else if( m_pmConfig.GetCloudCatalogUri().empty() )
            {
                WLOG_DEBUG( L"Can't validate proxy '%s:%d', type '%s': CloudCatalogUri is empty",
                    testProxy.GetProxyServer().c_str(), testProxy.GetProxyPort(), testProxy.GetProxyType().c_str() );
            }
            else
            {
                WLOG_DEBUG( L"Validating proxy '%s:%d', type '%s'",
                    testProxy.GetProxyServer().c_str(), testProxy.GetProxyPort(), testProxy.GetProxyType().c_str() );

                m_testHttp.SetHttpProxy(
                    StringUtil::Trim( StringUtil::WStr2Str( testProxy.GetProxyServer() ) ) + ":" + std::to_string( testProxy.GetProxyPort() ),
                    StringUtil::WStr2Str( testProxy.GetProxyUser() ),
                    StringUtil::WStr2Str( testProxy.GetProxyPassword() )
                );

                isValid =
                    CanReachUrl( m_pmConfig.GetCloudCheckinUri() ) &&
                    CanReachUrl( m_pmConfig.GetCloudEventUri() ) &&
                    CanReachUrl( m_pmConfig.GetCloudCatalogUri() );

                WLOG_DEBUG( L"Proxy '%s:%d', type '%s' %s", testProxy.GetProxyServer().c_str(),
                    testProxy.GetProxyPort(), testProxy.GetProxyType().c_str(),
                    isValid ? L"validation successful" : L"failed validation" );
            }
        }
    }
    catch( ... ) { }

    m_isBusy = false;
    return isValid;
}

bool PmProxyVerifier::CanReachUrl( const std::string& url )
{
    std::string responseContent = "";
    PmHttpExtendedResult eResult = {};

    m_testHttp.HttpGet( url, responseContent, eResult );

    //https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
    bool isReachable =
        eResult.subErrorCode == CURLE_PEER_FAILED_VERIFICATION || //subErrorDesc:SSL peer certificate or SSH remote key was not OK
        eResult.httpResponseCode >= 100 && //valid http response codes start from 100
        eResult.httpResponseCode != 511 && //511 Network Authentication Required (RFC 6585) - could be returned by a proxy that needs user/pass
        eResult.httpResponseCode != 598 && //598 (Informal convention) Network read timeout error - could be returned by the proxy itself
        eResult.httpResponseCode != 599;   //599 Network Connect Timeout Error - could be returned by the proxy itself

    LOG_DEBUG( "Url:%s  isReachable:%s, httpResponseCode:%d, subErrorType:%s, subErrorCode:%d, subErrorDesc:%s",
        url.c_str(),
        isReachable ? "true" : "false",
        eResult.httpResponseCode,
        eResult.subErrorType.c_str(),
        eResult.subErrorCode,
        m_testHttp.GetErrorDescription( eResult.subErrorCode ).c_str() );

    return isReachable;
}
