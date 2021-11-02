#pragma once

#include "IProxy.h"

class IWinHttpWrapper;

class Proxy : public IProxy
{
public:
    Proxy( IWinHttpWrapper& winHttpWrapper );
    virtual ~Proxy();

    virtual BOOL GetProxyInfo( PROXY_INFO_LIST *proxyList ) override;
    virtual int Init( LPCTSTR testURL, LPCTSTR urlPAC, CancelProxyDiscoveryCb cancelCb ) override;

protected:
    IWinHttpWrapper& m_winHttp;
    PROXY_INFO_LIST m_proxyList;

    BOOL Discovery( PROXY_INFO_SRC discoveryMode, LPCTSTR testURL, LPCTSTR urlPAC, PROXY_INFO_LIST& list );

    BOOL GetAutoProxyInfo( LPCTSTR testURL, LPCTSTR urlPAC, DWORD* options, PROXY_INFO_LIST& list );
    BOOL GetSystemProxyInfo( PROXY_INFO_LIST& list );
};
