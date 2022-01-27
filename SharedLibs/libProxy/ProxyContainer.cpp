#include "stdafx.h"
#include "ProxyContainer.h"
#include "ProxyDiscovery.h"
#include "ProxyInfoModel.h"
#include "ProxyDiscoveryEngine.h"
#include "ThreadedProxyDiscovery.h"
#include "WinHttpWrapper.h"

ProxyContainer::ProxyContainer()
    : m_WinHttp( CreateWinHttpWrapper(), ReleaseWinHttpWrapper )
    , m_proxyDiscoveryEngine( new ProxyDiscoveryEngine( *m_WinHttp ) )
    , m_ProxyDiscovery( new ProxyDiscovery( m_proxyDiscoveryEngine.get() ) )
    , m_threadedProxyDiscovery( new ThreadedProxyDiscovery( *m_ProxyDiscovery ) )
{
}

ProxyContainer::~ProxyContainer()
{
}

IProxyDiscovery& ProxyContainer::GetProxyDiscovery()
{
    return *m_threadedProxyDiscovery;
}
