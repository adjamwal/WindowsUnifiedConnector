#pragma once

#include "IProxyConsumer.h"
#include "ProxyInfoModel.h"
#include <list>

class IPmHttp;
class IProxyVerifier;

class PmProxyDiscoverySubscriber : public IProxyConsumer
{
public:
    PmProxyDiscoverySubscriber( IPmHttp& pmHttp, IProxyVerifier& proxyVerifier );
    ~PmProxyDiscoverySubscriber();

    void ProxiesDiscovered( const std::list<ProxyInfoModel>& proxySettings ) override;
private:
    IPmHttp& m_http;
    IProxyVerifier& m_proxyVerifier;
    ProxyInfoModel m_lastAppliedProxy;
};
