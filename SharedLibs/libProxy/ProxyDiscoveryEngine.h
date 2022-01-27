#pragma once

#include "IProxyDiscoveryEngine.h"

class IWinHttpWrapper;

class ProxyDiscoveryEngine : public IProxyDiscoveryEngine
{
public:
    ProxyDiscoveryEngine( IWinHttpWrapper& winHttpWrapper );
    virtual ~ProxyDiscoveryEngine();

    virtual BOOL GetProxyInfo( PROXY_INFO_LIST *proxyList ) override;
    virtual int Init( LPCTSTR testURL, LPCTSTR urlPAC, CancelProxyDiscoveryCb cancelCb ) override;

protected:
    IWinHttpWrapper& m_winHttp;
    PROXY_INFO_LIST m_proxyList;

    BOOL Discovery( PROXY_FIND_METHOD discoveryMethod, LPCTSTR testURL, LPCTSTR urlPAC, PROXY_INFO_LIST& list );

    BOOL GetAutoProxyInfo( LPCTSTR testURL, LPCTSTR urlPAC, DWORD* options, PROXY_INFO_LIST& list );
    BOOL GetSystemProxyInfo( PROXY_INFO_LIST& list );
    BOOL GetUserIEProxyInfo( PROXY_INFO_LIST& list );
};
