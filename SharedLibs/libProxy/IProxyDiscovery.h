#pragma once

#include <list>
#include <functional>
#include "ProxyInfoModel.h"

class IProxyConsumer;

class IProxyDiscovery
{
public:
    typedef std::function< bool() > CheckShutdownCb;

    IProxyDiscovery() {}
    virtual ~IProxyDiscovery() {}

    virtual bool RegisterForProxyNotifications( 
        IProxyConsumer* newConsumer ) = 0;
    virtual bool UnregisterForProxyNotifications(
        IProxyConsumer* newConsumer ) = 0;

    virtual void StartProxyDiscoveryAsync(
        const LPCTSTR testURL,
        const LPCTSTR urlPAC ) = 0;
    virtual void ProxyDiscoverAndNotifySync(
        const LPCTSTR testURL,
        const LPCTSTR urlPAC,
        std::list<ProxyInfoModel>& proxyList ) = 0;
    virtual void ProxyDiscoverySync( 
        const LPCTSTR testURL,
        const LPCTSTR urlPAC,
        std::list<ProxyInfoModel>& proxyList ) = 0;
    virtual void SetShutdownCallback( CheckShutdownCb shutdownCB ) = 0;
};
