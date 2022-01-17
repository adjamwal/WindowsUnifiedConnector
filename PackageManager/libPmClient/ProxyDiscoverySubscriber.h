#pragma once

#include "IProxyConsumer.h"
#include <list>

class IPmHttp;
class ProxyInfoModel;

class ProxyDiscoverySubscriber : public IProxyConsumer
{
public:
    ProxyDiscoverySubscriber( IPmHttp& pmHttp );
    ~ProxyDiscoverySubscriber();

    void ProxiesDiscovered( const std::list<ProxyInfoModel>& proxySettings ) override;
private:
    IPmHttp& m_http;
};
