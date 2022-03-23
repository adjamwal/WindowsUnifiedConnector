#include "stdafx.h"
#include "ProxyDiscoveryEngine.h"
#include "IUcLogger.h"
#include <Winhttp.h>
#include "ProxyInfoModel.h"
#include "ProxyStringParser.h"
#include "IWinHttpWrapper.h"

// Currently, this is not multi-thread safe as m_proxyList is not accessed simultaneously by more than one thread
ProxyDiscoveryEngine::ProxyDiscoveryEngine( IWinHttpWrapper& winHttp ) :
    m_winHttp( winHttp )
{
    m_proxyList.clear();
}

// Use local variable and destructor called when it is out of scope. Hence assumed multi-thread not needed
ProxyDiscoveryEngine::~ProxyDiscoveryEngine()
{
    m_proxyList.clear();
}

BOOL ProxyDiscoveryEngine::GetProxyInfo( PROXY_INFO_LIST* proxyList )
{
    if( m_proxyList.empty() ) return FALSE;

    *proxyList = m_proxyList;
    return TRUE;
}

/*
Assume proxy does not need to be queried for each URL. So need to call this at regular intervals
*/

/*
#define SAMPLE_URL		L"https://amp-mgmt-int-static.qa1.immunet.com/health/"
*/

BOOL ProxyDiscoveryEngine::GetAutoProxyInfo( const LPCTSTR testURL, const LPCTSTR urlPAC, DWORD* pOptionParam, PROXY_INFO_LIST& list )
{
    WINHTTP_AUTOPROXY_OPTIONS* options = ( WINHTTP_AUTOPROXY_OPTIONS* )pOptionParam;
    WINHTTP_PROXY_INFO proxySettings { 0 };
    BOOL status = FALSE, b = FALSE;

    if( !testURL || !wcslen( testURL ) ) {
        LOG_ERROR( "test URL is empty" );
        return FALSE;
    }

    HINTERNET hSession = m_winHttp.WinHttpOpen( NULL, 0, NULL, NULL, 0 );
    if( !hSession || !options ) {
        LOG_ERROR( "unable to open http connection: %d", GetLastError() );
        return FALSE;
    }

    WLOG_DEBUG( L"flags 0x%x, auto 0x%x, autocfg %s, test %s", 
        options->dwFlags, options->dwAutoDetectFlags, options->lpszAutoConfigUrl, testURL );
    b = m_winHttp.WinHttpGetProxyForUrl( hSession, testURL, options, &proxySettings );
    if( !b ) {
        WLOG_DEBUG( L"unable to get proxy using WPAD/PAC: error %d", GetLastError() );
        goto abortAuto;
    }

    WLOG_DEBUG( L"access 0x%x, proxy %s, bypass %s ", 
        proxySettings.dwAccessType, proxySettings.lpszProxy, proxySettings.lpszProxyBypass );

    if( proxySettings.lpszProxy ) {
        ProxyStringParser psp;
        DWORD discoveryMethod = PROXY_FIND_PAC;
        if( options->dwAutoDetectFlags & WINHTTP_AUTO_DETECT_TYPE_DHCP )
            discoveryMethod = PROXY_FIND_WPAD_DHCP;
        else if( options->dwAutoDetectFlags & WINHTTP_AUTO_DETECT_TYPE_DNS_A )
            discoveryMethod = PROXY_FIND_WPAD_DNS;
        status = psp.ParseProxyString( proxySettings.lpszProxy, list, discoveryMethod );
    }

abortAuto:
    m_winHttp.WinHttpCloseHandle( hSession );

    if( proxySettings.lpszProxy ) GlobalFree( proxySettings.lpszProxy );
    if( proxySettings.lpszProxyBypass ) GlobalFree( proxySettings.lpszProxyBypass );
    return status;
}

/*
Retrieves settings directly from system wide settings "HKLM\SOFTWARE\MICROSOFT\Windows\CurrentVersion\Internet Settings\Connections\WinHttpSettings"
Set proxy: netsh winhttp set proxy <ip>:<port>
Reset proxy: netsh winhttp reset proxy
https://docs.microsoft.com/en-us/microsoft-365/security/defender-endpoint/configure-proxy-internet?view=o365-worldwide#configure-the-proxy-server-manually-using-netsh-command
*/
BOOL ProxyDiscoveryEngine::GetSystemProxyInfo( PROXY_INFO_LIST& list )
{
    WINHTTP_PROXY_INFO proxySettings { 0 };
    BOOL status = FALSE, b = FALSE;

    b = m_winHttp.WinHttpGetDefaultProxyConfiguration( &proxySettings );
    if( !b ) {
        LOG_ERROR( "unable to get proxy using registry\n", GetLastError() );
        goto abortREG;
    }

    WLOG_DEBUG( L"access 0x%x, proxy %s, bypass %s", 
        proxySettings.dwAccessType, proxySettings.lpszProxy, proxySettings.lpszProxyBypass );
    if( proxySettings.dwAccessType == WINHTTP_ACCESS_TYPE_NO_PROXY ) goto abortREG;

    if( proxySettings.lpszProxy ) {
        ProxyStringParser psp;
        status = psp.ParseProxyString( proxySettings.lpszProxy, list, PROXY_FIND_REG );
    }

abortREG:
    if( proxySettings.lpszProxy ) GlobalFree( proxySettings.lpszProxy );
    if( proxySettings.lpszProxyBypass ) GlobalFree( proxySettings.lpszProxyBypass );
    return status;
}

/*
Retrieves proxy from IE settings for current user only
*/
BOOL ProxyDiscoveryEngine::GetUserIEProxyInfo( PROXY_INFO_LIST& list )
{
    WINHTTP_CURRENT_USER_IE_PROXY_CONFIG proxySettings { 0 };
    BOOL status = FALSE, b = FALSE;

    b = m_winHttp.WinHttpGetIEProxyConfigForCurrentUser( &proxySettings );
    if( !b ) {
        LOG_ERROR( "unable to get proxy from current user's IE settings\n", GetLastError() );
        goto abortIE;
    }

    WLOG_DEBUG( L"auto 0x%x, autocfg %s, proxy %s, bypass %s",
        proxySettings.fAutoDetect, proxySettings.lpszAutoConfigUrl,
        proxySettings.lpszProxy, proxySettings.lpszProxyBypass );
    if( proxySettings.fAutoDetect || !wcslen( proxySettings.lpszProxy ) ) goto abortIE;

    if( proxySettings.lpszProxy ) {
        ProxyStringParser psp;
        status = psp.ParseProxyString( proxySettings.lpszProxy, list, PROXY_FIND_IE );
    }

abortIE:
    if( proxySettings.lpszAutoConfigUrl ) GlobalFree( proxySettings.lpszAutoConfigUrl );
    if( proxySettings.lpszProxy ) GlobalFree( proxySettings.lpszProxy );
    if( proxySettings.lpszProxyBypass ) GlobalFree( proxySettings.lpszProxyBypass );
    return status;
}

BOOL ProxyDiscoveryEngine::Discovery( PROXY_FIND_METHOD discoveryMethod, const LPCTSTR testURL, const LPCTSTR urlPAC, PROXY_INFO_LIST& list )
{
    BOOL status = FALSE;
    WINHTTP_AUTOPROXY_OPTIONS options { 0 };

    WLOG_DEBUG( L"method: %d, size: %d, test: %s", discoveryMethod, list.size(), testURL );
    switch( discoveryMethod )
    {
    case PROXY_FIND_REG:
        status = GetSystemProxyInfo( list );
        break;
    case PROXY_FIND_IE:
        status = GetUserIEProxyInfo( list );
        break;
    case PROXY_FIND_PAC:
        if( urlPAC && wcslen( urlPAC ) ) {
            options.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
            options.lpszAutoConfigUrl = urlPAC;
            status = GetAutoProxyInfo( testURL, urlPAC, ( DWORD* )&options, list );
        }
        break;
    case PROXY_FIND_WPAD_DHCP:
        options.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
        options.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP;
        status = GetAutoProxyInfo( testURL, NULL, ( DWORD* )&options, list );
        break;
    case PROXY_FIND_WPAD_DNS:
        options.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
        options.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DNS_A;
        status = GetAutoProxyInfo( testURL, NULL, ( DWORD* )&options, list );
        break;
    default:
        LOG_ERROR( "method %d not supported" );
        break;
    }

    LOG_DEBUG( "status: %d, size: %d", status, list.size() );
    return status;
}

int ProxyDiscoveryEngine::Init( const LPCTSTR testURL, const LPCTSTR urlPAC, CancelProxyDiscoveryCb cancelCb )
{
    DWORD srcbfl = 0;
    bool abortDiscovery = false;
    int ret = PROXY_FIND_NONE;

    m_proxyList.clear();

    PROXY_FIND_METHOD discoveryOrder[] = {
        PROXY_FIND_REG,
        PROXY_FIND_PAC,
        PROXY_FIND_WPAD_DHCP,
        PROXY_FIND_WPAD_DNS
    };

    WLOG_DEBUG( L"%s, %s", testURL, urlPAC );

    for( PROXY_FIND_METHOD discMode : discoveryOrder ) {
        if( cancelCb && cancelCb() ) {
            abortDiscovery = true;
            goto abortInit;
        }

        Discovery( discMode, testURL, urlPAC, m_proxyList );
    }

    for( auto it = m_proxyList.begin(); it != m_proxyList.end(); it++ ) {
        if( cancelCb && cancelCb() ) {
            abortDiscovery = true;
            goto abortInit;
        }

        if( !it->GetProxyAccessType().compare( L"http" ) )
            it->SetProxyAccessType( L"http_proxy" );
        if( !it->GetProxyType().compare( L"http" ) )
            it->SetProxyType( L"http_proxy" );
        if( it->GetProxyType().empty() && it->GetProxyAccessType().empty() )
            it->SetProxyType( L"http_proxy" );
    }

abortInit:
    if( abortDiscovery ) {
        m_proxyList.clear();
        ret = PROXY_FIND_NONE;
    }

    return ret;
}

