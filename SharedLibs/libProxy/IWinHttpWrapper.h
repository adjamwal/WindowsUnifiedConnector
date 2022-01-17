#pragma once

#include <Windows.h>
#include <winhttp.h>

class IWinHttpWrapper
{
public:
    virtual ~IWinHttpWrapper() {};

    virtual BOOL WinHttpGetDefaultProxyConfiguration( WINHTTP_PROXY_INFO * pProxyInfo ) = 0;
    virtual BOOL WinHttpGetIEProxyConfigForCurrentUser( WINHTTP_CURRENT_USER_IE_PROXY_CONFIG* pIeProxyInfo ) = 0;
    virtual HINTERNET WinHttpOpen ( LPCWSTR pszAgentW, 
        DWORD dwAccessType,
        LPCWSTR pszProxyW,
        LPCWSTR pszProxyBypassW,
        DWORD dwFlags ) = 0;
    virtual BOOL WinHttpGetProxyForUrl( HINTERNET hSession,
        LPCWSTR lpcwszUrl,
        WINHTTP_AUTOPROXY_OPTIONS* pAutoProxyOptions,
        WINHTTP_PROXY_INFO* pProxyInfo ) = 0;
    virtual BOOL WinHttpCloseHandle( HINTERNET hInternet ) = 0;
};
