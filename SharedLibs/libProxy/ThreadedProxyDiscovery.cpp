#include "ThreadedProxyDiscovery.h"
#include "IUcLogger.h"

#define MAX_SHUTDOWN_RETRIES 200

ThreadedProxyDiscovery::ThreadedProxyDiscovery(
    IProxyDiscovery& proxyDiscovery )
    : m_proxyDiscovery( proxyDiscovery )
    , m_mutex()
    , m_threadRunning( false )
    , m_discoveryThread()
    , m_shutdown( false )
{
    m_proxyDiscovery.SetShutdownCallback( [this]() -> bool { return m_shutdown; } );
}

ThreadedProxyDiscovery::~ThreadedProxyDiscovery()
{
    int retries = 0;
    m_shutdown = true;

    while( true ) {
        Sleep( 100 );
        m_mutex.lock();
        if( !m_threadRunning ) {
            m_mutex.unlock();
            break;
        }
        m_mutex.unlock();

        ++retries;
        if( retries == MAX_SHUTDOWN_RETRIES ) {
            LOG_ERROR( __FUNCTION__ ": Timeout after 20 seconds during shutdown. This may crash" );
        }
    }
}

bool ThreadedProxyDiscovery::RegisterForProxyNotifications( IProxyConsumer* newConsumer )
{
    return m_proxyDiscovery.RegisterForProxyNotifications( newConsumer );
}

bool ThreadedProxyDiscovery::UnregisterForProxyNotifications( IProxyConsumer* newConsumer )
{
    return m_proxyDiscovery.UnregisterForProxyNotifications( newConsumer );
}

void ThreadedProxyDiscovery::StartProxyDiscoveryAsync( 
    const LPCTSTR testURL,
    const LPCTSTR urlPAC )
{
    std::unique_lock< std::mutex > lock( m_mutex );
    if( m_threadRunning ) {
        LOG_DEBUG( __FUNCTION__ ": Proxy discovery thread already running" );
        return;
    }

    m_threadRunning = true;

    try {
        m_discoveryThread.reset( new std::thread( &ThreadedProxyDiscovery::DiscoveryThread, this, testURL, urlPAC ) );
        m_discoveryThread->detach();
    }
    catch( const std::system_error& systemError ) {
        LOG_ERROR( __FUNCTION__ ": Failed to start proxy discovery - '%s'", systemError.what() );
        m_threadRunning = false;
    }
}

void ThreadedProxyDiscovery::ProxyDiscoverAndNotifySync(
    const LPCTSTR testURL,
    const LPCTSTR urlPAC,
    std::list<ProxyInfoModel>& proxyList )
{
    m_proxyDiscovery.ProxyDiscoverAndNotifySync( testURL, urlPAC, proxyList );
}

void ThreadedProxyDiscovery::ProxyDiscoverySync(
    const LPCTSTR testURL,
    const LPCTSTR urlPAC,
    std::list<ProxyInfoModel>& proxyList )
{
    m_proxyDiscovery.ProxyDiscoverySync( testURL, urlPAC, proxyList );
}

void ThreadedProxyDiscovery::DiscoveryThread( 
    const LPCTSTR testURL,
    const LPCTSTR urlPAC )
{
    m_proxyDiscovery.StartProxyDiscoveryAsync( testURL, urlPAC );

    std::unique_lock< std::mutex > lock( m_mutex );
    LOG_DEBUG( __FUNCTION__ ": exiting proxy discovery thread" );
    m_threadRunning = false;
}

void ThreadedProxyDiscovery::SetShutdownCallback( CheckShutdownCb shutdownCB )
{
    LOG_DEBUG( __FUNCTION__ ": Not allowed" );
}
