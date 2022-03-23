#pragma once

#include "MocksCommon.h"
#include "IProxyDiscoveryEngine.h"

class MockProxyDiscoveryEngine : public IProxyDiscoveryEngine
{
public:
    MockProxyDiscoveryEngine();
    virtual ~MockProxyDiscoveryEngine();

    MOCK_METHOD1( GetProxyInfo, BOOL( PROXY_INFO_LIST* ) );
    MOCK_METHOD3( Init, int( const LPCTSTR, const LPCTSTR, CancelProxyDiscoveryCb ) );
};
