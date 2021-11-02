#include "stdafx.h"
#include "proxy.h"
#include "IUcLogger.h"
#include <Winhttp.h>
#include "ProxyInfoModel.h"
#include "ProxyStringParser.h"
#include "IWinHttpWrapper.h"

// Currently, this is not multi-thread safe as m_proxyList is not accessed simultaneously by more than one thread
Proxy::Proxy( IWinHttpWrapper& winHttp ) :
    m_winHttp( winHttp )
{
    m_proxyList.clear();
}

// Use local variable and destructor called when it is out of scope. Hence assumed multi-thread not needed
Proxy::~Proxy()
{
    m_proxyList.clear();
}

BOOL Proxy::GetProxyInfo( PROXY_INFO_LIST* proxyList )
{
    if( m_proxyList.empty() ) return FALSE;

    *proxyList = m_proxyList;
    return TRUE;
}

/*
Assume proxy does not need to be queried for each URL. So need to call this at regular intervals
*/

/*
#define SAMPLE_URL		L"https://enterprise-mgmt.amp.sourcefire.com/health/"
*/

BOOL Proxy::GetAutoProxyInfo( LPCTSTR testURL, LPCTSTR urlPAC, DWORD* pOptionParam, PROXY_INFO_LIST& list )
{
    WINHTTP_AUTOPROXY_OPTIONS* options = ( WINHTTP_AUTOPROXY_OPTIONS* )pOptionParam;
    WINHTTP_PROXY_INFO proxySettings;
    BOOL status = FALSE, b = FALSE;

    if( !testURL ) {
        LOG_ERROR( __FUNCTION__ " test URL is empty" );
        return FALSE;
    }

    HINTERNET hSession = m_winHttp.WinHttpOpen( NULL, 0, NULL, NULL, 0 );
    if( !hSession || !options ) {
        LOG_ERROR( __FUNCTION__ " unable to open http connection: %d", GetLastError() );
        return FALSE;
    }

    memset( &proxySettings, 0, sizeof( proxySettings ) );

    WLOG_DEBUG( __FUNCTION__ L" 0x%x, 0x%x, %s", options->dwFlags, options->dwAutoDetectFlags, options->lpszAutoConfigUrl );
    b = m_winHttp.WinHttpGetProxyForUrl( hSession, testURL, options, &proxySettings );
    if( !b ) {
        WLOG_DEBUG( __FUNCTION__ L" unable to get proxy using WPAD/PAC file at url: %s, %d", options->lpszAutoConfigUrl, GetLastError() );
        goto abort;
    }

    WLOG_DEBUG( __FUNCTION__ L" %d, %s, %s ", proxySettings.dwAccessType, proxySettings.lpszProxy, proxySettings.lpszProxyBypass );

    if( proxySettings.lpszProxy ) {
        ProxyStringParser psp;
        DWORD discoveryMode = PROXY_INFO_PAC;
        if( options->dwAutoDetectFlags & WINHTTP_AUTO_DETECT_TYPE_DHCP )
            discoveryMode = PROXY_INFO_PAC_DHCP;
        else if( options->dwAutoDetectFlags & WINHTTP_AUTO_DETECT_TYPE_DNS_A )
            discoveryMode = PROXY_INFO_PAC_DNS;
        status = psp.ParseProxyString( proxySettings.lpszProxy, list, discoveryMode );
    }

abort:
    m_winHttp.WinHttpCloseHandle( hSession );

    if( proxySettings.lpszProxy ) GlobalFree( proxySettings.lpszProxy );
    if( proxySettings.lpszProxyBypass ) GlobalFree( proxySettings.lpszProxyBypass );
    return status;
}

/*
Retrieves settings directly from system wide settings "HKLM\SOFTWARE\MICROSOFT\Windows\CurrentVersion\Internet Settings\Connections\WinHttpSettings"
*/

BOOL Proxy::GetSystemProxyInfo( PROXY_INFO_LIST& list )
{
    WINHTTP_PROXY_INFO proxySettings {0};
    BOOL status = FALSE, b = FALSE;

    b = m_winHttp.WinHttpGetDefaultProxyConfiguration( &proxySettings );
    if( !b ) {
        LOG_ERROR( __FUNCTION__ " unable to get proxy using registry\n", GetLastError() );
        goto abort;
    }

    WLOG_DEBUG( __FUNCTION__ L" %d, %s, %s", proxySettings.dwAccessType, proxySettings.lpszProxy, proxySettings.lpszProxyBypass );
    if( proxySettings.dwAccessType == WINHTTP_ACCESS_TYPE_NO_PROXY ) goto abort;

    if( proxySettings.lpszProxy ) {
        ProxyStringParser psp;
        status = psp.ParseProxyString( proxySettings.lpszProxy, list, PROXY_INFO_REG );
    }

abort:
    if( proxySettings.lpszProxy ) GlobalFree( proxySettings.lpszProxy );
    if( proxySettings.lpszProxyBypass ) GlobalFree( proxySettings.lpszProxyBypass );
    return status;
}

BOOL Proxy::Discovery( PROXY_INFO_SRC discoveryMode, LPCTSTR testURL, LPCTSTR urlPAC, PROXY_INFO_LIST& list )
{
    BOOL status = FALSE;

    WINHTTP_AUTOPROXY_OPTIONS options;
    memset( &options, 0, sizeof( options ) );

    switch( discoveryMode ) {
    case PROXY_INFO_NONE:
    {
        ProxyInfoModel proxy;
        proxy.SetProxyDiscoveryMode( discoveryMode );
        LOG_DEBUG( __FUNCTION__ ": Add Direct Connection to discovery list" );
        list.push_back( proxy );
        status = true;
        break;
    }
    case PROXY_INFO_REG:
        status = GetSystemProxyInfo( list );

        break;
    case PROXY_INFO_PAC:
        if( testURL && urlPAC ) {
            options.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
            options.lpszAutoConfigUrl = urlPAC;
            status = GetAutoProxyInfo( ( LPCTSTR )testURL, urlPAC, ( DWORD* )&options, list );
        }
        break;
    case PROXY_INFO_PAC_DHCP:
        if( testURL ) {
            options.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
            options.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DHCP;
            status = GetAutoProxyInfo( ( LPCTSTR )testURL, NULL, ( DWORD* )&options, list );
        }
        break;
    case PROXY_INFO_PAC_DNS:
        if( testURL ) {
            options.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
            options.dwAutoDetectFlags = WINHTTP_AUTO_DETECT_TYPE_DNS_A;
            status = GetAutoProxyInfo( ( LPCTSTR )testURL, NULL, ( DWORD* )&options, list );
        }
        break;
    default:
        LOG_ERROR( __FUNCTION__ " %d discovery not supported" );
        break;
    }

    LOG_DEBUG( __FUNCTION__ " method: %d, size: %d, status: %d", discoveryMode, list.size(), status );

    return status;
}

int Proxy::Init( LPCTSTR testURL, LPCTSTR urlPAC, CancelProxyDiscoveryCb cancelCb )
{
    DWORD srcbfl = 0;
    bool abortDiscovery = false;
    int ret = PROXY_INFO_NONE;

    m_proxyList.clear();

    PROXY_INFO_SRC discoveryOrder[] = {
        PROXY_INFO_REG,
        PROXY_INFO_PAC,
        PROXY_INFO_PAC_DHCP,
        PROXY_INFO_PAC_DNS,
        PROXY_INFO_NONE
    };

    for( PROXY_INFO_SRC discMode : discoveryOrder ) {
        if( cancelCb && cancelCb() ) {
            abortDiscovery = true;
            goto abort;
        }

        Discovery( discMode, testURL, urlPAC, m_proxyList );
    }

    for( auto it = m_proxyList.begin(); it != m_proxyList.end(); it++ ) {
        if( cancelCb && cancelCb() ) {
            abortDiscovery = true;
            goto abort;
        }

        if( it->GetProxyDiscoveryMode() != PROXY_INFO_NONE ) {
            if( !it->GetProxyAccessType().compare( L"http" ) )
                it->SetProxyAccessType( L"http_proxy" );
            if( !it->GetProxyType().compare( L"http" ) )
                it->SetProxyType( L"http_proxy" );
            if( it->GetProxyType().empty() && it->GetProxyAccessType().empty() )
                it->SetProxyType( L"http_proxy" );
        }
    }

abort:
    if( abortDiscovery ) {
        m_proxyList.clear();
        ret = PROXY_INFO_NONE;
    }

    return ret;
}

