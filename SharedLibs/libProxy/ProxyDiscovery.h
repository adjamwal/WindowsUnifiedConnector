#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <vector>
#include <Windows.h>
#include "IProxyDiscovery.h"
#include "ProxyInfoModel.h"

class IProxy;

class ProxyDiscovery : public IProxyDiscovery
{
public:
    ProxyDiscovery( IProxy* proxy );
    virtual ~ProxyDiscovery();

    virtual bool RegisterForProxyNotifications( IProxyConsumer* newConsumer ) override;
    virtual bool UnregisterForProxyNotifications( IProxyConsumer* consumer ) override;

    virtual void StartProxyDiscoveryAsync(
        const LPCTSTR testURL,
        const LPCTSTR urlPAC ) override;
    virtual void StartProxyDiscoverySync( 
        const LPCTSTR testURL,
        const LPCTSTR urlPAC,
        PROXY_INFO_LIST& proxyList ) override;
    virtual void SetShutdownCallback( CheckShutdownCb shutdownCB ) override;

protected:
    void NotifyConsumers( const PROXY_INFO_LIST& proxyList );

protected:
    std::vector<IProxyConsumer*> m_Consumers;
    std::mutex m_proxyMutex;
    std::mutex m_consumerMutex;

    IProxy* m_Proxy;
    CheckShutdownCb m_shutdownCb;
};
