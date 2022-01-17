#include "WinHttpWrapper.h"

BOOL WinHttpWrapper::WinHttpGetDefaultProxyConfiguration( WINHTTP_PROXY_INFO * pProxyInfo )
{
    return ::WinHttpGetDefaultProxyConfiguration( pProxyInfo );
}

BOOL WinHttpWrapper::WinHttpGetIEProxyConfigForCurrentUser( WINHTTP_CURRENT_USER_IE_PROXY_CONFIG* pIeProxyInfo )
{
    return ::WinHttpGetIEProxyConfigForCurrentUser( pIeProxyInfo );
}

HINTERNET WinHttpWrapper::WinHttpOpen( LPCWSTR pszAgentW,
    DWORD dwAccessType,
    LPCWSTR pszProxyW,
    LPCWSTR pszProxyBypassW,
    DWORD dwFlags )
{
    return ::WinHttpOpen( pszAgentW, dwAccessType, pszProxyW, pszProxyBypassW, dwFlags );
}

BOOL WinHttpWrapper::WinHttpGetProxyForUrl( HINTERNET hSession,
    LPCWSTR lpcwszUrl,
    WINHTTP_AUTOPROXY_OPTIONS* pAutoProxyOptions,
    WINHTTP_PROXY_INFO* pProxyInfo )
{
    return ::WinHttpGetProxyForUrl( hSession, lpcwszUrl, pAutoProxyOptions, pProxyInfo );
}

BOOL WinHttpWrapper::WinHttpCloseHandle( HINTERNET hInternet )
{
    return ::WinHttpCloseHandle( hInternet );
}

IWinHttpWrapper* CreateWinHttpWrapper()
{
    return new WinHttpWrapper();
}

void ReleaseWinHttpWrapper( IWinHttpWrapper* winHttpWrapper )
{
    if( winHttpWrapper ) {
        delete winHttpWrapper;
    }
}
