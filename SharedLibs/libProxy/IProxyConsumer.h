#pragma once
#include <list>
class ProxyInfoModel;

class IProxyConsumer
{
public:
    IProxyConsumer() {}
    virtual ~IProxyConsumer() {}

    virtual void ProxiesDiscovered( const std::list<ProxyInfoModel>& proxySettings ) = 0;
};
