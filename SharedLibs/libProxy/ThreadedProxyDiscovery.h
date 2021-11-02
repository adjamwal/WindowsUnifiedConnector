#pragma once

#include <mutex>

#include "IProxyDiscovery.h"

class ThreadedProxyDiscovery : public IProxyDiscovery
{
public:
    ThreadedProxyDiscovery( IProxyDiscovery& proxyDiscovery );
    virtual ~ThreadedProxyDiscovery();

    virtual bool RegisterForProxyNotifications( IProxyConsumer* newConsumer ) override;
    virtual bool UnregisterForProxyNotifications( IProxyConsumer* newConsumer ) override;

    virtual void StartProxyDiscovery( 
        const LPCTSTR testURL,
        const LPCTSTR urlPAC ) override;
    virtual void StartProxyDiscoverySync(
        const LPCTSTR testURL,
        const LPCTSTR urlPAC,
        std::list<ProxyInfoModel>& proxyList ) override;
    void SetShutdownCallback( CheckShutdownCb shutdownCB );

private:
    void DiscoveryThread( 
        const LPCTSTR testURL,
        const LPCTSTR urlPAC );

private:
    IProxyDiscovery& m_proxyDiscovery;

    std::mutex m_mutex;
    bool m_threadRunning;
    bool m_shutdown;
    std::unique_ptr<std::thread> m_discoveryThread;
};

