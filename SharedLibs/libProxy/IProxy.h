#pragma once

#include <Windows.h>
#include <functional>
#include "ProxyInfoModel.h"

enum PROXY_INFO_SRC
{
    PROXY_INFO_NONE,
    PROXY_INFO_REG,
    PROXY_INFO_PAC,
    PROXY_INFO_PAC_DHCP,
    PROXY_INFO_PAC_DNS,
    PROXY_INFO_MAX,
};

#define AUTO_PROXY_FLAGS	(1 << PROXY_INFO_REG | 1 << PROXY_INFO_PAC_DHCP | 1 << PROXY_INFO_PAC_DNS)

class IProxy {
public:
    typedef std::function< bool() > CancelProxyDiscoveryCb;

    IProxy() {}
    virtual ~IProxy() {}

    virtual BOOL GetProxyInfo( PROXY_INFO_LIST* proxyList ) = 0;
    virtual int Init( LPCTSTR testURL, LPCTSTR urlPAC, CancelProxyDiscoveryCb cancelCb ) = 0;
};
