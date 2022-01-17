#pragma once

#include "IWinHttpWrapper.h"

class WinHttpWrapper : public IWinHttpWrapper
{
public:
    WinHttpWrapper() {}
    virtual ~WinHttpWrapper() {}

    virtual BOOL WinHttpGetDefaultProxyConfiguration( WINHTTP_PROXY_INFO * pProxyInfo ) override;
    virtual BOOL WinHttpGetIEProxyConfigForCurrentUser( WINHTTP_CURRENT_USER_IE_PROXY_CONFIG* pIeProxyInfo ) override;
    virtual HINTERNET WinHttpOpen( LPCWSTR pszAgentW,
        DWORD dwAccessType,
        LPCWSTR pszProxyW,
        LPCWSTR pszProxyBypassW,
        DWORD dwFlags ) override;
    virtual BOOL WinHttpGetProxyForUrl( HINTERNET hSession,
        LPCWSTR lpcwszUrl,
        WINHTTP_AUTOPROXY_OPTIONS* pAutoProxyOptions,
        WINHTTP_PROXY_INFO* pProxyInfo ) override;
    virtual BOOL WinHttpCloseHandle( HINTERNET hInternet ) override;
};

IWinHttpWrapper* CreateWinHttpWrapper();
void ReleaseWinHttpWrapper( IWinHttpWrapper* winHttpWrapper );
