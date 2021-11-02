#include "stdafx.h"
#include "ProxyContainer.h"
#include "ProxyDiscovery.h"
#include "ProxyInfoModel.h"
#include "proxy.h"
#include "ThreadedProxyDiscovery.h"
#include "WinHttpWrapper.h"

ProxyContainer::ProxyContainer()
    : m_WinHttp( CreateWinHttpWrapper(), ReleaseWinHttpWrapper )
    , m_Proxy( new Proxy( *m_WinHttp ) )
    , m_ProxyDiscovery( new ProxyDiscovery( m_Proxy.get() ) )
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
