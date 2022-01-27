#include "PmProxyDiscoverySubscriber.h"
#include "IPmHttp.h"
#include "IProxyDiscoveryEngine.h"
#include "IProxyVerifier.h"
#include "StringUtil.h"
#include "PmLogger.h"

PmProxyDiscoverySubscriber::PmProxyDiscoverySubscriber( IPmHttp& pmHttp, IProxyVerifier& proxyVerifier )
    : m_http( pmHttp )
    , m_proxyVerifier( proxyVerifier )
{
}

PmProxyDiscoverySubscriber::~PmProxyDiscoverySubscriber()
{
}

void PmProxyDiscoverySubscriber::ProxiesDiscovered( const std::list<ProxyInfoModel>& proxySettings )
{
    if( m_proxyVerifier.IsBusy() )
    {
        LOG_ERROR( "ProxyVerifier busy, ignoring proxy notification" );
        return;
    }

    for( ProxyInfoModel proxy : proxySettings )
    {
        if( proxy == m_lastAppliedProxy ) return;
        else  if( m_proxyVerifier.IsValidProxy( proxy ) )
        {
            m_http.SetHttpProxy(
                StringUtil::Trim( StringUtil::WStr2Str( proxy.GetProxyServer() ) ) + ":" + std::to_string( proxy.GetProxyPort() ),
                StringUtil::WStr2Str( proxy.GetProxyUser() ),
                StringUtil::WStr2Str( proxy.GetProxyPassword() )
            );
            m_lastAppliedProxy = proxy;
            return;
        }
    }

    //remove proxy if none of the detected proxies were valid
    m_http.SetHttpProxy( "", "", "" );
    m_lastAppliedProxy.Clear();
}
