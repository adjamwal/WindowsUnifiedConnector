#include "stdafx.h"
#include "ProxyDiscovery.h"
#include "IProxyConsumer.h"
#include "ProxyDiscoveryEngine.h"
#include "IUcLogger.h"

ProxyDiscovery::ProxyDiscovery( IProxyDiscoveryEngine* proxyDiscoveryEngine )
    : m_proxyDiscoveryEngine( proxyDiscoveryEngine )
    , m_shutdownCb( NULL )
{

}

ProxyDiscovery::~ProxyDiscovery()
{
}

bool ProxyDiscovery::RegisterForProxyNotifications( IProxyConsumer* newConsumer )
{
    bool bRtn = false;
    std::unique_lock< std::mutex > lock( m_consumerMutex );

    if( newConsumer ) {
        for( auto it : m_Consumers ) {
            if( it == newConsumer ) {
                LOG_ERROR( "Consumer is already registered" );
                goto abort;
            }
        }

        m_Consumers.push_back( newConsumer );

        LOG_DEBUG( "Added consumer %p", newConsumer );
        bRtn = true;
    }
    else {
        LOG_ERROR( "Consumer is null" );
    }

abort:
    return bRtn;
}

bool ProxyDiscovery::UnregisterForProxyNotifications( IProxyConsumer* consumer )
{
    bool bRtn = false;
    std::unique_lock< std::mutex > lock( m_consumerMutex );

    if( consumer ) {
        for( auto it = m_Consumers.begin(); it != m_Consumers.end(); it++ ) {
            if( *it == consumer ) {
                m_Consumers.erase( it );
                LOG_DEBUG( "Removed consumer %p", consumer );
                bRtn = true;
                break;
            }
        }
    }
    else {
        LOG_ERROR( "Consumer is null" );
    }

    return bRtn;
}

void ProxyDiscovery::StartProxyDiscoveryAsync(
    const LPCTSTR testURL,
    const LPCTSTR urlPAC )
{
    PROXY_INFO_LIST proxyList;
    ProxyDiscoverAndNotifySync( testURL, urlPAC, proxyList );
}

void ProxyDiscovery::ProxyDiscoverAndNotifySync(
    const LPCTSTR testURL,
    const LPCTSTR urlPAC,
    std::list<ProxyInfoModel>& proxyList )
{
    ProxyDiscoverySync( testURL, urlPAC, proxyList );
    NotifyConsumers( proxyList );
}

void ProxyDiscovery::ProxyDiscoverySync(
    const LPCTSTR testURL,
    const LPCTSTR urlPAC,
    PROXY_INFO_LIST& proxyList )
{
    std::unique_lock< std::mutex > lock( m_proxyMutex );
    m_proxyDiscoveryEngine->Init( testURL, urlPAC, m_shutdownCb );
    m_proxyDiscoveryEngine->GetProxyInfo( &proxyList );
    LOG_DEBUG( "Discovered %d proxies", proxyList.size() );
}

void ProxyDiscovery::NotifyConsumers( const PROXY_INFO_LIST& proxyList )
{
    std::unique_lock< std::mutex > lock( m_consumerMutex );
    for( auto it : m_Consumers ) {
        LOG_DEBUG( "Notify proxy consumer %p", it );
        it->ProxiesDiscovered( proxyList );
    }
}

void ProxyDiscovery::SetShutdownCallback( CheckShutdownCb shutdownCB )
{
    m_shutdownCb = shutdownCB;
}
