#pragma once

#include <Windows.h>
#include <functional>
#include "ProxyInfoModel.h"

enum PROXY_FIND_METHOD
{
    PROXY_FIND_NONE,
    PROXY_FIND_REG,
    PROXY_FIND_IE,
    PROXY_FIND_PAC,
    PROXY_FIND_WPAD_DHCP,
    PROXY_FIND_WPAD_DNS,
    PROXY_FIND_MAX
};

#define AUTO_PROXY_FLAGS	(1 << PROXY_FIND_REG | 1 << PROXY_FIND_WPAD_DHCP | 1 << PROXY_FIND_WPAD_DNS)

class IProxyDiscoveryEngine {
public:
    typedef std::function< bool() > CancelProxyDiscoveryCb;

    IProxyDiscoveryEngine() {}
    virtual ~IProxyDiscoveryEngine() {}

    virtual BOOL GetProxyInfo( PROXY_INFO_LIST* proxyList ) = 0;
    virtual int Init( LPCTSTR testURL, LPCTSTR urlPAC, CancelProxyDiscoveryCb cancelCb ) = 0;
};
