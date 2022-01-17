#pragma once

#include "MocksCommon.h"
#include "IProxyDiscovery.h"

class MockProxyDiscovery : public IProxyDiscovery
{
public:
    MockProxyDiscovery();
    virtual ~MockProxyDiscovery();

    MOCK_METHOD1( RegisterForProxyNotifications, bool( IProxyConsumer* ) );
    MOCK_METHOD1( UnregisterForProxyNotifications, bool( IProxyConsumer* ) );

    MOCK_METHOD2( StartProxyDiscoveryAsync, void( const LPCTSTR, const LPCTSTR ) );
    MOCK_METHOD3( StartProxyDiscoverySync, void( const LPCTSTR, const LPCTSTR, std::list<ProxyInfoModel>& ) );
    MOCK_METHOD1( SetShutdownCallback, void( CheckShutdownCb ) );
};
