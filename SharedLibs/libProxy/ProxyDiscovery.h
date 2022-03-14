#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <vector>
#include <Windows.h>
#include "IProxyDiscovery.h"
#include "ProxyInfoModel.h"

#define QA_PROXY_TEST_URL  L"https://mgmt.qa1.immunet.com/health/"

class IProxyDiscoveryEngine;

class ProxyDiscovery : public IProxyDiscovery
{
public:
    ProxyDiscovery( IProxyDiscoveryEngine* proxyDiscoveryEngine );
    virtual ~ProxyDiscovery();

    virtual bool RegisterForProxyNotifications( IProxyConsumer* newConsumer ) override;
    virtual bool UnregisterForProxyNotifications( IProxyConsumer* consumer ) override;

    virtual void StartProxyDiscoveryAsync(
        const LPCTSTR testURL,
        const LPCTSTR urlPAC ) override;
    virtual void ProxyDiscoverAndNotifySync(
        const LPCTSTR testURL,
        const LPCTSTR urlPAC,
        std::list<ProxyInfoModel>& proxyList ) override;
    virtual void ProxyDiscoverySync( 
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

    IProxyDiscoveryEngine* m_proxyDiscoveryEngine;
    CheckShutdownCb m_shutdownCb;
};
