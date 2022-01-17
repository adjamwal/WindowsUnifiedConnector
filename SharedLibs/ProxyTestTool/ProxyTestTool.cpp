// ProxyTestTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "ProxyContainer.h"
#include "IProxyDiscovery.h"
#include "IProxyConsumer.h"
#include "ConsoleLogger.h"

static ConsoleLogger consoleLogger;

void DisplayHelp()
{
    LOG_DEBUG( R"(
Usage:

ProxyTestTool.exe [-a ] [-h] [-t <Test URL>] [-p <PAC file URL>]

    -a, --async: Performs proxy discovery asynchonously

    -h, --help: Prints this menu

    -p, --pacUrl: Location of the PAC file. Used for WINHTTP_AUTOPROXY_CONFIG_URL. QA URL - http://10.85.185.204:9001/lab_noauth.pac

    -t, --testUrl: Used to find proxy within the PAC file

This tool excercises the proxy library. It can discovery 5 types of proxies:
* System proxy set though netsh winhttp set proxy Address:port
* Web Proxy Auto-Discovery Protocol
  * Configured through a config url ( -p )
  * Configured through DHCP
  * Configured through DNS
* Direct connection ( Not really a proxy )
)" );
}

void PrintProxies( const std::list<ProxyInfoModel>& proxyList )
{
    if( !proxyList.size() ) {
        LOG_DEBUG( "No Proxies found" );
        return;
    }

    for( auto& proxy : proxyList ) {
        WLOG_DEBUG( L"Port %d", proxy.GetProxyPort() );
        WLOG_DEBUG( L"Tunnel %d", proxy.GetProxyTunnel() );
        WLOG_DEBUG( L"Type %s", proxy.GetProxyType().c_str() );
        WLOG_DEBUG( L"AccessType %s", proxy.GetProxyAccessType().c_str() );
        WLOG_DEBUG( L"Server %s", proxy.GetProxyServer().c_str() );
        WLOG_DEBUG( L"User %s", proxy.GetProxyUser().c_str() );
        WLOG_DEBUG( L"Password %s", proxy.GetProxyPassword().c_str() );
        WLOG_DEBUG( L"DiscoveryMode %d", proxy.GetProxyDiscoveryMode() );
    }
}

class TestProxyConsumer : public IProxyConsumer
{
public:
    TestProxyConsumer()
        : m_done( false )
    {}

    ~TestProxyConsumer() {}

    void ProxiesDiscovered( const std::list<ProxyInfoModel>& proxySettings ) override
    {
        PrintProxies( proxySettings );
        m_done = true;
    }

    void Wait()
    {
        while( !m_done ) {
            Sleep( 100 );
        }
    }
private:
    bool m_done;
};

int wmain( int argc, wchar_t* argv[], wchar_t* envp[] )
{
    SetUcLogger( &consoleLogger );

    ProxyContainer proxyContainer;
    IProxyDiscovery& proxyDiscovery = proxyContainer.GetProxyDiscovery();
    std::list<ProxyInfoModel> proxyList;
    std::wstring testUrl, pacURL;
    TestProxyConsumer testProxyConsumer;
    bool async = false;

    for( int i = 0; i < argc; i++ ) {
        std::wstring arg = argv[ i ];

        if( ( arg == L"--help" ) || ( arg == L"-h" ) ) {
            DisplayHelp();
            return 0;
        }
        else if( ( arg == L"--testUrl" ) || ( arg == L"-t" ) ) {
            if( argc > ( i + 1 ) ) {
                testUrl = argv[ ++i ];
            }
            else {
                LOG_ERROR( "Invalid parameters" );
                return -1;
            }
        }
        else if( ( arg == L"--pacUrl" ) || ( arg == L"-p" ) ) {
            if( argc > ( i + 1 ) ) {
                pacURL = argv[ ++i ];
            }
            else {
                LOG_ERROR( "Invalid parameters" );
                return -1;
            }
        }
        else if( ( arg == L"--async" ) || ( arg == L"-a" ) ) {
            async = true;
        }
    }

    if( !async ) {
        proxyDiscovery.StartProxyDiscoverySync( testUrl.c_str(), pacURL.c_str(), proxyList );
        PrintProxies( proxyList );
    }
    else {
        proxyDiscovery.RegisterForProxyNotifications( &testProxyConsumer );
        proxyDiscovery.StartProxyDiscoveryAsync( testUrl.c_str(), pacURL.c_str() );
        testProxyConsumer.Wait();
        proxyDiscovery.UnregisterForProxyNotifications( &testProxyConsumer );
    }

    return 0;
}
