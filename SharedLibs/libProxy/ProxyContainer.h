#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <vector>
#include <Windows.h>

class IProxyDiscoveryEngine;
class IProxyDiscovery;
class IWinHttpWrapper;

class ProxyContainer
{
public:
    ProxyContainer();
    virtual ~ProxyContainer();

    IProxyDiscovery& GetProxyDiscovery();

protected:
    std::unique_ptr<IWinHttpWrapper, void( * )( IWinHttpWrapper* )> m_WinHttp;
    std::unique_ptr<IProxyDiscoveryEngine> m_proxyDiscoveryEngine;
    std::unique_ptr<IProxyDiscovery> m_ProxyDiscovery;
    std::unique_ptr<IProxyDiscovery> m_threadedProxyDiscovery;
};
