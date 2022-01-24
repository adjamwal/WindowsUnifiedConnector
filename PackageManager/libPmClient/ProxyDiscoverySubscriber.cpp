#include "ProxyDiscoverySubscriber.h"
#include "IPmHttp.h"
#include "ProxyInfoModel.h"
#include "IProxy.h"
#include "StringUtil.h"
#include "PmLogger.h"

ProxyDiscoverySubscriber::ProxyDiscoverySubscriber( IPmHttp& pmHttp )
    : m_http( pmHttp )
{

}

ProxyDiscoverySubscriber::~ProxyDiscoverySubscriber()
{

}

void ProxyDiscoverySubscriber::ProxiesDiscovered( const std::list<ProxyInfoModel>& proxySettings )
{
    for( auto proxy : proxySettings )
    {
        if( proxy.GetProxyDiscoveryMode() != PROXY_INFO_NONE )
        {
            WLOG_DEBUG( L"Found proxy type %s, server %s:%d", proxy.GetProxyType().c_str(), proxy.GetProxyServer().c_str(), proxy.GetProxyPort() );
            if( proxy.GetProxyType() == L"http_proxy" || proxy.GetProxyType() == L"http" )
            {
                m_http.SetHttpProxy(
                    StringUtil::WStr2Str( proxy.GetProxyServer() ) + ":" + std::to_string( proxy.GetProxyPort() ),
                    StringUtil::WStr2Str( proxy.GetProxyUser() ),
                    StringUtil::WStr2Str( proxy.GetProxyPassword() )
                );
                return;
            }
        }
    }

    //remove proxy if none of the detected proxies were http
    m_http.SetHttpProxy( "", "", "" );
}
